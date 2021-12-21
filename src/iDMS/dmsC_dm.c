/*
############# Intel Corporation Confidential information. ############
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
# $Id: dmsC_dm.c,v 1.1.2.23 1997/12/13 00:50:39 sorin Exp $
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
//        File: dmsC_dm.c  -- 
//----------------------------------------------------------------------------
*/

/* whoever started writing dmserver can't write C for shits and has a nasty case of "the complicator's glove"
 * Not to mention thinking that foregoing prototypes was a good idea
 * The code is in a suicidally bad shape, all of it.
 * 1. Fixed sized buffers everywhere
 *    a. Excessively sized buffers everywhere
 * 2. Using system() in a setuid app instead of fork-exec/spawn
 * 3. Calling functions without prototypes
 */
/* Fix predecessor's incompetence, or whatever attributed to this */
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <libgen.h>
#include <dirent.h> /* for directory access */

#include <fwimsg.h>
#include <dmsC_dm.h>
#include <dmsC_new_vars.h>
#include <dmsC_defines.h>
#include <idb_functions.h>
#include <idb_codes.h>
#include <os_functions.h>
#include <string.h>
#include <dmsC_functions2.h>
#include <dmsC_status.h>
#include <dmsC_reports.h>
#include <table_functions.h>
#include <dmsC_internal_functions.h>
#include <dmsC.h>
#include <dmsC_new.h>
#include "../turbo/dmsT_API.h"
/** #include <dmsC_triggers.h> **/

#define DSSRESULT HRESULT
#define DSS_SUCCESS SUCCEEDED
#define DSS_FAIL FAILED
#define UPDATE 1

#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
static int dmsUnCheckOutCellView(char *cName, char *vName, char *verName,
                           char *FPN, int *noError);
#endif

/* EXTERN VARS */

extern int dmsKeepTmpCopy;

/*-----------------------------------------------------------------------
 * dmsPIInitDMS
 *
 * Parameters:
 *    appName
 *    dmsBootName
 *    dmsPath
 *    nullgtMode
 *    noError
 *
 * Modifies:
 *    Static vars:
 *    Global vars: idmsNotInitialized
 *
 * Uses:
 *    Global vars:
 *
 *
 * Description: I would have eliminated this function but it wasn't used
 *              only in dmsCPIInitDMS so for now it will still be alive
 *              but made static asap.   -- Sorin 07/18/97
 *-----------------------------------------------------------------------*/

void dmsPIInitDMS(char *dmsBootName, char *dmsPath, int nullgtMode,
                  int *noError)
{
   /* Will only initialize Cadence PI once */

   static char *dmspath_default_name = "allPolicyUseDefault";
                                             /* from the WEIRD past */
   int initStatus = FALSE;


   if (!dmsPath)
     dmsPath = dmspath_default_name;

   if (idmsNotInitialized) {
     initStatus = dmsCInitVendorDM(dmsBootName);
   }

   if(initStatus) { 
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, INITDMS, DMSVERSION);
     *noError = TRUE;
    }
   else {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, INITDMS, DMSVERSION);
     *noError = FALSE;
    }

   if(*noError) {
     idmRegisterTriggers(nullgtMode);
     dmsParsePath(dmsPath, noError);
   }
}

static char _dmsTmpFile[BUFSIZE];
static char _dummyVer[] = DUMMY;
static char _dummyLocker[] = DUMMY;

static char _cell_buff[1024];
static char _pverstr[SMALLBUF];

int dmsSearchCellView(int open_first, int cdba, char *cName, char *vName,
                      char *verName, char *FPN, char *locker, char **latestVer,
		      int *noError, int tmpCopyFile)
{
   /* add here the turbo section */

   if ((turbo_dmserver) && (!open_first) && (!cdba)){
      const char *cell_name = dmsTGetLastCellName();
      if (debugDmsOn){
         dmsPrintIdmsLog("*IDMS DEBUG* - turbo last: %s , current: %s %s \n", cell_name, cName, vName);
      }
      if (cell_name){
         strcpy(_cell_buff, cName);
         strcat(_cell_buff, ".");
         strcat(_cell_buff, vName);
         if (strcmp(cell_name, _cell_buff) == 0){
            const char *version = dmsTGetLastVersion();
            if (debugDmsOn){
               dmsPrintIdmsLog("*IDMS DEBUG* - turbo found %s version %s\n", _cell_buff, version);
            }
            if (latestVer) {
               strcpy(_pverstr, version);
               *latestVer = _pverstr;
            }   
            *noError = idmsCvInWorkdir;
            dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, DMSFILEFOUND, cName, vName, *latestVer,
                       dmsTGetLastLibraryName(), dmsConfName);
            return save_status(*noError, 0, 0, cName, vName, (char *)version, "", FPN,
                           DONT_CHANGE_CODES);
         }
      } 
   }

   char *rPath=NULL;
   void *plib_id = idb_get_lib_id();
   char tmp[2048];
   char linkName[2048];
   size_t s = 2048;
   memset(linkName, '\0',s); /* initialize for readlink. */

   if (debugDmsOn){
      dmsPrintIdmsLog("*IDMS DEBUG* Procedure Search\n");
      dmsPrintIdmsLog("<<*C-CALL*>> dmsSearchCellView(%d,%d,%s,%s,%s,%s,%s,...,%d)\n", 
                     open_first, cdba, cName, vName, verName, FPN, locker, tmpCopyFile);
   }

   /** we pass NULLs in place of locker and pverstr to 
   *** idb_find_version_and_locker to reduce url calls.
   *** The locker and ver is obtained in dmsGetStatus with the
   *** hope that calls to dmsCGetLastCellViewInfo < calls to dmsCInfoCellView
   **/
  if (!(rPath = idb_find_version_and_locker(plib_id,
                     cName, vName, verName, locker, FPN, 1, latestVer))){
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, VERSIONNOTEXIST, cName, vName, dmsGetLastLibName(),
                     verName);
                     return save_status(*noError = idmsCvNotFound, 0, 0,
                        cName, vName, "", "", "", DONT_CHANGE_CODES);
   }



   if(cdba == 1)
     *noError = idmsCvInWorklib;
   else
     *noError = idmsCvInWorkdir;	/* assumption */

   if (rPath){
     if((cdba==CELLVIEW_INFO) && locker && strcmp(locker, "")) { /* && open_fist ?? */
       sprintf(tmp, "%s%s.%s", dmsGetWorkingDir((char *)vName, 0), (char *)cName, (char *)vName);
       if(!strcmp(locker, userName) && 
          (( (0< readlink(tmp, linkName, s))&&!strcmp(linkName,rPath) ) ||
           ( getenv("IDMS_CO_COPY")&&osFileExists(tmp)))) { 
         /* there's a link from wkdir && the file is checked out by current user 
            try to return the full path to the workdir and not the library.
         */
         strcpy(FPN, tmp);
       }
     } else {
       sprintf(tmp, "%s%s.%s", dmsGetWorkingDir((char *)vName, 0), (char *)cName, (char *)vName);
       if(!strcmp(locker, userName) && 
          (((0< readlink(tmp, linkName, s))&&!strcmp(linkName,rPath)) ||
           (getenv("IDMS_CO_COPY")&&osFileExists(tmp)))){
         strcpy(FPN, tmp);
       }
     }
     
     if(tmpCopyFile) {	/* set with dmsKeepTmpCopy in idmb modules */
       sprintf(_dmsTmpFile,"%s/%s.%s", dms_tmp_dir, cName, vName,verName);
       if (osFileExists(_dmsTmpFile)) { /* remove old copy */
         chmod(_dmsTmpFile, 777);
         osRemoveFile(_dmsTmpFile);
       }
       osCopyFile(FPN, _dmsTmpFile); /*rename(FPN,_dmsTmpFile);*/
     }
     
     /* Make an explicit call to dmsCGetLastCellViewInfo to get the
     ** actual values */
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, DMSFILEFOUND, cName, vName, *latestVer,
                    dmsGetLastLibName(), dmsConfName);
     
     return save_status(*noError, 0, 0, cName, vName, verName, "", FPN,
                        DONT_CHANGE_CODES);
   }

   FPN[0] = '\0';

   dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, NOTINSTALLED, cName, vName, dmsGetLastLibName()); 
   *noError = idmsCvNotFound;
      
   return save_status(*noError, 0, 0, cName, vName, "", "", FPN,
                      DONT_CHANGE_CODES);
} 


int dmsReadCellView(int what, int checkout, int create, const char *cName,
                     const char *vName, const char *verName, 
                     const char *viewType, char *FPN,
                     void **pplib_id, int *noError, int change_codes)
{
   struct elem *ptr;
   int i;
   char *dmsWorkDir;
   char *full_path;
   char *simplify_path2;
   char *simplify_path;
   void *plib_id;
   char *new_ver = NULL; 
   char *lib_path;
   char linkName[2048];
   size_t s = 2048;
   char *eNameList = NULL;
   static char tmp[BUFSIZE];
   static char eList[BUFSIZE];
   static char _extbuff[BUFSIZE];
   char fp[BUFSIZE];
   int open_first = (checkout == coCANCEL);

   static char locker[BUFSIZE];
   strcpy(locker, "");

   if(debugDmsOn){
      dmsPrintIdmsLog("<<*C-CALL*>> dmsReadCellView(%d, %d, %d, %s, %s, %s, %s, %s, ..., %d)\n", 
                                                     what, checkout, create,
                                                     cName, vName, verName, viewType, FPN, change_codes);
   }

   idb_set_content_at_id(pplib_id, 0);

   /* for cell search only */
   if(create ==0 ) {
     if(idmGetExtName(toolName.name, (char *)vName, &eNameList)) {
	if(eNameList){
          strcpy(eNameList,vName);
	  if(debugDmsOn)
	    dmsPrintIdmsLog("*IDMS DEBUG* NOT using ExtNameList: %s \n", eNameList);
        }
     }
     else {
       if(debugDmsOn)
	 dmsPrintIdmsLog("*IDMS DEBUG* Using ExtNameList: %s \n", eNameList);
     }
   }
   strcpy(tmp, eNameList);

   i = findPolicyEntry(rwpaths, rwpathsCount, toolName.name, vName);
   if (debugDmsOn)
      dmsPrintIdmsLog("*IDMS DEBUG* %d.rwpaths %s:\n", i+1, rwpaths[i].name);

   ptr = rwpaths[i].next;
   while (ptr){
      if (debugDmsOn)
         dmsPrintIdmsLog("*IDMS DEBUG* \t%s %2d (%s)\n",
                         action[ptr->opt%10], ptr->opt,ptr->dirLib);
      switch (ptr->opt%10){
         case WORKDIR_opt:
            if ((what == READ_CDBA) || (checkout == coLOCK) || 
                (checkout == coCANCEL))                      
               break; /* skip unix dir for cdba, co, ci, cancel */
	    if (verName && strcmp(verName,"")) 
	       break; /* if verName is indicated, find in libraries */
            /* Loop thru extension list. */
            strcpy(eList, tmp);
            eNameList = eList;
            dmsSeparateByColon(eNameList,_extbuff, eNameList);
        while(strcmp(_extbuff,"")) {

            dmsWorkDir = dmsGetWorkingDir((char *)_extbuff, 0);
	    if(dmsWorkDir) {
		full_path = dmsFormFileName(dmsWorkDir, (char *)cName, 
			     (char *)_extbuff, CALL_GET_EXT);
		if (osFileExists(full_path)) {
		  strcpy(FPN, full_path);
		  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, WORKDIRFILEFOUND, (char *)cName, 
				idmGetExtension((char *)_extbuff, 0), dmsWorkDir);
		  return save_status(*noError = idmsCvInWorkdir, 0, 0,
				    (char *)cName, (char *)_extbuff, "", "",
				    FPN, change_codes);
		}
	     }
          dmsSeparateByColon(eNameList, _extbuff, eNameList);
        }

            break;
         case UNIXDIR_opt:
            if ((what == READ_CDBA) || (checkout == coLOCK) ||
                (checkout == coCANCEL))
               break; /* skip unix dir for cdba, co, ci, cancel */
            if (verName && strcmp(verName,"")) 
               break; /* if verName is indicated, find in libraries */
    	    if (!ptr->dirLib || !strcmp(ptr->dirLib,"")) {
               break;
            }
            /* Loop thru extension list. */ 
    	    strcpy(eList, tmp);
  	    eNameList = eList;
	    dmsSeparateByColon(eNameList,_extbuff, eNameList);
     	while(strcmp(_extbuff,"")) {

            full_path = dmsFormFileName(ptr->dirLib, (char *)cName,
					(char *)_extbuff, CALL_GET_EXT);
	    dmsWorkDir = dmsGetWorkingDir((char *)_extbuff, 0);
            if (osFileExists(full_path)){
		strcpy(FPN, full_path);
		simplify_path = idb_simplify_file_name(full_path ,NULL); /* path of the file */
		if(!simplify_path || !FPN)
		  FPN=strdup("");
		else
		  simplify_path2 = strdup(simplify_path);
                  /* strcpy(FPN, simplify_path); */
	        /* get WorkDirPath fullpath filename */
                full_path = dmsFormFileName(dmsWorkDir, (char *)cName,
                                          (char *)_extbuff, CALL_GET_EXT);
		simplify_path = idb_simplify_file_name(full_path,NULL); /* path of the file in workdir */
		if(!simplify_path || !FPN)
		  *noError = idmsCvInUnixdir; /* work dir file does not exist. */
		else{
		  if (strcmp(simplify_path,simplify_path2) != 0)
                    *noError = idmsCvInUnixdir;
                  else { 
                    *noError = idmsCvInWorkdir;
		    strcpy(FPN, full_path);
		  }
		}
              dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, UNIXFILEFOUND, cName, _extbuff, ptr->dirLib);
	      free(simplify_path2);
              return save_status(*noError, 0, 0, (char *)cName, 
                                 (char *)_extbuff, "", "", FPN, 
                                 change_codes);
            }
	  dmsSeparateByColon(eNameList, _extbuff, eNameList);
	}

            break;
         case RW_opt: /* RW */
            if (checkout == coREAD_ONLY || checkout == coCANCEL){
               if (twin_turbo){
                  if (debugDmsOn){
                     dmsPrintIdmsLog("*IDMS DEBUG* - Twin Turbo RW\n");
                  } 
                  const char *cached_path = dmsTFindCellInCache(cName,vName);
                  if (cached_path) {
                     if (debugDmsOn){
                        dmsPrintIdmsLog("*IDMS DEBUG* - (RW_opt 1) - cell in cache: %s\n", cached_path);
                     }
                     /* we can just return the attributes here */
                     plib_id = (void *)dmsTGetLastLibraryName();
                     idb_set_lib_id(plib_id);
                     strcpy(FPN, cached_path);
                     *noError = idmsCvInWorkdir;
                     new_ver = dmsTGetLastVersion();
                     strcpy(dmsConfName, "");
                     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, DMSFILEFOUND, cName, vName, new_ver, plib_id, dmsConfName);
                     if (debugDmsOn){
                        dmsPrintIdmsLog("*IDMS DEBUG* - Twin Turbo found %s.%s;%s in library %s\n", cName, vName, new_ver, plib_id);
                     }
                     return save_status(*noError, 0, 0, (char *)cName, (char *)vName, "", "", FPN, change_codes);
                  }
               }
               if (!(plib_id = dms_find_lib_id("r", ptr->dirLib, cName, vName, noError,fp)))
                  break;
                  if(fp) 
                    strcpy(FPN, fp);
               dmsSearchCellView(open_first, what, (char *)cName, (char *)vName, 
                                 (char *)verName, FPN, locker, &new_ver, 
				  noError, dmsKeepTmpCopy);
               if (debugDmsOn){
                  dmsPrintIdmsLog("*IDMS DEBUG* returning %d for %s cN=%s vN=%s verName=%s new_ver=%s\n", *noError, FPN, cName, vName, verName, new_ver);
               }

               if (!*noError) 
                  return save_status(*noError, 0, 0, (char *)cName,
                                  (char *)vName, "", "", FPN, change_codes);
            }
            else{
               if (twin_turbo){
                  const char *cached_path = dmsTFindCellInCache(cName,vName);
                  if (cached_path) {
                     if (debugDmsOn){
                        dmsPrintIdmsLog("*IDMS DEBUG* - (RW_opt 2) - cell in cache: %s\n", cached_path);
                     }
                  }
               }
               if (!(plib_id = dms_find_lib_id("w", ptr->dirLib, cName, vName, noError,fp)))
                  break;
                  if(fp) 
                    strcpy(FPN, fp);
               dmsCheckOutCellView(what, !UPDATE, (char *)cName, (char *)vName, 
                                   (char *)verName, FPN, locker, &new_ver, noError);
               if (!*noError) /* we know that the function returns only 1 or 0
                                 and we know that 1 happens to be
                                 idmsCvInWorkDir */
                  return save_status(*noError, 0, 0, (char *)cName,
                                  (char *)vName, "", "", FPN,
                                  change_codes);
            }

               /* if we are here, we found the id */

            idb_set_content_at_id(pplib_id, plib_id);
            if (debugDmsOn){
                  dmsPrintIdmsLog("*IDMS DEBUG* id found: %s\n", (char *)idb_get_library_name(plib_id));
            } 

            return save_status(*noError = idmsCvInRwlib, 
               (char *)idb_get_library_directory(plib_id, "w"), 
               (char *)idb_get_library_name(plib_id),
               (char *)cName, (char *)vName, new_ver, locker, FPN,
               change_codes);

         case RO_opt: /* RO */
            if (!(plib_id = dms_find_lib_id("r", ptr->dirLib, cName, vName, noError,fp)))
               break;
               if(fp) 
                strcpy(FPN, fp);
            if (checkout != coLOCK){ 

               dmsSearchCellView(open_first, what, (char *)cName, (char *)vName, 
                                 (char *)verName, FPN, locker, &new_ver, 
				  noError, dmsKeepTmpCopy);
               if (debugDmsOn){ 
                  dmsPrintIdmsLog("*IDMS DEBUG* returning %d for %s\n", *noError, FPN);
               }

               if (!*noError) 
                  return save_status(*noError, 0, 0, (char *)cName,
                                  (char *)vName, "", "", FPN,
                                  DONT_CHANGE_CODES);
               idb_set_content_at_id(pplib_id, plib_id);

               return save_status(*noError = idmsCvInRolib,
                  (char *)idb_get_library_directory(plib_id, "r"),
                  (char *)idb_get_library_name(plib_id),
                  (char *)cName, (char *)vName, new_ver, locker, FPN,
                  change_codes);
            }
            else{
               dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, CHECKOUTREADONLY, cName, vName, 
                              dmsGetLastLibName());
               return save_status(*noError = idmsCvNotFound, "", "", 
                  (char *)cName, (char *)vName, "", "", "", change_codes);
            }
            break;
         default:
            break;
      }
      ptr=ptr->next;
   }

   if ((checkout != coREAD_ONLY) && create){
      dmsCreateCellView(what, (char *)cName, (char *)vName, (char *)viewType,
                        FPN, noError);
      if (!*noError)
         return *noError;
      idb_set_content_at_id(pplib_id, plib_id = idb_get_lib_id());
      /***
      idb_find_version_and_locker(plib_id, (char *)cName, (char *)vName,
         (char *)verName, locker, 0, GET_LAST_IF_NOT_LOCKED, &new_ver);
      ***/
      return save_status(*noError = idmsCvInWorklib,
         (char *)idb_get_library_directory(plib_id, "w"),
         (char *)idb_get_library_name(plib_id),
         (char *)cName, (char *)vName, new_ver, locker, FPN,
         change_codes);

   } 
   else{
      if ((checkout == coREAD_ONLY) || (checkout == coRO_LOCK_POLICY)){
         dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR,FILENOTFOUND,cName,vName,verName,dmsConfName);
         return save_status(*noError = idmsCvNotFound, "", "", 
            (char *)cName, (char *)vName, "", "", "", change_codes);
      }
      else{
         dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR,NOTINANYLIB,cName,vName,dmsConfName);
         return save_status(*noError = idmsCvNotFound, "", "", 
            (char *)cName, (char *)vName, "", "", "", change_codes);
      }
   }
}


int dmsWriteCellView(int cdba, int writeType, char *cName, char *vName,
                      char *verName, char *viewType, char *log, 
		      char *config, char *FPN, void **pplib_id, 
                      int *noError)
/* writeType 0 for checkin, 1 for update  2 for install */
{
   int i;
   struct elem *ptr;
   char extension[BUFSIZE];
   char *full_path;
   char *lib_path;
   void *plib_id;
   char locker[BUFSIZE];
   char *new_ver = NULL;
   char *dmsWorkDir = dmsGetWorkingDir(vName, 0); 
   char fp[BUFSIZE];

   *noError = idmsCvNotFound;
   idb_set_content_at_id(pplib_id, 0);

   if (!cdba) { /* from cadence co@ or any directory */
      idmGetExtension(vName, extension);

      /* make sure the file is in working dir and ready to be checkin */

      full_path = dmsFormFileName(dmsWorkDir, cName, vName, CALL_GET_EXT);
      if (osFileExists(full_path)){
	strcpy(FPN, full_path); 
	if (!osFileIsWritable(FPN)) {
	  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, CANTCIREADONLY, cName, extension, 
			 dmsWorkDir);
	  return save_status (*noError = idmsCvNotFound, "", "", cName, vName,
			      "", "", "", DONT_CHANGE_CODES);
	}
      }
      else {
	 dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, NOCHECKINFILE, cName, extension, dmsWorkDir);
	 return save_status (*noError=idmsCvNotFound, "", "", cName, vName,
			    "", "", "", DONT_CHANGE_CODES);
      }
   }

   /* idmSetCheckinGlobalVar(cName,vName); */
 
   if (writeType == 2) /* do the install */
      return dmsInstallCellView(cdba, cName, vName, viewType, log, 
					config, FPN, noError, CREATE);

   i = findPolicyEntry(rwpaths, rwpathsCount, toolName.name, vName);

   if (debugDmsOn)
      dmsPrintIdmsLog("*IDMS DEBUG* %d.rwpaths %s:\n", i+1, rwpaths[i].name);
   ptr=rwpaths[i].next;
   while (ptr) {
      if (debugDmsOn)
         dmsPrintIdmsLog("*IDMS DEBUG* \t%s %2d (%s)\n",
            action[ptr->opt%10],ptr->opt,ptr->dirLib);
      switch (ptr->opt%10) {
         case 4: /* WORKDIR */
         case 7: /* UNIX */
            /* UNIX Directory Is Always ReadOnly, So No Write Is Permitted */
            /* Skip unix directories on checkout & checkin & cancel checkout */
            break;
         case 1: /* RW */
            if (!dms_find_lib_id("w", ptr->dirLib, cName, vName, noError,fp))
               break;
               if(fp) 
                 strcpy(FPN, fp);
            if (idb_get_lib_id()) {
               if (writeType==1) {  /* do update */
                  dmsCheckOutCellView(0,UPDATE,cName,vName,verName,
				FPN, locker, &new_ver, noError);
                  if (!*noError) 
                     return save_status (*noError, "", "", cName, vName,
                                         "", "", "", DONT_CHANGE_CODES);
	       }
               dmsCheckInCellView(cdba, cName, vName, verName, log,config,
					FPN, locker, &new_ver, noError);
               if (!*noError) {
                  return save_status (*noError = idmsCvNotFound, "", "",
                            cName, vName, "", "", "", DONT_CHANGE_CODES);
	       }

               idb_set_content_at_id(pplib_id, idb_get_lib_id());

               return save_status(*noError = idmsCvInRwlib,
                  (char *)idb_get_library_directory(*pplib_id, "w"),
                  (char *)idb_get_library_name(*pplib_id),
                  (char *)cName, (char *)vName, new_ver, "", FPN,
                  DONT_CHANGE_CODES);
            }
            break;
         case 2: /* RO */
            if (!dms_find_lib_id("r", ptr->dirLib, cName, vName, noError,fp))
               break;
               if(fp) 
                 strcpy(FPN, fp);
            /* can not open RO lib for write */
            if (idb_get_lib_id()) {
               switch (writeType) {
                  case 0:
                     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR,CANTCHECKINROLIB,cName,vName, dmsGetLastLibName());
                     return save_status (*noError = idmsCvNotFound, "", "",
                               cName, vName, "", "", "", DONT_CHANGE_CODES);
                  case 1:
                     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR,CANTUPDATEROLIB,cName,vName, dmsGetLastLibName());
                     return save_status (*noError = idmsCvNotFound, "", "",
                               cName, vName, "", "", "", DONT_CHANGE_CODES);
               }
            }
            break;
       }
       ptr=ptr->next;
   }

   dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR,NOTINANYLIB,cName,vName,dmsConfName);
   return save_status (*noError = idmsCvNotFound, "", "", cName, vName,
                       "", "", "", DONT_CHANGE_CODES);
}


int dmsCreateCellView(int cdba, char *cName, char *vName, char *viewType, 
                       char *FPN, int *noError)
{
   int i;
   void *plib_id;
   char *p, *dotpos;
   static char sbuff[BUFSIZE];
   int wlen;
   if (FPN){
     *FPN=0;
   }
   if (strcmp(theLibName, "*") != 0) /* we actually have a library name */
      dmsSetLastLibName(theLibName);
   else{
      i = findPolicyEntry(worklib, worklibCount, toolName.name, vName);
      if (debugDmsOn)
         dmsPrintIdmsLog("*IDMS DEBUG* %d.worklib %s: %d %s\n", i+1, 
                          worklib[i].name, worklib[i].next->opt,
                          worklib[i].next->dirLib);
      p = worklib[i].next->dirLib;
      dotpos = strchr(p, '.');
      strncpy(sbuff, p, (wlen = MIN((size_t)(dotpos-p), BUFSIZE)));
      *(sbuff+wlen) = '\0';
      dmsSetLastLibName(sbuff);
   }
   if ( !(plib_id = idb_open_library(dmsGetLastLibName(), 0, "w"))){
      idb_capture_cdn_vendor_warning();
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, CANTOPENWORKLIB, dmsGetLastLibName(),
                     dmsGetLastSearchPath());
      *noError=FALSE;
      return save_status(idmsCvNotFound, "", "", cName, vName, "", "", "",
                         DONT_CHANGE_CODES);
   }

   /*** REM: configs
   dmsSetLastPathConfName("");
   open_config_if_exist(idb_get_lib_id(), dmsGetLastPathConfName(), theConfName,
                        dmsGetLastToolConfName());
   ***/

   if (!idb_create_cellview(plib_id, cName, vName, viewType, FPN)){
      if (idb_check_error_code(idb_CV_EXISTS))
         dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, CVEXIST, cName, vName, dmsGetLastLibName());
      else
         dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, CANNOTCREATE, cName, vName, dmsGetLastLibName());
      *noError=FALSE;
      return save_status(idmsCvNotFound, "", "", cName, vName, "", "", "",
                         DONT_CHANGE_CODES);
   }

   dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, CREATEOK, cName, vName, dmsGetLastLibName());
   *noError = idmsCvInWorklib;

   return save_status(*noError,
                      (char *)idb_get_library_directory(plib_id, "w"),
                      (char *)idb_get_library_name(plib_id), cName, vName,
                      NOBODY, "", FPN, 
                      DONT_CHANGE_CODES);
}


void dmsCheckOutCellView(int cdba, int update, char *cName, char *vName,
                         char *verName, char *FPN, char *locker, char **new_ver,
			 int *noError)
{
   char *extension;
   char  workDirFile[BUFSIZE];
   void *plib_id = idb_get_lib_id();
   void *ver_id;
   DSSRESULT chkoutStatus;
   int proceed = TRUE;
   int err = 0;

   if (debugDmsOn)
      dmsPrintIdmsLog("*IDMS DEBUG* Procedure Checkout\n");

   if (!(ver_id = (void *)idb_find_version_and_locker(plib_id = idb_get_lib_id(),
                     cName, vName, verName, locker, FPN, 1, new_ver))){
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR,VERSIONNOTEXIST,cName,vName,dmsGetLastLibName(),
                     verName);
                     *noError = idmsCvNotFound;
                     return;
   }

   extension = idmGetExtension(vName, 0);
   if(!cdba) {
     sprintf(workDirFile,"%s%s.%s", dmsGetWorkingDir(vName, 0), cName, extension);
   }

   *noError = idmsCvNotFound;

   if(!strcmp(locker, NOBODY) || !strcmp(locker, userName)) {

     if(!strcmp(locker, NOBODY)) {
       /* not checked out cellview with writable copy in workdir is illegal */
       if(!update && osFileExists(workDirFile)) {	
          dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, ILLEGALCOPY, dmsGetLastLibName(), cName, extension);
          return;
       }
       if (getenv("DMSERVER_USE_DSSC")){
          if (debugDmsOn) {
              dmsPrintIdmsLog("*IDMS DEBUG* dssc co %s\n", FPN);
          }
/*
          const char command[2048];
          sprintf(command, "dssc co -lock -noc %s", FPN);
          int ret_code = system(command); 
          chkoutStatus = (ret_code == 0)?S_OK:E_FAIL;
*/
         int f = fork();
         int ret_code;
         int return_status=0;
         char dssc_dir[300]="";

         switch(f){
           case 0:
             strcat(dssc_dir, getenv("SYNC_DIR"));
             strcat(dssc_dir,"/bin/dssc");
             return_status = execl(dssc_dir, "dssc", "co", "-lock","noc", FPN, NULL);
             if(return_status == 0){
               chkoutStatus = S_OK;
             } else {
               if(debugDmsOn){
                 printf("-W- Error when trying to execute dssc co -lock -noc %s\n",FPN);
                 chkoutStatus = E_FAIL;
               }
             }
             break;
           case -1:
	     chkoutStatus = E_FAIL;
             break;
           default:
             waitpid(f, &ret_code, 0);
             chkoutStatus = (ret_code == 0)?S_OK:E_FAIL;
             break;
         }
       }
       else { 
           FPN[0] = '\0';
           chkoutStatus = idb_checkout(plib_id, cName, vName, verName, FPN, locker, new_ver, cdba);
       }
     }
     else { 	/* checked out by current user is no error in old code */
       chkoutStatus = S_OK;
     }

     if(DSS_SUCCESS(chkoutStatus)) {
        if(!cdba) {
	  if(osFileExists(workDirFile)) {
	    if(update) {
	      osCopyFile(workDirFile,FPN); //osCopyFile(workDirFile, FPN);
            }
            strcpy(FPN, workDirFile);
            *noError = idmsCvInWorkdir;
          } else {
	    if(getenv("IDMS_CO_COPY")) {
	      if(osCopyFile(FPN, workDirFile) !=0) { 
 	        err = 1;
	      } else {
                strcpy(FPN, workDirFile); /* */
	        *noError = idmsCvInWorkdir;
              }
            } else {
	      if(osCreateSymLink(FPN, workDirFile)){
	        strcpy(FPN, workDirFile); /* */
	        *noError = idmsCvInWorkdir;
	      } else { /* link creation failed */
	        err = 1;
  	      }
	    }
          }
          if (err) {
            dmsUnCheckOutCellView(cName,vName,verName,FPN,noError);
            *noError = idmsCvNotFound;//*noError = idmsCvInRwlib;
            dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, CANTCREATEFILE, cName, vName, dmsGetWorkingDir(vName, 0) );  
            return; 
	  } else { 
	    *noError = idmsCvInWorkdir;
	  } 
	}
	else
	  *noError = idmsCvInWorklib; 
     }
     else {
#ifdef DISABLE_SYNCHRONICITY
       dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, DISABLESYNC);
#else
       dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, CANNOTCHECKOUT, cName, vName, *new_ver,
                      dmsGetLastLibName(), dmsConfName);
#endif
       return;
     }
   }
   else { /* checked out by another user */
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, FILERECHECKOUT,  dmsGetLastLibName(), 
                    cName, vName, *new_ver, FPN, locker);
     return;
   }

   dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, CHECKOUT, cName, vName, *new_ver,
                  dmsGetLastLibName(), dmsConfName);
}


void dmsCheckInCellView(int cdba, char *cName, char *vName, char *verName,
                        char *log, char *config, char *FPN, char *locker, char **new_ver, int *noError){

  char *version;
  char *workDirFile, *FPNBK, *message;
  int cpid, status;
  struct stat sfile, sfile2;
  DSSRESULT checkinStatus;
  void *library_id = idb_get_lib_id();
  const char *copymode = getenv("DMSERVER_NO_PRESERVE_OWNERSHIP") ? "--preserve=mode,timestamps" :  "--preserve=mode,ownership,timestamps";
  if (debugDmsOn)
    dmsPrintIdmsLog("*IDMS DEBUG* Procedure Checkin\n");

  if(!idb_find_version_and_locker(library_id, cName, vName, verName, locker, FPN, GET_LAST_IF_NOT_LOCKED, &version)){
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__,
      ERROR, VERSIONNOTEXIST, cName, vName, dmsGetLastLibName(),verName);
    *noError = idmsCvNotFound;
    return;
  }

  asprintf(&workDirFile, "%s", dmsFormFileName(dmsGetWorkingDir(vName, 0), cName, vName, CALL_GET_EXT));
  asprintf(&FPNBK, "%s~", FPN);

  if(strcmp(locker,userName)) { /* we don't own lock */
    if(!strcmp(locker, NOBODY)) { /* not checked out? */
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, NOTCHECKOUT, dmsGetLastLibName(), cName, vName,version);
      *noError = FALSE;
    } else { /* not checked out by someone else? */
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, FILERECHECKOUT, dmsGetLastLibName(),cName, vName, version, FPN, locker);
      *noError = FALSE;
    }
    if (debugDmsOn) {
      asprintf(&message, "dmsCheckInCellView lock is for %s is owned by %s, not user(%s)\n", FPN, locker, userName);
      dmsPrintIdmsLog(message);
    }
    return;
  } else { /* we own it and attempt to copy */
    cpid = fork();
    if(cpid < 0) {
      *noError = FALSE;
      asprintf(&message, "dmsCheckInCellView fork failure when copying %s -> %s\n", workDirFile, FPN);
      dmsCSetLastMessage(message);
      if (debugDmsOn) dmsPrintIdmsLog(message);
      return;
    }
    if(!cpid){
      if (stat(workDirFile, &sfile) || stat(FPN, &sfile2)) exit(1);
      /* check if same file, do not copy from WARD, could be a symlink */
      if(sfile.st_dev == sfile2.st_dev && sfile.st_ino == sfile2.st_ino) {
        execl("/bin/cp", "/bin/cp", copymode, "--remove-destination", "-f", "--no-dereference", FPN, FPNBK, NULL);
        exit(2);
      }
      execl("/bin/cp", "/bin/cp", copymode, "--remove-destination", "-f", "-L", "-H", "--backup", workDirFile, FPN, NULL);
      exit(3);
    } else {
      waitpid(cpid, &status, 0);
      if(WEXITSTATUS(status)){ /* error in copying to destination */
        *noError = FALSE;
        asprintf(&message, "dmsCheckInCellView copy error(%x) %s -> %s\n", status, workDirFile, FPN);
        dmsCSetLastMessage(message);
        if (debugDmsOn) dmsPrintIdmsLog(message);
        if(!lstat(FPNBK,&sfile)) {
          rename(FPNBK, FPN);
          unlink(FPNBK);
        }
        return;
      }
    }
  }

  /* attempt to check-in */
  checkinStatus = idb_checkin(library_id, cName, vName, FPN, locker, new_ver, cdba, log, NULL);
  if (DSS_SUCCESS(checkinStatus)) { /* check-in OK */
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, CHECKIN, cName, vName, version, *new_ver, dmsGetLastLibName(), config);
    *noError = TRUE;
    unlink(FPNBK);
    unlink(workDirFile);
  } else { /* check-in failed */
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, CANNOTCHECKIN, cName, vName, version, dmsGetLastLibName());
    *noError = FALSE;
    unlink(FPN);
    rename(FPNBK, FPN);
    return;
  }
}

static char INSTALLED_PATH[BUFSIZE];

int dmsInstallCellView(int cdba, char *cName, char *vName, char *viewType,
                       char *log, char *config, char *FPN, int *noError, int create)
{
   char message[BUFSIZE], workDirFile[BUFSIZE], tmpName[BUFSIZE];
   void *plib_id;
   char *dmsWorkDir;
   DSSRESULT installStatus = E_FAIL;
   char *libname = 0;
   char *tmp;
   char cPath[BUFSIZE]; 
   char *dest=NULL;
   int length = 0;
   int createret=0;
   char locker[BUFSIZE];
   char *new_ver = NULL;

   int file_was_deleted;
   struct stat s;
   int is_empty=0;
   FILE *fp;
   char command[BUFSIZE];
   char line[BUFSIZE];
   char *libraryPath = 0;

   /* god knows why this is done this way */
   memset(cPath, '\0',BUFSIZE);
   INSTALLED_PATH[0] = '\0';
   if(create) { /* If DONT_CREATE, then FPN must be set */
     /* if there is a problem *noError is set to FALSE */
     dmsCreateCellView(cdba, cName, vName, viewType, FPN, noError);
     if (FPN) {
        strcpy(INSTALLED_PATH, FPN);
     }
     if (debugDmsOn){
        dmsPrintIdmsLog("*IDMS - dmsCreateCellView  cN=%s, vN=%s, FPN=%s, cdba =%d -- status=%d\n", cName, vName, FPN, cdba, *noError);
     }
   } else {
    if(!FPN) {
     if (debugDmsOn){
        dmsPrintIdmsLog("*IDMS - dmsInstallCellView FPN NULL arg\n");
      }
      return save_status(*noError = idmsCvCanNotBeInstalled, "", "", cName, vName,
                        "", "", "", DONT_CHANGE_CODES);
    }
    if(!stat(FPN,&s)) {
     if (debugDmsOn){
        dmsPrintIdmsLog("*IDMS - dmsInstallCellView %s (%o) exists, abort install\n", FPN, s.st_mode);
      }
     return save_status(*noError = idmsCvCanNotBeInstalled, "", "", cName, vName,
                        "", "", "", DONT_CHANGE_CODES);
    }
   }

   if(*noError) {
     if(!cdba) {
	 dmsWorkDir = dmsGetWorkingDir(vName, 0);
	 strcpy(workDirFile, dmsFormFileName(dmsWorkDir, cName, vName, CALL_GET_EXT));
	 if(osFileExists(workDirFile)) {
	   if((osCopyFile(workDirFile,FPN)!=0)) {
	     *noError= idmsCvCanNotBeInstalled;
	     libname = (char *)idb_get_lib_id();
	     strcpy(message,"Failed to copy file to library  ");
	     if(libname) 
	       strcat(message, libname); 
      	     dmsCSetLastMessage(message);
	     return save_status(*noError = idmsCvNotFound, "", "", cName, vName,
                        "", "", "", DONT_CHANGE_CODES);      
	   }
	 }
     }

     /* this actually checks in the newly created cellview, or else 
     dessync does not recognize it as being managed by a dm 
     */
     
     if (debugDmsOn){
        dmsPrintIdmsLog("*IDMS - entering idb_install_cellview with FPN = %s\n", FPN);
     }
     //installStatus = idb_install_cellview(plib_id=idb_get_lib_id(), cName, vName, FPN, cdba, log, &file_was_deleted);
     if (debugDmsOn){
        dmsPrintIdmsLog("*IDMS - idb_install_cell_view cN=%s, vN=%s, FPN=%s, cdba =%d -- status=%d\n", cName, vName, FPN, cdba, installStatus);
     }
     installStatus = idb_checkin(plib_id=idb_get_lib_id(), cName, vName, FPN, NULL, NULL, cdba, log, "-new");
   } else {
     
     return createret;   
   }

   if (DSS_SUCCESS(installStatus)) {

      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,INSTALL,cName,vName, dmsGetLastLibName(),config);
      *noError=idmsCvInWorklib;
      /* remove copy from work directory after install is complete. */
      osRemoveFile(workDirFile);

      /************ REM: configs
      if (strcmp(config,"")) {
         strcpy(message, (char *)dmsCGetLastMessage());
         dmsAddCVToConfig(plib_id, config, cName, vName, 
                          idb_get_first_default_version(), idb_CONFIG_STATIC);
         strcat(message, (char *)dmsCGetLastMessage());
         dmsCSetLastMessage(message);
      }
      ************/
   }
   else {
     unlink(INSTALLED_PATH);
	 libraryPath = idb_get_libpath(theLibName);

	 while (strcmp(dirname(INSTALLED_PATH),libraryPath)) {
	 	sprintf(command, "ls %s >&1", INSTALLED_PATH);
		fp = popen(command, "r");
        if (!(fgets(line, BUFSIZE, fp))) {
			is_empty = 1;
		} else if (!(strcmp(line,"master.tag\n"))) {
			fgets(line, BUFSIZE, fp);
			if (!(strcmp(line,"master.tag\n"))) {
				is_empty = 1;
			}		
		}
	    pclose(fp);

		if (is_empty == 1) {
			is_empty = 0;
     		int pid = fork();
	 		int status;
     		if(pid == 0){
	   			execl("/bin/rm", "/bin/rm", "-rf", INSTALLED_PATH, NULL);
	   			exit(0);
	 		} else if (pid < 0) {
	   			//fork failure
	 		} else {
	   			waitpid(pid,&status, 0);
	   			//check return status with WEXITSTATUS(status)
	 		}
		}
	}

#ifdef DISABLE_SYNCHRONICITY
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, DISABLESYNC);
#else
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR,CANNOTINSTALL,cName,vName,dmsGetLastLibName(),config);
#endif
     return save_status(*noError = idmsCvNotFound, "", "", cName, vName,
                        "", "", "", DONT_CHANGE_CODES);
   }
  return idmsSuccess;
}

static int dmsUnCheckOutCellView(char *cName, char *vName, char *verName, 
  char *FPN, int *noError){
  char *tVerName, *message;
  int cpid, status;
  char *workDirFile, *workDirFileBK;
  struct stat s;
  char locker[BUFSIZE];
  void *library_id = idb_get_lib_id();
  struct stat sb;

 if (debugDmsOn) dmsPrintIdmsLog("*IDMS DEBUG* Procedure Unlocking\n");

  if(!idb_find_version_and_locker(library_id, cName, vName, verName, locker, FPN, GET_LAST_IF_NOT_LOCKED, &tVerName)){
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__,
      ERROR, VERSIONNOTEXIST, cName, vName, dmsGetLastLibName(),verName);
    return (*noError = idmsCvNotFound);
  }

  asprintf(&workDirFile, "%s", dmsFormFileName(dmsGetWorkingDir(vName, 0), cName, vName, CALL_GET_EXT));
  asprintf(&workDirFileBK, "%s~", workDirFile);

  if (stat(workDirFile, &sb) == 0 && S_ISREG(sb.st_mode)){
    if(rename(workDirFile,workDirFileBK)) { /* work area not writeable? */
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, FILENOTWRITEABLE, cName, vName, workDirFile);
      return save_status(*noError = idmsCvNotFound, "", "", cName, vName, "","", "", DONT_CHANGE_CODES);
    }
  } 

  if(strcmp(locker,userName)) { /* we don't own lock */
    if(!strcmp(locker, NOBODY)) { /* not checked out? */
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, NOTCHECKOUT, dmsGetLastLibName(), cName, vName,tVerName);
    } else { /* not checked out by someone else? */
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, FILERECHECKOUT, dmsGetLastLibName(),cName, vName, tVerName, FPN, locker);
    }
    if (debugDmsOn) {
      asprintf(&message, "dmsUnCheckOutCellView lock is for %s is owned by %s, not user(%s)\n", FPN, locker, userName);
      dmsPrintIdmsLog(message);
    }
    return(*noError = FALSE);
  }

 /* we own it and attempt unlock */
  if(getenv("DMSERVER_USE_DSSC")){
    cpid = fork();
    char dssc_dir[300]="";
    strcat(dssc_dir, getenv("SYNC_DIR"));
    strcat(dssc_dir,"/bin/dssc");

    if(cpid < 0) {
      asprintf(&message, "dmsUnCheckOutCellView fork failure when unlocking %s\n", FPN);
      dmsCSetLastMessage(message);
      if (debugDmsOn) dmsPrintIdmsLog(message);
      return (*noError = FALSE);
    }

    if(!cpid){ /* child process */
      if(getenv("IDMS_NOFORCE"))
        execl(dssc_dir, "dssc", "cancel", FPN, NULL);
      else
        execl(dssc_dir, "dssc", "cancel", "-force", FPN, NULL);
      exit(1);
    } else { /* parent process */
      waitpid(cpid, &status, 0);
      if(WEXITSTATUS(status)){ /* error in dssc */
unco_failed:
        asprintf(&message, "dmsUnCheckOutCellView copy in dssc unlock failed(%x)\n", status);
        dmsCSetLastMessage(message);
        if (debugDmsOn) dmsPrintIdmsLog(message);
        dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, CANTUNCHECKOUT, dmsGetLastLibName(), cName,vName, tVerName);
        rename(workDirFileBK, workDirFile);
        return(*noError = FALSE);
      } else { /* no errors in dssc */
unco_OK:
        unlink(workDirFileBK);
        dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, UNCHECKOUT, dmsGetLastLibName(), cName,vName, tVerName);
        return save_status(*noError = idmsCvInRwlib,
          (char *)idb_get_library_directory(library_id, "w"),
          (char *)idb_get_library_name(library_id),
          cName, vName, tVerName, "", FPN, DONT_CHANGE_CODES);
      }
    }
  } else { /* not using dssc */
    if (DSS_SUCCESS(status = idb_cancel_checkout(library_id, cName, vName, FPN, 0))) {
      goto unco_OK;
    } else { /* failed to check-in */
      goto unco_failed;
    }
  }
  return save_status(*noError = idmsCvNotFound, "", "", cName, vName, "",
                      "", "", DONT_CHANGE_CODES);
}

int dmsCancelCheckOutCellView(char *cName, char *vName, char *verName,
                               char *FPN, int *noError)
{
   struct elem *ptr;
   int i;
   char fp[BUFSIZE];

   /* idmSetCheckinGlobalVar(cName, vName); */ /*used by the post trigger */
   i = findPolicyEntry(rwpaths, rwpathsCount, toolName.name, vName);
   ptr = rwpaths[i].next;

   while (ptr) {
      switch (ptr->opt%10) {
         case 0: /* UNIX */
            /* Skip all unix directories while checkout & checkin
               & cancel checkout */
            break;
         case 1: /* RO */
            /* should be error condition, but allows it now */
         case 2: /* RW */
            if (!dms_find_lib_id("w", ptr->dirLib, cName, vName, noError,fp))
               break;
               if(fp) 
                 strcpy(FPN, fp);
            if (idb_get_lib_id()) {
               return dmsUnCheckOutCellView(cName,vName,verName,FPN,noError);
            }
            break;
      }
      ptr=ptr->next;
   }
   dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, NOTINANYLIB, cName, vName, dmsConfName);
   return save_status(*noError = idmsCvNotFound, "", "", cName, vName, "", "",
                      "", DONT_CHANGE_CODES);
} 




/*******************  Delete Functions  *****************/


int dmsDelete(int delete, const char *cName,
                     const char *vName, const char *verName, 
                     const char *viewType, char *FPN,
                     void **pplib_id, int *noError, int change_codes)
{
  struct elem *ptr;
  int i;
  char *dmsWorkDir;
  char *full_path;
  void *plib_id;
  char *new_ver = NULL;
  char *lib_path;
  char *simplify_path = NULL;
  char fp[BUFSIZE];
 
  static char locker[BUFSIZE];
  strcpy(locker, "");
  *noError = 0;
  idb_set_content_at_id(pplib_id, 0);

  i = findPolicyEntry(rwpaths, rwpathsCount, toolName.name, vName);

  if (debugDmsOn)
    dmsPrintIdmsLog("*IDMS DEBUG* %d.rwpaths %s:\n", i+1, rwpaths[i].name);

   ptr = rwpaths[i].next;
   while (ptr){

     if (debugDmsOn)
       dmsPrintIdmsLog("*IDMS DEBUG* \t%s %2d (%s)\n",
		       action[ptr->opt%10], ptr->opt,ptr->dirLib);
     switch (ptr->opt%10){
     case UNIXDIR_opt:  
       /* Delete file in Unix_Dir only if it's not in WORK_DIR also */
       full_path = dmsFormFileName(ptr->dirLib, (char *)cName,
				   (char *)vName, CALL_GET_EXT);
       if (osFileExists(full_path)){
	 simplify_path = idb_simplify_file_name(full_path ,NULL);
	 if(!simplify_path || !FPN)
	   FPN= strdup("");
         else 
	   strcpy(FPN, simplify_path);
	 dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, UNIXFILEFOUND, cName, vName, ptr->dirLib);
	 /* check if the UNIX dir is the workdir */
	 dmsWorkDir = dmsGetWorkingDir((char *)vName, 0);
	 full_path = dmsFormFileName(dmsWorkDir, (char *)cName,
                                     (char *)vName, CALL_GET_EXT);
 	 simplify_path = idb_simplify_file_name(full_path,NULL);
         if (simplify_path && FPN && (strcmp(FPN,simplify_path) != 0)){
           osRemoveFile(FPN);
           *noError = idmsCvInUnixdir; /* 1 */
           return save_status(*noError, 0, 0, (char *)cName, 
                            (char *)vName, "", "", FPN, change_codes); 
         }
       }
       
       break;

     case RW_opt:
       if(delete == 2){ /* delete cell */
	 if(!(plib_id = idb_check_cell_in_lib((char *)ptr->dirLib, (char *)cName)))
	   break;
		 
	 dmsRmCell((char *)ptr->dirLib, (char *)cName, (char *)vName, 
		   (char *)verName, FPN, locker, &new_ver, noError);
	 
       } else { /* delete cell view or cell version */
	 if (!(plib_id = dms_find_lib_id("w", ptr->dirLib, cName, vName, noError,fp))) 
           break; 
         if(fp) 
           strcpy(FPN, fp);
         dmsRmCellView(delete, (char *)"", (char *)cName, (char *)vName, 
		       (char *)verName, FPN, locker, &new_ver, noError);
       } 
       if (!*noError) /** if there is an error -> not found in RW lib **/
         return save_status(*noError, 0, 0, (char *)cName,
                            (char *)vName, "", "", FPN, change_codes);
       
       /* if we are here, we found the id  */      
       idb_set_content_at_id(pplib_id, plib_id);

       if(delete == 2)
         return save_status(*noError = idmsCvInRwlib, 
                            (char *)idb_get_library_directory(plib_id, "w"), 
                            (char *)idb_get_library_name(plib_id),
                            (char *)cName, "", "", locker, FPN,
                            change_codes);
       else
         return save_status(*noError = idmsCvInRwlib, 
                            (char *)idb_get_library_directory(plib_id, "w"), 
                            (char *)idb_get_library_name(plib_id),
                            (char *)cName, (char *)vName, new_ver, locker, FPN,
                            change_codes);
       
       break;
       
     case RO_opt: 
 
       if (!(plib_id = dms_find_lib_id("w", ptr->dirLib, cName, vName, noError,fp)))
	 break;
         if(fp) 
           strcpy(FPN, fp);

       /* found it in RO library, don't delete */ 
       dmsSearchCellView(0, 0, (char *)cName, (char *)vName, 
			 (char *)verName, FPN, locker, &new_ver, 
			 noError, dmsKeepTmpCopy);
       if (!*noError) /* error! Can't find it */
	 return save_status(*noError, 0, 0, (char *)cName,
			    (char *)vName, "", "", FPN, change_codes);
       
       idb_set_content_at_id(pplib_id, plib_id);
       
       return save_status(*noError = idmsCvInRwlib, 
			  (char *)idb_get_library_directory(plib_id, "w"), 
			  (char *)idb_get_library_name(plib_id),
			  (char *)cName, (char *)vName, new_ver, locker, FPN,
			  change_codes);  
       break;

     default:
       break;
     }
     ptr=ptr->next;
   }

   /* Check Work-Dir at the end */
   dmsWorkDir = dmsGetWorkingDir((char *)vName, 0);
   if(dmsWorkDir) {
     full_path = dmsFormFileName(dmsWorkDir, (char *)cName, 
                                 (char *)vName, CALL_GET_EXT);
     if (osFileExists(full_path)) {
       strcpy(FPN, full_path);
       osRemoveFile(full_path); 
       dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, WORKDIRFILEFOUND, (char *)cName, 
                      idmGetExtension((char *)vName, 0), dmsWorkDir); 
       return   save_status(*noError = idmsCvInWorkdir, 0, 0,
                            (char *)cName, (char *)vName, "", "",
                            FPN, change_codes);
       
     }
   }

   dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR,FILENOTFOUND,cName,vName,verName,dmsConfName);
   return save_status(*noError = idmsCvNotFound, "", "", 
		      (char *)cName, (char *)vName, "", "", "", change_codes);
}



void dmsRmCell(const char *libName, char *cName, char *vName,
		   char *verName, char *FPN, char *locker, 
		   char **new_ver, int *noError)
{
  char *cellviewList = 0;
  DSSRESULT deleteStatus;
  void *plib_id;
  char *temp;
  int i=0;
  int t=0;
  int done = 0;
  
  if (debugDmsOn)
    dmsPrintIdmsLog("*IDMS DEBUG* Procedure Delete Cell\n");
     
  if(idb_gen_cellview_list((char *)libName, (char *)cName, &cellviewList)){
    /* parse cellviewList to delete each cellview in the cell */
    if(cellviewList != NULL){
      temp = strdup(cellviewList);
      
      while(cellviewList[i] != '\0'){
	if(cellviewList[i] == ' '){
	  temp[t] = '\0';
	  dmsRmCellView(2,(char *)libName,(char *)cName,(char *)temp, "", 
			FPN, locker, new_ver, noError);
          if(!*noError) /* check if cell view is checked out... */
            done = 1;
          
	  t = 0;
	}
	else{
	  temp[t] = cellviewList[i];
	  t = t + 1;
	}
	i = i + 1;
      }
	
      /* delete the cell */
      FPN[0] = '\0';
      if(!done){
        plib_id = idb_check_cell_in_lib((char *)libName, (char *)cName);
        deleteStatus = idb_delete_cell(plib_id , cName, vName, verName, FPN, 
                                       locker, new_ver, 0);
      } else {
        *noError = 0; /* Some CellView is Checked Out */
        return;
      }
            
      if(DSS_SUCCESS(deleteStatus)){
	*noError = 1;
      } else {
        *noError = 0; /* Some CellView is Checked Out */
      }
      
    } else {
      *noError = 0; /*  No cell view found */
    }
    
  } else {
    *noError = 0; /* Cell doesn't exist  */
  }
#ifdef DISABLE_SYNCHRONICITY
  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, DISABLESYNC);
#endif
  return;
}



void dmsRmCellView(int delete, char *libName, char *cName, char *vName,
		   char *verName, char *FPN, char *locker, 
		   char **new_ver, int *noError)
{
  char *extension;
  char  workDirFile[BUFSIZE];
  void *plib_id = idb_get_lib_id();
  void *ver_id;
  DSSRESULT deleteStatus;
  int proceed = TRUE;
  
  if (debugDmsOn){
    dmsPrintIdmsLog("*IDMS DEBUG* - dmsRmCellView(%d)\n", delete );
  }

  if (!(ver_id = (void *)idb_find_version_and_locker(plib_id = idb_get_lib_id(),
						     cName, vName, verName, locker, 
						     FPN, 1, new_ver))){
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR,VERSIONNOTEXIST,cName,vName,dmsGetLastLibName(),verName);
    *noError = idmsCvNotFound; /* *noError = 0 */
    return;
  }
    
  extension = idmGetExtension(vName, 0);
  sprintf(workDirFile,"%s%s.%s", dmsGetWorkingDir(vName, 0), cName, extension);
  
  *noError = idmsCvNotFound; /* *noError = 0 */

  if(!strcmp(locker, NOBODY) || !strcmp(locker, userName)) {
    if(!strcmp(locker, NOBODY)) {
      /*** not checked out cellview with writable copy in workdir ***/
      if(osFileExists(workDirFile)) {
        osRemoveFile(workDirFile);
      }
      else /*** not checked out and not in workdir ***/
	FPN[0] = '\0';

      if(delete == 3)
	deleteStatus = idb_delete_version(plib_id, cName, vName, verName, FPN, 
					  locker, new_ver, 0);
      else /** if(delete == 1) **/
	deleteStatus = idb_delete_cellview(plib_id, cName, vName, verName, FPN, 
					   locker, new_ver, 0);
    } 
    else { /*** checked out by current user ***/
      dmsCSetLastMessage("File Checked out in Work Directory");
      deleteStatus = E_FAIL;
    }
   
    if (debugDmsOn){
       dmsPrintIdmsLog("*IDMS DEBUG* - dmsRmCellView - after idb_delete_cellview, status=%d\n", deleteStatus );
    }

    if(DSS_SUCCESS(deleteStatus)) {
      if(osFileExists(workDirFile)) {
        osRemoveFile(workDirFile);
        *noError = idmsCvInWorkdir; /* 1 */
      } else {
	*noError = idmsCvInWorklib; /* 5 */
      }
    } else {      /* Delete: Failed */     
#ifdef DISABLE_SYNCHRONICITY
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, DISABLESYNC);
#endif
      return;
    }
  }

  else { /* checked out by another user */
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, FILERECHECKOUT,  dmsGetLastLibName(), 
		   cName, vName, *new_ver, FPN, locker);
    return;
  }

  return;
}




int dmsGetCdslibFileName(char *fileName)
{
  char *cdslibPath;
  int error = 0;
  
  cdslibPath = idb_get_startup("cds.lib");
  if(osFileExists(cdslibPath)){
    strcpy(fileName,cdslibPath);
    error =0;
  }
  else
    error =1;

  return error;
}
