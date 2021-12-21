#ifndef DMSC_H
#define DMSC_H

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
# $Id: dmsC.h,v 1.1.2.12 1997/11/13 17:28:05 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

/**********************************************************************
 *
 * Name:	dmsC.h
 * Written by:  M. Gatlin, mgatlin@ichips.intel.com
 * Date:        07/08/97
 * Description: dms API routines - external header file 
 *              >>> PROVIDED FOR BACKWARD COMPATIBILITY ONLY.
 *              All existing/new idms functionality is provided through 
 *              the Coral API.
 *
 *********************************************************************/

#if (defined(_PROTOTYPES) || defined(__STDC__) || defined(__cplusplus)) && !defined(_NO_PROTO)
#undef  PARAMS
#define PARAMS(args) args
#else
#undef  PARAMS
#define PARAMS(args) ()
#endif

#include <fwierr.h>

#ifdef __cplusplus
  extern "C" {
#endif

/******************************* INCLUDES ****************************/

/************************* FUNCTION PROTOTYPES ***********************/

/****
 ****
 ****    R E G I S T R Y   F U N C T I O N S 
 ****
 ****/

 int  dmsCPIInitDMS                   PARAMS((char *appName, char *bootName, int nullgtMode));
 int  dmsCPIExitDMS                   PARAMS((void));

/****
 ****
 ****    F I L E   &   C M   F U N C T I O N S 
 ****
 ****/


 int  dmsCCreateCellView              PARAMS((char *cName, char *vName, char **FPN));
 int  dmsCCreateUnixFile              PARAMS((char *cName, char *vName, char **FPN));

 int  dmsCReadonlyCellView            PARAMS((char *cName, char *vName, char **FPN));
 int  dmsCReadonlyCellViewType        PARAMS((char *cName, char *vtName, char **vName,char **FPN));

 int  dmsCCheckoutCellView            PARAMS((char *cName, char *vName, char **FPN));
 int  dmsCLoadCellView                PARAMS((char *cName, char *vName, char **FPN));
 int  dmsCEditCellView                PARAMS((char *cName, char *vName, char **FPN));
 int  dmsCReadOrEditByPolicy          PARAMS((char *cName, char *vName, char **FPN));

 int dmsCDelete                       PARAMS((int whichDelete, char *cName, char *vName, char *verName, char **FPM));

 
 int  dmsCSaveCellView                PARAMS((char *cName, char *vName, char **FPN));
 int  dmsCCheckinCellView             PARAMS((char *cName, char *vName, char *children, int advance, char *log, char **FPN));
 int  dmsCUpdateCellView              PARAMS((char *cName, char *vName, char *children, int advance, char *log, char **FPN));
 int  dmsCInstallCellView             PARAMS((char *cName, char *vName, char *children, char *log, char **FPN));
 int  dmsCPurgeCellView               PARAMS((char *cName, char *vName, char **FPN));

 int  dmsCCancelCheckoutCellView      PARAMS((char *cName, char *vName, char **FPN));
 int  dmsCInfoCellView                PARAMS((int searchLibOnly, char *cName, char *vName, DMSINFO **information));
 int  dmsCGetLastCellViewInfo         PARAMS((DMSINFO **information));
 int dmsCInfoPackage                  PARAMS((int searchLibOnly, char *cName, char *vName, char **package));
 int dmsCLastInfoPackage              PARAMS((char **package));

 char *dmsCReportMessage              PARAMS((void));
 int  dmsCParsePath                   PARAMS((void));
 int dmsCParseThePath                 PARAMS((char *pathFile));
 char *dmsCPrintPath                  PARAMS((char *vName));

 char *dmsCGetSearchPath              PARAMS((void));
 int  dmsCGetWorkingDir               PARAMS((char *vName, char **workingDir));
 int  dmsCGetWorkingLib               PARAMS((char *vName, char **workingDir));

 char *dmsCGetPolicyViewList          PARAMS((char *tool, char *policy));

 int  dmsCReadonlyCellViewByLib       PARAMS((char *lName, char *cName, char *vName, char *verName, char **FPN));
 int  dmsCReadonlyCellViewTypeByLib   PARAMS((char *lName, char *cName, char *vtName, char *verName, char **vName, char **FPN));
 int  dmsCCheckoutCellViewByLib       PARAMS((char *lName, char *cName, char *vName, char *verName, char **FPN));
 int  dmsCEditCellViewByLib           PARAMS((char *lName, char *cName, char *vName, char *verName, char **FPN));
 int  dmsCReadOrEditByPolicyByLib     PARAMS((char *lName, char *cName, char *vName, char *verName, char **FPN));
 int  dmsCInstallCellViewByLib        PARAMS((char *lName, char *cName, char *vName, char *children, char *log, char **FPN));
 int  dmsCCheckinCellViewByLib        PARAMS((char *lName, char *cName, char *vName, char *verName, char *children, int advance, char *log, char **FPN));
 int  dmsCUpdateCellViewByLib         PARAMS((char *lName, char *cName, char *vName, char *verName, char *children, int advance, char *log, char **FPN));
 int  dmsCCancelCheckoutCellViewByLib PARAMS((char *lName, char *cName, char *vName, char *verName, char **FPN));

 int dmsCDeleteByLib	      PARAMS((int whichDelete, char *lName, char *cName, char *vName, char *verName, char **FPN));
 int dmsCGetCdslibFileName    PARAMS((char **fileName));
 int dmsCGetLibPath           PARAMS((char *lName, char **libPath));
 int dmsCGetRealLibPath       PARAMS((char *lName, char **libPath));

/****
 ****
 ****    A T T R I B U T E   F U N C T I O N S 
 ****
 ****/
 
 int  dmsCSetAttribute                PARAMS((char *libName, char *cellName, char *viewName, char *versionName, char *attrName, char *attrType, char *attrValue));
 int  dmsCSetAttributeList            PARAMS((char *libNameList, char *cellNameList, char *viewNameList, char *versionNameList, char *attrNameList, char *attrTypeList, char *attrValueList));
 int  dmsCGetAttribute                PARAMS((char *libName, char *cellName, char *viewName, char *versionName, char *attrName, char **attrType, char **attrValue));
 int  dmsCGetAllAttributes            PARAMS((char *libName, char *cellName, char *viewName, char *versionName, char **attrNameList, char **attrTypeList, char **attrValueList));
 int  dmsCDeleteAttribute             PARAMS((char *libName, char *cellName, char *viewName, char *versionName, char *attrName));

/****
 ****    CONFIGURATION FUNCTIONS
 ****/
int dmsCCreateConfig                      PARAMS((char *libName, char *confName));
int dmsCRemoveConfig                      PARAMS((char *libName,char *confName));
char *dmsCGetConfigsOfCV                    PARAMS((char *libName,char *cName,char *vName,char *ver));
int dmsDiffConfig                         PARAMS((char *libName1,char *confName1, char *libName2, char *confName2,char *oFile));
int dmsCCreateConfigComment               PARAMS((char *libName,char *confName,char *comment,char *append));
int dmsCCreateConfigInstance              PARAMS((char *libName,char *confName,char *instName));
int dmsCExportConfig                      PARAMS((char *libName,char *confName,char *toWhere));
int dmsCInsertConfig                      PARAMS((char *libName,char *confName,char *toConfName));
char *dmsCListConfig                        PARAMS((char *libName));

/****
 ****
 ****    M I S C E L L A N E O U S   F U N C T I O N S 
 ****
 ****/

 char *dmsCVersionNumber              PARAMS((void));
 int  dmsCPrintIdmsLog                PARAMS((char *log));

 int dmsCGetMultipleViews             PARAMS((char **viewList));
 int dmsCSetMultipleViews             PARAMS((char *viewList));
 int dmsCGetMultipleExts	      PARAMS((char **extList));
 char *dmCGetVersionAndLocker	      PARAMS((char *lName, char *cName, char *vName, 
						 char *ver, char *locker, char *path,
						 int get_last_if_not_locked, char **pverstr));
 int dmsCGetAllVersions		      PARAMS((char *lName, char *cName, char *vName, 
                                              char *storeVers ));
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
 char *dmCGenObjRel                   PARAMS((char*library, char*name, int isCellName));
 char *dmCGetLibraryPath	      PARAMS((char*library));
#endif
    
/****
 ****
 ****    O B S O L E T E   F U N C T I O N S
 ****
 ****/

int  dmsCExitDMS                      PARAMS((void));
 
#ifdef __cplusplus
  }
#endif

#endif /* ifndef DMSC_H */
