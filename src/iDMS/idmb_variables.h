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
# $Id: idmb_variables.h,v 1.1 1997/11/14 23:04:12 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef _IDMS_PARSER_VARS_H
#define _IDMS_PARSER_VARS_H

/*----------------------------------------------------------------------------*
 * Created by Sorin to store the global variables used by the idms parser     *
 *----------------------------------------------------------------------------*/

#define STDPORT stdout
#define RPORT stderr

#include <stdio.h>
#include <stdlib.h>

#include <fwierr.h>

/* The idea is to #define IDMS_MAIN in fwidms.yacc for now */ 

#ifdef IDMB_MAIN
#   define EXTERNAL
#   define INITIAL_VALUE(x) x
#else
#   define EXTERNAL extern
#   define INITIAL_VALUE(x)
#endif

#include <dmsC_types.h>
#include <dmsC_defines.h>

/* found when cleaning fwircs.c */

EXTERNAL char dmsWorkingDir[BUFSIZE];
EXTERNAL void *dmsLibID;
EXTERNAL  void *dmsCellViewID;

EXTERNAL int promptFlag INITIAL_VALUE(=1);

EXTERNAL char dmsBuf[BUFSIZE];
EXTERNAL  void *versionID;

EXTERNAL int ircsIsOn INITIAL_VALUE(=FALSE);

EXTERNAL  void *cellID;
EXTERNAL  void *viewID;
EXTERNAL  void *cellViewID;
EXTERNAL char checkinPath[BUFSIZE];
EXTERNAL char rcsHeader[BUFSIZE],rcsLog[BUFSIZE],rcsRev[BUFSIZE],rcsComment[BUFSIZE];
EXTERNAL char rcsSource[BUFSIZE],rcsAuthor[BUFSIZE],rcsVersion[BUFSIZE],rcsLocker[BUFSIZE];
EXTERNAL char rcsDate[BUFSIZE];

EXTERNAL char idmsDirectory[BUFSIZE] INITIAL_VALUE(="/usr/cad/alpha/idms");
EXTERNAL char ircsDirectory[BUFSIZE] INITIAL_VALUE(="/usr/cad/alpha/ircs");
EXTERNAL char idmbDirectory[BUFSIZE] INITIAL_VALUE(="/usr/cad/alpha/idmb");

/* Found while cleaning fwidmb.c */

EXTERNAL int idmbGraphicMode INITIAL_VALUE(=FALSE);

#endif
