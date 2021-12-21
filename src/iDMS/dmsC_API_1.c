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
# $Id: dmsC_API_1.c,v 1.1.2.50 1998/01/12 18:47:28 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

/* For getuid(), getpwuid() */

#include <turbo/dmsT_API.h>

#include <dirent.h>

#include <sys/types.h>
#include <pwd.h>
#ifdef HPPA
#include <unistd.h>
#endif

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dmsC.h>
#include <dmsC_new.h>
#include <dmsC_new_vars.h>
#include <dmsC_functions2.h>
#include <dmsC_internal_functions.h>
#include <dmsC_reports.h>
#include <idb_functions.h>
#include <dmsC_triggers.h>
#include <fwimsg.h>
#include <dmsC_status.h>
#include <table_functions.h>
#include <dmsC_dm.h>
#include <os_functions.h>

extern DMSINFO dmsLastCVInfo;

Boolean idmInitUserTriggers(int nullgt); 

static char _cdslibbuff[BUFSIZE];

/*==================*
 * STATIC FUNCTIONS *
 *==================*/

/*-----------------------------------------------------------------------
 * EXIT FUNCTIONS 
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:
 *    Global vars: idmsNotInitialized
 *
 * Uses:
 *    Global vars:
 *
 *
 * Description: Exiting
 *-----------------------------------------------------------------------*/


static void dmsExitDMS(int *noError)
{
   dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, EXITDMS, DMSVERSION);
   *noError = TRUE;
}


static void dmsPIExitDMS(int *noError)
{
   /* close all libraries to avoid iAWS left link locks on libraries */
   dmsCRefreshAllLib(0);

   idb_terminate();

   dmsTExit();

   idmsNotInitialized = TRUE;
   deallocate_dms_structs();
   deallocate_all_structs();
   dmsExitDMS(noError);
}


/*======================*
 * EXPORTABLE FUNCTIONS *
 *======================*/

/*----------------*
 * Init Functions *
 *----------------*/

void coralCDisableObsoleteMessages()
{
   disable_obsolete_messages = 1;
}


Boolean dmsCCheckEnvSettings(char **message)
{
   return (Boolean)idb_check_env_settings(message);
}

Boolean dmsCRegisterTriggers(int mode)
{
   return idmRegisterTriggers(mode);
}


Boolean dmsCParseDmsPathFile(CONST char *filename)
{
   int noError;
   dmsParsePath((char *)filename, &noError);
   return (noError)?TRUE:FALSE;
}


Boolean dmsCRegisterUserTriggers(int mode)
{
   return idmInitUserTriggers(mode);
}


static char *_sys_cmd;

CONST char *dmsCInitDmsPath(CONST char *appName, CONST char *bootName,
                            int nullgtMode)
{
   char *dmsPathFile = 0;
   struct __dirstream *tmp_dir = 0;
   char *turbo_value;

   int noError, error;

   /* Initialize variables */

   if (!dmsAppName)
      dmsAppName = strdup(appName);
   toolName.name = strdup(appName);
   toolName.next = 0;

   init_dms_structs();

   userName = strdup(getpwuid(getuid())->pw_name);
  
   /* here is a good place to create the /tmp/$LOGNAME/idms directory */

   sprintf(dms_tmp_dir, "/tmp/%s/idms", userName);
   dms_tmp_usr_dir_attempted = 1;
   tmp_dir = opendir(dms_tmp_dir);
  
   if (tmp_dir){
      closedir(tmp_dir);
   }
   else {
      asprintf(&_sys_cmd, "/bin/mkdir -p %s", dms_tmp_dir);
      system(_sys_cmd);
      free(_sys_cmd);
      tmp_dir = opendir(dms_tmp_dir);
      if (tmp_dir) {
         closedir(tmp_dir);
      }
      else { /* directory creation failed */
         strcpy(dms_tmp_dir, "/tmp");
      } 
   }

   /* FLAGS */

   if (dmsAppGetEnv("DMSLOGFILE"))
      dmsDoLogAfterAll = 1;
   if (dmsAppGetEnv("DEBUGDMS"))
      debugDmsOn = 1;

   if (dmsAppGetEnv("DEBUG_IDB"))
      debug_idb = 1;

   if (dmsAppGetEnv("DMS_FIND_UNMANAGED")) {
      dmsFindUnmanaged = 1;
   }
   if (dmsAppGetEnv("DMS_DO_NOT_FIND_UNMANAGED")) {
      dmsFindUnmanaged = 0;
   }

   if (turbo_value = dmsAppGetEnv("TURBO_DMSERVER")){
      turbo_dmserver = 1;
      dmsTInit();
      if (strncasecmp(turbo_value, "TWIN", strlen("TWIN")) == 0){
         twin_turbo = dmsTSetTwinTurboMode();
      }
   }

   if (dmsAppGetEnv("TURBO_DEBUG")){
      dmsTSetDebug();
   }

   /* dms.path file */

   if (dmsAppGetEnv("DMSPATH"))
       dmsPathFile = strdup(dmsAppGetEnv("DMSPATH"));
   else
       dmsPathFile = 0;
   return dmsPathFile;
}


Boolean dmsCInitVendorDM(CONST char *dmsBootName)
{
   int returnVal;

   returnVal = idb_init((void *)dmsBootName);
   if(returnVal) idmsNotInitialized = 0;
   /****
   idb_set_user_rights(userName);
   idb_set_refresh_period(30);
   ****/
   return returnVal; 
}


/*-----------------------------------------------------------------------
 * dmsCPIInitDMS
 *
 * Parameters:
 *   appName   : application name
 *   bootName  : module name, the Mainsail ghost is still well!
 *   nullgtMode: Opus specific rather
 *
 * Modifies:
 *    Static vars:
 *    Global vars: dmsAppName, dmsDoLogAfterAll, debugDmsOn, toolName,
 *                 userName, noError
 *
 * Uses:
 *    Global vars:
 *
 *
 * Description: Initializes the idms Data structures.
 *
 *-----------------------------------------------------------------------*/


int dmsCPIInitDMS(char *appName, char *bootName, int   nullgtMode)
{
  int noError, error;
  char *dmsPathFile = (char *)dmsCInitDmsPath(appName, bootName, nullgtMode);

  if (debugDmsOn)
     dmsPrintIdmsLog("<<*C-CALL*>> dmsCPIInitDMS(%s,%s,%d)\n", appName, 
                     bootName, nullgtMode);

  dmsPIInitDMS(bootName, dmsPathFile, nullgtMode, &noError);
  error=dmsGenErrorCode(noError);

  if (debugDmsOn) 
     dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCPIInitDMS(%s,%s,%d)\n",
                    error, appName, bootName, nullgtMode);
  if (debugDmsOn) 
     dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber, dmsCGetLastMessage());

  /* deallocate */

  if (dmsPathFile)
     free(dmsPathFile);

  return error;

} /* dmsCPIInitDMS */


/*-----------------------------------------------------------------------
 * dmsCPIExitDMS
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:
 *    Global vars: debugDmsOn
 *
 * Uses:
 *    Global vars:
 *
 *
 * Description: Exits dms
 *
 *-----------------------------------------------------------------------*/

int dmsCPIExitDMS()
{
   int noError,error;
      if (debugDmsOn)
         dmsPrintIdmsLog("<<*C-CALL*>> dmsCPIExitDMS()\n");

   dmsPIExitDMS(&noError);
   error = dmsGenErrorCode(noError);

   if (debugDmsOn){
     dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCPIExitDMS()\n",error);
     dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber,
                     dmsCGetLastMessage());
   }

   if (idmsLogFilePtr)
      fclose(idmsLogFilePtr);
   return error;
}


/*-----------------------------------------------------------------------
 * dmsCReportMessage 
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
 * Description: Kept here because of compatibility reasons. This function'
 *              never actually reported anything but returned a char pointer.
 *-----------------------------------------------------------------------*/


char *dmsCReportMessage()
{
   return (char *)dmsCGetLastMessage();
}


/*-----------------------------------------------------------------------
 * dmsCGetSearchPath 
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
 * Description: Just a wrapper (because static var defined in another file)
 *-----------------------------------------------------------------------*/


char * dmsCGetSearchPath()
{
   return dmsGetLastSearchPath();
}


/*-----------------------------------------------------------------------
 * CellView Functions
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
 * Description: Just a wrapper (because static var defined in another file)
 *-----------------------------------------------------------------------*/

static char _cellviewbuff[BUFSIZE];

int dmsCCreateCellView(char *cName, char *vName, char **FPN)
{
  int noError, error;
  char *tempfilename;
  char *dmsWorkingDir = dmsGetWorkingDir(vName, 0);

  if (FPN)
     *FPN = 0;
  if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCCreateCellView(%s,%s,%s)\n",
                      cName, vName, *FPN);

  if (idmsNotInitialized) {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
     return idmsNotInit;
  }

  if ((noError = dmsCExecutePreTrigger("create", "", cName, vName, "", "")) 
      != idmsSuccess)
     return noError; 

  strcpy(_cellviewbuff, "");

  dmsCreateCellView(0, cName, vName, BIN, _cellviewbuff, &noError);

  dmsCExecutePostTrigger("create", idb_get_last_library_name(), cName, vName, 
                         dmsGetLastVerName(), _cellviewbuff);

  /** no install trigs are called here. this is to emulate the create of 4.3 **/
  if(noError) {
    dmsInstallCellView(0, cName,vName,"BIN","", "", _cellviewbuff, &noError, DONT_CREATE);
  }

  *FPN = _cellviewbuff;

  error = dmsGenErrorCode(noError);

  if (debugDmsOn){
     dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCCreateCellView(%s,%s,%s)\n",
                     error, cName, vName, *FPN);
     dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber, 
                     dmsCGetLastMessage());
  }
  return(error);
}


int dmsCCreateUnixFile(char *cName, char *vName, char **FPN)
{
   char *extension = idmGetExtension(vName, 0);
   char *dmsWorkingDir = dmsGetWorkingDir(vName, 0);
   int error = idmsSuccess;
   int noError;

   *FPN = dmsFormFileName(dmsWorkingDir, cName, extension, DONT_CALL_GET_EXT);
   if (debugDmsOn) 
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCCreateUnixFile(%s,%s,%s)\n",
                      cName,vName,*FPN);

  if ((noError = dmsCExecutePreTrigger("create", "", cName, vName, "", ""))
      != idmsSuccess)
     return noError;

  if (!osFileExists(*FPN)) {
    if (osCreateFile(*FPN)){
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, CREATEUNIXOK, cName, extension, dmsWorkingDir);
      save_status(noError = idmsCvInWorkdir, 0, 0, cName, vName, "", "",
                  *FPN, DONT_CHANGE_CODES);
    }
    else {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, error = CANTCREATEFILE, cName, extension,
                     dmsWorkingDir);
      save_status(noError = idmsCvNotFound, 0, 0, cName, vName, "", "", "",
                  DONT_CHANGE_CODES);
    }
  }
  else {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, error = EXISTALREADY, cName, extension,
                     dmsWorkingDir);
      save_status(noError = idmsCvNotFound, 0, 0, cName, vName, "", "", "",
                  DONT_CHANGE_CODES);
   }

   /**
   if (noError)
      dmsCSetGeneratedAttribute("create", 0, cName, vName, 0);   
   **/
   dmsCExecutePostTrigger("create", idb_get_last_library_name(), cName, vName,
                         dmsGetLastVerName(), *FPN);

   if (debugDmsOn){
      dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCCreateUnixFile(%s,%s,%s)\n",
                      error, cName, vName, *FPN);
      dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n",dmsLastErrorNumber,
                      dmsCGetLastMessage());
   }
   return error;
} 

int dmsCReadonlyCellViewByLib(char *lName, char *cName, char *vName,
                              char *verName, char **FPN)
{
  int noError, error;
  void *plib_id = idb_get_empty_lib_id();

  *FPN = 0;
  if (debugDmsOn)
     dmsPrintIdmsLog("<<*C-CALL*>> dmsCReadonlyCellViewByLib(%s,%s,%s,%s,%s)\n",
                     lName, cName, vName, verName, *FPN);

  if (idmsNotInitialized) {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
     return idmsNotInit;
  }

  strcpy(theLibName, lName);
  /** REM: read non cdba stuff ***/
  dmsReadCellView(0, 0, DONT_CREATE, cName, vName, verName, BIN, _cellviewbuff,
                  &plib_id, &noError, DONT_CHANGE_CODES);
  *FPN = _cellviewbuff;
  error=dmsGenErrorCode(noError);

  if (debugDmsOn){
    dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCReadonlyCellViewByLib(%s,%s,%s,%s,%s)\n",
                    error, lName, cName, vName, verName, *FPN);
    dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n",dmsLastErrorNumber,
                    dmsCGetLastMessage());
  }
  return error;
}


int dmsCReadonlyCellView(char *cName, char *vName, char **FPN)
{
   return(dmsCReadonlyCellViewByLib("*",cName,vName,"",FPN));
}


/* SHOULD BE SPED UP !!! */

int dmsCReadonlyCellViewTypeByLib(char *lName, char *cName, char *vtName,
                                  char *verName, char **vName, char **FPN)
{
  static char _viewbuff[BUFSIZE];
  static char tmp[BUFSIZE];
  int noError, error;
  void *plib_id = idb_get_empty_lib_id();
  char *vNameList;

  *vName = 0;
  *FPN = 0;
  if (debugDmsOn) 
     dmsPrintIdmsLog("<<*C-CALL*>> dmsCReadonlyCellViewTypeByLib(\
                      %s,%s,%s,%s,%s,%s)\n", lName, cName, vtName, verName,
                      *vName, *FPN);

  if (idmsNotInitialized) {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
     return idmsNotInit;
  }

  strcpy(theLibName,lName);
  if (idmGetViewName(toolName.name, vtName, &vNameList)) {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, VIEWMAPERROR, toolName.name, vtName);
     noError=idmsCvNotFound;
  }
  else {
     strcpy(tmp, vNameList);
     dmsSeparateByColon(vNameList,_viewbuff, vNameList);
     while(strcmp(_viewbuff,"")) {
        dmsReadCellView(0, 0, DONT_CREATE, cName, _viewbuff, verName, BIN, _cellviewbuff,
                        &plib_id, &noError, DONT_CHANGE_CODES);
        if (noError) {
           *vName = _viewbuff;
           break;
        }
        dmsSeparateByColon(vNameList, _viewbuff, vNameList);
     }
     if (dmsLastErrorNumber==idmsCvNotFoundOnRwpaths) {
        sprintf(_viewbuff,"%s(%s)",vtName, tmp);
        dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR,FILENOTFOUND,cName,_viewbuff,verName,dmsConfName);
        noError = idmsCvNotFound;
     }
  }
   if (noError == idmsCvNotFound){
      save_status(noError, 0, 0, cName, _viewbuff, "", "", "", DONT_CHANGE_CODES);
      _cellviewbuff[0] = '\0';
   }
   *FPN = _cellviewbuff;
   error = dmsGenErrorCode(noError);

   if (debugDmsOn){
      dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCReadonlyCellViewTypeByLib(\
                      %s,%s,%s,%s,%s,%s)\n", error, lName, cName, vtName,
                      verName, *vName, *FPN);
      dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n",dmsLastErrorNumber,
                      dmsCGetLastMessage());
   }
   return error;
}


int dmsCReadonlyCellViewType(char *cName, char *vtName, char **vName,
                             char **FPN)
{
  return dmsCReadonlyCellViewTypeByLib("*", cName, vtName, "", vName, FPN);
}


int dmsCCheckoutCellViewByLib(char *lName, char *cName, char *vName,
                              char *verName, char **FPN)
{
   int noError, error;
   void *plib_id = idb_get_empty_lib_id();
   *FPN = 0;
   if (debugDmsOn) 
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCCheckoutCellViewByLib(\
                      %s,%s,%s,%s,%s)\n", lName, cName, vName, verName, *FPN);

   if (idmsNotInitialized) {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
      return idmsNotInit;
   }
   strcpy(theLibName,lName);

  /** This is not necessary since the sync sys execs the co triggers ?? */
  if ((noError = dmsCExecutePreTrigger("checkout", "", cName, vName, "", ""))
      != idmsSuccess)
     return noError;

   dmsReadCellView(0, 1, DONT_CREATE, cName, vName, verName, BIN, _cellviewbuff,
                   &plib_id, &noError, DONT_CHANGE_CODES);
   *FPN = _cellviewbuff;

   /**
   if (noError)
      dmsCSetGeneratedAttribute("checkout", idb_get_last_library_name(),
                                cName, vName, dmsGetLastVerName());
   **/
   dmsCExecutePostTrigger("checkout", idb_get_last_library_name(), cName, vName,
                          dmsGetLastVerName(), _cellviewbuff);

   error = dmsGenErrorCode(noError);

   if (debugDmsOn){
     dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCCheckoutCellViewByLib(\
                     %s,%s,%s,%s,%s)\n", error, lName, cName, vName, verName,
                     *FPN);
     dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n",dmsLastErrorNumber, 
                     dmsCGetLastMessage());
   }
   return error;
}


int dmsCCheckoutCellView(char *cName, char *vName, char **FPN)
{
  return dmsCCheckoutCellViewByLib("*", cName, vName, "", FPN);
}


int dmsCLoadCellView(char *cName, char *vName, char **FPN) 
    /* establishes link lock */
{
   int  noError, error;
   char extension[BUFSIZE];
   char *_ibuff;
   char *dmsWorkDir;

   *FPN = 0;
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCLoadCellView(%s,%s,%s)\n", cName,
                      vName, *FPN);

   dmsWorkDir = dmsGetWorkingDir(vName, 0);
   idmGetExtension(vName, extension);
   _ibuff = dmsFormFileName(dmsWorkDir, cName, extension, DONT_CALL_GET_EXT);
   if (osFileExists(_ibuff)) {
      if (osFileIsReadable(_ibuff)) {
         dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, LOADFROMWORKDIR, cName, extension,
                        dmsWorkDir);
          noError = idmsCvInWorkdir;
      }
      else {
         dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, FILENOTREADABLE, cName, extension,
                        dmsWorkDir);
         noError = idmsCvNotFound;
      }
   }
   else {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, NOTEXISTTOLOAD, cName, extension, dmsWorkDir);
      noError = idmsCvNotFound;
   }
   *FPN = _ibuff;
   if (noError == idmsCvNotFound)
      save_status(noError, 0, 0, cName, vName, "", "", "", DONT_CHANGE_CODES);
   else
      save_status(noError, 0, 0, cName, vName, "", "", _ibuff,
                  DONT_CHANGE_CODES);
   error = dmsGenErrorCode(noError);

   if (debugDmsOn){
      dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCLoadCellView(%s,%s,%s)\n",
                      error, cName, vName, *FPN);
      dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber,
                       dmsCGetLastMessage());
   }
   return error;
}


int dmsCEditCellViewByLib(char *lName, char *cName, char *vName, 
                          char *verName, char **FPN)
{
   int error;
   void *plib_id = idb_get_empty_lib_id();

   *FPN = 0;
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCEditCellViewByLib(%s,%s,%s,%s,%s)\n",
                      lName, cName, vName, verName, *FPN);

  if (idmsNotInitialized) {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
     return idmsNotInit;
  }
  strcpy(theLibName,lName);

   if (error = dmsCLoadCellView(cName,vName,FPN))
      error = dmsCCheckoutCellViewByLib("*",cName,vName,"",FPN);

   if (debugDmsOn){
      dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCEditCellViewByLib(%s,%s,%s,%s,%s)\n",
                      error, lName, cName, vName, verName, *FPN);
      dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n",dmsLastErrorNumber,
                      dmsCGetLastMessage());
   }
   return error;
} 


int dmsCEditCellView(char *cName, char *vName, char **FPN)
{
   return dmsCEditCellViewByLib("*", cName, vName, "", FPN);
}


int dmsCReadOrEditByPolicyByLib(char *lName, char *cName, char *vName,
                                char *verName, char **FPN)
{
  int noError, error;
  void *plib_id = idb_get_empty_lib_id();

  *FPN = NULL;
  if (debugDmsOn) 
     dmsPrintIdmsLog("<<*C-CALL*>> dmsCReadOrEditByPolicyByLib(\
                     %s,%s,%s,%s,%s)\n", lName, cName, vName, verName, *FPN);

  if (idmsNotInitialized) {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
     return idmsNotInit;
  }
  strcpy(theLibName,lName);

  dmsReadCellView(0, 2, DONT_CREATE, cName, vName, verName, BIN, _cellviewbuff, &plib_id,
                  &noError, DONT_CHANGE_CODES);
  if (noError == idmsCvInRwlib) /* establish link lock for loading */
    return(dmsCLoadCellView(cName,vName,FPN));
  *FPN = _cellviewbuff;
  error = dmsGenErrorCode(noError);

  if (debugDmsOn){
     dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCReadOrEditByPolicyByLib(\
                      %s,%s,%s,%s,%s)\n", error, lName, cName, vName, verName,
                      *FPN);
     dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber, 
                     dmsCGetLastMessage());
  }
  return error;
}


int dmsCReadOrEditByPolicy(char *cName, char *vName, char **FPN)
{
  return dmsCReadOrEditByPolicyByLib("*",cName,vName,"",FPN);
}


int dmsCGetLastCellViewInfo(DMSINFO **information)
{
   *information = NULL;
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCGetLastCellViewInfo()\n");

   if (idmsNotInitialized) {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
      return idmsNotInit;
   }

   *information = dmsGetStatus(TRUE);

  if (debugDmsOn){
     dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCGetLastCellViewInfo()\n",
                     dmsLastCVInfo.location);
     dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber,
                     dmsCGetLastMessage());
  }
  return dmsLastCVInfo.location;
}


int dmsCInfoCellView(int searchLibOnly, char *cName, char *vName,
                     DMSINFO **information)
   /* searchLibOnly --  0: full rwpaths, 1: libraries only */
{
   int noError, returnValue;
   char FPN[BUFSIZE];
   void *plib_id = idb_get_empty_lib_id();

   *information = NULL;
   FPN[0] = '\0';

   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCInfoCellView(%d,%s,%s)\n",
                      searchLibOnly, cName, vName);

   if (idmsNotInitialized) {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
      return idmsNotInit;
   }

   /** REM: ???? theLibrary ?? ***/
   if (searchLibOnly)
     /* 3 for skip unix dir */
      returnValue = dmsReadCellView(2, 3, DONT_CREATE, cName, vName, "", BIN,
                                    FPN, &plib_id, &noError, CHANGE_CODES);
   else
     /* 0 for also unix dir */
      returnValue = dmsReadCellView(2, 0, DONT_CREATE, cName, vName, "", BIN,
                                    FPN, &plib_id, &noError, CHANGE_CODES);

   switch (returnValue) {
      case idmsInfoCvNotFoundOnRwpaths:
         if (searchLibOnly) /* not in lib but maybe in unix dir */
            returnValue = idmsInfoCvNotFoundInAnyLibrary;
         break;
      case idmsInfoCvIsInReadonlyLibrary: /* in RO lib */
      case idmsInfoCvCheckedOutByAnotherUser: /* in RW lib */
      case idmsInfoCvCanBeCheckedOut: /* in RW lib */
      case idmsInfoCvCheckedOutByMe: /* in RW lib */
    /* release link lock, the info call should not leave the link lock on */
         idb_close_cellview(idb_get_lib_id(),cName,vName,""); 
         idb_capture_dm_vendor_warning();
         break;
   }

   dmsCGetLastCellViewInfo(information); 
   /**information = dmsGetStatus(FALSE)*/;

   if (debugDmsOn){
      dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCInfoCellView(%d,%s,%s)\n",
                      returnValue, searchLibOnly, cName, vName);
      dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber,
                      dmsCGetLastMessage());
   }
   return returnValue;
}


/* pack info as string to send to mainsail and unpack */
/* searchLibOnly 0 for full rwpaths, 1 for libraries only */

static char _packbuff[BUFSIZE];

int dmsCInfoPackage(int searchLibOnly, char *cName, char *vName, char **package)
{
   int error;
   DMSINFO *information;

   *package = NULL;
   error = dmsCInfoCellView(searchLibOnly, cName, vName, &information);
   sprintf(_packbuff, "%d:%s:%s:%s:%s:%s:%s:%s", information->location,
           information->libPath, information->libName, information->cellName,
           information->viewName,information->verName, information->coUser,
           information->fullPathName);
   *package = _packbuff;
   return error; 
}


/* pack info as string to send to mainsail and unpack */
int dmsCLastInfoPackage(char **package)
{
   int error;
   DMSINFO *information;

   *package=NULL;
   error = dmsCGetLastCellViewInfo(&information);
   sprintf(_packbuff, "%d:%s:%s:%s:%s:%s:%s:%s", information->location,
           information->libPath, information->libName, information->cellName,
           information->viewName,information->verName, information->coUser,
           information->fullPathName);
   *package = _packbuff;
   return error;
} 


/* return path for file saving */

int dmsCSaveCellView(char *cName, char *vName, char **FPN) 
{
   int noError,error;
   char *extension;
   static char _ibuff[BUFSIZE];
   char *dmsWorkDir;

   *FPN = NULL;
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCSaveCellView(%s,%s,%s)\n", cName,
                      vName, *FPN);

   if ((noError = dmsCExecutePreTrigger("save", "", cName, vName, "", ""))
       != idmsSuccess)
      return noError;

   dmsWorkDir = dmsGetWorkingDir(vName, 0);
   extension  = idmGetExtension(vName, 0);  
   strcpy(_ibuff, 
          dmsFormFileName(dmsWorkDir, cName, extension, DONT_CALL_GET_EXT));
   if (osFileExists(_ibuff)) {
      if (osFileIsWritable(_ibuff)) {
         dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, SAVETOWORKDIR, cName, extension, dmsWorkDir);
         noError = idmsCvInWorkdir;
      }   
      else {
         dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, FILENOTWRITEABLE, cName, extension,dmsWorkDir);
         noError = idmsCvNotFound;
      }
  }
  else {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, NOTEXISTTOSAVE, cName, extension, dmsWorkDir);
     noError = idmsCvNotFound;
  }
  *FPN = _ibuff;
  if (noError == idmsCvNotFound)
     save_status(noError, 0, 0, cName, vName, "", "", "", DONT_CHANGE_CODES);
  else
     save_status(noError, 0, 0, cName, vName, "", "", _ibuff, DONT_CHANGE_CODES);

  /**
  if (noError)
     dmsCSetGeneratedAttribute("save", idb_get_last_library_name(),
                               cName, vName, 0);
  **/
   dmsCExecutePostTrigger("save", idb_get_last_library_name(), cName, vName,
                          dmsGetLastVerName(), _ibuff);

  error = dmsGenErrorCode(noError);

  if (debugDmsOn){
     dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCSaveCellView(%s,%s,%s)\n", error,
                     cName, vName, *FPN);
     dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber, 
                     dmsCGetLastMessage());
  }
  return error;
} 


int dmsCInstallCellViewByLib(char *lName, char *cName, char *vName, 
                             char *children, char *log, char **FPN)
{
   int noError, error;
   void *plib_id = idb_get_empty_lib_id();

   *FPN = NULL;
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCInstallCellViewByLib(\
                       %s,%s,%s,%s,%s,%s)\n", lName, cName, vName, children,
                       log, *FPN);

   if (idmsNotInitialized) {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
      return idmsNotInit;
   }
   strcpy(theLibName, lName);

   if ((noError = dmsCExecutePreTrigger("install", "", cName, vName, "", ""))
       != idmsSuccess)
      return noError;

   dmsWriteCellView(0,2,cName,vName,"", BIN, log, "", _cellviewbuff, 
                    &plib_id, &noError);
   /* 0 for non cdba, 1 for cdba */
   /* 0 for checkin, 1 for update, 2 for install */
   *FPN = _cellviewbuff;

   /**
   if (noError)
      dmsCSetGeneratedAttribute("install", idb_get_last_library_name(),
                                cName, vName, idb_get_first_default_version());
   **/
   dmsCExecutePostTrigger("install", idb_get_last_library_name(), cName, vName,
                          dmsGetLastVerName(), _cellviewbuff);

   error = dmsGenErrorCode(noError);

   if (debugDmsOn){
       dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCInstallCellViewByLib(\
                       %s,%s,%s,%s,%s,%s)\n", error, lName, cName, vName,
                       children, log, *FPN);
       dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber, 
                       dmsCGetLastMessage());
   }
   return error;
}


int dmsCInstallCellView(char *cName, char *vName, char *children, char *log,
                        char **FPN)
{
   return dmsCInstallCellViewByLib("*", cName, vName, children, log, FPN);
}


int dmsCCheckinCellViewByLib(char *lName, char *cName, char *vName,
                             char *verName, char *children, int advance,
                             char *log, char **FPN)
{
   int noError,error;
   static char _ibuff[BUFSIZE];
   void *plib_id = idb_get_empty_lib_id();

   *FPN = NULL;
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCCheckinCellViewByLib(\
                      %s,%s,%s,%s,%s,%d,%s,%s)\n", lName, cName, vName, 
                      verName, children, advance, log, *FPN);

   if (idmsNotInitialized) {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
      return idmsNotInit;
   }
   strcpy(theLibName, lName);

   /** This is no longer necessary since the sync system execs the ci triggers */
   if ((noError = dmsCExecutePreTrigger("checkin", "", cName, vName, "", ""))
       != idmsSuccess)
      return noError;

   dmsWriteCellView(0, 0, cName, vName, verName, TEXT, log, "", _ibuff, 
				&plib_id, &noError);
   *FPN= _ibuff;

   /* 0 for non cdba, 1 for cdba */
   /* 0 for checkin, 1 for update, 2 for install */

   /**
   if (noError)
      dmsCSetGeneratedAttribute("checkin", idb_get_last_library_name(),
                                cName, vName, dmsGetLastVerName());
   **/
   dmsCExecutePostTrigger("checkin", idb_get_last_library_name(), cName, vName,
                          dmsGetLastVerName(), _ibuff);

   error = dmsGenErrorCode(noError);

   if (debugDmsOn){
       dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCCheckinCellViewByLib(\
                       %s,%s,%s,%s,%s,%d,%s,%s)\n", error, lName, cName, vName,
                       verName, children, advance,log,*FPN);
       dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber, 
                       dmsCGetLastMessage());
   }
   return(error);
}


int dmsCCheckinCellView(char *cName, char *vName, char *children, int advance,
                        char *log, char **FPN)
{
  return dmsCCheckinCellViewByLib("*", cName, vName, "", children, advance,
                                  log, FPN);
}


int dmsCUpdateCellViewByLib(char *lName, char *cName, char *vName,
                            char *verName, char *children, int advance,
                            char *log, char **FPN)
{
   int noError, error;
   static char _ibuff[BUFSIZE];
   void *plib_id = idb_get_empty_lib_id();

   *FPN = NULL;
   if (debugDmsOn) 
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCUpdateCellViewByLib(\
                      %s,%s,%s,%s,%s,%d,%s,%s)\n", lName, cName, vName, 
                      verName, children, advance, log, *FPN);

   if (idmsNotInitialized) {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
      return idmsNotInit;
   }
   strcpy(theLibName, lName);

   if ((noError = dmsCExecutePreTrigger("update", "", cName, vName, "", ""))
       != idmsSuccess)
      return noError;

   dmsWriteCellView(0, 1, cName, vName, verName, TEXT, log, "", _ibuff, 
				&plib_id, &noError);
   /* 0 for non cdba, 1 for cdba */
   /* 0 for checkin, 1 for update, 2 for install */
   *FPN= _ibuff;

   /**
   if (noError)
      dmsCSetGeneratedAttribute("update", idb_get_last_library_name(),
                                cName, vName, dmsGetLastVerName());
   **/
   dmsCExecutePostTrigger("update", idb_get_last_library_name(), cName, vName,
                          dmsGetLastVerName(), _ibuff);

   error=dmsGenErrorCode(noError);

   if (debugDmsOn){
      dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCUpdateCellViewByLib(\
                      %s,%s,%s,%s,%s,%d,%s,%s)\n", error, lName, cName, vName,
                      verName, children, advance, log, *FPN);
      dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber,
                      dmsCGetLastMessage());
   }
   return error ;
}


int dmsCUpdateCellView(char *cName, char *vName, char *children, int advance,
                       char *log, char **FPN)
{
  return dmsCUpdateCellViewByLib("*", cName, vName, "", children, advance,
                                 log, FPN);
}


/* release link lock */

/** REM: this needs to be rewritten ***/
int dmsCPurgeCellView(char *cName, char *vName, char **FPN)
{
   int noError;
   void *plib_id = idb_get_empty_lib_id();

   *FPN = NULL;
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCPurgeCellView(%s,%s,%s)\n",
                      cName, vName, *FPN);

  if (idmsNotInitialized) {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
     return idmsNotInit;
  }

  /* 3 for checking library only, only library cellviews need to be purged */

   dmsReadCellView(0, 3, DONT_CREATE, cName, vName, "", BIN, _cellviewbuff, &plib_id,
                   &noError, DONT_CHANGE_CODES);
   idb_close_cellview(idb_get_lib_id(), cName, vName, "");
   idb_capture_dm_vendor_warning();
   *FPN = _cellviewbuff;

   if (debugDmsOn){
      dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCPurgeCellView(%s,%s,%s)\n",
                      idmsSuccess, cName, vName, *FPN);
      dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber,
                      dmsCGetLastMessage());
   }
   return idmsSuccess;
}


int dmsCCancelCheckoutCellViewByLib(char *lName, char *cName, char *vName,
                                    char *verName, char **FPN)
{
   int noError,error;
   static char _ibuff[BUFSIZE];
  
    *FPN = 0;
    if (debugDmsOn)
       dmsPrintIdmsLog("<<*C-CALL*>> dmsCCancelCheckoutCellViewByLib(\
                       %s,%s,%s,%s,%s)\n", lName, cName, vName, verName, *FPN);

   if (idmsNotInitialized) {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
      return idmsNotInit;
   }
   strcpy(theLibName, lName);

   if ((noError = dmsCExecutePreTrigger("cancelCheckout", "", cName, vName,
                                         "", "")) != idmsSuccess)
      return noError;
 
   dmsCancelCheckOutCellView(cName,vName,verName, _ibuff ,&noError);
   *FPN = _ibuff;


   /**
   if (noError)
      dmsCSetGeneratedAttribute("cancelCheckout", idb_get_last_library_name(),
                                cName, vName, dmsGetLastVerName());
   **/
   dmsCExecutePostTrigger("cancelCheckout", idb_get_last_library_name(),
                          cName, vName, dmsGetLastVerName(), _ibuff);
 
   error=dmsGenErrorCode(noError);

   if (debugDmsOn){
      dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCCancelCheckoutCellViewByLib(\
                      %s,%s,%s,%s,%s)\n", error, lName, cName, vName, verName,
                      *FPN);
      dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber,
                      dmsCGetLastMessage());
  }
  return error;
}


int dmsCCancelCheckoutCellView(char *cName, char *vName, char **FPN)
{
   return dmsCCancelCheckoutCellViewByLib("*", cName, vName, "", FPN);
}


/*-----------------------------------------------------------------
 * This function is added to be used only in LINUX when Cadence
 * is not available.  We have our own idb_gen_obj_rel to generate
 * a list of cell/view names in a specific library.  This is for
 * the DmBrowserTreeWindow
 *
 * UPDATED: (JUNE, 2004)
 * isCellName is also used to indicate the value of "getLocker" -
 * which is the last parameter of idb_gen_obj_rel.
 *   isCellName == 0:  isCellName=0, getLocker=1
 *   isCellName == 1:  isCellName=1, getLocker=1
 *   isCellName == 2:  isCellName=0, getLocker=2
 *   isCellName == 3:  isCellName=0, getLocker=3        <-- no header with libpath.
 *   isCellName == 4:  isCellName=0, getLocker=0        <-- no header with libpath.
 * NOTE: getLocker == 2 is added to output strings with 
 *       complete cell information.
 -----------------------------------------------------------------*/
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
char *dmCGenObjRel(char*library, char*name, int isCellName)
{
    if(isCellName == 0) {
       return idb_gen_obj_rel(library, name, 0,1); 
    } else if (isCellName == 1) {
       return idb_gen_obj_rel(library, name, 1,1); 
    } else if (isCellName == 2) {
       return idb_gen_obj_rel(library, name, 0,2); 
    } else if (isCellName == 3) {
       return idb_gen_obj_rel(library, name, 0,3); 
    } else if (isCellName == 4) {
       return idb_gen_obj_rel(library, name, 0,0); 
    } 
    return idb_gen_obj_rel(library, name, isCellName,1); 
}

static char _dmsLibPathBuff[BUFSIZE];

int dmsCGetLibPath(char *lName, char **libPath)
{
  char* l = idb_get_libpath(lName);
  if(l==NULL)
    return idmsFailure;
  else
    strcpy(_dmsLibPathBuff, l);

  *libPath = _dmsLibPathBuff;
  return idmsSuccess;

}


static char _rp[BUFSIZE];

int dmsCGetRealLibPath(char *lName, char **libPath)
{
  char *rp;
  char* l = idb_get_libpath(lName);
  if(l==NULL)
    return idmsFailure;
  else {
    strcpy(_dmsLibPathBuff, l);
    rp = realpath(_dmsLibPathBuff, _rp); 
    strcpy(_dmsLibPathBuff,  rp);
  }

  *libPath = _dmsLibPathBuff;
  return idmsSuccess;

}




#endif
#ifdef DISABLE_SYNCHRONICITY
static char _emptystring[BUFSIZE];
#endif
/*------------------------------------------------------------------
Jose needed the following two functions 

* For dmCGetVersionAndLocker:
* User responsible for allocating space for locker and path in order to
* avoid an extra level of indirection.
* ver/locker/get_last_if_not_locked/pverstr can be set to NULL based
* on requirements
------------------------------------------------------------------*/
char *dmCGetVersionAndLocker(char *lName, char *cName, char *vName,
                                  char *ver, char *locker, char *path,
                                  int get_last_if_not_locked, char **pverstr)
{
void *plib_id;
char *fullpath;
int noError;

plib_id = idb_find_lib_id(lName, 0, "r"); 

if (debug_idb){
   printf("*DEBUG_IDB* - dmCGetVersionAndLocker %d %s %s %s\n", plib_id, lName, cName, vName);
}

fullpath = idb_find_cellview(plib_id, cName, vName);
if (fullpath) {
   strcpy(path, fullpath);
}

#ifdef DISABLE_SYNCHRONICITY
strcpy(_emptystring, "");
ver = _emptystring;
*pverstr = _emptystring;
#endif
return idb_find_version_and_locker(plib_id, cName, vName, ver, 
				  	locker, path, get_last_if_not_locked, 
					pverstr);
}

/* vers are stored as space seperated elems
** Note: If the cv has been chked out, you will get in storeVers:
** "v1 v2 v3 v4 -> v5"
*/
int dmsCGetAllVersions(char *lName, char *cName, char *vName, char *storeVers )
{
void *plib_id;

plib_id = idb_find_lib_id(lName, 0, "r");

return idb_get_all_versions(plib_id, cName, vName, storeVers);
}

int dmsCParsePath()
{
   int noError, error;
   char dmsPathFile[BUFSIZE];
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCParsePath()\n");

   if (dmsAppGetEnv("DMSPATH"))
      strcpy(dmsPathFile, dmsAppGetEnv("DMSPATH"));
   else
      strcpy(dmsPathFile, ALLPOLICYUSEDEFAULT);
  
   dmsParsePath(dmsPathFile,&noError);
   error = dmsGenErrorCode(noError);

   if (debugDmsOn){
       dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCParsePath()\n",error);
       dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n",dmsLastErrorNumber,
                       dmsCGetLastMessage());
   }
   return error;
}


int dmsCParseThePath(char *pathFile)
{
   int noError, error;
   char dmsPathFile[BUFSIZE];
 
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCParseThePath(%s)\n",pathFile);

   if (strcmp(pathFile,"")) {
      strcpy(dmsPathFile, pathFile);
   }
   else {
      if (dmsAppGetEnv("DMSPATH"))
        strcpy(dmsPathFile,dmsAppGetEnv("DMSPATH"));
      else
        strcpy(dmsPathFile,ALLPOLICYUSEDEFAULT);
   }

   dmsParsePath(dmsPathFile,&noError);
   error=dmsGenErrorCode(noError);

   if (debugDmsOn){
      dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCParseThePath(%s)\n",
                      error, pathFile);
      dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber, 
                      dmsCGetLastMessage());
   }
   return(error);
} 


char * dmsCPrintPath(char *vName)
{
   dmsFormPrintPath(toolName.name, vName);
   return (char *)dmsCGetLastMessage();
}


char * dmsCVersionNumber()
{
   return DMSVERSION ;
}


int dmsCPrintIdmsLog(char *log)
{
  if (strlen(log)< MAXBUFSIZE)
     dmsPrintIdmsLog("%s\n",log);
  return idmsSuccess ;
}


char * dmsCGetPolicyViewList(char *tool, char *policy)
{
   int i;
   char tName[BUFSIZE],vName[BUFSIZE];
   static char _ibuff[BUFSIZE];

   strcpy(_ibuff,"");
   if (!strcmp(policy,"variable")) {
      for (i=0;i<varCount;i++) {
         strcat(_ibuff,var[i].name);
         strcat(_ibuff," ");
      }
   }
   else if (!strcmp(policy,"libpath")) {
      for (i=0;i<libpathCount;i++) {
         dmsSeparateByDot(libpath[i].name,tName,vName);
         if (dmsMatchExpression(tName,tool)) {
            strcat(_ibuff, vName);
            strcat(_ibuff," ");
         }
      }
   }
   else if (!strcmp(policy,"tconfig")) {
      for (i=0;i<tconfigCount;i++) {
         dmsSeparateByDot(tconfig[i].name,tName,vName);
         if (dmsMatchExpression(tName,tool)) {
            strcat(_ibuff,vName);
	    strcat(_ibuff," ");
         }
      }
   }
   else if (!strcmp(policy,"worklib")) {
      for (i=0;i<worklibCount;i++) {
         dmsSeparateByDot(worklib[i].name,tName,vName);
         if (dmsMatchExpression(tName,tool)) {
            strcat(_ibuff,vName);
            strcat(_ibuff," ");
         }
      }
   }
   else if (!strcmp(policy,"workdir")) {
      for (i=0;i<workdirCount;i++) {
         dmsSeparateByDot(workdir[i].name,tName,vName);
         if (dmsMatchExpression(tName,tool)) {
            strcat(_ibuff,vName);
            strcat(_ibuff," ");
         }
      }
   }
   else if (!strcmp(policy,"rwpaths")) {
      for (i=0;i<rwpathsCount;i++) {
         dmsSeparateByDot(rwpaths[i].name,tName,vName);
         if (dmsMatchExpression(tName,tool)) {
            strcat(_ibuff,vName);
            strcat(_ibuff," ");
         }
      }
   }
   return _ibuff;
}


/*-----------------------------------------------------------------------
 * Extensions Functions
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
 * Description: Just a wrapper (because static var defined in another file)
 *-----------------------------------------------------------------------*/

static char globalMultipleExtList[BUFFER_LENGTH];

/* procedure to read and return the mapping of the
** mutiple extensions stuff
*/
int dmsCGetMultipleExts(char **extList)
{
    char toolName[1024];
    idmTable *tablePtr;
    int i, rowNumber;
    char *tableEntry, *token;

    /* get the tool name */
    if (!dmsCGetToolName(toolName))
        return(idmsError);

    /* set the return value to point to the global var */
    *extList = globalMultipleExtList;
    globalMultipleExtList[0] = '\0';

    /* get info from the tables */
    if (dmsCGetTable(IDMS_EXTTYPE_EXTNAME_TABLE, &tablePtr) != idmsSuccess)
                return(idmsError);

    /* walk through the table and for each entry with this
    ** toolName extract the viewType and viewNameList from the
    ** table and put it into our global string
    */
    rowNumber = 1;
    while((i=dmsCGetTableEntry(tablePtr,rowNumber,&tableEntry))==idmsSuccess){
        token = strtok(tableEntry," ");
        if (strcmp(token,toolName) == 0) {
            /* get the viewType */
            token = strtok(NULL," ");
            if (token == NULL)
                return(idmsError);
            if ((strlen(token)+1) >= BUFFER_LENGTH) {
                dmsCPrintWarning("Buffer Length exceeded in dmsCGetMultiple views:%s\n",
                        token);
                return(idmsError);
            }  
            strcat(globalMultipleExtList,token);
            strcat(globalMultipleExtList,":");
            /* get the viewNameList */
            token = strtok(NULL," ");
            if (token == NULL)
                return(idmsError);
            if ((strlen(token)+1) >= BUFFER_LENGTH) {
                dmsCPrintWarning("Buffer Length exceeded in dmsCGetMultiple views:%s\n",
                        token);
                return(idmsError);
            }  
            strcat(globalMultipleExtList,token);
            strcat(globalMultipleExtList,";");
        }
        ++rowNumber;
    }
    /* the table call returns idmsNoRow the case there are no more
    ** entries left in the table
    */
    if (i != idmsNoRow)
        return(idmsError);
    return(idmsSuccess);
}


/*-----------------------------------------------------------------------
 * Views Functions
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
 * Description: Just a wrapper (because static var defined in another file)
 *-----------------------------------------------------------------------*/


static char globalMultipleViewList[BUFFER_LENGTH];

/* procedure to read and return the mapping of the
** mutiple views stuff
*/
int dmsCGetMultipleViews(char **viewList)
{
    char toolName[1024];
    idmTable *tablePtr;
    int i, rowNumber;
    char *tableEntry, *token;

    /* get the tool name */
    if (!dmsCGetToolName(toolName))
	return(idmsError);
    
    /* set the return value to point to the global var */
    *viewList = globalMultipleViewList;
    globalMultipleViewList[0] = '\0';

    /* get info from the tables */
    if (dmsCGetTable(IDMS_VIEWTYPE_VIEWNAME_TABLE, &tablePtr) != idmsSuccess) 
		return(idmsError);
    
    /* walk through the table and for each entry with this
    ** toolName extract the viewType and viewNameList from the
    ** table and put it into our global string
    */
    rowNumber = 1;
    while((i=dmsCGetTableEntry(tablePtr,rowNumber,&tableEntry))==idmsSuccess){
	token = strtok(tableEntry," ");
	if (strcmp(token,toolName) == 0) {
	    /* get the viewType */
	    token = strtok(NULL," ");
	    if (token == NULL)
		return(idmsError);
	    if ((strlen(token)+1) >= BUFFER_LENGTH) {
		dmsCPrintWarning("Buffer Length exceeded in dmsCGetMultiple views:%s\n",
			token);
		return(idmsError);
	    }	
	    strcat(globalMultipleViewList,token);
	    strcat(globalMultipleViewList,":");
	    /* get the viewNameList */
	    token = strtok(NULL," ");
	    if (token == NULL)
		return(idmsError);
	    if ((strlen(token)+1) >= BUFFER_LENGTH) {
		dmsCPrintWarning("Buffer Length exceeded in dmsCGetMultiple views:%s\n",
			token);
		return(idmsError);
	    }	
	    strcat(globalMultipleViewList,token);
	    strcat(globalMultipleViewList,";");
	}
	++rowNumber;
    }
    /* the table call returns idmsNoRow the case there are no more
    ** entries left in the table
    */
    if (i != idmsNoRow)
	return(idmsError);
    return(idmsSuccess);
}

/* procedure which will take user input and
** using that modify the in-memory tables 
*/
int dmsCSetMultipleViews(char *viewList)
{
    char *token1, *token2, *marker;
    char temp[1024], tempViewList[2048];
    char toolName[1024];
    int retVal;
    char viewNameList[1024];
    char viewType[1024];
    int i,length;
    
    /* get the tool name of the tool that is running */
    if (!dmsCGetToolName(toolName))
	return(idmsError);

    /* parse the string extract each sublist, take the
    ** viewtype and viewName list seperately and then
    ** call the lower level procedure
    */
    marker = viewList;
    length = strlen(viewList);

    i = 1;
    while ((token1 = strtok(marker,";")) != NULL) {
	/* move the marker ahead by 1 byte, but
	 ** do not go beyond end of string
	 */
	marker += strlen(token1) + 1;
	i += strlen(token1)+1;

	strcpy(temp,token1);
	token2 = strtok(temp,":");
	if (token2 != NULL) {
	    strcpy(viewType,token2);
	    token2 = strtok(NULL,";");
	    if (token2 != NULL)
		strcpy(viewNameList,token2);
	    else
		return(idmsError);
	}
	else
	    return(idmsError);
	if ((retVal = idmSetViewName(toolName,viewType,viewNameList)) 
		!= idmsSuccess)
	    return(retVal);
	if (i >= length)
            break;
    }
    return(idmsSuccess);
}

/*-----------------------------------------------------------------------
 * GET FUNCTIONS
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

int dmsCGetWorkingDir(char *vName, char **workingDir)
{
   if (!workingDir)
      return ERROR;
   *workingDir = dmsGetWorkingDir(vName, 0);
   dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, WORKINGDIR, toolName.name, vName, *workingDir); 
   return idmsSuccess;
}


static char _dmsWorkLibBuff[BUFSIZE];

int dmsCGetWorkingLib(char *vName, char **workingLib)
{
   int i;
   if (!workingLib)
      return ERROR;
   i = findPolicyEntry(worklib, worklibCount, toolName.name, vName);
   if (strlen(worklib[i].next->dirLib)) {
      strcpy(_dmsWorkLibBuff, worklib[i].next->dirLib);
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE, WORKINGLIB, toolName.name, vName, _dmsWorkLibBuff);
   }
   else
      strcpy(_dmsWorkLibBuff, "");
  *workingLib = _dmsWorkLibBuff;
  return idmsSuccess;
}


/*-----------------------------------------------------------------------
 * OBSOLETE FUNCTIONS
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
 * Description: These functions should no longer be used
 *-----------------------------------------------------------------------*/

int dmsCExitDMS()
{
   int noError,error;
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCExitDMS()\n");
   dmsExitDMS(&noError);

   dmsTExit();
   error=dmsGenErrorCode(noError);

   if (debugDmsOn)
      dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCExitDMS()\n",error);
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber,
        dmsCGetLastMessage());

   if (idmsLogFilePtr)
      fclose(idmsLogFilePtr);

   return error;
}


int dmsCDeleteByLib(int whichDelete, char *lName, char *cName, char *vName,
                              char *verName, char **FPN)
{
  int noError;
  void *plib_id = idb_get_empty_lib_id();
  *FPN = 0;

  if (idmsNotInitialized) {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
    return idmsNotInit;
  }
  strcpy(theLibName,lName);

  dmsDelete(whichDelete, cName, vName, verName, BIN, _cellviewbuff,
                   &plib_id, &noError, DONT_CHANGE_CODES);

  *FPN = _cellviewbuff;  
  return noError;
}


int dmsCDelete(int whichDelete, char *cName, char *vName, char *verName, char **FPN)
{
  
  return dmsCDeleteByLib(whichDelete, "*", cName, vName, verName, FPN);
}


int dmsCGetCdslibFileName(char **fileName)
{
  int error;
  *fileName = 0;
  strcpy(_cdslibbuff, "");

  error = dmsGetCdslibFileName(_cdslibbuff);
  *fileName = _cdslibbuff;
  return error;  
}

