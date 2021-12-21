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
# $Id: idmb_internal_functions.c,v 1.1.2.1 1997/11/14 23:17:41 sorin Exp $
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
//        File: idmb_internal_functions.h -- prototypes for the functions used
//                                          internally by idmb.
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Variable # of params -- different for C/C++ */

#include <stdarg.h>

#include <dmsC_defines.h>
#include <dmsC_internal_functions.h>
#include <idmb_internal_functions.h>

#define STDPORT stdout

/*--------------------*
 * External variables *
 *--------------------*/

extern int debugDmsOn;
extern int idmbGraphicMode;
extern FILE* fptr;
/*------------------*
 * Static variables *
 *------------------*/

static char *_tmpBuff1 = 0;
static int  _LtmpBuff1 = 0;


/*-----------------------------------------------------------------------
 * idmbPrintResult
 *
 * VARIABLE # OF ARGUMENTS
 *
 * Modifies:
 *    Static vars:  _tmpBuff1, _LtmpBuff1
 *
 * Uses:
 *    Global vars: idmbGraphicMode
 *    define MAXBUFSIZE.
 *
 * Description: prints the message into the FW window if one opened
 *-----------------------------------------------------------------------*/

void idmbPrintResult(FILE *port, char *format, ...)
{
  int rc = check_for_buff_space(&_tmpBuff1, &_LtmpBuff1, MAXBUFSIZE);
  va_list         ptr;

  va_start(ptr, format);

  vsprintf(_tmpBuff1, format, ptr);
  va_end(ptr);
  fprintf(port, _tmpBuff1);
  fflush(port);
}


void idmbReportMsgIfAny(const char *message, char *outString)
{
   if (message){
      if (strlen(message) > 0){
          idmbPrintResult(fptr,"%s",message);
          strcpy((char *)message,"");
          if (outString){
             if (strlen(outString) > 0){
                idmbPrintResult(fptr,"%s\n",outString);
                strcpy(outString,"");
             }
          }
          idmbPrintResult(fptr," \n");
      }
   }
}

