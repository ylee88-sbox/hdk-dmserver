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
# $Id: dmsC_internal_functions.c,v 1.1.2.51 1998/09/10 23:40:20 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

/*
//----------------------------------------------------------------------------
//            (C) Copyright Intel Corporation, 1994
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
//        Intel Design Technology Copyright reserved 1994
//        Date: 04/14/97
//
//        Authors: Sorin Blebea
//
//        File: dmsC_internal_functions.c -- functions used internally by idms
//                  
//        Whenever possible they were implemented in ANSI C (translated from
//        an ancient K&R very bad written C).
//        All the functions in this file should pass a code review.
//        Some of them are weird because of the huge amount of bad 
//        legacy code inherited.
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/types.h>
#include <time.h>

#include <dirent.h>
#include <unistd.h>
#include <fnmatch.h>

#include <dmsC_defines.h>
#include <dmsC_reports.h>
#include <dmsC_functions2.h>
#include <dmsC_internal_functions.h>
#include <fwierr.h>
#include <fwimsg.h>


#include <idb_functions.h>
#include <os_functions.h>

/*-----------------------*
 * Global variables used *
 *-----------------------*/

#include <dmsC_new_vars.h>
#include <dmsC_new.h>
#include <dmsC_status.h>

/*------------------*
 * Static variables *
 *------------------*/

static char *_tmpBuff1 = 0;
static int  _LtmpBuff1 = 0;
static int  simplifyLibs = 0;

/*---------------------------*
 * Internally used functions * 
 *---------------------------*/

/*-----------------------*
 * Functions definitions *
 *-----------------------*/
 int isCadenceLib(char *);
 char** findCadenceLibs(FILE *, char *, int *,int *, char **);
 int isIntelLib(char *);
 char *getIntelLibType(const char *);

/*----------------*
 * parser related *
 *----------------*/

/*-----------------------------------------------------------------------
 * dmsParsePath
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars: 
 *    Global vars: ALL THE PARSER VARS
 *
 * Uses:
 *    Global var: 
 *
 *
 * Description:  calls the yacc parser after clearing previous data structs
 *-----------------------------------------------------------------------*/


static int deallocate_info_struct(INFO *table, int *pcount)
{
   int i; 
   struct elem *ptr;
 
   if (!*pcount){
      return 0;
   }
   for (i = 0; i < *pcount; i++){
      if (table[i].name)
         free(table[i].name);
      ptr = table[i].next;
      while(ptr){
         if (ptr->dirLib)
            free(ptr->dirLib);
         if (ptr->absPath)
            free(ptr->absPath);
         ptr = ptr->next;
      }
   }
   *pcount = 0;
   return 1;
}


static int deallocate_parser_structs()
{
   deallocate_info_struct(var, &varCount);
   deallocate_info_struct(worklib, &worklibCount);
   deallocate_info_struct(workdir, &workdirCount);
   deallocate_info_struct(tconfig, &tconfigCount);
   deallocate_info_struct(libpath, &libpathCount);
   deallocate_info_struct(rwpaths, &rwpathsCount); 
   return 1;
}


static int reset_parser_vars()
{
   incCount=0;
   includeDepth=0;
   appendType=0; 
   dmsIsOn=0;
   errorCount=0;
   dmsLineNumber=0;
   resultBuf[0] = '\0';
   return 1;
}


int deallocate_all_structs()
{
   FREE_NZ(dmsAppName);
   FREE_NZ(toolName.name);
   return 1;
}

/** for lib name mapping from . to #2e **/
/* Do you have access to an nmp.h in the itk? If so use it. If not: */
#define NMP_MAX_STR 2048
extern char *nmpMapName(const char *fromSpaceName,
                         const char *toSpaceName,
 			const char *identifier,
/*memAllocFuncs*/       void* allocFunc, void *allocData);

char libNameInLibraryUnix[NMP_MAX_STR];
char libBuf[SUPERBUFSIZE];

/*******************************************************************
* For 4.4: This function creates a new cds.lib from parsing
* the dms.pth file. Thus appending to previous paths does not
* arise at this point. -sh 1/13/00 
*******************************************************************/
void dmsSetSearchPath()
{
   time_t timer;
   int counter = 0; /* number in path */
   int max = 0;
   int j =0;
   int i = findPolicyEntry(libpath, libpathCount, toolName.name, "*");
   OPERATION *ptr = libpath[i].next;
   char **path = 0;
   FILE *filePtr;
   char *cdslibPath, *libName;
   int hasSeen = 0;
   char *simplified_path = 0; 
   
   /* in case the new path has the same library name as old path does */
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
   char *libType;
#endif
     cdslibPath = idb_get_startup("cds.lib");
     if(getenv("DISABLE_IDMS_CDSLIB")){
       filePtr=fopen("/dev/null","w");
     } else {
       libBuf[0] = '\0';
       if(osFileIsRW(cdslibPath)){
         filePtr = fopen(cdslibPath, "w");
       }
       else{
         filePtr = fopen("./cds.lib", "w");
       }
     }
     

     if(filePtr) {
       time(&timer);
       fprintf(filePtr, "-- Created by %s using idms --\n", osGetUsername());
       fprintf(filePtr, "-- %s\n", ctime(&timer));
       /* create an array to store path */
       while(ptr){
         max++;
         ptr=ptr->next;
       }
       ptr = libpath[i].next;
       max = max*2;
       path = (char **)malloc(max*sizeof(char *));
       if(!path){
         fprintf(filePtr, "out of memory in setSearchPath\n");
         return;
       }
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
       idb_clear_libs(max);  /* init. libName, libPath in idb for LINUX port */
#endif     
       while (ptr) {
         if (ptr->dirLib) {
           simplified_path = (simplifyLibs)?idb_simplify_file_name(ptr->dirLib,NULL):ptr->dirLib;
           if(counter != 0){
             hasSeen = 0;
             for(j=0; (j<counter) && !hasSeen; j++){
               if( simplified_path && path[j] ){
                 if(!strcmp(simplified_path, path[j]) &&
                    (strlen(simplified_path) == strlen(path[j])))
                   hasSeen = 1;
               }
             }
           }
           if(!hasSeen){
             if( simplified_path ){
               if(counter < max){
                 path[counter] = strdup(simplified_path );
                 counter++;
               } else { /* resize the array */
                 max = 2*max;
                 path = realloc((void *)path,max*sizeof(char *));
                 if(path){
                   path[counter] = strdup(simplified_path );
                   counter++;
                 }
               }
           
               if(!strcmp(simplified_path, ".")) {
                 path=findCadenceLibs(filePtr, simplified_path,&max,&counter,path);
               }
               else {
                 if(isCadenceLib(simplified_path)) { 
                   libName = idb_strip_last_token(simplified_path, "/");
                   if(libName) {
                     if(nmpMapName("CDBA","LibraryUnix", libName,0, libNameInLibraryUnix)){
                       fprintf(filePtr, "DEFINE %s %s\n", libNameInLibraryUnix,simplified_path); 
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
                       idb_add_libs(libNameInLibraryUnix, simplified_path,"cadenceLib");
#endif                     
                     } else {
                       fprintf(filePtr, "DEFINE %s %s\n", libName, simplified_path);
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
                       idb_add_libs(libName,simplified_path,"cadenceLib");
#endif                     
                     }
                   }
                 }
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
                 else if(isIntelLib(simplified_path)) {
                   libName = idb_strip_last_token(simplified_path, "/");
                   if(libName) {
                     fprintf(filePtr, "DEFINE %s %s\n", libName, simplified_path);
                     libType = getIntelLibType(simplified_path);
                     if (libType) {
                       idb_add_libs(libName,simplified_path,libType);
                     } else {
                       fprintf(stderr,"Could not open library control file .type.lib in path %s\n",simplified_path);
                     }
                   }
                 }
#endif               
                 else
                   path=findCadenceLibs(filePtr, simplified_path,&max,&counter,path);
               } 
             }
           }
         }
         ptr=ptr->next;
       }
       idb_capture_dm_vendor_warning();
       fclose(filePtr);
     }
     
   

   /* refresh all libraries will make sure new path is activate */
   idb_refresh_all_libraries();
   /* deallocate memory */
   for(j=0; j<counter; free(path[j++]));
   free(path);   
}


void dmsParsePath(char *dmsPathFile, int *noError)
{
  deallocate_parser_structs();
  reset_parser_vars();

  if (!dmsPathFile)
     dmsPathFile = "allPolicyUseDefault";
  errorCount=dmsYaccParser(dmsPathFile);
  if (dmsPathFileName)
    free(dmsPathFileName);
  dmsPathFileName = strdup(dmsPathFile);

  if (debugDmsOn) {
    dmsPrintIdmsLog("*IDMS DEBUG* errorCount=%d\n",errorCount);
    dmsPrintIdmsLog("*IDMS DEBUG* incCount=%d\n",incCount);
    dmsPrintIdmsLog("*IDMS DEBUG* maxDepth=%d\n",maxDepth);
  }

  dmsSetSearchPath();
  dmsSetToolConfig();

  if (debugDmsOn) {
    dmsPrintIdmsLog("*IDMS DEBUG* Library Search Path: %s\n", dmsGetLastSearchPath());

    /* another masterpiece of the ancient code */

    dmsFormPrintPath("*", "*");
    dmsPrintIdmsLog("*IDMS DEBUG* %s\n", dmsCGetLastMessage());
  }

  if (errorCount) {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, DMSPATHERROR, resultBuf);
    *noError=FALSE;
  } else {
    if (!strcmp(dmsPathFile, "allPolicyUseDefault")) {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, DMSENVNOTSET);
      *noError=FALSE;
    } else {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, DMSPATHOK, resultBuf);
      *noError=TRUE;
    }
  }
}



/*---------------------*
 * Global vars related *
 *---------------------*/


/*-----------------------------------------------------------------------
 * dmsAppGetEnv
 *
 * Modifies:
 *    Static vars:  _tmpBuff1, _LtmpBuff1
 *
 * Uses:
 *    Global var: dmsAppName
 *
 *
 * Description: 
 *    tries first to look for a variable in form <dmsAppName><envVarName>
 *    if not succesfull, reverts to normal getEnv
 *-----------------------------------------------------------------------*/

char * dmsAppGetEnv(const char *envVarName)
{
   static char *_empty = "";
   char *ret_str = 0;
   int rc;
   if (!dmsAppName)
      if (toolName.name)
         dmsAppName = strdup(toolName.name);
      else
         dmsAppName = _empty;
   rc = check_for_buff_space(&_tmpBuff1, &_LtmpBuff1, 
            strlen(dmsAppName) + strlen(envVarName) + 1);

   strcpy(_tmpBuff1,dmsAppName);
   strcat(_tmpBuff1,envVarName);
   if ((ret_str = getenv(_tmpBuff1)) == 0)
      ret_str = getenv(envVarName);
   return ret_str;
}


/*-----------------------------------------------------------------------
 * dmsSetToolConfig 
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars: 
 *    Global vars: toolConfName
 *
 * Uses:
 *    Global var:
 *
 *
 * Description: 
 *-----------------------------------------------------------------------*/

void dmsSetToolConfig()
{
  int i;

  /* in case the new path has the same library name as old path does */
  /* refresh all libraries will make sure new path is activate */
  dmsCRefreshAllLib(0);

  i = findPolicyEntry(tconfig, tconfigCount, toolName.name, "");
  if (!dmsSetLastToolConfName(tconfig[i].next->dirLib)){
     dmsCPrintError(" cannot set tool config (mem alloc)");
     return;
  }
  if (debugDmsOn);
     dmsPrintIdmsLog("*IDMS DEBUG* Tool Configuration: %s\n",
                     dmsGetLastToolConfName);
}


/*-----------------------------------------------------------------------
 * dms_get_workdir_nosimplify() 
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
 * Description: Returns the workdir for a tool and view without calling
 *              the expensive link solving. If last param is null returns
 *              directly a pointer in the DS. DO NOT DEALLOCATE THE RESULT!
 *
 *-----------------------------------------------------------------------*/

static char *_wempty = "";

char *dms_get_workdir_nosimplify(char *tName, char *vName, char *wdir)
{
   int i;
   if ((!tName) || (!vName))
      return _wempty;
   i = findPolicyEntry(workdir, workdirCount, tName, vName);   
   if (!wdir)
      return(workdir[i].next->dirLib)?(workdir[i].next->dirLib):_wempty;
   else 
      return(workdir[i].next->dirLib)?(strcpy(wdir, workdir[i].next->dirLib)):
         _wempty;
}


/*------------------------------------------------------------------------
 * dmsC_deallocate_vars
 *
 * Modifies: _tmpBuff1, _LtmpBuff1
 *
 * Description:
 *    deallocates the static vars used. Sort of a C destructor
 *------------------------------------------------------------------------*/

int dmsC_deallocate_vars()
{
   if (_LtmpBuff1){
      free(_tmpBuff1);
      _tmpBuff1 = 0;
      _LtmpBuff1 = 0;
   }
   return 0;
}


/*------------*
 * DM Related *
 *------------*/

/*-----------------------------------------------------------------------
 * dmsGenLibNameList 
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
 * Description: Generates a library list fitting the pattern. If vName is
 *              wild card, it pulls in the libraries using
 *              idb_append_to_lib_list(), otherwise attempts to use the info
 *              from rwpaths. 
 *              If libNameList is NULL, uses internal buffer
 *-----------------------------------------------------------------------*/


static char *_ln1 = 0;
static int _Lln1 = 0;

char *dmsGenLibNameList(char *pattern, char *vName, char *outputList)
{
   struct elem *ptr;
   int use_int_buff=0;
   char *gen_lib_buff = 0;
   char *tmp;
   char *libNameList = outputList;
   
   if (is_wild_exp(vName)) /* undetermined view name */
      return idb_append_to_lib_list(pattern, libNameList, 1); /* clear buff first */

   /* Make sure have allocated buffer if needed */

   if (!libNameList){
      use_int_buff = 1;
      if (!_ln1)
         if (!(_ln1 = (char *)malloc(_Lln1 = BUFSIZE))){
            _Lln1 = 0;
            return 0;
         }
      libNameList = _ln1;
   }
   libNameList[0] = '\0';
  
   ptr = 
     rwpaths[findPolicyEntry(rwpaths, rwpathsCount, toolName.name, vName)].next;

   while (ptr) {
      switch (ptr->opt % 10) {
      case RW_opt:
      case RO_opt:
         if (*(ptr->dirLib) == '\0'){
            if (!gen_lib_buff){
               if (!use_int_buff)
                  idb_append_to_lib_list(pattern, libNameList, 0);
               else{
                  gen_lib_buff = idb_append_to_lib_list(pattern, 0, 0);
                  if (!gen_lib_buff)
                     return 0; /* failed to alloc buffer */
                  if ((strlen(gen_lib_buff) + strlen(libNameList) + 2) >= _Lln1){
                     if (!(tmp = strdup(libNameList)))
                         return 0;
                     if (!(_ln1 = (char *)malloc(_Lln1 =
                        strlen(libNameList) + strlen(gen_lib_buff) + 2 + BUFSIZE))){
                        _Lln1 = 0;
                        return 0;
                     }
                     libNameList = _ln1;
                     strcpy(libNameList, tmp);
                     free(tmp);
                  }
                  strcat(libNameList, gen_lib_buff);
                  strcat(libNameList, " ");
               }
            }
         }
         else
            if (dmsMatchExpression(pattern, ptr->dirLib)) {
               if (use_int_buff){
                  if ((strlen(ptr->dirLib) + strlen(libNameList) + 2) >= _Lln1){
                     if (!(tmp = strdup(libNameList)))
                         return 0;
                     if (!(_ln1 = (char *)malloc(_Lln1 =
                        strlen(libNameList) + (!gen_lib_buff)?0:(strlen(gen_lib_buff)) + 2 
                        + BUFSIZE))){
                        _Lln1 = 0;
                        return 0;
                     }
                     libNameList = _ln1;
                     strcpy(libNameList, tmp);
                     free(tmp);
                  }
               }
               strcat(libNameList, ptr->dirLib);
               strcat(libNameList, " ");
            }
         break;
      }
      ptr=ptr->next;
   }
   if(libNameList && outputList){ 
     strcpy(outputList,libNameList);
   }
   return libNameList;
}


/*-----------------------------------------------------------------------
 * find_lib_in_list 
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


/*
 * This function is looking for something like: 
 *   
 *      lib1[.cf1] lib2[.cf2] ... libn[.cfn]
 *      ^    ^  ^
 *      first|  |
 *           dot|
 *              last
 *
 *  Tries to find in all matching libraries the specd cell name, view name
 *
 */

static char *_lnam = 0;
static int   _Llnam= 0;
static char *_cnam = 0;
static int   _Lcnam= 0;
static int  to_extend = 1024; /* to avoid continuos realloc */

void *find_lib_in_list(char *libNameList, char *cName, char *vName, int *noError, char *fp)
{
   if (debugDmsOn) {
       dmsPrintIdmsLog("<<*C-CALL*>> find_lib_in_list(%s, %s, %s, ...)\n", libNameList, cName, vName);
   }
   void *plib_id = 0;
   int lnsize = 0;
   int cnsize = 0;
   int ready  = 0;
   char *p;
   char *first_chr = 0;
   char *dotpos = 0;
   char *last_chr = 0; /* points to the last char in library name */
   char *fullpath = 0;
   if (libNameList[0] == '\0')
      p = dmsGenLibNameList("*", vName, 0); 
   else
      p = libNameList;
   
   while (!ready){
      /* skip the spaces till next lib */
      while (*p)
        if (*p == ' ')
          p++;
        else
          break;
      if (*p == '\0') /* no more libraries left */
         break; 
      
      /* here we are pointed to the first char in the name */
      dotpos = 0;
      first_chr = p;
      while (*p){
         if (*p == ' ')
            break;
         if (*p == '.')
            dotpos = p;
         p++;
      } 
      if (dotpos){
         last_chr = dotpos-1; 
         cnsize = p-dotpos-1;
      }
      else{
         last_chr = p-1;
         cnsize = 0;
      }
      lnsize = (last_chr - first_chr) + 1;
      check_for_buff_space(&_lnam, &_Llnam, lnsize + 1 + to_extend);
      check_for_buff_space(&_cnam, &_Lcnam, cnsize + 1 + to_extend);
      if (lnsize <= 0)
         _lnam[0] = '\0';
      else{
         strncpy(_lnam, first_chr, lnsize);
         *(_lnam+lnsize) = '\0';
      }
      if (cnsize <= 0)
         dmsSetLastPathConfName("");
      else{
         strncpy(_cnam, dotpos+1, cnsize);
         *(_cnam+cnsize) = '\0';
         dmsSetLastPathConfName(_cnam);
      }
      if (!dmsMatchExpression(theLibName, _lnam))
         continue;

	 /**
      if (debugDmsOn)
         dmsPrintIdmsLog("*IDMS DEBUG* Library Search Path: [%s]\n",
                         idb_get_library_path());
	 **/
      if (!(plib_id = idb_find_lib_id(_lnam, 1, "r"))){
         idb_capture_dm_vendor_warning();
         continue;
      }
      if (debugDmsOn)
         dmsPrintIdmsLog("*IDMS DEBUG* Search Library: %s\n",
                          idb_get_library_name(plib_id)); 
      /* check cellview existence */
      fullpath = idb_find_cellview(plib_id, cName, vName);
      if (fullpath) {
	 strcpy(fp, fullpath);
         dmsSetLastLibName((char *)idb_get_library_name(plib_id));
         *noError=TRUE;
         if (debugDmsOn) {
            dmsPrintIdmsLog("*IDMS DEBUG* idb_find_cellview found %s in library %s\n", fullpath, (char *)idb_get_library_name(plib_id));
         }
     
         /* just make sure that we store the just found path */

         if (turbo_dmserver){
            dmsTInsertCellRecordAndWholeDir((char *)idb_get_library_name(plib_id), vName, cName, "", fp, 0, 0);
         }
         return plib_id;
     }
     else 
        idb_capture_dm_vendor_warning(); /* clear warning buffer and discard
                                            message */
   }
   *noError=idmsCvNotFound; 
   return idb_set_lib_id(0);
}

/*-----------------------------------------------------------------------
 * dms_find_lib_id 
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
 * Description: returns 1 if a library matching the params can be found
 *-----------------------------------------------------------------------*/


void *dms_find_lib_id(const char *operation, const char *dir_lib,
                     const char *cName, const char *vName, int *noError, char *fp)
{
   void *plib_id;

   if (!(plib_id = find_lib_in_list((char *)dir_lib, (char *)cName, (char *)vName, noError,fp)))
     return 0;
   
   if (!strcmp(operation, "w") && (*noError)){
      if (!(plib_id = idb_open_library(dmsGetLastLibName(), 
             idb_get_library_directory(plib_id, "w"), "w"))){
         idb_capture_dm_vendor_warning();
         dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, CANTWRITELIB, dmsGetLastLibName(), 
                        idb_get_library_directory(plib_id, "w"));
         *noError = idmsCvNotFound;
         dmsReportMsgIfAny((char *)dmsCGetLastMessage(), "");
         return 0;
     }
   }
   return plib_id;
}


/*-----------------------------------------------------------------------
 * open_config_if_exists
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:
 *    Global vars: dmsConfName
 *
 * Uses:
 *    Global var:
 *
 *
 * Description:
 *-----------------------------------------------------------------------*/

/* preserving the past -- for what is worth
 * the code uses a gozzillion of configuration  variables which of course are
 * not all necessary but are very difficult to clean at this time
 */


/***********
void open_config_if_exist(void *plib_id, char *pathConfName, char *theConfName,
                          const char *toolConfName)
{
   * priority: theConfName -> pathConfName -> toolConfName *
   if (idb_get_config_id())
      idb_close_config(plib_id);
   idb_capture_dm_vendor_warning();
   if (debugDmsOn)
      dmsPrintIdmsLog("*IDMS DEBUG* Config(in use:%s the:%s path: %s tool:%s)\n",
                      dmsConfName, theConfName, pathConfName, toolConfName);
   if (pathConfName[0] == '\0')
      strcpy(pathConfName, toolConfName);
   if (strcmp(theConfName, "") != 0) { * not empty *

      if (strcmp(theConfName, "default")) * if is NOT the default config *
         strcpy(pathConfName, theConfName);

      * if pathConfName is not empty or "LATEST" get the one from library *

      if (strcmp(pathConfName,"") && strcmp(pathConfName, "LATEST")) {
         strcpy(dmsConfName, pathConfName);
         if (idb_find_config(plib_id, dmsConfName)) {
            idb_capture_dm_vendor_warning();
            idb_open_config(plib_id, dmsConfName);
            idb_capture_dm_vendor_warning();
         }
         else {
             idb_capture_dm_vendor_warning();
             strcat(dmsConfName, " (not exist)");
             idb_set_config_id(0);
         }
      }
      else {
         strcpy(dmsConfName, "");
         idb_set_config_id(0);
      }
   }
   else {
      strcpy(dmsConfName, "");
      idb_set_config_id(0);
   }
   if (debugDmsOn)
      dmsPrintIdmsLog("*IDMS DEBUG* Lib: %s Configuration: %s\n",
                      idb_get_library_name(plib_id), dmsConfName);
}
***********/


int dmsFindOSFile(char *dir, char *cName, char *vName, int call_get_ext)
{
   char *fn = dmsFormFileName(dir, cName, vName, call_get_ext);
   if (!fn)
      return 0;
   else
      return osFileExists(fn);
}


static char _dmsWorkDirBuff[BUFSIZE];

/*
 * This crap to emulate old idms which always return success - YACK!!!
 */

char *dmsGetWorkingDir(char *vName, char *dmsWorkingDir)
{
   OPERATION *op = workdir[findPolicyEntry(workdir, workdirCount,
                          toolName.name, vName)].next;
   char *simplify_path = 0;
   if (!dmsWorkingDir){
      dmsWorkingDir = _dmsWorkDirBuff;
      dmsWorkingDir[0] = '\0';
   }

   if (!op->absPath){
      if (!op->dirLib){
         if (!(op->absPath = strdup(".")))
            return 0;
      }
      else {
	 simplify_path = idb_simplify_file_name(op->dirLib, 0);
	 if (!simplify_path){ /* non-exisiting workdir */
	   if (!(op->absPath = strdup(op->dirLib)))
	     return 0;
         } else
           if (!(op->absPath = strdup(simplify_path)))
             return 0;
      }
   }
   strcpy(dmsWorkingDir, op->absPath);
   if(dmsWorkingDir && strcmp(dmsWorkingDir,""))
     strcat(dmsWorkingDir, "/");
   return dmsWorkingDir;
}


void compare_locations(DMSINFO *l1, DMSINFO *l2)
{
   if (l1->location != l2->location)
      printf("LC diff before/after: (%d)/(%d)\n", l1->location, l2->location);
   if (strcmp(l1->libPath, l2->libPath))
      printf("LP diff before/after: (%s)/(%s)\n", l1->libPath, l2->libPath);
   if (strcmp(l1->libName, l2->libName))
      printf("LN diff before/after: (%s)/(%s)\n", l1->libName, l2->libName);
   if (strcmp(l1->cellName, l2->cellName))
      printf("CN diff before/after: (%s)/(%s)\n", l1->cellName, l2->cellName);
   if (strcmp(l1->viewName, l2->viewName))
      printf("VN diff before/after: (%s)/(%s)\n", l1->viewName, l2->viewName);
   if (strcmp(l1->verName, l2->verName))
      printf("VS diff before/after: (%s)/(%s)\n", l1->verName, l2->verName);
   if (strcmp(l1->coUser, l2->coUser))
      printf("US diff before/after: (%s)/(%s)\n", l1->coUser, l2->coUser);
   if (strcmp(l1->fullPathName, l2->fullPathName))
      printf("FP diff before/after: (%s)/(%s)\n", l1->fullPathName,
              l2->fullPathName);
}

static char pathBuffer[BUFSIZE];

char** findCadenceLibs(FILE *filePtr, char *dirPath, int *max,int *counter, char **path)
{
  struct dirent *dp = 0;
#ifndef OH_MACH_LINUX
  struct __dirdesc *dirp = 0;
#else
  struct __dirstream *dirp = 0;
#endif
#if defined(NEW_LIB_STRUCTURE) || defined(OH_MACH_LINUX)
  char *libType;
  char *ptr=0;
  int hasBeenDefined =0;
#endif
  int hasSeen =0;
  int j;
  dirp = opendir(dirPath);

  if(dirp) {
    while((dp = readdir(dirp)) != NULL) {
      if(strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..") && strcmp(dp->d_name, ".SYNC")) {
        sprintf(pathBuffer, "%s/%s", dirPath, dp->d_name);
        if(isCadenceLib(pathBuffer)){
          /* check for duplication  */
          hasSeen = 0;
          for(j=0; (j<*counter) && !hasSeen; j++){
            if( pathBuffer && path[j] ){
              if(!strcmp(pathBuffer,path[j]) &&
                 (strlen(pathBuffer) == strlen(path[j])))
                hasSeen = 1;
            }
          }
          if(!hasSeen){
            if(*counter < *max){
              path[*counter] = strdup(pathBuffer);
              *counter = *counter + 1;
            } else { /* resize the array */
              *max = *max + *max;
              path = realloc((void *)path,(*max)*sizeof(char *));               
              if(path){
                path[*counter] = strdup(pathBuffer);
                *counter = *counter + 1;
              }
            }
            if(nmpMapName("CDBA","LibraryUnix", dp->d_name,0, libNameInLibraryUnix)){
              fprintf(filePtr, "DEFINE %s %s\n", libNameInLibraryUnix, 
                      pathBuffer);
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
              idb_add_libs(libNameInLibraryUnix,pathBuffer,"cadenceLib");
#endif
            } else {
              fprintf(filePtr, "DEFINE %s %s\n", dp->d_name, pathBuffer);
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
              idb_add_libs(dp->d_name,pathBuffer,"cadenceLib");
#endif
	    }
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
	    hasBeenDefined=1;
#endif
          }
        }
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
        else if(isIntelLib(pathBuffer)){
          /* check for duplication  */
          hasSeen = 0;
          for(j=0; (j<*counter) && !hasSeen; j++){
            if( pathBuffer && path[j] ){
              if(!strcmp(pathBuffer,path[j]) &&
                 (strlen(pathBuffer) == strlen(path[j])))
                hasSeen = 1;
            }
          }
          if(!hasSeen){
            if(*counter < *max){
              path[*counter] = strdup(pathBuffer);
              *counter = *counter + 1;
            } else { /* resize the array */
              *max = *max + *max;
              path = realloc((void *)path,(*max)*sizeof(char *));               
              if(path){
                path[*counter] = strdup(pathBuffer);
                *counter = *counter + 1;
              }
            }         
          
            fprintf(filePtr, "DEFINE %s %s\n", dp->d_name, pathBuffer);
            libType = getIntelLibType(pathBuffer);
            if(libType)
              idb_add_libs(dp->d_name,pathBuffer,libType);
            else
               fprintf(stderr,"Could not open library control file .typeLib in path %s\n",pathBuffer);
	    hasBeenDefined=1;
          }
        }
#endif        
      }
    }
    (void) closedir(dirp);
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
    /* if cannot find any library in libpath, then default this libpath dir to free-form library. */
    if(!hasBeenDefined){
      if(ptr = strrchr(dirPath, '/')) {
        ptr++;
	fprintf(filePtr, "DEFINE %s %s\n", ptr, dirPath);
	idb_add_libs(ptr, dirPath, "byNone");
  	dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, FILENOTFOUND, ".type", "lib", dirPath, "Default to Free Form library");	
	dmsCSetLastMessage("Default Directory to Free Form LIbrary");
      } 
    }
#endif
  }
  return path;  
}

/* returns TRUE if it is a Cadence Library -- library directory
   contains .cdsinfo.tag file.

   Adding also .oalib as a valid cadence lib identifier

 */
int isCadenceLib(char *fullpath) {
int isLib=FALSE;
struct __dirstream *dirp = NULL;
struct dirent *dp = NULL;

/*
 * make sure there is no .type.lib,
 * Cadence IC6x dropped the .oalib file when user has write permission
 * Not a Cadence lib if there is an Intel lib
 */
if(isIntelLib(fullpath)) return FALSE;

/* yes this is an assignment */
if(dirp = opendir(fullpath)) {
  while (!isLib && (dp = readdir(dirp))) {
   /* Make sure there is no .type.lib file */
   if ( !fnmatch("cdsinfo.tag", dp->d_name,0) || !fnmatch(".oalib", dp->d_name,0) )
      isLib = TRUE;
  }
  closedir(dirp);
}
return isLib;
}

/* returns TRUE if it is an Intel Library -- library directory
   contains .type.lib file.
 */
int isIntelLib(char *fullpath) {
  int isLib=FALSE;
#ifndef OH_MACH_LINUX
  struct __dirdesc *dirp = 0;
#else
  struct __dirstream *dirp = 0;
#endif
  struct dirent *dp = 0;

  dirp = opendir(fullpath);
  
  if(dirp) {
    while (!isLib && (dp = readdir(dirp)) != NULL) {
      if((fnmatch(".type.lib", dp->d_name,0)) == 0)
        isLib = TRUE;
    }
    (void) closedir(dirp);
  }
  return isLib;
}

static char fullpathBuffer[BUFSIZE];
static char type[BUFSIZE];

char *getIntelLibType(const char *fullpath) 
{
  FILE *fptr;
 
  sprintf(fullpathBuffer, "%s/%s", fullpath, ".type.lib");
  if( (fptr = fopen(fullpathBuffer, "r")) != 0) {
    fscanf(fptr, "%s", type);
    fclose(fptr);
    return type;
  }
  else
    return 0;
}

