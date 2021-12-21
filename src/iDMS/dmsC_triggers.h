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
# $Id: dmsC_triggers.h,v 1.1 1997/08/01 21:11:41 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef _DMS_TRIGGERS_H
#define _DMS_TRIGGERS_H

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
#include <dmsC_defines.h>

#ifdef __cplusplus
extern "C" {
#endif

Boolean idmRegisterTriggers(int nullgt);

#ifdef __cplusplus
  }
#endif

#endif
