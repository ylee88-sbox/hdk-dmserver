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
# $Id: fwconfig.c,v 1.1.6.1.4.2.6.14 1997/11/14 23:18:11 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

#include <stdio.h>
#include <string.h>

#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
#include <itkDB.h>
#else
#define dbPropType char** 
#define dbPropValue char** 
#endif

#include <idb_functions.h>
#include <dmsC_new.h>
#include <dmsC_dm.h>
#include <dmsC_defines.h>
#include <dmsC_reports.h>

/* idms messages */
#include "fwimsg.h" /* include fwierr.h internally */

#define idb_capture_dm_vendor_warning() idb_capture_dm_vendor_warning()
#define ABORTCMD \
        {\
	   dmsFormMessage(WARNING,BREAKBYSEMICOL);\
	   return(FALSE);\
        }

extern int debugDmsOn;
extern theLibName[BUFSIZE],theConfName[BUFSIZE];
extern void * dmsLibID;
extern char *userName;

#define STDPORT stdout

/** originally was dmsMyHomeConfig */
char *dmsCGetConfigsOfCV(char *libName,char *cName,char *vName,char *ver)
{
  static char home[BUFSIZE];
  void *libID;

  libID=idb_open_library(libName, NULL, "r");
  if (!libID) {
    idb_capture_dm_vendor_warning();
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTWRITELIB,libName,"LIBPATH");
    return(FALSE);
  }

  strcpy(home, "");

  idb_get_cellView_tags(libID, cName, vName, ver, home);

  return(home);
} 


int dmsCAddSCToConfig(char *libName,char *confName,char *subLibName, char *subcfName)
{
  void *libID;

  libID=idb_open_library(libName, NULL, "r");
  if (!libID) {
    idb_capture_dm_vendor_warning();
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTWRITELIB,libName,"LIBPATH");
    return(FALSE);
  }

  if(confName && subcfName) {
    if(!strcmp(confName, "") || !strcmp(subcfName, "")) {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,ADDSUBCONFIGERROR,subcfName,confName,libName);
      return(FALSE);
    }
  }
  else {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,ADDSUBCONFIGERROR,"NULL","NULL",libName);
    return(FALSE);
  }


  if (!idb_add_subconfig(libID,confName,subcfName)) {
    idb_capture_dm_vendor_warning();
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,ADDSUBCONFIGERROR,subcfName,libName, confName,libName);
    return(FALSE);
  }
  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,ADDSUBCONFIGOK,subcfName,libName, confName,libName);
  return(idmsCvInRwlib);
} /* dmsAddSCToConfig */


/* REM: usage of the subConfigTag is temporary, Will get a better soln. later */
int dmsCRemoveSCFromConfig(char *libName,char *confName,char *lName,char *subcfName)
{
  char subConfigTag[BUFSIZE];
  void *libID;

  libID=idb_open_library(libName, NULL, "w");
  if (!libID) {
    idb_capture_dm_vendor_warning();
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTWRITELIB,libName,"LIBPATH");
    return(FALSE);
  }

  sprintf(subConfigTag, "%s_SC_%s", confName, subcfName);

  if (idb_delete_config(libID,subConfigTag)) {
    idb_capture_dm_vendor_warning();
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,RMSUBCONFIGERROR,subcfName,lName,confName,libName);
    return(FALSE);
  }
  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,RMSUBCONFIGOK,subcfName,lName,confName,libName);
  return(TRUE);
} /* dmsRemoveSCFromConfig */


int dmsCCreateConfigInstance(char *libName,char *confName,char *instName)
{
  void *libID;

  libID=idb_open_library(libName, NULL, "r");
  if (!libID) {
    
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTREADLIB,libName,"LIBPATH");
    return(FALSE);
  }

  if (!dmsCFindConfig(libName, confName)) {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,NOSUCHCONF,confName,libName);
    return(FALSE);
  }

  if (!idb_create_config_instance(libID,confName,instName)) {
    
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CRTCONFINSTERROR,instName,libName,confName);
    return(FALSE);
  }
  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,CRTCONFINSTOK,instName,libName,confName);
  return(idmsCvInRwlib);
} /* dmsCreateConfigInstance */


/* Create a library DM property of name "Comment:ConfName" with the specified
 * comment as property value.
 */
int dmsCCreateConfigComment(char *libName,char *confName,char *comment,char *append)
{
  void *libID;
  dbPropType propType;
  dbPropValue propValue;
  char propName[BUFSIZE];
  char tmpString[BUFSIZE], *newPropValue;

  libID=idb_open_library(libName, NULL, "r");
  if (!libID) {
    idb_capture_dm_vendor_warning();
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTWRITELIB,libName,"LIBPATH");
    return(FALSE);
  }

  if (!dmsCFindConfig(libName, confName)) {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,NOSUCHCONF,confName,libName);
    return(FALSE);
  }

  if (!comment || comment[0]=='\0') {
    return(FALSE);
  }
 
  sprintf(propName, "Comment:%s", confName);
  if (append) {
     /* Get the existing comment. */
     if (dmsCGetAttribute(libName, NULL, NULL, NULL, propName, &propType, &propValue)){
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
         sprintf(tmpString, "%s %s", propValue.aString, comment);
#else
         sprintf(tmpString, "%s %s", "", comment);
#endif
         newPropValue = tmpString;
     } else
         newPropValue = comment;
  }

  if (dmsCSetAttributeList(libName, NULL, NULL, NULL, propName, "String",
                           newPropValue))
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,CRTCONFIGCMTOK,newPropValue,libName,confName);

  return(TRUE);

} /* dmsCreateConfigComment */


/* List all the tags of every version of cellview in a library. */
char* dmsCListConfig(char *libName)
{
  void *libID;
  char *ddObjPath;
  int  numConfig;
  char *libConfigs, *idx, *tmp;

  libID=idb_open_library(libName, NULL, "r");
  if (!libID) {
    idb_capture_dm_vendor_warning();
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTREADLIB,libName);
    return(NULL);
  }

  idb_get_lib_tags(libID, &libConfigs);
  if (libConfigs) {
      tmp = libConfigs;
      while (idx = (char *)strchr(tmp, ' ')) {
         idx[0] = '\0';
         dmsPrintResult(STDPORT,"Configuration: %s\n", tmp);
         if (idx[1] == '\0')
             break;
         if (idx[0] == '\0')
             idx[0] = ' ';
         tmp = idx+1;
      }
      dmsPrintResult(STDPORT,"Configuration: %s\n", tmp);
      /* caller needs to free the mmy.
       * free(libConfigs);
       */
  }
 
  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,LISTCONFIGOK,libName);

  return(libConfigs);
} /* dmsListConfig */


/* Dump and load configuration is not supported in Synchronicity DM. */
int dmsCDumpConfig(char *libName,char *confName,char *fileName,int expand)
{
#ifdef OBSOLETE
  FILE *fo;
  char oFileName[BUFSIZE];
  void *libID;

  libID=idb_open_library(libName, NULL, "r");
  if (!libID) {
    idb_capture_dm_vendor_warning();
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTREADLIB,libName);
    return(FALSE);
  }

  getTimeIn();

  if (!strcmp(fileName,""))
    sprintf(oFileName,"/tmp/%s.ascii",confName);
  else
    strcpy(oFileName,fileName);

  printf("WILL BE IMPLEMENTED LATER\n");

  if (!(fo = fopen(oFileName, "r")))
     printf("<CANNOT OPEN FILE %s>\n", oFileName);

  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,DUMPCONFIGOK,confName,libName,oFileName);
  return(TRUE);
  /****** 
  idb_capture_dm_vendor_warning();
  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR,DUMPCONFIGERROR,confName,libName,oFileName);
  return(FALSE);
  *****/
#endif
  printf("Function dmsCDumpConfig is not supported anymore.\n");
  return(FALSE);
} /* dmsDumpConfig */


/* Dump and load configuration is not supported in Sync's DM system. */
int dmsCLoadConfig(char *libName,char *iFileName)
{
#ifdef OBSOLETE
  char tmpString[BUFSIZE],configName[BUFSIZE];
  list aList,tList;
  void *libID;

  libID=idb_open_library(libName, NULL, "r");
  if (!libID) {
    
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTREADLIB,libName,"LIBPATH");
    return(FALSE);
  }

  getTimeIn();

  printf("WILL BE IMPLEMENTED LATER\n");
#endif

  printf("Function dmsCDumpConfig is not supported anymore.\n");
  return(FALSE);

} /* dmsLoadConfig */


typedef struct configRec {
  char cellName[BUFSIZE];
  char viewName[BUFSIZE];
  char verName1[BUFSIZE];
  char verName2[BUFSIZE];
} CONFIGREC;

/* Comparing differences between two configuration.
 * With Sync's report utility, comparing two configuration can
 * only has one workspace path. Without given configuration,
 * two workspace path may be given.
 * I am assuming we are comparing two configuration of the same library.
 * Any other comparison can be future enhencement.
 * whagi
 */
int dmsDiffConfig(char *libName1,char *confName1,
			char *libName2,char *confName2,char *oFile)
{

  void *libID1;

  libID1=idb_find_lib_id(libName1, (int)NULL, "r");

  if (!libID1) {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTREADLIB,libName1);
     return(FALSE);
  }

  if (!dmsCFindConfig(libName1, confName1)) {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,NOSUCHCONF,confName1,libName1);
     return(FALSE);
  }
  if (!dmsCFindConfig(libName1, confName2)) {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,NOSUCHCONF,confName2,libName1);
     return(FALSE);
  }

  if (libName2) {
      printf("WARNING: Ignoring library %s.\n", libName2);
  }

  if (!idb_diff_config(libID1, NULL, confName1, confName2, oFile)) {
      return(FALSE);
  }

  return(TRUE);
} /* dmsDiffConfig */


int dmsCExportConfig(char *libName,char *confName,char *toWhere)
{
  void *libID;

  libID=idb_open_library(libName, NULL, "r");
  if (!libID) {
    
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTREADLIB,libName);
    return(FALSE);
  }

  if (!dmsCFindConfig(libName, confName)) {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,NOSUCHCONF,confName,libName);
    return(FALSE);
  }

  if(idb_export_config(libID, confName, toWhere)) {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,EXPORTCOUNT,1,confName,libName);
    return(TRUE);
  }

  return(FALSE);
} /* dmsExportConfig */


int dmsCInsertConfig(char *libName,char *confName,char *toConfName)
{
  void *libID;

  libID=idb_open_library(libName, NULL, "r");
  if (!libID) {
    
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTWRITELIB,libName,"LIBPATH");
    return(FALSE);
  }

  if (!dmsCFindConfig(libName, confName)) {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,NOSUCHCONF,confName,libName);
    return(FALSE);
  }

  if(idb_insert_config(libID, confName, toConfName)) {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,INSERTCFOK,libName,confName,toConfName);
    return(TRUE);
  }

  return(FALSE);
} /* dmsInsertConfig */


/* REM: since it will be very expensive to find out if a tag
** is being used in any of the cellViews of a particular lib 
** (so it should not be created again), at this time this 
** function checks the existance of the lib only. -sh 6/7/00
**
** whagi- with the report utility performance improvement,
** getting all tags of a library is not expensive anymore.
** Routine also checks the existing of the confName in the library.
*/
int dmsCCreateConfig(char *libName,char *confName)
{
  void *libID;
  char *libConfigs, *token;

  libID=idb_find_lib_id(libName, (int)NULL, "r");
  if (!libID) {
    idb_capture_dm_vendor_warning();
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTWRITELIB,libName,"LIBPATH");
    return(FALSE);
  }

  /* Check to see if the confName is already existed in the library.
   */
  if (dmsCFindConfig(libName, confName)) {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR,CONFEXIST,confName,libName);
     return(FALSE);
  } else {
     return(TRUE);
  }
} /* dmsCreateConfig */


int dmsCRemoveConfig(char *libName,char *confName)
{
  void *libID;

  libID=idb_open_library(libName, NULL, "r");
  if (!libID) {
    
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTREADLIB,libName,"LIBPATH");
    return(FALSE);
  }

  if (!dmsCFindConfig(libName, confName)) {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,NOSUCHCONF,confName,libName);
    return(FALSE);
  }

  if (idb_delete_config(libID,confName)) {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,REMOVECONFIGOK,confName,libName);
    return(TRUE);
  } else {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR,REMOVECONFIGERROR,confName,libName);
    return(FALSE);
  }
} /* dmsRemoveConfig */


int dmsCFindConfig(char *libName,char *confName)
{
  void *libID;
  char *libConfigs, *token;

  libID=idb_find_lib_id(libName, (int)NULL, "r");
  if (!libID) {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTREADLIB,libName,"LIBPATH");
    return(FALSE);
  }

  if (!strcmp(libName, "Latest")) {
      return(TRUE);
  }

  /* Check to see if the confName is already existed in the library.
   */
  idb_get_lib_tags(libID, &libConfigs);
  if (libConfigs) {
      token = (char *)strtok(libConfigs, " ");
      while(token != NULL) {
         if(!strcmp(confName, token)) {
             free(libConfigs);
             return(TRUE);
         }
         token = (char *)strtok(NULL, " ");
      }
      free(libConfigs);
  }
  return(FALSE);
}


/************************************************************************
REM: Not used in fwidmb.c, so not converted. 
Will be looked at a lter time :)	-sh 6/7/00

int
dmsAddHierCVToConfig(libName,confName,cName,vName,verName,bind)
char *libName,*confName,*cName,*vName,*verName;
int bind;
{
  char tmpString[BUFSIZE],clName[BUFSIZE],ccName[BUFSIZE],cvName[BUFSIZE],cverName[BUFSIZE];
  list aList,tList;
  dmLibId libID;
  int index;

  libID=dmOpenLib(libName, NULL, NULL, "w");
  if (!libID) {
    idb_capture_dm_vendor_warning();
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTWRITELIB,libName,"LIBPATH");
    return(FALSE);
  }

  getTimeIn();

  if (!dmsAddCVToConfig(&libID,confName,cName,vName,verName,bind))
    dmsReportMsgIfAny(dmsCGetLastMessage(), "");

  sprintf(tmpString,"dmsSkillGetChildren(\"%s\" \"%s\" \"%s\" \"%s\" )",libName,cName,vName,verName);
  aList=ilCar((list) idb_evaluate_string(tmpString,1));

  if (ilIsNotNull(aList)) {
    index=0;
    tList=ilNth(index,aList);
    while (ilIsNotNull(tList)) {
      if (ilIsNotNull(ilNth(0,tList)))
	strcpy(clName,ilGetString(ilNth(0,tList)));
      else
	strcpy(clName,"");
      if (ilIsNotNull(ilNth(1,tList)))
	strcpy(ccName,ilGetString(ilNth(1,tList)));
      else
	strcpy(ccName,"");
      if (ilIsNotNull(ilNth(2,tList)))
	strcpy(cvName,ilGetString(ilNth(2,tList)));
      else
	strcpy(cvName,"");
      if (ilIsNotNull(ilNth(3,tList)))
	strcpy(cverName,ilGetString(ilNth(3,tList)));
      else
	strcpy(cverName,"");
      dmsAddHierCVToConfig(clName,confName,ccName,cvName,cverName,bind);
      index++;
      tList=ilNth(index,aList);
    }
  }
  return(TRUE);
} 
************************************************************************/
/* dmsAddHierCVToConfig */


