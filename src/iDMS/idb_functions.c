/*
############## Intel Corporation Confidential information. ############
#                                                                     #
#              Intel  confidential		                      #
#                                                                     #
# This listing is supplied under the terms of a  license  agreement   #
# with Intel Corporation and may not be copied nor disclosed except   #
# in accordance with the terms of the agreement.                      #
#                                                                     #
############## Intel Corporation Confidential information. ############

#######################################################################
# RCS information:                                                    #
# $Id: idb_functions.c,v 1.1.2.64 1998/05/08 21:47:08 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
/*
//----------------------------------------------------------------------------
//            (C) Copyright Intel Corporation, 1994 - 2003
//
//        Licensed material --  Program property of Intel
//        Corporation All Rights Reserved
//
//        This program is the   property of Intel  and is
//        furnished  pursuant  to  a    written   license
//        agreement.  It may not  be used, reproduced, or
//        disclosed  to others  except in accordance with
//        the terms and conditions of that agreement.
//
//        Intel Design Technology Copyright reserved
//        Date: 04/23/97
//
//        Authors: Sorin Blebea
//
//        File: idb_functions.c  -- implementation for the "idb_" functions
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h> /* for directory access */
#endif
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <idb_functions.h>
#include <idb_codes.h>
#include <errno.h>
/* cadence not available in linux
 */
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
#include <pwd.h>
#else
#include <itkDB.h>
#endif

#include <os_functions.h>
#include <dmsC_defines.h>
#include <dmsC_reports.h>

/*----------*
 * Typedefs *
 *----------*/
#define GETWS 0
#define GETVAULT 1

/*--------------------*
 * External variables *
 *--------------------*/

#include <dmsC_new_vars.h>

/*--------------------*
 * Global variables *
 *--------------------*/
int sync_on;  /* 0: synchronicity disabled.  1: synchronicity is on. */
int _retired_cellview=0; /* 0: last creation was brand new cell.  1: created a retired cell. */

uid_t DMeuid = 0;
uid_t DMruid = 0;

#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
/* these variables are used to keep track of library list */
char** libName=0;
char** libPath=0;
char** libType=0;
static size_t libMax = 0;
static size_t libNum = 0;

char *BFS(const char *fileName);
char *findObject(const char *dirPath, const char *fileName);
void clearQ(void);
void pushQ(const char*dirPath);

#endif

/*------------*
 * Prototypes *
 *------------*/

/* Since voErrSet.h can no longer be found the following
 * func prototypes have been defined 
 */
char *voSimplifyFileName(const char *, const char *);
char *idb_get_vault(void *);
char *idmGetPattern(char *viewName, char *pattern_buff);
const char *idb_get_record_dm_vendor_string(int keep);

/*=======================*
 * Functions Definitions *
 *=======================*/

static char _env_mess_buff[BUFSIZE];
static char fnb[256];
static char* ward = 0;
static char *_env_table[] = {"CDS_INST_DIR",
				"SYNC_DIR",
                           };
static int _env_table_len = sizeof(_env_table) / sizeof (char *);
static char outstring[MAXBUFSIZE];
int idb_check_env_settings(char **message)
{
   int i;
   int error_detected = 0;
   char *crt_value;

   if (message){
      *message = _env_mess_buff;
       strcpy(*message, 
          "\n   Following variables need to be defined correctly:");
   }
   for (i = 0; i < _env_table_len; i++){
      if (!(crt_value = getenv(_env_table[i]))){
         error_detected = 1;
         if (message){
            strcat(*message, "\n      ");
            strcat(*message, _env_table[i]);
         }
      }
      else if (!osFileExists(crt_value)){
         error_detected = 1;
         if (message){
            strcat(*message, "\n      ");
            strcat(*message, _env_table[i]);
            strcat(*message, " wrongly set (");
            strcat(*message, crt_value);
            strcat(*message, ")");
         } 
      }
   }
   if (error_detected){
      if (message)
         strcat(*message, "\n");
      return 0;
   }
   else{
      if (message)
          *message[0] = '\0';
      return 1;    
   }
}

/*-------------------------------------*
 * Hamburger helpers - dmserver take 2 *
 *-------------------------------------*/

/* breadcrumbs when creating new cells in DB_ROOT */

static char _last_directory_cell[BUFSIZE];

const char *idb_set_last_directory_cell (const char *dn)
{
   if (! dn)
      _last_directory_cell[0] = '\0';
   else 
      strcpy(_last_directory_cell, dn);

   return _last_directory_cell;
}

const char *idb_get_last_directory_cell ()
{
   return _last_directory_cell;
}


static char _last_directory1_cell[BUFSIZE];

const char *idb_set_last_directory1_cell (const char *dn)
{
   if (! dn)
      _last_directory1_cell[0] = '\0';
   else
      strcpy(_last_directory1_cell, dn);
   return _last_directory1_cell;
}

const char *idb_get_last_directory1_cell ()
{
   return _last_directory1_cell;
}


static char _last_cell_name[BUFSIZE];

const char *idb_set_last_cell_name (const char *cn)
{
  if (!cn)
     _last_cell_name[0] = '\0';
  else 
     strcpy( _last_cell_name, cn);

  return _last_cell_name; 
}


const char *idb_get_last_cell_name ()
{
   return _last_cell_name;
}


static char _last_workfile[BUFSIZE];

const char *idb_set_last_workfile (const char *dn)
{
   if (! dn)
      _last_workfile[0] = '\0'; 
   else
      strcpy(_last_workfile, dn);

   return _last_workfile;
}

const char *idb_get_last_workfile ()
{
   return _last_workfile; 
}


static char *_bempty = "";

/*----------------------------------------------------------------------------
 * --------
 * idb_init -- initializes/sets some dm specific structures/vars 
 * --------
 * idb_terminate
 *
 * Comments do not belong to me, just pulled in and isolated the part that
 * refers to various cadence initializations
 *
 * For 4.4.x: Changes to these 2 functions are based on idaInitPI() and for 
 * the sole purpose of getting thru the DesSync evaulation.		-sh 10/13/99
 *----------------------------------------------------------------------------*/

int check_read_only(char*ffpath,char *op,int cache) 
{
  static int recursive=0;
  struct stat fileStat;
  char tmp[BUFSIZE];
  char fpath[BUFSIZE];
  
  char tmp1[BUFSIZE];
  int isok=1;
  FILE*file=0;
  
  file=fopen("dmserver.log","a");
  if (!file) {
    file=stderr;
  }
  
  strcpy(tmp1,ffpath);
  
  /*if (strstr(tmp1,"lnf.sync.cds")) {
    *strstr(tmp1,"lnf.sync.cds")=0;
  
    strcat(tmp1,"lnf/master.tag");
    strcpy(fpath,tmp1);
  } else if (strstr(tmp1,"dice.sync.cds")) {
    *strstr(tmp1,"dice.sync.cds")=0;
  
    strcat(tmp1,"dice/master.tag");
    strcpy(fpath,tmp1);

  } */

  if (strstr(tmp1, ".sync.cds")) {
    *strstr(tmp1, ".sync.cds") = 0;
    strcat(tmp1, "/master.tag");
    strcpy(fpath,tmp1);
  } else {
    strcpy(fpath,tmp1);
  }
  
  
  if(stat(fpath, &fileStat)==0) {
    memset(tmp,0,BUFSIZE);
    if(readlink(fpath, tmp,BUFSIZE)<0){
      if (cache==1){
        fprintf(file,"-E- %s CACHE -File %s should be a link but it is a real file. error %s number %d\n",op,fpath,strerror(errno),errno);
        isok=0;
      }
      if(cache==2) {
        if ((S_IWUSR&fileStat.st_mode) ||
            (S_IWGRP&fileStat.st_mode) ||
            (S_IWOTH&fileStat.st_mode)) {
          fprintf(file,"-E- %s CACHE -File %s has WRITE permisons %o\n",op,tmp,fileStat.st_mode);
          isok=0;
            
          } else {
            fprintf(file,"-I- %s CACHE -File %s with permisons %o\n",op,fpath,fileStat.st_mode);
          }
      } else {
        fprintf(file,"-I- %s File %s is a real file.\n",op,fpath);
	  }
      
    } else {
      if (cache) {
        if (cache==2) {
          isok=check_read_only(tmp,op,0);
        } else {
          if ((S_IWUSR&fileStat.st_mode) ||
              (S_IWGRP&fileStat.st_mode) ||
              (S_IWOTH&fileStat.st_mode)) {
            fprintf(file,"-E- %s CACHE -File %s has WRITE permisons %o\n",op,tmp,fileStat.st_mode);
            isok=0;
            
          } else {
            fprintf(file,"-I- %s CACHE -File %s is a link to %s permisons %o\n",op,fpath,tmp,fileStat.st_mode);
          }
        }
        
      } else {
        fprintf(file,"-E- %s CACHE -File %s should be a file but it is a link %s.\n",op,fpath,tmp);        
        isok=0;
      }
      
    }
      
  } else {
    isok=0;
    
    
     fprintf(file,"-E- %s CACHE -File %s Cannot be stat\n",op,fpath);
  }
  if (file!=stderr ){
    fclose(file);
  }
  return isok;
  
  
}


int idb_init(void *init_data)
{
  int print_sync_msg = 0;
  int returnVal = TRUE;
  int argc=1;
  char *program[15];
  char *disable_cds_lib=0;
  
  FILE *cdsFile=NULL;
  char *umask_env;
  mode_t umask_val;
  int f=geteuid(),f1=getuid();

  idb_set_last_directory_cell("");
  idb_set_last_directory1_cell("");
  idb_set_last_cell_name("");
 
#ifdef HPPA
  static struct sigaction msl_sigfpe_handler;
#endif
  
   /* Will only initialize Cadence PI once */

   /* On HP, the floating point Signal Handler used by Mainsail has also
      been used by Cadence.  The workaround is to save Mainsail's
      floating point Signal Handler in here (before getting into Opus),
      and restore it.  -- ttang 2/17/95 */
  program[0]=(char*)init_data;
  /* setreuid (geteuid(),getuid()); */
#ifdef HPPA
      if (sigaction(SIGFPE, NULL, &msl_sigfpe_handler) != 0) {
         /* If fail to save MAINSAIL's SIGFPE handler, print a warning
            message because Mainsail will hang in debug mode */

         voWarn("Unable to save MAINSAIL SIGFPE handler\n");
         voWarn("Debugging MAINSAIL code not possible\n");
      } 
#endif
      
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
      _main();
#endif
 
      argc=3;
      ward=getenv("WORK_AREA_ROOT_DIR");
      disable_cds_lib=getenv("DISABLE_IDMS_CDSLIB");
      if (disable_cds_lib) {
        argc=1;
        *fnb=0;
        program[1] = NULL;
      } else {
        if (ward) {
          sprintf(fnb,"%s/rundir/.%s_%d_cds.lib",ward,dmsAppName,getpid()); 
          cdsFile=fopen(fnb,"a");
          if (!cdsFile) {
            if (debugDmsOn)
              dmsPrintIdmsLog("*IDMS - CADENCE WARNING: Cannot create cds.lib file in desired location $WOR_AREA_ROOT_DIR/rundir %s\n", fnb);
          }
        
        }
        if (!cdsFile) {
          sprintf(fnb,".%s_%d_cds.lib",dmsAppName,getpid()); 
          cdsFile=fopen(fnb,"a");
        }
      
        program[0] = "dbAccessIntel";
        if (cdsFile) {
          if (debugDmsOn)
            dmsPrintIdmsLog("*IDMS - Using cds.lib in: %s\n", fnb);
          program[1] = "-cdslib";
          program[2] = fnb;
          program[3] = NULL;
          fclose(cdsFile);
        }else {
          argc=1;
          *fnb=0;
          program[1] = NULL;
        }
      }
      
      
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
      if (dbInit(&argc, program) == FALSE) {
	returnVal = FALSE;
	voWarn("could not initialize Cadence database.\n");
      }
      else
	returnVal = TRUE;
#else
      returnVal = TRUE;
#endif

      /* Set the umask for the DM server if specified */
      umask_env = getenv("DMSERVER_UMASK");
      if(umask_env != NULL) {
        umask_val = strtol(umask_env, NULL, 8); /* always assume octal numbers */
        umask(umask_val);
      }

      //. some users don't want to see this msg.
      if(getenv("DISABLE_SYNC_MSG") == 0) {
        print_sync_msg = 1;
      }

#ifdef DISABLE_SYNCHRONICITY
      sync_on = 0;
      if(print_sync_msg) {
        printf("-I- Synchronicity is disabled [pid: %zu].\n", (size_t) getpid());
      }
#else 
      if(getenv("DISABLE_SYNC")) {
      sync_on=0;
        if(print_sync_msg) {
          printf("-I- Synchronicity is disabled [pid: %zu].\n", (size_t) getpid());
        }
      } else {
        sync_on=1;
        DMeuid = geteuid();
        DMruid = getuid();

  char *rname;
  char *ename; 
  asprintf(&rname, "%s", getpwuid(DMruid) ? getpwuid(DMruid)->pw_name : "?unknown?");
  asprintf(&ename, "%s", getpwuid(DMeuid) ? getpwuid(DMeuid)->pw_name : "?unknown?");

        printf("Initialize Synchronicity. Running as ruid %zu (%s) and euid %zu (%s) [pid: %zu]\n",
          (size_t) DMruid,
          rname,
          (size_t) DMeuid,
          ename,
          (size_t) getpid()
        );

  free(rname);
  free(ename);

        if(returnVal) {
	  if(SUCCEEDED(sapiInitialize(argc, program))) {
	    returnVal = TRUE;
	  }
	  else {
	    printf("sapiInitialize: Could not initialize\n");
	    returnVal = FALSE;
	  }
        }
      }
#endif

#ifdef HPPA

      /* HP floating point signal workaround -- ttang */
      if (sigaction(SIGFPE, &msl_sigfpe_handler, NULL)!= 0) {
         /* If fail to restore SIGFPE handler, print a warning message
            because Mainsail will hang in debug mode */
         voWarn("Unable to restore MAINSAIL SIGFPE handler\n");
         voWarn("Debugging MAINSAIL code not possible\n");
      }
#endif /* End of HP floating point signal workaround -- ttang */

  return returnVal;

}

char *idb_get_startup(const char *cdslib) {
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
  return fnb;
#else
  return *fnb ? fnb : ddGetStartup((char *)cdslib, NULL);
#endif
}

void idb_terminate()
{
/*  if(!getenv("DISABLE_SYNC")) */
  if(sync_on==1)
   sapiShutdown();
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
   dbExit();
#endif   
   if (fnb) {
     unlink(fnb);
     *fnb=0;
   }
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
   idb_clear_libs(0);
#endif
}

/*----------------------------------------------------------------------------
 * Manipulate error codes
 *----------------------------------------------------------------------------*/

static int __errcode = 0;

int idb_clear_errors()
{
   return (__errcode = 0);
}


int idb_OR_error_code(int errc)
{
   return (__errcode |= errc);
}


int idb_set_error_code(int errc)
{
   return (__errcode = errc);
}


int idb_check_error_code(int errc)
{
   return (__errcode & errc)?1:0;
}


int idb_check_success()
{
   return (__errcode == 0);
}


/*----------------------------------------------------------------------------
 * ------------------------------
 * idb_get_cdn_vendor_message
 * idb_set_cdn_vendor_message
 * idb_capture_cdn_vendor_warning
 * ------------------------------   
 * 
 * Provided to capture the last message set by a vendor DM engine if the engine
 * actually supports it.
 *
 *----------------------------------------------------------------------------*/

static char *_cdn_vendor_buff = 0;
int _last_allocated = 0;

const char *idb_get_cdn_vendor_message()
{
   if (!_cdn_vendor_buff){
      if ((_cdn_vendor_buff = (char *)malloc(_last_allocated = 1024)) == 0){
         _last_allocated = 0;
         return 0;  
      }
      _cdn_vendor_buff[0] = '\0';
   }
   //return _cdn_vendor_buff;
   return idb_get_record_dm_vendor_string(1);
//idb_get_dm_vendor_message();
}


const char *idb_set_cdn_vendor_message(const char *new_message)
{
   if (!_cdn_vendor_buff)
      if ((_cdn_vendor_buff = (char *)malloc(_last_allocated = 1024)) == 0)
         return 0;   /* some initial value for the buffer to most likely */
                     /* not need any reallocation */

   if (!new_message) /* just clear it */
      _cdn_vendor_buff[0] = '\0';
   else{
      if (strlen(new_message) >= (_last_allocated - 1)){
         free(_cdn_vendor_buff);
         _cdn_vendor_buff = (char *)malloc(_last_allocated = 
            strlen(new_message) + 1);
         if (!_cdn_vendor_buff){
            _last_allocated = 0;
            return 0;
         }
      }
      strcpy(_cdn_vendor_buff, new_message); 
   }
   return _cdn_vendor_buff;
}


const char *idb_capture_cdn_vendor_warning()
{
   idb_set_cdn_vendor_message(voGetWarn());
   if (debugDmsOn)
      dmsPrintIdmsLog("*IDMS - CADENCE WARNING: %s\n", _cdn_vendor_buff);
   return _cdn_vendor_buff;
}


static char *_vendor_string = 0;

const char *idb_get_cdn_vendor_string()
{
  if (!_vendor_string){
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
    _vendor_string = strdup("*IDMS - CADENCE*");
#else
    _vendor_string = strdup("*IDMS - SYNCHRONICITY*");
#endif
  }
  return _vendor_string;
}


const char *idb_set_dm_vendor_string(const char *str)
{
   if (!str)
      return idb_get_cdn_vendor_string();
   if (_vendor_string)
     free(_vendor_string);
   return (_vendor_string = strdup(str));
}

static int oldstdoud =0;
static int oldstderd =0;
static FILE *wfile=0;
const char *idb_record_dm_vendor_string()
{
  if(!oldstdoud){
    voFlushWarn();
    fflush(stdout);
    /* create a temp file that will hold both stdout/stderr streams */
    wfile = tmpfile();
    if(wfile) {
      /* store original stdout/stderr streams */
      oldstdoud = dup(fileno(stdout));
      oldstderd = dup(fileno(stderr));
      /* close 1,2 streams and copy wfile to 1 and 2 */
      dup2(fileno(wfile),1);
      dup2(fileno(wfile),2);
    }
  } 
}

static char __buffer[BUFSIZE]="";

const char *idb_get_record_dm_vendor_string(int keep)
{
  if(oldstdoud){
    strcpy(__buffer,"");
    fflush(wfile);
    fseek(wfile,0,0);
    /* extract contents of wfile */
    while(!feof(wfile) ){
      fgets(__buffer+strlen(__buffer),sizeof(__buffer)-strlen(__buffer)-1,wfile);
    }
    if(wfile)
      fclose(wfile);
    fflush(stdout);
    /* restore stdout/stderr streams */
    dup2(oldstdoud, fileno(stdout));
    dup2(oldstderd, fileno(stderr));
    /* close these temporary file descriptors and reset them to 0. */
    close(oldstdoud);
    close(oldstderd);
    oldstdoud=0;
    oldstderd=0;

    voFlushWarn();
    if (__buffer[0]=='\n'){
      __buffer[0]=' ';
    }
    voWarn(__buffer);
  }
  if (!keep){
   __buffer[0]=0;
  }
  return __buffer;
}

/*-----------------------------------------------------------------------
 * idb_set_library_path
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description: Uses ddCreateLib to modify the cds.lib. If libName exists
 *		at the mentioned libPath then OK. Else creates a lib. 
 *		The libName is	obtained from the tail of the libPath.		
 *								-sh 2/14/00
 *-----------------------------------------------------------------------*/
int idb_set_library_path(const char *libpath)
{
  char *libName;
  ddId ddLibId;

  libName = idb_strip_last_token(libpath, "/");

  if(libName) {
    ddLibId = ddCreateLib((char *)libName, (char *)libpath);
    if(ddLibId){
      return (int)ddUpdateLibList();
    }
  }
  else
    return (int)0;
}


/*-----------------------------------------------------------------------
 * idb_append_to_lib_list
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description: Appends to the given buffer all the libraries that
 *              match the pattern. If second param is null uses internal
 *              buffer (recommended because it checks for space).
 *-----------------------------------------------------------------------*/

static char *__libname_buff = 0;
static size_t __Llibname_buff = 0;

char *idb_append_to_lib_list(char *pattern, char *libNameList, int reset_string)
{
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
   int use_int_buff = 0;
   char *tmp;
   size_t j=0;
   
   if (!libNameList){
      use_int_buff = 1;
      if (!__libname_buff)
         if (!(__libname_buff = (char *)malloc(__Llibname_buff = BUFSIZE))){
            __Llibname_buff = 0;
            return 0;
         }
         else{
            __libname_buff[0] = '\0';
         }
      libNameList = __libname_buff;
   }
   if (reset_string){
     libNameList[0] = '\0';
   }

   for(j=0; j<libNum; j++){
     if(dmsMatchExpression(pattern, (char*)libName[j])) {
       if (use_int_buff){
         if ((strlen(libNameList) + strlen((char*)libName[j]) +2) >= __Llibname_buff){
           if (!(tmp = strdup(libNameList)))
             return 0;
           if (!(__libname_buff = (char *)malloc( __Llibname_buff = strlen(libNameList)
                       + strlen((char*)libName[j]) + 2 + BUFSIZE))){
             __Llibname_buff = 0;
             return 0;
           }
           libNameList = __libname_buff;
           strcpy(libNameList, tmp);
           free(tmp);
         }
       }
       strcat(libNameList, (char*)libName[j]);	/* space sep libNames */
       strcat(libNameList, " ");
     }
   }
#else
   ddLibListGenState *pState;
   ddId  libId;
   char *ptr1;
   int use_int_buff = 0;
   char *tmp;

   if (!libNameList){
      use_int_buff = 1;
      if (!__libname_buff)
         if (!(__libname_buff = (char *)malloc(__Llibname_buff = BUFSIZE))){
            __Llibname_buff = 0;
            return 0;
         }
         else{
            __libname_buff[0] = '\0';
         }
      libNameList = __libname_buff;
   }

   if (reset_string){
     libNameList[0] = '\0';
   }

   pState = ddStartGenLibListLib();

   if (pState != NULL) {
      while (libId = ddGenLibListLib(pState)) {
         if (dmsMatchExpression(pattern, ptr1 = ddGetObjName(libId))) {
             if (use_int_buff){
                if ((strlen(libNameList) + strlen(ptr1) +2) >= __Llibname_buff){
                   if (!(tmp = strdup(libNameList)))
                      return 0;
                   if (!(__libname_buff = (char *)malloc(
                         __Llibname_buff =
                         strlen(libNameList) + strlen(ptr1) + 2 + BUFSIZE))){
                      __Llibname_buff = 0;
                      return 0;
                   }
                   libNameList = __libname_buff;
                   strcpy(libNameList, tmp);
                   free(tmp);
                }
             }
             strcat(libNameList, ptr1);	/* space sep libNames */
             strcat(libNameList, " ");
         }
      }
      ddStopGen(pState);
   }
#endif   
   return libNameList;
}


/*-----------------------------------------------------------------------
 * idb_open_library 
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:  __crt_lib_id
 *    Global vars: 
 *
 * Uses:
 *    Global var:
 *
 *
 * Description: Opens a library. For Cadence, it returns a ddId, which
 *              is translated into a void * through a local variable 
 *              RETURN 0 or (ddId) FOR CONVENIENCE !!!
 *                 
 * Redundant:   absolute_path
 *-----------------------------------------------------------------------*/

/* transitional hack */

static ddId __crt_lib_id = 0;

void *idb_open_library(const char *name, const char *absolute_path,
                             const char *mode)
{
    __crt_lib_id = ddGetObj((char *)name, NULL, NULL, NULL, NULL, (char *)mode);
    if (debugDmsOn) {
       dmsPrintIdmsLog("*IDMS DEBUG* -- __crt_lib_id set to %d through idb_open_library\n", __crt_lib_id ); 
    }
    return (__crt_lib_id)?(void *)__crt_lib_id:0;
}


/*-----------------------------------------------------------------------
 * idb_create_library
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:  __crt_lib_id
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description: Creates a new library. For Cadence, it returns a ddId, which
 *              is translated into a void * through a local variable 
 * 		RETURN 0 or ddId FOR CONVENIENCE !!!
 * Redundant:   workGroup userGroup
 *
 *-----------------------------------------------------------------------*/

void  *idb_create_library(const char *libName, const char *path,
                          const char *workGroup, const char *userGroup)
{
    __crt_lib_id = ddCreateLib((char *)libName, (char *)path);
    if (debugDmsOn) {
       dmsPrintIdmsLog("*IDMS DEBUG* -- __crt_lib_id set to %d through idb_create_library\n", __crt_lib_id );
    }
    return (__crt_lib_id)?(void *)__crt_lib_id:0;
}


/*-----------------------------------------------------------------------
 * idb_find_lib_id
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:  __crt_lib_id
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 * In 4.4.x:	all you care is an entry is present in cds.lib
 * 		Same as idb_open_library
 * Redundant:   absolute_path
 *-----------------------------------------------------------------------*/



void *idb_find_lib_id(const char *name, int set_static_lib_id,
                             const char *mode)
{
    void *ddLibObj;
    
    if(set_static_lib_id) {
      __crt_lib_id = ddGetObj((char *)name, NULL, NULL, NULL, NULL, (char *)mode);
      if (debugDmsOn) {
         dmsPrintIdmsLog("*IDMS DEBUG* -- __crt_lib_id set to %d through idb_find_lib_id\n", __crt_lib_id );
      }
      return (__crt_lib_id)?(void *)__crt_lib_id:0;
    }
    else
      return ddGetObj((char *)name, NULL, NULL, NULL, NULL, (char *)mode);
}



/*-----------------------------------------------------------------------
 * idb_set_lib_id 
 * idb_get_lib_id
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars: 
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description: sets/gets the lib id.
 *              For Cadence, lib id is a ddId. We are using void *
 *              to stay general. 
 *-----------------------------------------------------------------------*/

void *idb_set_lib_id(void *plib_id)
{  
   if (!plib_id){
      __crt_lib_id = 0;
      return 0;
   }
   __crt_lib_id  = (ddId)plib_id;
    if (debugDmsOn) {
       dmsPrintIdmsLog("*IDMS DEBUG* -- __crt_lib_id set to %d through idb_set_lib_id\n", __crt_lib_id );
    }
   return (void *)__crt_lib_id;
}

/* returns 0 not address if id == 0 so it can be used in !idb_get_lib_id() ...*/

void *idb_get_lib_id()
{
  if(__crt_lib_id)
    return (void *)__crt_lib_id;
  else
    return 0;
}

/* to prevent set content at id to crash */


static ddId __empty_lib_id = 0;

void *idb_get_empty_lib_id() 
{
   __empty_lib_id = 0;
   return (void *)__empty_lib_id;
}


int idb_valid_lib_id(void *plib_id)
{
   ddId plid = (ddId)plib_id;
   if (!plid)
      return 0;
   return (ddGetObjType(plid)==ddLibType)?1:0;
}


const char *idb_get_library_name(void *plib_id)
{
   ddId plid = (ddId)plib_id;
   if (!plib_id)
      return 0;
   return (const char *)dbGetLibName(plid);  
}


static ddId __last_updated_id = 0;
static char __last_lib_name[BUFSIZE];

char *idb_get_last_library_name()
{
   if (!__crt_lib_id)
      return _bempty;
   if (__last_updated_id == __crt_lib_id)
      return __last_lib_name;
   else{
     __last_updated_id = __crt_lib_id;
     if (debugDmsOn){
        dmsPrintIdmsLog("*IDMS DEBUG* __crt_lib_id = %d\n", __crt_lib_id);
        char *c = __crt_lib_id;
        dmsPrintIdmsLog("*IDMS DEBUG* -- first few chars %c%c%c%c%c%c\n", c[0], c[1], c[2], c[3], c[4], c[5]);
     }
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
     return strcpy(__last_lib_name, dbGetLibName(__crt_lib_id));
#else
     return strcpy(__last_lib_name, (char *)__crt_lib_id);
#endif     
   } 
}


/* This becomes redundant in 4.4.x but it is still there for compatibility */
void **idb_set_content_at_id(void **plib_id, void *plib_id_content)
{
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
  ddId *plid, plid_content;
   if (!plib_id)
      return 0;
   plid = (ddId *)plib_id;
   if (!plib_id_content){
      *plid = 0;
   }
   else{
      plid_content = (ddId)plib_id_content;
      *plid = plid_content;
   }
#endif
   return plib_id;
}

/* plib can be a (char *)libName or a ddId */
const char *idb_get_library_directory(void *plib, char *mode) 
{
   ddId plid;

   if (!plib)
      return 0;

   /* if not a valid libId it must be a libName */
   if(ddGetObjType(plib) == ddLibType)  
     plid = (ddId)plib;
   else {
     plid = ddGetObj((char *)plib, NULL, NULL, NULL, NULL, mode);
     if(!plid) return 0;
   }

   if(mode && !strcmp(mode, "w"))
     return ddGetObjWritePath(plid);
   else 
     return ddGetObjReadPath(plid);
}


int idb_refresh_all_libraries(void) {

return (int)ddUpdateLibList();

}

static char **dssResults;
static char **dssObjList;
static char **dssTagOpts;
static char **dssPopulateOpts;
static char **dssVaultOpts;
static char _verBuff[SMALLBUF];
static char _newVerBuff[SMALLBUF];
static char _dssObjVaultPath[BUFSIZE];
static char _intelCVName[SMALLBUF];
static char _dssObjWsPath[BUFSIZE];
static char _FPN[2][BUFSIZE];
static char _libNames[SMALLBUF][BUFSIZE];
static char _libVaults[SMALLBUF][BUFSIZE];
/*-----------------------------------------------------------------------
 * Config related functions
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description:
 *-----------------------------------------------------------------------*/

int idb_remove_cellview_from_config(void *plib_id, char *conf_name,
                                    char *cName, char *vName)
{
   ddId plid;
   ddId cvId;
   char *ddObjPath;
   HRESULT dssRetHandle;

   if (!plib_id){
      idb_set_error_code(idb_INVALID_LIB);
      return 0;
   }
   plid  = (ddId)plib_id;

   ddObjPath = idb_get_collection_obj(plib_id, cName, vName, !GETVAULT);

   dssRetHandle = sapiMakeStrArray(&dssObjList, ddObjPath, NULL);

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
	sapiMakeStrArray(&dssTagOpts, DELETE, NULL) : dssRetHandle ;

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
   	sapiTag(NULL, dssObjList, conf_name , dssTagOpts, &dssResults) :
		dssRetHandle ;

   if(!SUCCEEDED(dssRetHandle)) { 
      idb_set_error_code(idb_CANT_REMOVE_CVCNF);
      idb_capture_dm_vendor_warning();
      return FALSE;
   }
   else
     return TRUE;
}


int idb_add_cellview_to_config(void *plib_id, char *conf_name, char *cName, 
				char *vName, char *verName, int bind)
{
   ddId plid;
   ddId cvId;
   char *ddObjPath;
   HRESULT dssRetHandle;

   if (!plib_id){
      idb_set_error_code(idb_INVALID_LIB);
      return 0;
   }
   plid  = (ddId)plib_id;

   ddObjPath = idb_get_collection_obj(plib_id, cName, vName, !GETVAULT);

   dssRetHandle = sapiMakeStrArray(&dssObjList, ddObjPath, NULL);

   if(verName)
     dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
	sapiMakeStrArray(&dssTagOpts, VERSION, verName, NULL) : dssRetHandle ;
   else
     dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
	sapiMakeStrArray(&dssTagOpts, VERSION, "", NULL) : dssRetHandle ;

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
   	sapiTag(NULL, dssObjList, conf_name , dssTagOpts, &dssResults) :
		dssRetHandle ;

   if(!SUCCEEDED(dssRetHandle)) { 
      idb_set_error_code(idb_CANT_ADD_CVTOCNF);
      idb_capture_dm_vendor_warning();
      return FALSE;
   }
   else
     return TRUE;
}


int idb_get_cellView_tags( void *plib_id, char *cName, char *vName, 
					char *verName, char *storeTags)
{
   ddId plid;
   ddId cvId;
   char *ddObjPath;
   HRESULT dssRetHandle;
   int i=0;

   if (!plib_id){
      idb_set_error_code(idb_INVALID_LIB);
      return 0;
   }
   plid  = (ddId)plib_id;

   ddObjPath = idb_get_collection_obj(plib_id, cName, vName, !GETVAULT);

   if(verName)
     dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
	sapiMakeStrArray(&dssTagOpts, VERSION, verName, NULL) : dssRetHandle ;
   else
     dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
	sapiMakeStrArray(&dssTagOpts, VERSION, "Latest", NULL) : dssRetHandle ;

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
   	sapiUrlTags(NULL, ddObjPath, dssTagOpts, &dssResults) :
		dssRetHandle ;

   if(!SUCCEEDED(dssRetHandle)) { 
      idb_set_error_code(idb_SYNC_ERROR);	
      idb_capture_dm_vendor_warning();
      return FALSE;
   }
   else if(storeTags) {
       while(dssResults && dssResults[i]) {
       strcat(storeTags, dssResults[i]);
       strcat(storeTags, " ");
       ++i;
       }
  }
   return TRUE;
}

int idb_get_lib_tags( void *plib_id, char **storeTags)
{
   const char *ddObjPath;
   HRESULT dssRetHandle;
   char *retValue, buff[BUFSIZE];
   int i=0;

   if (!plib_id){
      idb_set_error_code(idb_INVALID_LIB);
      return 0;
   }

   ddObjPath = (const char *)idb_get_library_directory(plib_id, (char *)"w");

   /* The standard 'report' function is actually an alias for the 'report_int'
    * procedure. "report status" is extremely efficient function. It uses of a
    * direct read of the tags database.
    * Drawback: This function will report tags for objects that are not
    * currently populated, either because they are not on the branch that
    * is being used, or because the user simply did not check them out.
    * Since 434 DM use model does not have vault, workspace concept,
    * Getting tags of a library means getting all tags of a library.
    * If user wants to only get tags of objects that are currently
    * populated with tags of all versions of the objects, we need
    * to request Sync for an enhancement to the report utils.
    */
   (void)sprintf(buff, "report_int status -nobranch -list -tagsUsed -vault %s", ddObjPath);

   dssRetHandle = sapiExecuteCmd(buff, &retValue);

   if(!SUCCEEDED(dssRetHandle)) { 
      idb_set_error_code(idb_SYNC_ERROR);	
      idb_capture_dm_vendor_warning();
      *storeTags = 0;
      return FALSE;
   }
   else {
       /* Don't need to process the return value.  Directly pass the
        * value back to caller routine and let the caller free the mmy.
        */
       *storeTags = retValue;
   }

   return TRUE;
}

/****************************************************************************
** If conf_name exists:
** then tag "instName" to all the Latest versions of objs which have 
** "conf_name" tag
** else
** tag "instName" to all "Latest" tags
****************************************************************************/
int idb_create_config_instance(void *plib_id, char *conf_name, char *instName)
{
   char *ddObjPath;
   HRESULT dssRetHandle;

   if (!plib_id){
      idb_set_error_code(idb_INVALID_LIB);
      return 0;
   }

   ddObjPath = ddGetObjReadPath(plib_id);

   dssRetHandle = sapiMakeStrArray(&dssObjList, ddObjPath, NULL);

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiMakeStrArray(&dssTagOpts, RECURSIVE, NULL) : dssRetHandle ;

   if(conf_name) {
     sprintf(_verBuff, "%s:%s", conf_name, "Latest");
     dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
       sapiAddStrings(dssTagOpts, VERSION, _verBuff, NULL) : dssRetHandle ;
   }

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
   	sapiTag(NULL, dssObjList, instName , dssTagOpts, &dssResults) :
		dssRetHandle ;

   if(!SUCCEEDED(dssRetHandle)) { 
      idb_set_error_code(idb_CANT_CRE_CNF);
      idb_capture_dm_vendor_warning();
      return FALSE;
   }
   else
     return TRUE;
}

/* REM: usage of the subConfigTag is temporary, Will get a better soln. later */
int idb_add_subconfig(void *plib_id, char *conf_name, char *subconfigName)
{
   char *ddObjPath, subConfigTag[BUFSIZE];
   HRESULT dssRetHandle;

   if (!plib_id){
      idb_set_error_code(idb_INVALID_LIB);
      return 0;
   }
   if(!conf_name || !subconfigName)
     return 0;

   ddObjPath = ddGetObjReadPath(plib_id);

   dssRetHandle = sapiMakeStrArray(&dssObjList, ddObjPath, NULL);

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiMakeStrArray(&dssTagOpts, RECURSIVE, NULL) : dssRetHandle ;

   /* tag all objects tagged with subconfigName tag with <subConfigTag> */
   sprintf(_verBuff, "%s", subconfigName);
   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
       sapiAddStrings(dssTagOpts, VERSION, _verBuff, NULL) : dssRetHandle ;

   sprintf(subConfigTag, "%s_SC_%s", conf_name, subconfigName);

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
   	sapiTag(NULL, dssObjList, subConfigTag , dssTagOpts, &dssResults) :
		dssRetHandle ;

   if(!SUCCEEDED(dssRetHandle)) { 
      idb_set_error_code(idb_CANT_CRE_CNF | idb_SYNC_ERROR);
      idb_capture_dm_vendor_warning();
      return FALSE;
   }
   else
     return TRUE;
}


int idb_insert_config(void *plib_id, char *confName, char *tocfgName)
{
   char *ddObjPath;
   HRESULT dssRetHandle;

   if (!plib_id){
      idb_set_error_code(idb_INVALID_LIB);
      return 0;
   }

   ddObjPath = ddGetObjReadPath(plib_id);

   dssRetHandle = sapiMakeStrArray(&dssObjList, ddObjPath, NULL);

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiMakeStrArray(&dssTagOpts, RECURSIVE, NULL) : dssRetHandle ;

   if(confName) {
     dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
       sapiAddStrings(dssTagOpts, VERSION, confName, NULL) : dssRetHandle ;
   }

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
   	sapiTag(NULL, dssObjList, tocfgName, dssTagOpts, &dssResults) :
		dssRetHandle ;

   if(!SUCCEEDED(dssRetHandle)) { 
      idb_set_error_code(idb_CANT_CRE_CNF);
      idb_capture_dm_vendor_warning();
      return FALSE;
   }
   else
     return TRUE;
}

/* get the vault of the current lib path
** set the toDir vault to the libpath vault
** then populate toDir
*/
int idb_export_config(void *plib_id, char *confName, char *toDir)
{
   char *ddObjPath;
   HRESULT dssRetHandle;

   if (!plib_id){
      idb_set_error_code(idb_INVALID_LIB);
      return 0;
   }

   ddObjPath = ddGetObjReadPath(plib_id);

   dssRetHandle = sapiMakeStrArray(&dssObjList, ddObjPath, NULL);

   dssRetHandle = sapiUrlVault(NULL, ddObjPath, NULL, &dssResults);

   if(SUCCEEDED(dssRetHandle) && dssResults[0] && strcmp("", dssResults[0])) {
     strcpy(_dssObjVaultPath, dssResults[0]);
   }
   else {
     idb_capture_dm_vendor_warning();
     return FALSE;
   }

   dssRetHandle = sapiMakeStrArray(&dssVaultOpts, NULL);

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
   			sapiSetvault(NULL, _dssObjVaultPath, toDir, 
			dssVaultOpts, &dssResults) : dssRetHandle;

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiMakeStrArray(&dssPopulateOpts, RECURSIVE, SHARE, NULL) : dssRetHandle ;

   if(confName) {
     dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
       sapiAddStrings(dssPopulateOpts, VERSION, confName, NULL) : dssRetHandle ;
   }

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
       sapiAddStrings(dssPopulateOpts, DIR, toDir, NULL) : dssRetHandle ;

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
   	sapiPopulate(NULL, dssPopulateOpts, &dssResults) : dssRetHandle ;

   if(!SUCCEEDED(dssRetHandle)) { 
      idb_set_error_code(idb_CANT_CRE_CNF);
      idb_capture_dm_vendor_warning();
      return FALSE;
   }
   else
     return TRUE;
}

int idb_delete_config(void *plib_id, char *conf_name)
{
   char *ddObjPath;
   HRESULT dssRetHandle;

   if (!plib_id){
      idb_set_error_code(idb_INVALID_LIB);
      return 0;
   }

   ddObjPath = ddGetObjReadPath(plib_id);

   dssRetHandle = sapiMakeStrArray(&dssObjList, ddObjPath, NULL);

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiMakeStrArray(&dssTagOpts, RECURSIVE, NULL) : dssRetHandle ;

   if(conf_name) {
     dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
       sapiAddStrings(dssTagOpts, DELETE, NULL) : dssRetHandle ;
   }

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
   	sapiTag(NULL, dssObjList, conf_name , dssTagOpts, &dssResults) :
		dssRetHandle ;

   if(!SUCCEEDED(dssRetHandle)) { 
      idb_set_error_code(idb_CANT_DEL_CNF);
      idb_capture_dm_vendor_warning();
      return FALSE;
   }
   else
     return TRUE;
}


/* Comparing differences between two configuration.
 * With Sync's report utility, comparing two configuration can
 * only has one workspace path. Without given configuration,
 * two workspace path may be given.
 * I am assuming we are comparing two configuration of the same library.
 * Any other comparison can be future enhencement.
 * whagi
 */
int idb_diff_config(void *libID1, void *libID2, char *confName1, char *confName2,
                    char *oFile)
{
    char *libPath1, *libPath2;
    HRESULT dssRetHandle;
    char *retValue, buff[BUFSIZE];

    if (!libID1){
       idb_set_error_code(idb_INVALID_LIB);
       return(FALSE);
    }
    libPath1 = ddGetObjReadPath(libID1);
    (void)sprintf(buff, "report_int compare -list -config %s -config %s %s -output %s",
                  confName1, confName2, libPath1, oFile);

    dssRetHandle = sapiExecuteCmd(buff, &retValue);

   if(!SUCCEEDED(dssRetHandle)) {
      idb_set_error_code(idb_SYNC_ERROR);
      idb_capture_dm_vendor_warning();
      return(FALSE);
   }

   return(TRUE);
}

/*-----------------------------------------------------------------------
 * Cell View Related 
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars: 
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description: Creates CV under lib/cell/view/. For registered CVs, creates
 *		the master.tag file with the data file. For unregistered CVs, 
 *		CV is cell.view.
 *		While installing unix files, the FPN has the src path.
 *-----------------------------------------------------------------------*/

static ddId __crt_cellview_id = 0;

void *idb_create_cellview(void *plib_id, char *cName, char *vName, 
         char *viewType, char *FPN)
{
   char *libName, *masterTagPath, *dataRegFile; 
   ddId plibid;
   ddId masterTagId;
   int cvExists,cvRetired;
   char *tmp;

   if (!sync_on){ /* no synchronicity */
     idb_set_error_code(idb_SYNC_ERROR);
     __crt_cellview_id = NULL;
     strcpy(FPN, "");
     return 0;
   }
   
   if (!plib_id){
      idb_set_error_code(idb_INVALID_LIB);
      return 0;
   }
   plibid = (ddId)plib_id;
   if (!(libName = dbGetLibName(plibid))){
      idb_set_error_code(idb_INVALID_LIB);
      return 0;
   }

   if (debugDmsOn){
      dmsPrintIdmsLog("*IDMS DEBUG* - inside idb_create_cellview\n");
   }


#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
   dataRegFile = idb_get_dataReg_file(viewType, vName);
#else
   tmp = idb_get_libtype(plib_id);
   if(tmp && !strcmp(tmp, "cadenceLib"))
     dataRegFile = idb_get_dataReg_file(viewType, vName);
   else
     dataRegFile = 0;
#endif

   if (debugDmsOn){
      dmsPrintIdmsLog("*IDMS DEBUG* - dataRegFile = %s\n", dataRegFile);
   }


   cvExists = idb_url_exists(plibid, cName, vName, NULL);
   cvRetired = idb_url_retired(plib_id, cName, vName);
   /* create a cellview if it does not exist OR it was previously retired */
   if (!cvExists || cvRetired) {
     if(!dataRegFile) { /* not a collection obj */

        sprintf(_intelCVName, "%s.%s", cName, vName) ;

	__crt_cellview_id = 
		ddGetObj(libName, cName, vName, _intelCVName, NULL, "w");
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
	if(__crt_cellview_id) {
	
	  /** REM: remove the master.tag file otherwise dss gets confused **/
	  masterTagId = ddGetObj(libName, cName, vName, "master.tag", NULL, "w");
	  if(masterTagId) {
	    masterTagPath = ddGetObjWritePath(masterTagId);
	    if(osRemoveFile(masterTagPath) < 0) {
	       printf("ERROR: in idb_create_cellview: cannot remove %s\n", 
								masterTagPath);
	       idb_set_error_code(idb_CANT_CRE_CV);
	       __crt_cellview_id = NULL;
	       strcpy(FPN, "");
	       return 0;
	    }

	  }	/* if masterTagId */

	}	/* if __crt_cellview_id */
#endif
      }		/* if !dataRegFile	*/
     else {  /* collection obj */
       /* ddGetObj creates the cellview */
         if(ddGetObj(libName, cName, vName, dataRegFile, NULL, "w")) {
	   __crt_cellview_id = 
	     	ddGetObj(libName, cName, vName, dataRegFile, NULL, "w");
	 }
	 else {
	   __crt_cellview_id = NULL;
	 }
     }

     if(__crt_cellview_id ) {
       strcpy(FPN, ddGetObjWritePath(__crt_cellview_id));
     }
     else {
       idb_capture_cdn_vendor_warning();
       idb_set_error_code(idb_CANT_CRE_CV); 
       strcpy(FPN, "");
     }
     return (__crt_cellview_id)?(void *)__crt_cellview_id:0;
   }
   else {
     idb_set_error_code(idb_CANT_CRE_CV | idb_CV_EXISTS);
     __crt_cellview_id = 0;
     strcpy(FPN, "");
     return 0;
   }
}


void *idb_set_cellview_id(void *pcv_id)
{
   ddId pcid;
   if (!pcv_id){
      __crt_cellview_id = 0;
      return 0;
   }
   pcid = (ddId)pcv_id;
     __crt_cellview_id = pcid;
   return (void *)__crt_cellview_id;
}


/* returns 0 not address if id == 0 so it can be used in !idb_getcellview_id()
   ...*/

void *idb_get_cellview_id()
{
   return (__crt_cellview_id)?(void *)__crt_cellview_id:0;
}


/** For 4.4.x: We don't know if the cellview is collection obj or not **/
/** so use similar method to check collection obj **/
void *idb_find_cellview(void *plib_id, char *cName, char *vName) 
{
ddId plibid;
ddId ddObject;

if (!plib_id)
  return NULL;
plibid = (ddId)plib_id;

if(!(ddObject = ddGetObj(NULL, cName, vName, NULL, plibid, "r")))
  return NULL;

/* #ifndef OH_MACH_LINUX */
if(!ddGetObj(NULL, cName, vName, "master.tag", plibid, "r")) {
  sprintf(_intelCVName, "%s.%s", cName, vName);
  ddObject = ddGetObj(NULL, cName, vName, _intelCVName, plibid, "r");
}
else   
  ddObject = ddGetObj(NULL, cName, vName, NULL, plibid, "r");
/* #else
  ddObject = ddGetObj(NULL, cName, vName, NULL, plibid, "r");
  #endif*/ 
  __crt_cellview_id = ddObject;

return (__crt_cellview_id)?(void *)__crt_cellview_id:0;
}


/************************************************************ 
 Delete a whole cell in both Synchronicity and Cadence
************************************************************/

static char **dssDeleteCellOpts;

HRESULT idb_delete_cell(void *plib_id, char *cName, char *vName, char *ver,
               		char *FPN, char *locker, char **new_ver, int CDBA) 
{
  
   ddId ddLibId;
   HRESULT dssRetHandle;
   char *dssObjName, *dssFldName;
   
   ddLibId = (ddId)plib_id;
   strcpy(FPN, "");
   if (!sync_on) {
     idb_set_error_code(idb_SYNC_ERROR);
     return E_FAIL;
   }
   
   if (!plib_id || !ddIsId(ddLibId)) {
     idb_set_error_code(idb_INVALID_LIB | idb_CANT_CO);
     return E_FAIL;
   }

   dssFldName = ddGetObjReadPath(plib_id);   
   if(!dssFldName ) {
     idb_capture_cdn_vendor_warning();
     idb_set_error_code(idb_CANT_FIND_CV | idb_CANT_CO);
     return E_FAIL;
   }

   dssRetHandle = sapiUrlVault(NULL, dssFldName, NULL, &dssResults);
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }
   dssObjName = dssResults[0];

   strcat(dssObjName, "/");
   strcat(dssObjName, cName);
   strcat(dssObjName, ";");

   dssRetHandle = sapiMakeStrArray(&dssObjList, dssObjName, NULL); 
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiMakeStrArray(&dssDeleteCellOpts, RECURSIVE,  NULL) : dssRetHandle ;
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiRmfolder(NULL, dssObjList, dssDeleteCellOpts, &dssResults) : dssRetHandle;
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }

   strcat(dssFldName, "/");
   strcat(dssFldName, cName);

   dssRetHandle = sapiMakeStrArray(&dssObjList, dssFldName, NULL);
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiMakeStrArray(&dssDeleteCellOpts, RECURSIVE, NULL) : dssRetHandle ;
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }
   
   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiRmfolder(NULL, dssObjList, dssDeleteCellOpts, &dssResults) : dssRetHandle;
   if (FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }

   strcpy(FPN, dssObjList[0]);
   
   return dssRetHandle;
}


/************************************************************ 
 Helper function of Delete Cell - check if cell is in lib
************************************************************/

void *idb_check_cell_in_lib(char *libName, char *cellName)
{
  void *plib_id;
  
  if(!libName)
    return NULL;

  if(! (plib_id = ddGetObj((char *)libName, (char *)cellName, NULL, NULL, NULL, "r")))
    return 0;
  
  if (!(plib_id = idb_open_library(libName, 
				   idb_get_library_directory(plib_id, "w"), "w"))){
    idb_capture_dm_vendor_warning();
    dmsReportMsgIfAny((char *)dmsCGetLastMessage(), "");
    return 0;
  }
  
  return plib_id;
}

/************************************************************ 
 Helper function of Delete Cell - generate a list of views in cell
************************************************************/

int idb_gen_cellview_list(char *libName, char *cellName,char **cellviewList)
{
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
  ddObjRelState* pState1;
  ddId viewId;
  ddId libID;
  ddId cellId;
  char * tmpcellviewList=0;
  int size=0;
 
  if(!libName)
    return 0;
  
  libID = ddGetObj((char *)libName,NULL,NULL,NULL,NULL,"r");
  
  if (!libID) {
    return 0;
  }

  cellId=ddGetObj(libName,cellName,NULL,NULL,NULL,"r");
  pState1=ddStartGenObjRel(cellId,ddObjChildRelType);
  while (viewId=ddGenObjRel(pState1)) {
    size+=strlen(ddGetObjName(viewId))+2;
  }

  ddStopGen(pState1);
  tmpcellviewList=(char*)malloc(size+2);
  *tmpcellviewList=0;
  pState1=ddStartGenObjRel(cellId,ddObjChildRelType);
  while (viewId=ddGenObjRel(pState1)) {
    strcat(tmpcellviewList,ddGetObjName(viewId));
    strcat(tmpcellviewList," ");
  }
  ddStopGen(pState1);
  
  *cellviewList=tmpcellviewList;
  return 1;
#else
  return 0;
#endif 
}

/************************************************************ 
 Delete a single cell view in both Synchronicity and Cadence
************************************************************/

static char **dssDeleteOpts;

HRESULT idb_delete_cellview(void *plib_id, char *cName, char *vName, char *ver,
               		char *FPN, char *locker, char **new_ver, int CDBA) 
{
   if (debugDmsOn){
       dmsPrintIdmsLog("*IDMS DEBUG* - entering idb_delete_cellview\n");
   }
   ddId ddLibId;
   HRESULT dssRetHandle;
   char *dssObjName, *datafile, *dssFldName;
   char* sync_msg=0;
 
   ddLibId = (ddId)plib_id;
   strcpy(FPN, "");
   if (!sync_on) {
     idb_set_error_code(idb_SYNC_ERROR);
     return E_FAIL;
   }
   
   if (!plib_id || !ddIsId(ddLibId)) {
      idb_set_error_code(idb_INVALID_LIB | idb_CANT_CO);
      return E_FAIL;
   }
  
   if (debugDmsOn){
       dmsPrintIdmsLog("*IDMS DEBUG* - idb_delete_cellview check point ** 1 **\n");
   } 

 
   dssObjName = idb_get_collection_obj(plib_id, cName, vName, GETVAULT);
   if(!dssObjName ) {
     idb_capture_cdn_vendor_warning();
     idb_set_error_code(idb_CANT_FIND_CV | idb_CANT_CO);
     return E_FAIL;
   }


   if (debugDmsOn){
       dmsPrintIdmsLog("*IDMS DEBUG* - idb_delete_cellview check point ** 2 **\n");
   }

   dssRetHandle = sapiMakeStrArray(&dssObjList, dssObjName, NULL); 
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }
/*
   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiMakeStrArray(&dssDeleteOpts, NOKEEPVID,  NULL) : dssRetHandle ;
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }
*/

   if (debugDmsOn){
       dmsPrintIdmsLog("*IDMS DEBUG* - idb_delete_cellview check point ** 3 **\n");
   }


   idb_record_dm_vendor_string();
   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiRetire(NULL, dssObjList, dssDeleteOpts, &dssResults) : dssRetHandle;
   sync_msg=idb_get_record_dm_vendor_string(1);
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }

   if (debugDmsOn){
       dmsPrintIdmsLog("*IDMS DEBUG* - idb_delete_cellview check point ** 4 **\n");
   }
    
   dssFldName = idb_get_collection_obj(plib_id, cName, vName, !GETVAULT);
   if(!dssFldName) {
     idb_capture_cdn_vendor_warning();
     idb_set_error_code(idb_CANT_FIND_CV | idb_CANT_CO);
     return E_FAIL;
   }

   if (debugDmsOn){
       dmsPrintIdmsLog("*IDMS DEBUG* - idb_delete_cellview check point ** 5 **\n");
   }
   
   dssRetHandle = sapiMakeStrArray(&dssObjList, dssFldName, NULL);
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }

   if (debugDmsOn){
       dmsPrintIdmsLog("*IDMS DEBUG* - idb_delete_cellview check point ** 6 **\n");
   }

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiMakeStrArray(&dssDeleteOpts, NULL) : dssRetHandle ;
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }

   if (debugDmsOn){
       dmsPrintIdmsLog("*IDMS DEBUG* - idb_delete_cellview check point ** 7 **\n");
   }
   
   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiRmfile(NULL, dssObjList, dssDeleteOpts, &dssResults) : dssRetHandle;
   if (FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }

   if (debugDmsOn){
       dmsPrintIdmsLog("*IDMS DEBUG* - idb_delete_cellview check point ** 8 **\n");
   }

   strcpy(FPN, _FPN[GETWS]);		

   return dssRetHandle;
}




/************************************************************ 
 Delete a single view version in both Synchronicity and Cadence
************************************************************/

static char **dssDeleteVersionOpts;

HRESULT idb_delete_version(void *plib_id, char *cName, char *vName, char *ver,
               		char *FPN, char *locker, char **new_ver, int CDBA) 
{
   ddId ddLibId;
   HRESULT dssRetHandle;
   char *dssObjName;
   int i =1;
   
   ddLibId = (ddId)plib_id;
   strcpy(FPN, "");
   if (!sync_on) {
     idb_set_error_code(idb_SYNC_ERROR);
     return E_FAIL;
   }
   if (!plib_id || !ddIsId(ddLibId)) {
     idb_set_error_code(idb_INVALID_LIB);
     return E_FAIL;
   }
   
   dssObjName = idb_get_collection_obj(plib_id, cName, vName, GETVAULT);
   if(!dssObjName ) {
     idb_capture_cdn_vendor_warning();
     idb_set_error_code(idb_CANT_FIND_CV);
     return E_FAIL;
   }
   strcat(dssObjName, ver);

   dssRetHandle = sapiMakeStrArray(&dssObjList, dssObjName, NULL); 
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_SYNC_ERROR);
     return dssRetHandle;
   }

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiMakeStrArray(&dssDeleteVersionOpts, FORCE,  NULL) : dssRetHandle ;
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_SYNC_ERROR); 
     return dssRetHandle;
   }

   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiRmversion(NULL, dssObjList, dssDeleteVersionOpts, &dssResults) : dssRetHandle;
   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_SYNC_ERROR);
     dmsCSetLastMessage("Operation failed - Make sure not to delete initial version (1.1)");
     return dssRetHandle;
   }

   strcpy(FPN, _FPN[GETWS]);		
   
   return dssRetHandle;
}




/** This is really a idb_cellview_exists function **/
/** If ver is specified, and is not there in the WS, returns NULL **/
/***************************************************************************
void *idb_open_cellview(void *plib_id, char *cName, char *vName,
                               char *ver, char *mode, int cdba, char **rPath)
{
   ddId plibid;
   ddId ddTmpObject = NULL;
   HRESULT hresult;

   if (!plib_id)
    return 0;
   plibid = (ddId)plib_id;
   if (!plibid)
    return 0;

   sprintf(_intelCVName, "%s.%s", cName, vName);
   ** assume it is a cadence cv **
   __crt_cellview_id = ddGetObj(NULL, cName, vName, NULL, plibid, "r");

   if(cdba == 0) 
     __crt_cellview_id = ddGetObj(NULL, cName, vName, _intelCVName, plibid, "r");
   else if(cdba != 1) { ** eg. cdba==2 for dmsInfo **
     ddTmpObject = ddGetObj(NULL, cName, vName, _intelCVName, plibid, "r");
     if(ddTmpObject) __crt_cellview_id = ddTmpObject;
   }


   if (__crt_cellview_id) {
      *rPath = ddGetObjReadPath(__crt_cellview_id);
      ** check verson only if necessary**
      if(ver && strcmp(ver, "")) {
	hresult = sapiUrlVersionid(NULL, *rPath, NULL, &dssResults);
	if(SUCCEEDED(hresult) && dssResults[0] && strcmp(dssResults[0], "")) {
	  if(!strcmp(ver, dssResults[0])) {
	    *rPath = NULL;
	    __crt_cellview_id = NULL;
	  }
	}
      }
   }
   return (__crt_cellview_id)?(void *)&__crt_cellview_id:0;
}
***************************************************************************/


int idb_close_cellview(void *plib_id, char *cName, char *vName, 
                         char *ver)
{
   ddId plibid;

   if (!plib_id)
      return 0;
   plibid = (ddId)plib_id;
   __crt_cellview_id = 0;

  return 1;
}

/*-----------------------------------------------------------------------
 * Version Related
 *
 * idb_get_default_version
 * ............................
 * idb_find_last_version
 * idb_find_last_locked_version
 * idb_find_version
 * idb_find_version_and_locker
 * idb_get_all_versions
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description:
 *   These functions are put here to find and return the version string for
 *   the last version or specified (ver = "") means last for functions which
 *   have a version parameter.
 *
 *   If a locker is not found, the string contained by macro NOBODY is returned
 *   for backward compatibility.
 *   If a locker is found, the name is returned and also the path where it
 *   was checkout.
 *   (That is if locker != 0 and path != 0).
 *
 *   User responsible for allocating space for locker and path in order to
 *   avoid an extra level of indirection.
 *
 *   Recommended use of BUFSIZE buffers. This will be fixed in the future
 *   by adding the possibility of internal buffers but for now I am pressed
 *   by time -- Sorin 08/22/97
 *
 *   For 4.4.x:
 *   latest version is stored in pverstr 
 *   if ver!=0 && ver<latest then path=pathof(ver)
 *-----------------------------------------------------------------------*/

char *idb_find_version_and_locker(void *plib_id, char *cName, char *vName,
                                  char *ver, char *locker, char *path,
                                  int get_last_if_not_locked, char **pverstr)
{
char *dssFPNPath[2], *cachePath;
HRESULT hresult;
int locked = FALSE;
int getLast = FALSE;
int isCadenceLib = FALSE;
char collectionPath [BUFSIZE];
char cacheFPN [BUFSIZE];
char *dest=NULL;
 int length = 0;
 int record_msg=0;
 
 int f=geteuid(),f1=getuid();

if (!plib_id )
  return NULL;

/* dssVaultObj = dssWsObj = NULL; */
_verBuff[0] = _newVerBuff[0] = '\0';

if (!ver || ver[0] == '\0')
  getLast = TRUE;

if (locker)
  strcpy(locker, NOBODY);

if (debugDmsOn){
     dmsPrintIdmsLog("<<*C-CALL*>> idb_find_version_and_locker(%d, %s, %s, %s, %s, %s, %d, %p) - getLast =%d\n",
                      plib_id, cName, vName, ver, locker, path, get_last_if_not_locked, pverstr, getLast);
}

int call_api = sync_on;

if( path && (path[0] != '\0') ) { 

/* see if library is managed - if not turn off sync_on */

  if (call_api && dmsFindUnmanaged) {
     static char _contents_path[BUFSIZE];
     static char _directory[BUFSIZE];
     static char _oneUp[BUFSIZE];
     strcpy(_directory, path);
     char *end = strrchr(_directory, '/');
     if (end) {
        *end = '\0'; 
     }
     snprintf(_contents_path, BUFSIZE, "%s/.SYNC/Contents", _directory);
     FILE *fc; 
     int found = 0;
     if (! (fc = fopen(_contents_path, "r")) ){ /* go one dir up to see if Cadence collection like layout.oa */
        /* see if cadence collection */
        strcpy(_oneUp, _directory);
        char *end_of_str = _oneUp + strlen(_oneUp) - 1;
        while ((end_of_str >= _oneUp) && (*end_of_str == '/')) {
           end_of_str --;
        } 
        while ((end_of_str >= _oneUp) && (*end_of_str != '/')) {
           end_of_str --;
        }   
        if ((end_of_str >= _oneUp) && (*end_of_str == '/')) { 
           *end_of_str = '\0';
           strcat(_oneUp, "/.SYNC/Contents");
           if (fc = fopen(_oneUp, "r")) {
              found = 1;
              fclose(fc);
           }
        } 

     }
     else {
        found = 1;
        fclose(fc);
     }

    
     if (found) {
        if (debugDmsOn){
           dmsPrintIdmsLog("directory %s is managed\n", _directory);
        } 
     } 
     else {
        call_api = 0;
        if (debugDmsOn){
           dmsPrintIdmsLog("directory %s unmanaged - not calling api\n", _directory);
        } 
     }
  }

  if(!call_api) { /* Synchronicity not on, return path only */
    if(locker && idb_get_locker(plib_id, cName, vName, path, 1)){
      char*po=strchr(outstring,':');
      strcpy(locker, po?po+1:outstring);
    }

    //strcpy(_verBuff, path);
    if(pverstr)
      *pverstr = _verBuff;
    return path;
  }
  
} else { /* path is NULL or empty */
 return NULL;

}

if(!strcmp(idb_get_libtype(plib_id), "cadenceLib")) {
  isCadenceLib=TRUE;
  collectionPath[0] = '\0';
  dest = strrchr(path, '/');
  length = dest - path;
  if((dest != NULL)&&(length!=0)) {
    strncpy(collectionPath, path, length);
    collectionPath[length] = '\0';
    sprintf(collectionPath, "%s.sync.cds", collectionPath);
  } else {
    strcpy(collectionPath, path);
  }
} 

/**************************************************************
get the latest versionId 
Note: if the file is checked out, dssResults is:
dssResults[0] : oldVer 
dssResults[1] : ->
dssResults[2] : newVer
Using  dssResults[0] -sh 3/15/00
****************************************************************/

/**  Check to see if cell is lockekd or checked out.  
     This request checks Cellname/.SYNC/Contents file so
     it does not do server query -> saves time. 
     Returns "L" if cell is locked and checked out, else it's "F".
**/


/* This is the "Caddilac of the memory leaks" - lets try to fix it - sorin 11/21/2008 */

if (debugDmsOn){
     dmsPrintIdmsLog("<<* == AGAIN == *>> idb_find_version_and_locker(%d, %s, %s, %s, %s, %s, %d, %d) - getLast =%d\n",
                      plib_id, cName, vName, ver, locker, path, get_last_if_not_locked, pverstr, getLast);
}

if (dmsAppGetEnv("INST_LOCK_CHECK")) {
	if (isCadenceLib) {
  		hresult = sapiUrlGetprop(NULL, collectionPath, "State",NULL, &dssResults);
	} else {
  		hresult = sapiUrlGetprop(NULL, path, "State",NULL, &dssResults);
	}
 
        if(debugDmsOn) {
          dmsPrintIdmsLog("dssResults: %s, %s\n", dssResults[0], dssResults[1]); 
        }
 
	if(SUCCEEDED(hresult) && dssResults[0] && !strcmp("L", dssResults[0])) {
  		locked = TRUE;
	} else {
  		locked = FALSE;
	}

} else {
	if (isCadenceLib) {
    		hresult = sapiUrlGetprop(NULL, collectionPath, "locked",NULL, &dssResults);
  	} else {
    		hresult = sapiUrlGetprop(NULL, path, "locked",NULL, &dssResults);
  	}

	if(SUCCEEDED(hresult) && dssResults[0] && strcmp("0", dssResults[0])) { /* assuming locked if succeeded */
  		locked = TRUE;
                // ylee88 - we do not need to rerun this command just to get the locker information later on
                if (locker) {
                  strcpy(locker, dssResults[0]);
                }
	} else {
  		locked = FALSE;
	}
}

if(pverstr) {

  if (debugDmsOn) {
    dmsPrintIdmsLog("*DEBUG* - Entering pverstr check\n");
  }
  if (!locked){
    if (isCadenceLib) {
      hresult = sapiUrlGetprop(NULL, collectionPath, "Version",NULL, &dssResults);
    } else {
      hresult = sapiUrlGetprop(NULL, path, "Version",NULL, &dssResults);
    }

  }
  if(locked || !SUCCEEDED(hresult) ||  !strcmp(dssResults[0], "")) {
    if (isCadenceLib) {
      hresult = sapiUrlVersionid(NULL, collectionPath, NULL, &dssResults);
    } else {
      hresult = sapiUrlVersionid(NULL, path, NULL, &dssResults);
    }
  } 
  if(SUCCEEDED(hresult) && dssResults[0] && strcmp(dssResults[0], "")) {
    strcpy(_verBuff, dssResults[0]);
    if(getLast)
      *pverstr = _verBuff;
    else 
      *pverstr = ver;
    if(dssResults[1] && !strcmp(dssResults[1], "->") && dssResults[2]) {
      strcpy(_newVerBuff, dssResults[2]);
      locked = TRUE;
    }
  }
}

if(!pverstr && locker) { /** get the state since we have no clue yet*/
  locked = TRUE;
}

/*This is only required if $INST_LOCK_CHECK is used if locker is being requested*/

if(locker && locked && dmsAppGetEnv("INST_LOCK_CHECK")) {
  /** get owner **/
  if (isCadenceLib) {
    hresult = sapiUrlGetprop(NULL, collectionPath, "locked",NULL, &dssResults);
  } else {
    hresult = sapiUrlGetprop(NULL, path, "locked",NULL, &dssResults);
  }
  if(SUCCEEDED(hresult) && dssResults[0] && strcmp("0", dssResults[0]))
    strcpy(locker, dssResults[0]);
}

/* this does not look righ 
if(!getLast && pverstr && ver && !strcmp(_verBuff,"")&& strcmp(ver, _verBuff)) {
*/

if(!getLast && pverstr && (strcmp(ver, _verBuff) != 0)){
  if (isCadenceLib) {
    sprintf(_FPN[GETVAULT], "%s;%s", collectionPath, ver);
  } else {
    sprintf(_FPN[GETVAULT], "%s;%s", path, ver);
  }
  dssFPNPath[0] = _FPN[GETVAULT];
  dssFPNPath[1] = NULL;
  
  if(!oldstdoud){
    record_msg=1;
    idb_record_dm_vendor_string();
  }
  if (debugDmsOn){
     dmsPrintIdmsLog("*IDMS DEBUG* - calling sapiUrlEnsurepathname with dssFPN[0] = %s\n", dssFPNPath[0]);
  }
  if(SUCCEEDED(sapiUrlEnsurepathname(NULL, dssFPNPath, NULL, &dssResults))) {
    if (debugDmsOn){
       dmsPrintIdmsLog("*IDMS DEBUG* -  dssResults[0] = %s\n",  dssResults[0]);
    }
    strcpy(path, dssResults[0]);  /* now strip the cache path */
    cachePath = idb_strip_nth_token(path, "} {", 2);
    if (isCadenceLib) {
      sprintf(cacheFPN, "%s/%s/%s", cachePath, vName, idmGetPattern(vName,0));
    } 
    if(cachePath == NULL)
      strcpy(path, "");
    else {
      if (isCadenceLib) {
	strcpy(path, cacheFPN);
      } else { 
        strcpy(path, cachePath);  
      }
    }
    check_read_only(path,"ensure",2);
  }
  else
    strcpy(path, "");
  if (record_msg){
    idb_get_record_dm_vendor_string(1);
  }
  
}
 
 
 if(path) {
   
   
  if(path[0] == '\0')
    return NULL;
  else
    return path;
}

return NULL;
}


/* first try workspace lib. If doesn't exist go to vault */
/* REM: ver is now redundant, can be used later */
int idb_url_exists(void *plib_id, char *cName, char *vName, char *ver) {
ddId ddObj = NULL;
char *ddLibPath = NULL;
HRESULT hresult;

if(!plib_id || !ddIsId(plib_id)) 
 return FALSE;

/* exists in workspace ? */
if(idb_find_cellview(plib_id, cName, vName))
  return TRUE;

ddLibPath = ddGetObjReadPath(plib_id);

if(!ddLibPath)  
  return FALSE;

if (!sync_on) 
  return FALSE;
/* get vault path of the lib */
hresult = sapiUrlVault(NULL, ddLibPath, NULL, &dssResults);
if(FAILED(hresult) || (dssResults[0] && !strcmp("", dssResults[0])))
  return FALSE;

strcpy(_dssObjVaultPath, dssResults[0]);

sprintf(_dssObjVaultPath, "%s/%s/%s/%s;", _dssObjVaultPath,cName, vName, _intelCVName); 
hresult = sapiUrlExists(NULL, _dssObjVaultPath, NULL, &dssResults);

if(SUCCEEDED(hresult) && (dssResults[0] && !strcmp(dssResults[0], "1")))
  return TRUE;

sprintf(_dssObjVaultPath, "%s/%s/%s.sync.cds;", _dssObjVaultPath, 
				  cName, vName);

hresult = sapiUrlExists(NULL, _dssObjVaultPath, NULL, &dssResults);
if(SUCCEEDED(hresult) && (dssResults[0] && !strcmp(dssResults[0], "1")))
  return TRUE;

return FALSE;
}


/* is this cell a retired cell? (check in the vault.) 
   return 0: if not a retired cell.
          1: if it is a retired cell.
*/
int idb_url_retired(void *plib_id, char *cName, char *vName) {
  char *ddLibPath = NULL;
  HRESULT hresult;
  char libVaultPath[BUFSIZE];
 
  if((!sync_on) || !plib_id || !ddIsId(plib_id)) 
    return FALSE;

  ddLibPath =  ddGetObj(plib_id,cName,vName,NULL,NULL,"r");
  if(!ddLibPath)  
    return FALSE;

  /* get vault path of the lib */
  hresult = sapiUrlVault(NULL, ddLibPath, NULL, &dssResults);
  if(FAILED(hresult) || (dssResults[0] && !strcmp("", dssResults[0])))
    return FALSE;
  strcpy(libVaultPath, dssResults[0]);

  hresult = sapiUrlRetired(NULL,libVaultPath, NULL, &dssResults);
  if(SUCCEEDED(hresult) && (dssResults[0] && !strcmp(dssResults[0], "1"))) {
    return TRUE;
  }
 
  return FALSE;
}


int idb_get_all_versions(void *plibid, char *cName, char *vName, 
				char *storeVers) {

char *dssObjPath, *ver;
int i=0, cdba=TRUE;

if(!(dssObjPath = idb_get_collection_obj(plibid, cName, vName, !GETVAULT)))
  return FALSE;
/* #ifndef OH_MACH_LINUX */
if (!sync_on) 
  return FALSE;
 
if(SUCCEEDED(sapiUrlVersions(NULL, dssObjPath, NULL, &dssResults))) {
  if(storeVers) {
    strcpy(storeVers,""); /* clear buffer */
    while(dssResults && dssResults[i]) {
      ver = idb_strip_last_token(dssResults[i], ";");  
      if(ver) {
      if(i==0) 
	strcpy(storeVers, ver);
      else
        strcat(storeVers, ver);
      strcat(storeVers, " ");
      }
      ++i;
    }
  }
  return TRUE;
}
else
  return FALSE;
/* #else 
 if(storeVers){
   strcpy(storeVers, "1.1");
 }
 return TRUE;
#endif */
}


/* TEST: If master.tag exists then it is a collection object.
** If not a collection obj returns l/c/v/c.v 
** Also stores the path to the local data file in _FPN
*/
char *idb_get_collection_obj(void *plibid, char *cName, char *vName, int getvault)
{
ddId ddObject;
char *ddObjPath, *dataRegFile, *libVaultPath, buffer[SMALLBUF];
int dataReg=TRUE;
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
char *ptr=0;
char *libraryPath = idb_get_libpath((char*)plibid);
#endif
 
strcpy(_FPN[GETWS], "");
strcpy(_FPN[GETVAULT], "");
strcpy(_dssObjWsPath, "");
strcpy(_dssObjVaultPath, "");

if(getvault)
  libVaultPath = idb_get_vault(plibid);
else
  libVaultPath = NULL;
 
if(!(ddObject = ddGetObj(NULL, cName, vName, NULL, plibid, "r")))
  return NULL;

if(!ddGetObj(NULL, cName, vName, "master.tag", plibid, "r")) {
  sprintf(_intelCVName, "%s.%s", cName, vName);
  ddObject = ddGetObj(NULL, cName, vName, _intelCVName, plibid, "r");
  dataReg=FALSE;
}
 
if(dataReg) {  /* get ddObj of l/c for l/c/v.sync.cds */
  ddObject = ddGetObj(NULL, cName, NULL, NULL, plibid, "r");
  if(!(dataRegFile = idb_get_dataReg_file(NULL, vName)))
    dataRegFile = "master.tag";
}

if(!(ddObjPath=ddGetObjReadPath(ddObject)))  /* ws path */
  return NULL;

if(!dataReg) {
  if(getvault) {
    strcpy(_FPN[GETWS], ddObjPath);	/* ws/l/c/v/c.v */
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
    if(!libraryPath)
      sprintf(_FPN[GETVAULT], "%s/%s/%s/%s;", libVaultPath, cName, vName, _intelCVName);
    else { /* lib has diff structure for linux. */
      if(ptr = strstr(ddObjPath, libraryPath)) {  
        ptr=ptr+strlen(libraryPath)+1;
        sprintf(_FPN[GETVAULT], "%s/%s;", libVaultPath, ptr);
      } else  /* ws/l/c/v/c.v */
        sprintf(_FPN[GETVAULT], "%s/%s/%s/%s;", libVaultPath, cName, vName, _intelCVName); 
    }
#else      
    sprintf(_FPN[GETVAULT], "%s/%s/%s/%s;", libVaultPath, cName, vName, _intelCVName);
#endif    
    strcpy(_dssObjWsPath, ddObjPath);	/* ws/l/c/v/c.v */
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
    if(!ptr)
      sprintf(_dssObjVaultPath, "%s/%s/%s/%s;", libVaultPath, cName, vName, _intelCVName);
    else
      sprintf(_dssObjVaultPath, "%s/%s;", libVaultPath, ptr);
#else      
    sprintf(_dssObjVaultPath, "%s/%s/%s/%s;", libVaultPath, cName, vName, _intelCVName);
#endif    
  }
  else {
    strcpy(_FPN[GETWS], ddObjPath);	/* ws/l/c/v/c.v */
    strcpy(_dssObjWsPath, _FPN[GETWS]);
  }
}

if(dataReg) {
  sprintf(buffer, "%s.sync.cds", vName);
  if(getvault) {
    sprintf(_FPN[GETWS], "%s/%s/%s", ddObjPath, vName, dataRegFile);
    sprintf(_FPN[GETVAULT], "%s/%s/%s/%s;", libVaultPath, cName, vName, dataRegFile);
    sprintf(_dssObjWsPath, "%s/%s", ddObjPath, buffer);
    sprintf(_dssObjVaultPath, "%s/%s/%s;", libVaultPath, cName, buffer);
  }
  else {
    /* ws/l/c/v/datafile */
    sprintf(_FPN[GETWS], "%s/%s/%s", ddObjPath, vName, dataRegFile);
    /* ws/l/c/v.sync.cds */
    sprintf(_dssObjWsPath, "%s/%s", ddObjPath, buffer);
  }
}

if(getvault)
  return _dssObjVaultPath;
else
  return _dssObjWsPath;
}

/* TEST: If master.tag exists then it is a collection object. */
int idb_is_collection_obj(void *plibid, char *cName, char *vName)
{
ddId ddObject;
char *ddObjPath;
int dataReg=TRUE;

if(!(ddObject = ddGetObj(NULL, cName, vName, NULL, plibid, "r")))
  return FALSE;

if(!ddGetObj(NULL, cName, vName, "master.tag", plibid, "r")) {
  sprintf(_intelCVName, "%s.%s", cName, vName);
  ddObject = ddGetObj(NULL, cName, vName, _intelCVName, plibid, "r");
  dataReg=FALSE;
}
return dataReg;
}

/*-----------------------------------------------------------------------
 * DM Operations
 *
 * idb_checkout
 * idb_checkin
 * idb_cancel_checkout 
 * idb_install_cellview
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description: Checks out a file and returns the path to it.
 *              If create_file_if_required is true it will create the file
 *              if not present (for Cadence 0.0 file which is just a place
 *              holder).
 *		The option create_empty_file_if_required has been deleted
 *-----------------------------------------------------------------------*/


static char **dssChkinOpts =0;
static char **dssChkoutOpts=0;
static char **dssCancelOpts=0;

/** TODO: version ckout has been disabled for now due to branching issues
*** of DesSync. 
**/
HRESULT idb_checkout(void *plib_id, char *cName, char *vName, char *ver,
               		char *FPN, char *locker, char **new_ver, int CDBA) 
{
   ddId ddLibId;
   HRESULT dssRetHandle;
   char *dssObjName, *dataFile;
   const char* sync_msg=0;
   size_t dssResults_ptr;

   if (debugDmsOn){
       dmsPrintIdmsLog("<<*C-CALL*>> - idb_checkout(cName=[%s], vName=[%s], CDBA=%d FPN=%s\n", cName, vName, CDBA, FPN);
   } 
   
 
   ddLibId = (ddId)plib_id;
   strcpy(FPN, "");
   if (!sync_on){
     idb_set_error_code(idb_SYNC_ERROR);
     return E_FAIL;
   }
   
   if (!plib_id || !ddIsId(ddLibId)) {
      idb_set_error_code(idb_INVALID_LIB | idb_CANT_CO);
      return E_FAIL;
   }

   dssObjName = idb_get_collection_obj(plib_id, cName, vName, !GETVAULT);

   if(!dssObjName ) {
     idb_capture_cdn_vendor_warning();
     idb_set_error_code(idb_CANT_FIND_CV | idb_CANT_CO);
     return E_FAIL;
   }

   dssRetHandle = sapiMakeStrArray(&dssObjList, dssObjName, NULL);

   /** make options **/
   /** a co after a cancelco needs FORCE **/
   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
	sapiMakeStrArray(&dssChkoutOpts, LOCK, NULL) : dssRetHandle ;

  sapiAddStrings(dssChkoutOpts,NOCOMMENT,NULL);
  if(!getenv("IDMS_NOFORCE"))
    sapiAddStrings(dssChkoutOpts,FORCE,NULL);

   /*********************************************************************
   specific version ckouts need branching 
   if(ver && strcmp(ver, "")) 
     dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
	sapiAddStrings(dssChkoutOpts, VERSION, ver, NULL) : dssRetHandle ;
   *********************************************************************/

   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return dssRetHandle;
   }
   check_read_only(dssObjName,"checkout",1);
   idb_record_dm_vendor_string();
   /*printf("\n\n-----------------------------------%d----%d\n", getuid(),geteuid());
   if(setreuid(DMeuid,DMeuid) < 0) printf("ERRRRR\n");
   printf("\n\n-----------------------------------%d----%d\n", getuid(),geteuid());*/
   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiCheckout(NULL, dssObjList, dssChkoutOpts, &dssResults) : dssRetHandle;
   /*printf("\n\n-----------------------------------%d----%d\n", getuid(),geteuid());
   if(setreuid(DMruid,DMruid) < 0) printf("ERRRRR\n");
   printf("\n\n-----------------------------------%d----%d\n", getuid(),geteuid());*/
   sync_msg= idb_get_record_dm_vendor_string(1);

   for(dssResults_ptr = 0; dssResults[dssResults_ptr] != NULL; dssResults_ptr++){
      /* m/^Objects failed/i */
      if(strncasecmp("Objects failed", dssResults[dssResults_ptr],14) == 0){
        dssRetHandle = E_FAIL; /* maybe failed for some objects */
        break;
      }
   }

   if(!check_read_only(dssObjName,"checkout",0) || FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CO);
     return E_FAIL;
   }    
   else {
     if(locker) strcpy(locker, getpwuid(getuid())->pw_name);  /* assumption */
     strcpy(FPN, _FPN[GETWS]);		/* _FPN is set in idb_get_collection_obj */
   }

       /* force writable by user */
       if(getenv("DMSERVER_FORCE_CHECKOUT_CHMOD")){
         char *dirpath = NULL;
	 struct stat s;
         asprintf(&dirpath, "%s/%s", dirname(dssObjName), vName);
	 if(stat(dirpath,&s) || !S_ISDIR(s.st_mode)){ /* if libname/cellname/viewname not exist or not a dir */
	   free(dirpath);
	   asprintf(&dirpath, "%s", dssObjName);
	 }

         if(debugDmsOn)
           fprintf(stderr,"-I- dssObjName: %s dir %s\n",dssObjName, dirpath);
         /* DIR is used by something a macro that is completely unrelated */
         struct __dirstream* dir = opendir(dirpath);
         struct dirent *dire;
         char *fullpath;
         while(dire = readdir(dir)){
	       asprintf(&fullpath,"%s/%s", dirpath, dire->d_name);
               lstat(fullpath,&s);
               if(debugDmsOn) fprintf(stderr,"-I- Found %s, mode %o, it is a %s file\n", fullpath, s.st_mode & S_IFMT,
                 (S_ISREG(s.st_mode)) ? "regular" :
                 (S_ISLNK(s.st_mode)) ? "link" : 
                 (S_ISDIR(s.st_mode)) ? "directory" : "unknown");
           if(S_ISREG(s.st_mode)){ /* if regular file during checkout */
             if(fullpath){
	           if(debugDmsOn)
                 fprintf(stderr,"-I- Checkout File %s is a real file, forcing chmod writeable by user.\n",fullpath);
               chmod(fullpath, s.st_mode | S_IRUSR | S_IWUSR);
	         }
           }
           free(fullpath);
         }
        closedir(dir);
	free(dirpath);
        }
   return dssRetHandle;
}

HRESULT idb_checkin(void *plib_id, char *cName, char *vName, char *FPN, 
		char *locker, char **new_ver, int CDBA, char *logMsg, 
		char *newCell)
{
   ddId ddLibId;
   int dssResults_ptr;
   HRESULT dssRetHandle;
   char  *libName; 
   char *dssObjName;
   const char* sync_msg=0;

   int always_use_skip = 0;
   if (getenv("DMSERVER_ALWAYS_USE_SKIP")) {
      always_use_skip = 1;
   }

   ddLibId = (ddId)plib_id;
   libName = NULL;
   strcpy(FPN, "");
   if (!sync_on){
     idb_set_error_code(idb_SYNC_ERROR);
     return E_FAIL;
   }
   if (!plib_id || !ddIsId(ddLibId)){
      idb_set_error_code(idb_INVALID_LIB | idb_CANT_CHECKIN);
      return E_FAIL;
   }
   if(!(libName = ddGetObjName(ddLibId))) {
      idb_set_error_code(idb_INVALID_LIB | idb_CANT_CHECKIN);
      return E_FAIL;
   }
   
   dssObjName = idb_get_collection_obj(plib_id, cName, vName, !GETVAULT);

   if(!dssObjName) {
     idb_capture_cdn_vendor_warning();
     idb_set_error_code(idb_CANT_OPEN_CV | idb_CANT_CHECKIN);
     return E_FAIL;
   }
   
   dssRetHandle = sapiMakeStrArray(&dssObjList, dssObjName, NULL);

   /** make options **/
   if(logMsg != NULL && strcmp(logMsg, ""))  {
     dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     			sapiMakeStrArray(&dssChkinOpts, COMMENT, 
					logMsg, NULL) : dssRetHandle;
   }
   else {
     dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     			sapiMakeStrArray(&dssChkinOpts, 
				NOCOMMENT, "", NULL) : dssRetHandle;
   }
   if(newCell != NULL ) {
     dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     			sapiAddStrings(dssChkinOpts,NEW, "", NULL) : dssRetHandle;
     /*** was this a retired cellview? need to skip version in case there are more than 1. 
          _retired_cellview was set at cellview creation ***/
     _retired_cellview = idb_url_retired(plib_id, cName, vName);
     if (always_use_skip || (_retired_cellview != NULL) ) {
        dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     			sapiAddStrings(dssChkinOpts, 
					SKIP, "", NULL) : dssRetHandle;
	    _retired_cellview = 0;
     } 
   }
   else {
     /* FORCE is needed for idms update option */
     dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
			sapiAddStrings(dssChkinOpts, NULL) : dssRetHandle ;
     /*FORCE, "", NULL) : dssRetHandle ;*/
   }

  if(!getenv("DMSERVER_NO_SHARE"))
    sapiAddStrings(dssChkinOpts,SHARE,NULL);

   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CHECKIN | idb_SYNC_ERROR);
     return dssRetHandle;
   }
   check_read_only(dssObjName,"checkin",0);
   idb_record_dm_vendor_string();
   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     sapiCheckin(NULL, dssObjList, dssChkinOpts, &dssResults) : dssRetHandle;

   sync_msg= idb_get_record_dm_vendor_string(1);
   check_read_only(dssObjName,"checkin",1);

   for(dssResults_ptr = 0; dssResults[dssResults_ptr] != NULL; dssResults_ptr++){
      /* m/^Objects failed/i */
      if(strncasecmp("Objects failed", dssResults[dssResults_ptr],14) == 0){
        dssRetHandle = E_FAIL; /* maybe failed for some objects */
        break;
      }
   }

   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CHECKIN | idb_SYNC_ERROR);
   }    
   else {
     /* Sometimes need to force a non-locked checkout to make it populate right,
      * we don't really care if it fails at this point */
     sapiCheckout(NULL, dssObjList, NULL, &dssResults);
     if(locker) strcpy(locker, NOBODY);	/* assumption */
     if(new_ver) *new_ver = _newVerBuff;
     strcpy(FPN, _FPN[GETWS]);		/* _FPN is set in idb_get_collection_obj */
   }

   return dssRetHandle;
}


HRESULT idb_cancel_checkout(void *plib_id, char *cName, char *vName,
                        char *FPN, int CDBA)
{
   ddId ddLibId;
   ddId ddObject;
   HRESULT dssRetHandle;
   char  *libName;
   char *dssObjName;
   char* sync_msg=0;
   int f=geteuid(),f1=getuid();
   ddLibId = (ddId)plib_id;
   strcpy(FPN, "");

   if (debugDmsOn){
       dmsPrintIdmsLog("<<*C-CALL*>> - idb_cancel_checkout(cName=[%s], vName=[%s], CDBA=%d FPN=%s\n", cName, vName, CDBA, FPN);
   } 

   if (!sync_on){
     idb_set_error_code(idb_SYNC_ERROR);
     return E_FAIL;
   }
   if (!plib_id){
      idb_set_error_code(idb_INVALID_LIB | idb_CANT_CANCEL_CO);
      return E_FAIL;
   }
   if(!(libName = ddGetObjName(ddLibId))) {
      idb_set_error_code(idb_INVALID_LIB | idb_CANT_CANCEL_CO);
      return E_FAIL;
   }

   dssObjName = idb_get_collection_obj(plib_id, cName, vName, !GETVAULT);

   if(!dssObjName){
      idb_capture_cdn_vendor_warning();
      idb_set_error_code(idb_CANT_FIND_CV | idb_CANT_CANCEL_CO);
      return E_FAIL;
   }

   if (debugDmsOn){
       dmsPrintIdmsLog("<<*C-CALL*>>  idb_cancel_checkout - dssObjName = %s\n", dssObjName);
   }

   dssRetHandle = sapiMakeStrArray(&dssObjList, dssObjName, NULL);
   /** make options */
   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
		sapiMakeStrArray(&dssCancelOpts, getenv("IDMS_NOFORCE")?NULL:FORCE, "", NULL) : dssRetHandle ;

  if(!getenv("DMSERVER_NO_SHARE"))
    sapiAddStrings(dssCancelOpts,SHARE,NULL);

   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CANCEL_CO | idb_SYNC_ERROR);
     return dssRetHandle;
   }
   check_read_only(dssObjName,"cancel",0);
   idb_record_dm_vendor_string();
   dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
     	sapiCancel(NULL, dssObjList, dssCancelOpts, &dssResults) : dssRetHandle;
   sync_msg=idb_get_record_dm_vendor_string(1);
   check_read_only(dssObjName,"cancel",1);

   

   if(FAILED(dssRetHandle)) {
     idb_capture_dm_vendor_warning();
     idb_set_error_code(idb_CANT_CANCEL_CO | idb_SYNC_ERROR);
   }    
   else
     strcpy(FPN, _FPN[GETWS]);		/* _FPN is set in idb_get_collection_obj */

   /* sapiFreeStrArray(dssResults); */
   return dssRetHandle;
}


/** In 4.4.x installing a cellview, is a ci -new. **/
HRESULT idb_install_cellview(void *plib_id, char *cName, char *vName, 
                          char *FPN, int CDBA, char *logMsg, int *file_was_deleted)
{
   HRESULT dssRetHandle;
   ddId plibid = (ddId)plib_id;
   char*sync_msg=0;

   *file_was_deleted = 0;

   if (!sync_on){
     idb_set_error_code(idb_SYNC_ERROR);
     return E_FAIL;
   }
   if (!plib_id){
      idb_set_error_code(idb_INVALID_LIB | idb_CANT_INSTALL_CV);
      return E_FAIL;
   }
   idb_record_dm_vendor_string();

   static char INSTALLED_PATH[BUFSIZE];
   if (FPN)
     strcpy(INSTALLED_PATH, FPN);

   dssRetHandle = idb_checkin(plibid, cName, vName, FPN, NULL, NULL, 
   							CDBA, logMsg, NEW);
   sync_msg=idb_get_record_dm_vendor_string(1);
   if(FAILED(dssRetHandle)) {
     if (debugDmsOn){
        dmsPrintIdmsLog("*IDMS - idb_install_cellview *FAILED* for %s\n", INSTALLED_PATH);
        dmsPrintIdmsLog("*IDMS - LAST DIR  CREATED: %s\n", idb_get_last_directory_cell());
        dmsPrintIdmsLog("*IDMS - LAST FILE CREATED: %s\n", idb_get_last_cell_name());
        
     }


     /* lets remove the clutter if we are sure */
      
     char *file_name = idb_get_last_cell_name();
     static char cmdbuff[BUFSIZE];
     static char failed_directory[BUFSIZE];

     /* First remove the file from DB_ROOT */    

     if (!strcmp(INSTALLED_PATH, file_name)){
        osRemoveFile(file_name);
        *file_was_deleted = 1;
        if (debugDmsOn){
           dmsPrintIdmsLog("*IDMS - *REMOVING FILE %s\n", file_name);
        }
     }

     /* if the directory was just created, we will actually erase everything here */

     /* get the directory */

     strcpy(failed_directory, INSTALLED_PATH);
     char *p = (char *)failed_directory + strlen(failed_directory); 
     while (p > failed_directory){
        if (*p == '/')
           break;
        p--;   
     }
     *p = '\0';

     if (debugDmsOn){
          dmsPrintIdmsLog("*IDMS - current directory: %s\n", failed_directory);
     }

     if (!strcmp(idb_get_last_directory_cell(), failed_directory)){
        if (strcmp(failed_directory, "") != 0){ /* don't do something stupid */
           sprintf(cmdbuff, "/bin/rm -rf %s", failed_directory);
           system(cmdbuff);
           if (debugDmsOn){
              dmsPrintIdmsLog("*IDMS - *REMOVING DIRECTORY %s\n", failed_directory);
           }
           if (strcmp(idb_get_last_directory1_cell(), "") != 0){
               char *p = (char *)failed_directory + strlen(failed_directory);
               while (p > failed_directory){ 
                  if (*p == '/')
                     break;
                  p--;   
               }
               *p = '\0';
               if (!strcmp(idb_get_last_directory1_cell(), failed_directory)){
                  sprintf(cmdbuff, "/bin/rm -rf %s", failed_directory);
                  system(cmdbuff);
                  if (debugDmsOn){
                     dmsPrintIdmsLog("*IDMS - *REMOVING DIRECTORY %s\n", failed_directory);
                  }
               }
           }
        }       
     }

     /* reset the breadcrumbs */ 

     idb_set_last_directory_cell("");
     idb_set_last_directory1_cell("");
     idb_set_last_cell_name("");

     idb_delete_cellview(plib_id,cName,vName,"",FPN,"",&sync_msg,CDBA);
     idb_capture_dm_vendor_warning();
      idb_set_error_code(idb_CANT_INSTALL_CV|idb_SYNC_ERROR);
      return dssRetHandle;
   }

   /* Now reset the breadcrumbs so if we go "byView" and we croak in the middle it works */

   idb_set_last_directory_cell("");
   idb_set_last_directory1_cell("");
   idb_set_last_cell_name("");
     
   idb_clear_errors();
   return dssRetHandle;
}

/*----------------------------------------------------------------------------*
 *
 * Provided to capture the last message set by a DESSYNC DM engine 
 *
 *----------------------------------------------------------------------------*/
void idb_capture_dm_vendor_warning(void)
{
  int i=0;

   if (debugDmsOn) {
      while(dssResults && dssResults[i]) 
	dmsPrintIdmsLog("*IDMS - DESSYNC: dssResults[%d]:%s\n", 
						i, dssResults[i++]);
   }
}
  
/*==================*
 * TRIGGERS RELATED *
 *==================*/


/* Triggers we are using at present */

/*
 This procedure makes sure the import and export list
 for the user will be correctly set - so that
 this user will not get anybody else's private copy. Or
 allow anyone to read its own checked out version.
*/

/*
Boolean idb_open_lib_trigger(dmLibId dmsLibID)
{
   idb_set_user_rights(cuserid(0));
   return TRUE;
}
*/


/*============*
 * File Based *
 *============*/

 /* Put here to take advantage of the vo Functions from Cadence since the
    first idb package is implemented around Cadence */
 /* voSimplifyFileName is no longer listed in itkDB.h but nm lists it */

char *idb_simplify_file_name(const char *f, const char *d)
{
 return voSimplifyFileName(f, d);
}

/* if viewType can't be recognised, then hopefully the vName alias
** is set in the data.reg, so try vName after vType
** REM: This function has a lot to be desired. If Cadence can
** provide a function which gets the data file after looking
** at the master.tag, given a l/c/v path file without a dbOpen. 
** Till then it will be a half hearted effort to guess the data 
** file given a viewName.
*/

char *idb_get_dataReg_file(char *viewType, char *viewName)
{ 
  char *dataRegFile=NULL;

/***
 *** Cadence stuff (old HP behavior )
 ***/
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
  if(viewType) {
    dataRegFile = ddMapGetDataTypeFileName(viewType);
  }
  if(viewName && !dataRegFile) {
    dataRegFile = ddMapGetDataTypeFileName(viewName);
  }

#else
/*** 
 *** LINUX | HP with new lib structure defined - look up globaltable for data file pattern.
 ***/
  if(viewName && !dataRegFile){
    dataRegFile = idmGetPattern(viewName, 0);  
  }
#endif

  return dataRegFile;
} 

/*---------*
 * General *
 ----------*/

#define PATHMAX BUFSIZE 
static char _anyPath[PATHMAX];
static char *token, *prevToken;

char *idb_strip_last_token(const char *path, const char *seperator) 
{

  strcpy(_anyPath, path);

  token = strtok(_anyPath, seperator);

  do{
    prevToken = token;
  }while(token = strtok(NULL, seperator));

  return prevToken;
}

char *idb_strip_nth_token(const char *path, const char *seperator, int n)
{
int i = 1;

  strcpy(_anyPath, path);

  token = strtok(_anyPath, seperator);
  if(n <= 1) return token;

  while(i<n && token) {
    token = strtok(NULL, seperator);
    ++i;
  }
  return token;
}


char *idb_get_vault(void *plib) {
char *libName; 
char *libPath;
int i=0;
HRESULT hresult;

libName = dbGetLibName(plib);
libPath = ddGetObjReadPath(plib);

 while(_libNames[i][0] != '\0' && libName && strcmp(libName, _libNames[i]))  
  ++i;

if(_libNames[i][0] == '\0') {
 if(sync_on)
  hresult = sapiUrlVault(NULL, libPath, NULL, &dssResults);
 else
  hresult = E_FAIL;
  if(FAILED(hresult) || (dssResults[0] && !strcmp("", dssResults[0])))
    return NULL;
  else {
    strcpy(_libVaults[i], dssResults[0]);
    strcpy(_libNames[i], libName);
  }
}

return _libVaults[i];
}



/*-----------------------------*
 * Functions Defined for LINUX *
 *-----------------------------*/

#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
void idb_clear_libs(int max)
{
  int j=0;
  /*  deallocate */
  if(libName || libPath || libType) { 
    for(j=0; j<libNum; free(libName[j++]));
    free(libName);
    for(j=0; j<libNum; free(libPath[j++]));
    free(libPath);
    for(j=0; j<libNum; free(libType[j++]));
    free(libType);
    libNum=0;
    libMax=0;
  }
  /* allocate up to max if max>0 */
  if(max>0){
    libMax = max;
    libName = (char **)malloc(libMax*sizeof(char *));
    libPath = (char **)malloc(libMax*sizeof(char *));
    libType = (char **)malloc(libMax*sizeof(char *));
    if(!libName || !libPath || !libType){
      if (debugDmsOn)
        dmsPrintIdmsLog("*IDMS - failed to reallocate libName/libPath in idb_clearLibs\n");
    }
  }
  
}

void idb_add_libs(const char *name, const char *path, const char * type)
{
  int j=0;

  if(libNum < libMax){
    libName[libNum] = strdup(name);
    libPath[libNum] = strdup(path);
    libType[libNum] = strdup(type);
    libNum++;
  } else { /* resize the array */
    libMax = 2*libMax;
    libName = realloc((void *)libName,libMax*sizeof(char *));
    libPath = realloc((void *)libPath,libMax*sizeof(char *));
    libType = realloc((void *)libType,libMax*sizeof(char *));
    if(libName && libPath && libType){
      libName[libNum] = strdup(name);
      libPath[libNum] = strdup(path);
      libType[libNum] = strdup(type);
      libNum++;
    }
  }
}

char *idb_get_libpath(const char *name)
{
  int j=0;
  if(!name)
    return 0;
  
  for(j=0; j<libNum; j++){
    if(libName[j] && !strcmp(name, libName[j]))
      return libPath[j];
  }
  return 0;
}

char *idb_get_libtype(const char *name)
{
  int j=0;
  if(!name)
    return 0;
  
  for(j=0; j<libNum; j++){
    if(libName[j] && !strcmp(name, libName[j]))
      return libType[j];
  }
  return 0;
}


static char file[BUFSIZE];
static char f1[BUFSIZE];
static char f2[BUFSIZE];
static char f3[BUFSIZE];
       
void *ddGetObj(char *libName, char *cellName, char *viewName,
                 char *fileName, void *contextId, char *mode)
{
  
  char *libraryPath = 0;
  char *libraryType = 0;
  struct dirent *dp = 0;
  struct __dirstream *dirp = 0;
  char *collectionView =0;
  int done = 0;
  FILE *stream; 
  FILE *stream2;

  int dir_created =  0;
  int dir2_created = 0;
  int file_created = 0;

/*
  if (debugDmsOn){
     dmsPrintIdmsLog("*IDMS DEBUG* - entering ddGetObj\n");
  }
*/
 
  if(!idb_get_libpath(libName)){
    libraryPath = idb_get_libpath(contextId);
    libraryType = idb_get_libtype(contextId);
  } else {
    libraryPath = idb_get_libpath(libName);
    libraryType = idb_get_libtype(libName);
  }
  if(!libraryPath || !libraryType)
    return (void *)0;

  /* Handle "w" mode here to create cellviews on disk */
  if(mode && !strcmp(mode, "w")){
    if(libName && fileName && cellName && viewName){
      if(!osFileExists(libraryPath))
        return (void *)0;
      
      if(!strcmp(libraryType, "byNone")) {
        /* Due to Design, we have to disable installing/creating cellviews if the library is byNone type */
        return (void *)0;
      }
      else if(!strcmp(libraryType, "byCell")){
        sprintf(f1,"%s/%s",libraryPath,cellName);
        if (debugDmsOn){
           dmsPrintIdmsLog("*IDMS DEBUG* - ddGetObj - byCell - f1=%s\n", f1);
         }

        if (! (dir_created = osFileExists(f1))){
           dir_created = osCreateDir(f1);
           if (dir_created){
              if (debugDmsOn){
                 dmsPrintIdmsLog("*IDMS DEBUG* - ddGetObj - byCell ** -created dir %s\n", f1);
              }
              idb_set_last_directory_cell(f1);
              idb_set_last_directory1_cell("");
           }
        }        
        if(dir_created){ 
          sprintf(file,"%s/%s/%s",libraryPath,cellName,fileName);
          if (! (file_created = osFileExists(file))){
             file_created = osCreateFile(file);
             if (file_created ) {
                if (debugDmsOn){
                   dmsPrintIdmsLog("*IDMS DEBUG* - ddGetObj - byCell ** -created file %s\n", file);
                }
                idb_set_last_cell_name(file); 
                return (void *)file;
             }
          }
          else 
             return (void *)file;
        } 
      }
      else if(!strcmp(libraryType, "byView")){
        sprintf(f1,"%s/%s",libraryPath,viewName);
        if (debugDmsOn){
           dmsPrintIdmsLog("*IDMS DEBUG* - ddGetObj - byView - f1=%s\n", f1);
        }

        if (debugDmsOn){
           dmsPrintIdmsLog("*IDMS DEBUG* - ddGetObj - byCell - f1=%s\n", f1);
         }

        if (! (dir_created = osFileExists(f1))){
           dir_created = osCreateDir(f1);
           if (dir_created){
              if (debugDmsOn){
                 dmsPrintIdmsLog("*IDMS DEBUG* - ddGetObj - byView ** -created dir %s\n", f1);
              }
              idb_set_last_directory_cell(f1);
              idb_set_last_directory1_cell("");
           }
        }

        if(dir_created){
          sprintf(file,"%s/%s/%s",libraryPath,viewName,fileName);
          if (! (file_created = osFileExists(file))){
             file_created = osCreateFile(file);
             if (file_created ) {
                if (debugDmsOn){
                   dmsPrintIdmsLog("*IDMS DEBUG* - ddGetObj - byView ** -created file %s\n", file);
                }
                idb_set_last_cell_name(file);
                return (void *)file;
             }
          }
        }
      }
      else { /* cadenceLib */
        sprintf(f1,"%s/%s",libraryPath,cellName);
        sprintf(f2,"%s/%s/%s",libraryPath,cellName,viewName);
        if (debugDmsOn){
           dmsPrintIdmsLog("*IDMS DEBUG* - ddGetObj - cadenceLib - f1=%s f2=%s\n", f1, f2);
        }
            
        if (! (dir_created = osFileExists(f1))){
           dir_created = osCreateDir(f1);
           if (dir_created){
              if (debugDmsOn){
                 dmsPrintIdmsLog("*IDMS DEBUG* - ddGetObj - cadenceLib ** -created dir %s\n", f1);
              }
              idb_set_last_directory1_cell(f1);
           }
        }

       
        if (dir_created) {
           if (! (dir2_created = osFileExists(f2))){
              dir2_created = osCreateDir(f2);
              if (dir2_created){
                 if (debugDmsOn){
                    dmsPrintIdmsLog("*IDMS DEBUG* - ddGetObj - cadenceLib ** -created dir %s\n", f2);
                 }
                 idb_set_last_directory_cell(f2);
              }
           }
        }


        if (dir2_created) {
          /* create cell/view directories successfully. */
          sprintf(file,"%s/%s/%s/%s",libraryPath,cellName,viewName, fileName);
          sprintf(f3,"%s/%s/%s/%s",libraryPath,cellName,viewName,"master.tag");
         if (! (file_created = osFileExists(file))){
             file_created = osCreateFile(file);
             if (file_created ) {
                if (debugDmsOn){
                   dmsPrintIdmsLog("*IDMS DEBUG* - ddGetObj - cadenceLib ** -created file %s\n", file);
                }
                idb_set_last_cell_name(file);
             }
          }

          if (file_created) {
            if((stream = fopen(f3, "w")) != 0) {
	      sprintf(f2, "%s\n%s", "-- Master.tag File, Rev:1.0", fileName);
              fprintf(stream, "%s", f2);
              fclose(stream);
	      sprintf(f1, "%s/%s/%s/.%s.sync.cds.syncmd", libraryPath,cellName,viewName,viewName);
	      if((stream2 = fopen(f1, "w")) != 0) {
		sprintf(f2, "%s/%s\n%s/master.tag\n%s/.%s.sync.cds.syncmd\n",viewName,fileName,viewName,viewName,viewName);
	 	fprintf(stream2, "%s", f2); 
		fclose(stream2);
                return (void *)file;
	      }
            }

          }
        }
      }   
      return (void *)0; /* if any creation fails, return 0 */
    }
    else { /* just return library Name */
      if(!libName)
        return (void *)contextId;
      else
        return (void *)libName;
    }
  }
  
  if(fileName)
    if(!strcmp(fileName, "master.tag") &&
       (!strcmp(libraryType, "byCell") || !strcmp(libraryType, "byView") ||
        !strcmp(libraryType, "byNone")))
      return (void *)0;
  
  if((cellName!=0) && (viewName!=0)){
    /* make sure to use filename if differs from cname.vname */
    sprintf(file, "%s.%s", cellName, viewName);
    if(fileName != 0){ 
      if(strcmp(fileName, file) != 0)
        sprintf(file, "%s", fileName);
    }
    if(!strcmp(libraryType, "byNone"))
      return (void *)findObject(libraryPath,file);
    else if(!strcmp(libraryType, "byCell")){
      sprintf(file,"%s/%s/%s.%s",libraryPath,cellName,cellName,viewName);
      if(osFileExists(file))
        return (void *)file;
      else
        return 0;
    }
    else if(!strcmp(libraryType, "byView")){
      sprintf(file,"%s/%s/%s.%s",libraryPath,viewName,cellName,viewName);
      if(osFileExists(file))
        return (void *)file;
      else
        return 0;
    }
    else {/* cadence lib */
      sprintf(file,"%s/%s/%s/%s",libraryPath,cellName,viewName,"master.tag");
      /* check if it's looking for master.tag file */
      if((fileName!=0) && (!strcmp(fileName, "master.tag"))){
        if(osFileExists(file))
          return file;
        else
          return 0;
      }
      /* it's looking for the data file  in globaltable dfII Registry table */
      if(osFileExists(file)){
	collectionView = idmGetPattern(viewName,0);
	sprintf(file,"%s/%s/%s/%s",libraryPath,cellName,viewName,collectionView); 
        if(osFileExists(file))
          return (void *)file;
        else 
          return 0;
      } else { /* check if it's c/v/c.v format (not a collection obj).*/
        sprintf(file,"%s/%s/%s/%s.%s",libraryPath,cellName,viewName, cellName, viewName);
        if(osFileExists(file))
          return (void *)file;
        return 0;
      }
    } 
  }
  else if(cellName != 0){
    if(!strcmp(libraryType, "byNone") || !strcmp(libraryType, "byView"))
      return 0;
    else { /* byCell or cadence lib */
      sprintf(file,"%s/%s",libraryPath,cellName);
      if(osFileIsDir(file))
        return (void *)file;
      else
        return 0;
    }
  }
  else if(viewName != 0){
    return (void *)libraryPath;
  }
  else { /* both cellname and viewname are NULL -> just return library Name */
    if(!libName)
      return (void *)contextId;
    else
      return (void *)libName; 
  }
}

static char** dirQ=0;
static int Qtail=0;
static int Qhead=0;
static int Qmax=BUFSIZE;

void pushQ(const char*dirPath)
{
  if(Qtail < Qmax){
    dirQ[Qtail] = strdup(dirPath);
    Qtail++;
  } else { /* resize the array */
    Qmax = 2*Qmax;
    dirQ = realloc((void *)dirQ,Qmax*sizeof(char *));
    if(dirQ){
      dirQ[Qtail] = strdup(dirPath);
      Qtail++;
    }
  }
}

void clearQ(void)
{
  int j;
  /*  deallocate */
  if(dirQ) { 
    for(j=0; j<Qtail; free(dirQ[j++]));
    free(dirQ);
    dirQ=0;
    Qtail=0;
    Qhead=0;
    Qmax=BUFSIZE;
  }
}

char *findObject(const char *dirPath, const char *fileName)
{
  char *fullpath;

  if(dirQ) /* make sure Q starts clean */
    clearQ();
  
  /* allocate dirQ to Qmax size */
  if (!dirQ) {
     dirQ=(char **)malloc(Qmax*sizeof(char *));
    if(!dirQ){
      if (debugDmsOn)
        dmsPrintIdmsLog("*IDMS - failed to allocate dirQ in find Object\n");
      return 0;
    }
  }

  pushQ(dirPath);
  while(Qtail > Qhead){
    fullpath = BFS(fileName);
    if(fullpath != 0){
      clearQ();
      return fullpath;
    }
  }
  clearQ();
  return 0;
}

static char path[BUFSIZE];

/* perform BFS on dirPath.*/
char *BFS(const char *fileName)
{
  struct dirent *dp = 0;
#ifndef OH_MACH_LINUX
  struct __dirdesc *dirp = 0;
#else
  struct __dirstream *dirp = 0;
#endif
  int hasSeen =0;
  /* char *fullPath; */

  sprintf(path, "%s/%s", dirQ[Qhead], fileName);
  if(osFileExists(path)){
    /* fullPath=strdup(path); */
    return path; /* fullPath */
  } else {
    Qhead++;
    
    return 0;
    
    /* push all subdirectories onto Queue */
    dirp = opendir(dirQ[Qhead]);  
    if(dirp) {
      while((dp = readdir(dirp)) != NULL) {
        if(strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")) {
          sprintf(path, "%s/%s", dirQ[Qhead], dp->d_name);
          pushQ(path);
        }
      }
      (void) closedir(dirp);
    }
    Qhead++; /* pop item 1 on Q */
  }  
  return 0;
}




static char pathBuffer[BUFSIZE];
static char fileBuffer[BUFSIZE];
static char * __cellList_buff = 0;
static int __CcellList_buff = 0;
static char * __viewList_buff = 0;
static int __VviewList_buff = 0;

/* recursively traverse through (sub)directories of dirpath and
   find files that match the given cells/views.  Append views/cells
   that match the given cells/views.
 */
char *getListbyName(const char *dirpath, const char *library, const char *name, int isCellName, int getLocker)
{
  int wild_name=0;
  
  if (!name)
    wild_name = 1;
  else if(!strcmp(name, "*"))
    wild_name = 1;
  
  if(dirQ) /* make sure Q starts clean */
    clearQ();
  
  /* allocate dirQ to Qmax size */
  dirQ=(char **)malloc(Qmax*sizeof(char *));
  if(!dirQ){
    if (debugDmsOn)
      dmsPrintIdmsLog("*IDMS - failed to allocate dirQ in getListbyName\n");
    return 0;
  }

  pushQ(dirpath);
  
  if(!isCellName){
    if(!__cellList_buff){
      clearQ();
      return 0;
    } else
      __cellList_buff[0] = '\0';
  } else { /* it is cellname */
    if(wild_name==1){
      clearQ();
      return 0;
    }
    if(!__viewList_buff){
      clearQ();
      return 0;
    } else
      __viewList_buff[0] = '\0';
  }
  
  while(Qtail > Qhead){
    if(!checkName(library, name, isCellName,getLocker)){
      clearQ();
      return 0;
    }
  }
  
  clearQ();
  if(isCellName)
    return __viewList_buff;
  else
    return __cellList_buff;
}

/* perform BFS on dirPath to find files that match cellname.*/
char *checkName(const char* library, const char *name, int isCellName, int getLocker)
{
  char cellname[BUFSIZE];
  struct dirent *dp = 0;
  struct __dirstream *dirp = 0;
  char *tmp =0;
  int wild_name=0;
  
  if (!name)
    wild_name = 1;
  else if(!strcmp(name, "*"))
    wild_name = 1;
  
  dirp = opendir(dirQ[Qhead]);  
  if(dirp) {
    while((dp = readdir(dirp)) != NULL) {
      if(strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..") && strcmp(dp->d_name, ".type.lib")) {
        
        sprintf(pathBuffer, "%s/%s", dirQ[Qhead], dp->d_name);
        if(osFileIsDir(pathBuffer)){
          /* push all subdirectories onto Queue */
          pushQ(pathBuffer);
          
        } else { /* it's a file so check the name */
          if(isCellName){
            sprintf(fileBuffer, "%s", dp->d_name);
            if(tmp = strrchr(fileBuffer, '.')){
              *tmp = '\0';
              if(!strcmp(fileBuffer, name)){
                /* add view to the viewlist */
                if(tmp = strrchr(dp->d_name, '.')) {
                  *tmp++;
                  /* check for size of viewList_buff */
                  if ((strlen(__viewList_buff)+strlen(tmp)+2) >= __VviewList_buff) {
                    __VviewList_buff = 2*__VviewList_buff + (strlen(tmp)+2);
                    __viewList_buff = realloc((char *)__viewList_buff,__VviewList_buff*sizeof(char));
                    if(!__viewList_buff)
                      return 0;
                  }
                  strcat(__viewList_buff, tmp);
                  strcat(__viewList_buff, " ");
                }
              }
            }
          } else { /* It's view Name */
            if(tmp = strrchr(dp->d_name, '.')){
              *tmp++;
              sprintf(fileBuffer, "%s", tmp);
              if(wild_name==1){
                /* add cell to the celllist */
                sprintf(fileBuffer, "%s", dp->d_name);
                if(tmp = strrchr(fileBuffer, '.')){
                  *tmp = '\0';
                  /* check for size of cellList_buff */
                  if ((strlen(__cellList_buff)+strlen(fileBuffer)+2) >= __CcellList_buff) {
                    __CcellList_buff = 2*__CcellList_buff + (strlen(fileBuffer)+2);
                    __cellList_buff = realloc((char *)__cellList_buff,__CcellList_buff*sizeof(char));
                    if(!__cellList_buff)
                      return 0;
                  }
                  strcat(__cellList_buff, fileBuffer);
                  strcat(__cellList_buff, " ");
                }
              } else {
              /**** NOT WILD VIEW NAMES ****/  
              if(!strcmp(fileBuffer, name)){
                /* add cell to the celllist */
                sprintf(fileBuffer, "%s", dp->d_name);
                if(tmp = strrchr(fileBuffer, '.')){
                  *tmp = '\0';
		  if(getLocker) {
                    strcpy(cellname, fileBuffer);
                    strcpy(outstring, "");
                    if(idb_get_locker(library, cellname, name, pathBuffer, getLocker)) {
                      strcpy(fileBuffer, outstring);
                    } else {
                      strcpy(fileBuffer, cellname);
                    }
                  }
                  /* check for size of cellList_buff */
                  if ((strlen(__cellList_buff)+strlen(fileBuffer)+2) >= __CcellList_buff) {
                    __CcellList_buff = 2*__CcellList_buff + (strlen(fileBuffer)+2);
                    __cellList_buff = realloc((char *)__cellList_buff,__CcellList_buff*sizeof(char));
                    if(!__cellList_buff)
                      return 0;
                  }
                  strcat(__cellList_buff, fileBuffer);
                  strcat(__cellList_buff, " ");
                }
              }
              }
              
            }
          }
          
        }
      }
    }
    (void) closedir(dirp);
  }
  Qhead++; /* pop item 1 on Q */
  if(isCellName)
    return __viewList_buff;
  else
    return __cellList_buff;
}


/*-----------------------------------------------------------------------
 * idb_gen_obj_rel
 * Parameters:  library = library Name to search
 *              name    = cellname or viewname (could be NULL or wild char.)
 *              isCellName = 0: name is viewname, 1: name is cellname.
 * Description: Appends to the buffer all cellnames or viewnames found in the
 *              library given.  It is able to return a list of cellnames if
 *              viewname is NULL or wild char.  But it is not implemented to
 *              return a list of viewnames if the cellname is NULL or wild
 *              char - return 0 in this case.
 * ADDED (May 20, 2003):
 * added Parameter "getLocker" will check file stat and get the locker of the
 * cell and append it in the list. i.e. CEllName:Locker
 * This is to speed up the process populating library reading & ONLY applies to non-wild names.
 *
 * ADDED (June, 2004):
 * "getLocker == 2" is added to output strings with complete cell information.
 *   isCellName == 0:  isCellName=0, getLocker=1
 *   isCellName == 1:  isCellName=1, getLocker=1
 *   isCellName == 2:  isCellName=0, getLocker=2
 *   isCellName == 3:  isCellName=0, getLocker=3 <-- don't include header(libpath) & locker info. 
 *
 *-----------------------------------------------------------------------*/

char *idb_gen_obj_rel(char*library, char*name, int isCellName, int getLocker)
{
  struct __dirstream *dirp = 0;
  struct dirent *dp = 0;
  struct __dirstream *dirp2 = 0;
  struct dirent *dp2 = 0;
  char *tmp=0;
  char *fullpath = 0;
  int wild_name = 0;
  char *libtype = idb_get_libtype(library);
  char cellname[BUFSIZE];
  char viewname[BUFSIZE];

  if(!library || !libtype)
    return 0;
  else {
    if (!name)
      wild_name = 1;
    else if(!strcmp(name, "*"))
      wild_name = 1;
    fullpath = idb_get_libpath(library);
  }


 if (debugDmsOn) {
     dmsPrintIdmsLog("*IDMS - idb_gen_obj_rel %s %s %d %d wild=%d libtype=%s\n", library, name, isCellName, getLocker, wild_name, libtype);
 } 

/****** 
 ****** Given ViewName - Get a list of cell names 
 ******/
  if(!isCellName){
    /* allocate size to some big number (BUFSIZE) first */
    if (!__cellList_buff)
      if (!(__cellList_buff = (char *)malloc(__CcellList_buff = BUFSIZE ))) { 
        __CcellList_buff = 0;
        return 0;
      }
      else{
        __cellList_buff[0] = '\0';
      }
   
    if(getLocker==2) { /* add library fullpath in there. */
      if ((strlen(fullpath)+1) >= __CcellList_buff) {
         __CcellList_buff = 2*__CcellList_buff + (strlen(fullpath)+1);
         __cellList_buff = realloc((char *)__cellList_buff,__CcellList_buff*sizeof(char));
         if(!__cellList_buff)
           return 0;
      }
      strcpy(__cellList_buff, fullpath);
      strcat(__cellList_buff, " ");
    }
 
    if(!strcmp(libtype, "byNone")){
      return getListbyName(fullpath, library, name, isCellName,getLocker);
    } else {
      if(!strcmp(libtype, "byView")){
        if(wild_name==1)
          sprintf(pathBuffer, "%s", fullpath);
        else 
          sprintf(pathBuffer, "%s/%s", fullpath, name);
      } else
        sprintf(pathBuffer, "%s", fullpath);
      dirp = opendir(pathBuffer);
      if(dirp) {
        if(getLocker!=2)
          __cellList_buff[0] = '\0';
        while ((dp = readdir(dirp)) != 0) {
          if(strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..") && (dp->d_name[0] != '.')) {
            /*****
	     *****  WILD NAME
  	     *****/ 
             if (debugDmsOn) {
                dmsPrintIdmsLog("*IDMS - idb_gen_obj_rel dir entry: %s\n", dp->d_name);
             }
 
            if(wild_name==1){
              if(!strcmp(libtype, "byView")){
                sprintf(pathBuffer, "%s/%s", fullpath, dp->d_name);
                dirp2 = opendir(pathBuffer);
                if(dirp2) {
                  while ((dp2 = readdir(dirp2)) != 0) {
                    if(strcmp(dp2->d_name, ".") && strcmp(dp2->d_name, "..") && (dp->d_name[0] != '.')) {
                      sprintf(fileBuffer, "%s", dp2->d_name);
                      if(tmp = strrchr(fileBuffer, '.')){
                        *tmp = '\0';
                        /* check for size of cellList_buff */
                        if ((strlen(__cellList_buff)+strlen(fileBuffer)+2) >= __CcellList_buff) {
                          __CcellList_buff = 2*__CcellList_buff + (strlen(fileBuffer)+2);
                          __cellList_buff = realloc((char *)__cellList_buff,__CcellList_buff*sizeof(char));
                          if(!__cellList_buff)
                            return 0;
                        }
                        strcat(__cellList_buff, fileBuffer);
                        strcat(__cellList_buff, " ");
                      }
                    }
                  }
                  (void) closedir(dirp2);
                }
              }
              else if(!strcmp(libtype, "byCell")){
                sprintf(pathBuffer, "%s/%s", fullpath, dp->d_name);
                sprintf(fileBuffer, "%s", dp->d_name);
                dirp2 = opendir(pathBuffer);
                if(dirp2) {
                  while ((dp2 = readdir(dirp2)) != 0) {
                    if(strcmp(dp2->d_name, ".") && strcmp(dp2->d_name, "..") && (dp->d_name[0] != '.')) {
                      /* only add the cellname to cellList if there's a cellfile in cell dir */
                      if ((strlen(__cellList_buff)+strlen(fileBuffer)+2) >= __CcellList_buff) {
                        __CcellList_buff = 2*__CcellList_buff + (strlen(fileBuffer)+2);
                        __cellList_buff = realloc((char *)__cellList_buff,__CcellList_buff*sizeof(char));
                        if(!__cellList_buff)
                          return 0;
                      }
                      strcat(__cellList_buff, fileBuffer);
                      strcat(__cellList_buff, " ");
                      break;
                    }
                  }
                  (void) closedir(dirp2);
                }
              }
              else { /* Cadence Lib */
                sprintf(pathBuffer, "%s/%s", fullpath, dp->d_name);
                /* sprintf(fileBuffer, "%s", dp->d_name); */
                dirp2 = opendir(pathBuffer); /* open up each cell dir */
                if(dirp2) {
                  while ((dp2 = readdir(dirp2)) != 0) {
                    if(strcmp(dp2->d_name, ".") && strcmp(dp2->d_name, "..") && (dp->d_name[0] != '.')) {
                      sprintf(fileBuffer, "%s/%s/%s", pathBuffer, dp2->d_name, idmGetPattern(dp2->d_name,0));
                      if(osFileExists(fileBuffer)){
                        sprintf(fileBuffer, "%s", dp->d_name);
                        /* add cell name if any view directory contains master.tag */
                        if ((strlen(__cellList_buff)+strlen(fileBuffer)+2) >= __CcellList_buff) {
                          __CcellList_buff = 2*__CcellList_buff + (strlen(fileBuffer)+2);
                          __cellList_buff = realloc((char *)__cellList_buff,__CcellList_buff*sizeof(char));
                          if(!__cellList_buff)
                            return 0;
                        }
                        strcat(__cellList_buff, fileBuffer);
                        strcat(__cellList_buff, " ");
                        break;
                      }
                    }
                  }
                  (void) closedir(dirp2);
                }
              }
            } else { 
	     /***** 
	      ***** NOT WILD NAME
	      *****/
              if(!strcmp(libtype, "byView")){
  	        sprintf(fileBuffer, "%s", dp->d_name);
		if(tmp = strrchr(fileBuffer, '.')){
		  *tmp = '\0';
                  if(getLocker) {
                    strcpy(cellname, fileBuffer);
                    sprintf(fileBuffer, "%s/%s/%s", fullpath, name, dp->d_name);
                    strcpy(outstring, "");
                    if(idb_get_locker(library, cellname, name, fileBuffer, getLocker)) {
                      strcpy(fileBuffer, outstring);
                    } else {
                      strcpy(fileBuffer, cellname);
                    }
                  }
                  /* check for size of cellList_buff */
                  if ((strlen(__cellList_buff)+strlen(fileBuffer)+2) >= __CcellList_buff) {
                    __CcellList_buff = 2*__CcellList_buff + (strlen(fileBuffer)+2);
                    __cellList_buff = realloc((char *)__cellList_buff,__CcellList_buff*sizeof(char));
                    if(!__cellList_buff)
                      return 0;
                  }
                  strcat(__cellList_buff, fileBuffer);
                  strcat(__cellList_buff, " ");
                }
              } else {
                 
                if(!strcmp(libtype, "byCell"))
                  sprintf(fileBuffer, "%s/%s/%s.%s", fullpath, dp->d_name, dp->d_name, name);
                else
                  sprintf(fileBuffer, "%s/%s/%s/%s", fullpath, dp->d_name, name, idmGetPattern(name,0)); 
 if (debugDmsOn) {
     dmsPrintIdmsLog("*IDMS - idb_gen_obj_rel #4 fileBuffer=%s\n", fileBuffer);
 }

                strcpy(outstring, "");
                if(idb_get_locker(library, dp->d_name, name, fileBuffer, getLocker)){
                  strcpy(fileBuffer, outstring); 
                  if ((strlen(__cellList_buff)+strlen(fileBuffer)+2) >= __CcellList_buff) {
                    __CcellList_buff = 2*__CcellList_buff + (strlen(fileBuffer)+2);
                    __cellList_buff = realloc((char *)__cellList_buff,__CcellList_buff*sizeof(char));
                    if(!__cellList_buff)
                      return 0;
                  }
                  strcat(__cellList_buff, fileBuffer);
                  strcat(__cellList_buff, " ");
                }
                else {
                      if (debugDmsOn) {
                          dmsPrintIdmsLog("*IDMS - idb_gen_obj_rel - NO BANANAS for #4 fileBuffer=%s\n", fileBuffer);
                      }

                }
               
              }
              
            }
          }
        }
        (void) closedir(dirp);
        if (debugDmsOn) {
           dmsPrintIdmsLog("*IDMS - idb_gen_obj_rel - __cellList_buff=%s\n", __cellList_buff);
        }
        return __cellList_buff;
      }
    }
  } else { 
/****** 
 ****** Given CellName - Return a list of ViewNames 
 ******/
    if(wild_name==1) /* NOT implemented to return viewList if cellname give is wild. */
      return 0;
    
    /* allocate size to some big number (BUFSIZE) first */
    if (!__viewList_buff)
      if (!(__viewList_buff = (char *)malloc(__VviewList_buff = BUFSIZE ))) { 
        __VviewList_buff = 0;
        return 0;
      }
      else{
        __viewList_buff[0] = '\0';
      }

    if(!strcmp(libtype, "byNone")){
      return getListbyName(fullpath, library, name, isCellName, getLocker);
    } else {
      if(!strcmp(libtype, "byView"))
        sprintf(pathBuffer, "%s", fullpath);
      else
        sprintf(pathBuffer, "%s/%s", fullpath, name);
      
      dirp = opendir(pathBuffer);
      if(dirp) {
        __viewList_buff[0] = '\0'; 
        while ((dp = readdir(dirp)) != 0) {
          if(strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..") && (dp->d_name[0] != '.')) {
            
            if(!strcmp(libtype, "byCell")){
              if(tmp = strrchr(dp->d_name, '.')){
                *tmp++;
                /* check for size of viewList_buff */
                if ((strlen(__viewList_buff)+strlen(tmp)+2) >= __VviewList_buff) {
                  __VviewList_buff = 2*__VviewList_buff + (strlen(tmp)+2);
                  __viewList_buff = realloc((char *)__viewList_buff,__VviewList_buff*sizeof(char));
                  if(!__viewList_buff)
                    return 0;
                }
                strcat(__viewList_buff, tmp);
                strcat(__viewList_buff, " ");
              }
            } else {
              if(!strcmp(libtype, "byView"))
                sprintf(fileBuffer, "%s/%s/%s.%s", fullpath, dp->d_name, name, dp->d_name);
              else
                sprintf(fileBuffer, "%s/%s/%s/%s", fullpath, name, dp->d_name, "master.tag");
              if(osFileExists(fileBuffer)){
                /* check for size of viewList_buff */
                if ((strlen(__viewList_buff)+strlen(dp->d_name)+2) >= __VviewList_buff) {
                  __VviewList_buff = 2*__VviewList_buff + (strlen(dp->d_name)+2);
                  __viewList_buff = realloc((char *)__viewList_buff,__VviewList_buff*sizeof(char));
                  if(!__viewList_buff)
                    return 0;
                }
                strcat(__viewList_buff, dp->d_name);
                strcat(__viewList_buff, " ");
                
              }
            }
            
          }
        }
        (void) closedir(dirp);
        return __viewList_buff;
      }
    }
  }
  return 0;
}




/*  JUNE 2004:
    "getLocker == 2" is added to output strings with complete cell information.
    "getLocker == 3" is added to output strings with complete cell information, but no header in idb_gen_obj.
*/
int idb_get_locker(const char*library, const char*cellname, const char*viewname, char *fpath, int getLocker)
{
  char *ckoutVer; 
  char locker[1024];
  struct stat fileStat;
  ushort bmode = 0;
  int res = 1;

  char version[1024];
  int locked = 0;
  char cpath [BUFSIZE];
  HRESULT hresult;
  char *dest=NULL;
  int length = 0;

  char tmp[2048];
  char linkName[2048];
  size_t s = 2048;
  memset(linkName, '\0',s); /* initialize for readlink. */


  if(!fpath || (fpath[0] == '\0') ) {
    if (debugDmsOn) {
       dmsPrintIdmsLog("*IDMS - idb_get_locker exiting because of null path: %s %s %s\n", library, cellname, viewname);
    }
    return 0;
  }
  strcpy(locker, "NONE");
  strcpy(version, "");

  int found = 0;
  FILE *f;
  int stat_err = stat(fpath, &fileStat);
  if (stat_err == 0) {
     found = 1;
  }
  else {
      if (f = fopen(fpath, "r")) {
         found = 1;
         fclose(f);
      } 
  }
  if (found) {
    if(getLocker) {
#ifndef OH_MACH_LINUX 
      bmode = fileStat.st_basemode;
#else
      bmode = fileStat.st_mode;
#endif 
      res = bmode & 146;
      if(res == 0) {
        locked = 0;
      } else {
        locked = 1;
      }

      if(getLocker==1) {
        if (locked ==0) {
          sprintf(outstring, "%s:NONE", cellname);
        } else {
          if((getuid()==fileStat.st_uid) && userName)
            sprintf(outstring, "%s:%s",cellname,userName);
          else
            sprintf(outstring, "%s:somebody",cellname);
        }
      } else if(getLocker==2 || getLocker==3) {  /* Get Complete Cell Info */
        /* get collection path for cadenceLib. */
        if(!strcmp(idb_get_libtype(library), "cadenceLib")) {
          cpath[0] = '\0';
          dest = strrchr(fpath, '/');
          length = dest - fpath;
          if((dest != NULL)&&(length!=0)) {
            strncpy(cpath, fpath, length);
            cpath[length] = '\0';
            sprintf(cpath, "%s.sync.cds", cpath);
          } else {
            strcpy(cpath, fpath);
          }
        } else {
          strcpy(cpath, fpath);
        }

	/* get Version */
        hresult = sapiUrlGetprop(NULL, cpath, "Version", NULL, &dssResults);
        if(SUCCEEDED(hresult) && dssResults[0] && strcmp(dssResults[0], "")) {
          strcpy(version, dssResults[0]);
        } else { /* Cell does NOT exist */
            if (debugDmsOn) {
                dmsPrintIdmsLog("*IDMS - idb_get_locker -sapiUrlGetprop returns cell %s does not exist\n", cpath);
            }
            return 0;
        }

        /* if locked -> get locker */ 
        if(locked) {
            hresult = sapiUrlGetprop(NULL, cpath, "locked", NULL, &dssResults);
            if(SUCCEEDED(hresult) && dssResults[0] && strcmp("0", dssResults[0]))
              strcpy(locker, dssResults[0]);
            
            /* return fullpath from WorkDir if it's checked out by user. */
            sprintf(tmp,"%s%s.%s",dmsGetWorkingDir((char*)viewname,0),(char*)cellname,(char*)viewname);
            if(!strcmp(locker, userName) &&
               ((getenv("IDMS_CO_COPY")&&osFileExists(tmp)) || 
                ((0< readlink(tmp, linkName, s))&&!strcmp(linkName,fpath)))) {
              strcpy(fpath, tmp);
	    }
        }

        sprintf(outstring, "%s:%s:%s:%s:%s",cellname,viewname,version,fpath,locker);
      }
    } else {
      strcpy(outstring, cellname);
    }
  } else {
      if (debugDmsOn) {
          dmsPrintIdmsLog("*IDMS - idb_get_locker - stat() for fpath %s returns %d (%s) \n", fpath, stat_err, strerror(stat_err));
      }
      //fprintf(stderr, "\n*IDMS ERROR* -- stat() for fpath %s returns %d (%s) \n", fpath, stat_err, strerror(stat_err)); 
      return 0;
  }
    
  return 1;
}

int ddGetObjType(void *id)
{
  if(!idb_get_libpath((char *)id)) /* not a lib in list */
    return 0; /* ddUndefinedType */
  else
    return ddLibType;
}
char *ddGetObjWritePath(void *id)
{
  if(!strrchr((char*)id,'/'))
    return (char *)(idb_get_libpath((char *)id));
  else
    return (char *)id;
}
char *ddGetObjReadPath(void *id)
{
  if(!strrchr((char*)id,'/'))
    return (char *)(idb_get_libpath((char *)id));
  else
    return (char *)id;
}
/* creates an Intel Library with type NONE. */
void *ddCreateLib(char *libName, char *libpath)
{
  if(!idb_get_libpath(libName)){ /* not found in list, then create */
    if(osCreateDir((char *)libpath)){
      /***  TO DO: create .type.lib file in this dir */
      idb_add_libs(libName, libpath, "byNone");
      return (void *)libName;
    }
    else
      return (void *)0;
  } 
}
char *dbGetLibName(void *id)
{
  if(!idb_get_libpath((char *)id))
    return 0;
  else
    return (char *)id;
}
char *ddGetObjName(void *id)
{
  /* redo - Given obj_id, return its name */
  return (char *)id;
}
int ddIsId(void *id)
{
  if(!id)
    return FALSE;
  else
    return TRUE;
}
int  ddUpdateLibList(void)
{
  return 1;
}
/* Deletes any objId from the disk */
int  ddDeleteObj(void *objId)
{
  /* needs to determine if it's a library or a file.... */
  return 1;
}


char *nmpMapName(const char *fromSpaceName,
                 const char *toSpaceName,
                 const char *identifier,
                 void* allocFunc, void *allocData)
{
  return (char *)0;
}
  

/* this is for voWarn to keep track of warning messages */
static char *_warning_message=0;
static int  _SIZE_warning_message=0;

/* append msg to warning message */
void voWarn(const char *msg)
{
   char *tmp;
   if (!_warning_message){
      if ((_warning_message = (char *)malloc(_SIZE_warning_message = MAXBUFSIZE)) == 0){
         _SIZE_warning_message = 0;
         return;
      }
      _warning_message[0] = '\0';
   }
   if (!msg || (msg[0]=='\0'))
      return;
   if ((strlen(_warning_message)+ strlen(msg)) >= (_SIZE_warning_message - 1)){
         if (!(tmp = strdup(_warning_message)))
            return;
         free(_warning_message);
         _warning_message = (char *)malloc(_SIZE_warning_message =
                      strlen(_warning_message) + strlen(msg) + 1);
         if (!_warning_message){
            _SIZE_warning_message = 0;
            return;
         }
         strcpy(_warning_message, tmp);
         free(tmp);
      }
   strcat(_warning_message, msg);
}
/* return warning message */
char *voGetWarn(void)
{
  return _warning_message;
}
/* clear the warning message */
void voFlushWarn(void)
{
  free(_warning_message);
  _warning_message=0;
}
/* append msg to part of the last message in dmsC_API_new.c */
void voInfo(const char *msg)
{
  dmsCAppendToLastMessage(msg);
}

#endif

#ifndef OH_MACH_LINUX
#ifdef IC445
void _Isinf() {
printf("Accessing Isinf\n");
raise(SIGABRT);
}
#endif
#endif

#ifdef DISABLE_SYNCHRONICITY
HRESULT sapiMakeStrArray (char*** strArray, ...)
{
  return E_FAIL;
}

HRESULT sapiTag (void* extra,
                 char** fileList,
                 char* tag,
                 char** inputOptions,
                 char*** results)
{
  return E_FAIL;
}

HRESULT sapiUrlTags (void* extra,
                     char* webObject,
                     char** inputOptions,
                     char*** results)
{
  return E_FAIL;
}

HRESULT sapiExecuteCmd (char* tclCmd, char** result)
{
  return E_FAIL;
}

HRESULT sapiAddStrings (char** strArray, ...)
{
  return E_FAIL;
}

HRESULT sapiUrlVault (void* extra,
                      char* webObject,
                      char** inputOptions,
                      char*** results)
{
  return E_FAIL;
}

HRESULT sapiSetvault (void* extra,
                      char* vaultDirURL,
                      char* localDir,
                      char** inputOptions,
                      char*** results)
{
  return E_FAIL;
}

HRESULT sapiPopulate (void* extra,
                      char** inputOptions,
                      char*** results)
{
  return E_FAIL;
}

HRESULT sapiRmfolder (void* extra,
                      char** folderList,
                      char** inputOptions,
                      char*** results)
{
  return E_FAIL;
}

HRESULT sapiRmvault (void* extra,
                     char** vaultList,
                     char** inputOptions,
                     char*** results)
{
  return E_FAIL;
}

HRESULT sapiRmfile (void* extra,
                    char** fileList,
                    char** inputOptions,
                    char*** results)
{
  return E_FAIL;
}

HRESULT sapiRmversion (void* extra,
                       char** versionList,
                       char** inputOptions,
                       char*** results)
{
  return E_FAIL;
}

HRESULT sapiUrlVersionid (void* extra,
                          char* webObject,
                          char** inputOptions,
                          char*** results)
{
  return E_FAIL;
}

HRESULT sapiUrlFetchedstate (void* extra,
                             char* webObject,
                             char** inputOptions,
                             char*** results)
{
  return E_FAIL;
}

HRESULT sapiUrlGetprop (void* extra,
                        char* webObject,
                        char* propName,
                        char** inputOptions,
                        char*** results)
{
  return E_FAIL;
}

HRESULT sapiUrlEnsurepathname (void* extra,
                               char** versionList,
                               char** inputOptions,
                               char*** results)
{
  return E_FAIL;
}

HRESULT sapiUrlExists (void* extra,
                       char* webObject,
                       char** inputOptions,
                       char*** results)
{
  return E_FAIL;
}

HRESULT sapiUrlVersions (void* extra,
                         char* webObject,
                         char** inputOptions,
                         char*** results)
{
  return E_FAIL;
}

HRESULT sapiCheckout (void* extra,
                      char** webObjectList,
                      char** inputOptions,
                      char*** results)
{
  return E_FAIL;
}

HRESULT sapiCheckin (void* extra,
                     char** webObjectList,
                     char** inputOptions,
                     char*** results)
{
  return E_FAIL;
}

HRESULT sapiCancel (void* extra,
                    char** webObjectList,
                    char** inputOptions,
                    char*** results)
{
  return E_FAIL;
}

HRESULT sapiUrlSetprop (void* extra,
                        char* webObject,
                        char* propName,
                        char* propValue,
                        char** inputOptions,
                        char*** results)
{
  return E_FAIL;
}

HRESULT sapiInitialize (int argc, char** argv)
{
  return E_FAIL;
}

void sapiShutdown ()
{

}

HRESULT sapiUrlRetired (void* extra,
                        char* webObject,
                        char** inputOptions,
                        char*** results)
{
  return E_FAIL;
}
HRESULT sapiRetire (void* extra,
                    char** webObjectList,
                    char** inputOptions,
                    char*** results)
{
  return E_FAIL;
}

HRESULT sapiFreeStrArray (char** strArray)
{
  return E_FAIL;
}
 

#endif
