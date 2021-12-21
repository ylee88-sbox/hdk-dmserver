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
# $Id: dmsC_dm.h,v 1.1.2.16 1997/11/14 21:26:29 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef _DMS_DM_H
#define _DMS_DM_H

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
//        File: dmsC_dm.h -- prototypes for the dm functions used
//                                          internally by iDMS.
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>

#include <dmsC_types.h>
#include <dmsC_defines.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Still in idms source files */

/*
void idmSetCheckinGlobalVar(char *cellName, char *viewName);
Boolean idmExecuteUserTrigger(char *event, Boolean preTrigger, char *libName,
                             char *cellName, char *viewName, char *versionName,
                             char *path);
Boolean idmSetGeneratedAttribute(char *event, char *libName, char *cellName,
                                 char *viewName, char *versionName);
*/


/* Implemented in Coral */

void dmsPIInitDMS(char *dmsBootName, char *dmsPath, int nullgtMode,
                  int *noError);

int dmsSearchCellView(int open_first, int cdba, char *cName, char *vName,
                      char *verName, char* FPN, char *locker, char **tVerName,
			int *noError, int copy_file_first);

int dmsReadCellView(int what, int checkout, int create, const char *cName,
                     const char *vName, const char *verName, 
                     const char *viewType, char *FPN, 
		     void **lib_id, int *noError, int change_codes);
int dmsWriteCellView(int cdba, int writeType, char *cName, char *vName,
                      char *verName, char *viewType, char *log, 
		      char *config, char *FPN, void **pplib_id,
                      int *noError);
/***
int dmsAddCVToConfig(void *plib_id, char *confName, char *cName,
                     char *vName, char *verName, int bind);
***/

int dmsCreateCellView(int cdba, char *cName, char *vName, char *viewType,
                     char *FPN, int*noError);

void dmsCheckOutCellView(int cdba, int dummy, char *cName, char *vName,
                         char *verName, char *FPN, char *locker, char **new_ver,
			 int *noError);
void dmsCheckInCellView(int cdba, char *cName, char *vName, char *verName,
                        char *log, char *config, char *FPN, char *locker,
			char **new_ver, int *noError);
int dmsInstallCellView(int cdba, char *cName, char *vName, char *viewType, 
                       char *log, char *config, char *FPN, int *noError, int create);  
int dmsCancelCheckOutCellView(char *cName, char *vName, char *verName,
                               char *FPN, int *noError);

int dmsDelete(int delete, const char *cName,
                     const char *vName, const char *verName, 
                     const char *viewType, char *FPN,
                     void **pplib_id, int *noError, int change_codes);

void dmsRmCellView(int delete, char *libName, char *cName, char *vName,
		   char *verName, char *FPN, char *locker, 
		   char **new_ver, int *noError);
void dmsRmCell(const char *libName, char *cName, char *vName,
		   char *verName, char *FPN, char *locker, 
		   char **new_ver, int *noError);
int dmsGetCdslibFileName(char *fileName);

#ifdef __cplusplus
  }
#endif

#endif
