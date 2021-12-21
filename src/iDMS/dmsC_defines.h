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
# $Id: dmsC_defines.h,v 1.1.2.16 1997/12/17 00:00:02 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef _IDMS__DEFINES_H
#define _IDMS__DEFINES_H

/*----------------------------------------------------------------------------*
 * Created by Sorin to store the defines 
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

/*-----------------------*
 * Defines from the past *
 *-----------------------*/

/* And this is probably the lucky # of the previous developer : */

/*#define MAXBUFSIZE  30720  max string in mainsail is 32766 */
/* #define MAXBUFSIZE  70720 */

/* this was way to small to debug NHM env - setting to 1M - Sorin */
#define MAXBUFSIZE 10000000

/* Another out of the blue value */

#define BUFFER_LENGTH 50000
#define BUFSIZE 50000 
#define SMALLBUF 256
#define SUPERBUFSIZE 65536
#define MAXINCLUDEDEPTH 256

/*--------------------------------------------------------------*
 * Boolean stuff -- always a favorite to define and define .... *
 *--------------------------------------------------------------*/

#ifndef Boolean
#ifndef itkDBvo_H 
typedef char Boolean;
#endif
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* From past */

#define NOBODY "NONE"
#define BIN    "binary"
#define TEXT   "text"
#define CREATE      1
#define DONT_CREATE 0
#define ALLPOLICYUSEDEFAULT "allPolicyUseDefault"

/*-------------*
 * NEW DEFINES *
 *-------------*/

#define CALL_GET_EXT 1
#define DONT_CALL_GET_EXT 0

/* for the dmsReadCellView function */

#define READ_NON_CDBA 0
#define READ_CDBA 1
#define CELLVIEW_INFO 2

#define coREAD_ONLY 0
#define coLOCK 1
#define coRO_LOCK_POLICY 2
#define coCANCEL 3


/* for the save_status functions */

#define CHANGE_CODES 1
#define DONT_CHANGE_CODES 0

#define GET_LAST_IF_NOT_LOCKED 1
#define DONT_GET_LAST_IF_NOT_LOCKED 0

#define NULLGT_MODE 1 

#define DUMMY "DUMMY"

/*------*
 * MISC *
 *------*/

#ifndef MIN
#define MIN(a, b) (a<=b)?a:b
#endif

#ifndef MAX
#define MAX(a, b) (a>=b)?a:b
#endif

#ifndef FREE_NZ
#define FREE_NZ(p) if (p) {free(p); p = 0;}
#endif

#endif
