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
# $Id: dmsC_status.h,v 1.1.2.10 1998/01/05 20:10:03 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef _DMS_STATUS_H
#define _DMS_STATUS_H

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
//        Date: 08/20/97
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

int init_dms_structs();
int deallocate_dms_structs();

int get_last_status_code();
int save_status(int code, char *lP, char *lN, char *cN, char *vN, char *ver,
                char *user, char *fpn, int change_codes);
 
DMSINFO *dmsGetStatus(int get_version_and_locker);
char *dmsGetLastLibName();
char *dmsSetLastLibName(char *s);
char *dmsGetLastLibPath();
char *dmsSetLastLibPath(char *s);
char *dmsGetLastVerName();
char *dmsSetLastVerName(char *s);
char *dmsGetLastFullPathName();
char *dmsSetLastFullPathName(char *s);

/* New status fields */

char *dmsGetLastSearchPath();
char *dmsSetLastSearchPath(char *s);
char *dmsAppendToLastSearchPath(char *s);

char *dmsGetLastPathConfName();
char *dmsSetLastPathConfName(char *s);

char *dmsGetLastToolConfName();
char *dmsSetLastToolConfName(char *s);

#ifdef __cplusplus
  }
#endif

#endif
