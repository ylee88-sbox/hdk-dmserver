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
# $Id: idmb_internal_functions.h,v 1.1 1997/07/22 23:10:13 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef _IDMB_INTERNAL_FUNCTIONS_H
#define _IDMB_INTERNAL_FUNCTIONS_H

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
//        File: idmb_internal_functions.h -- prototypes for the functions used
//                                          internally by idmb.
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>

/*-------------------------*
 * Variable # of arguments *
 *-------------------------*/

   void idmbPrintResult(FILE *port, char *format, ...);

void myReplyFuncCB(char *result, void *replyClientData);
void idmbReportMsgIfAny(const char *message, char *outString);

#endif
