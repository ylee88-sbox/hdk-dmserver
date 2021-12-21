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
# $Id: dmsC_internal_functions.h,v 1.1.2.31 1997/12/17 00:00:06 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef _DMS_INTERNAL_FUNCTIONS_H
#define _DMS_INTERNAL_FUNCTIONS_H

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
//        File: dms_internal_functions.h -- prototypes for the functions used
//                                          internally by iDMS.
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>

#include <dmsC_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*----------------*
 * parser related *
 *----------------*/

void dmsParsePath(char *dmsPathFile, int *noError);

/* Defined in the yacc parser */

int dmsYaccParser(char *dmsName);

/*---------------------*
 * Global vars related *
 *---------------------*/

char * dmsAppGetEnv(const char *envVarName);
int deallocate_all_structs();

void dmsSetToolConfig();
void dmsSetSearchPath();

char *dms_get_workdir_nosimplify(char *tName, char *vName, char *wdir);

/*------------*
 * DM Related *
 *------------*/

void *dms_find_lib_id(const char *operation, const char *dir_lib,
                     const char *cName, const char *vName, int *noError,char *fp);

/**
void open_config_if_exist(void *plib_id, char *pathConfName, char *theConfName,
                          const char *toolConfName);
**/
int dmsFindOSFile(char *dir, char *cName, char *vName, int call_get_ext);

char *dmsGenLibNameList(char *pattern, char *vName, char *libNameList);
char *dmsGetWorkingDir(char *vName, char *dmsWorkingDir);

#ifdef __cplusplus
  }
#endif

#endif
