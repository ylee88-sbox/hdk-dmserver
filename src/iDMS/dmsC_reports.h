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
# $Id: dmsC_reports.h,v 1.1 1997/08/07 18:35:04 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef _DMS_REPORTS_H
#define _DMS_REPORTS_H

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
//        Date: 8/7/97
//
//        Authors: Sorin Blebea
//
//        File: dmsC_reports.h 
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>

#include <dmsC_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Error reporting, etc... */

void dmsFormPrintPath(char *tName, char *vName);
void dmsReportMsgIfAny(char *message, char *outString);
int dmsGenErrorCode(int noError);


/*-------------------------*
 * Variable # of arguments *
 *-------------------------*/

  void dmsPrintIdmsLog(char *format, ...);
  void dmsPrintResult(FILE *port, char *format, ...);
  int dmsFormMessage(const char * cfunction, const char * cfile, const int cline, int type, int number, ...);

#ifdef __cplusplus
  }
#endif

#endif
