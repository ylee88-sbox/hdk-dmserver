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
# $Id: ifw_functions.h,v 1.1 1997/07/16 00:22:50 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef __IFW_FUNCTIONS_H
#define __IFW_FUNCTIONS_H

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
//        Date: 04/23/97
//
//        Authors: Sorin Blebea
//
//        File: ifw_functions.h  -- prototypes for the "ifw_" functions
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef void (*Reply_Fun_Type)(char *, void *);
void *ifw_idmb_send_msg_and_print(const char *message, Reply_Fun_Type fun);

#ifdef __cplusplus
}
#endif

#endif
