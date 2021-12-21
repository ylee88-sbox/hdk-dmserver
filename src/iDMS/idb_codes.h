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
# $Id: idb_codes.h,v 1.1.2.10 1997/11/04 00:03:36 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef __IDB_CODES_H
#define __IDB_CODES_H

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
//        File: idb_codes.h  -- error codes for idb_functions
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>

/*------------------*
 * idb status codes *
 *------------------*/

#define idb_OK            0

#define idb_ERROR             1 << 0
#define idb_SYNC_ERROR        1 << 1 
#define idb_RES2              1 << 2
#define idb_RES3              1 << 3
#define idb_RES4              1 << 4
#define idb_CANT_OPEN_LIB     1 << 5
#define idb_CANT_FIND_LIB     1 << 6
#define idb_INVALID_LIB       1 << 7
#define idb_CANT_CRE_CNF      1 << 8
#define idb_CANT_ADD_CVTOCNF  1 << 9
#define idb_CANT_REMOVE_CVCNF 1 << 10
#define idb_CANT_CRE_CV       1 << 11
#define idb_CV_EXISTS         1 << 12
#define idb_CANT_FIND_CV      1 << 13
#define idb_CANT_OPEN_CV      1 << 14
#define idb_CANT_DEL_CNF      1 << 15
#define idb_RES16             1 << 16
#define idb_RES17             1 << 17
#define idb_CANT_GET_TIME     1 << 18
#define idb_INVALID_VERID     1 << 19
#define idb_CANT_FIND_VER     1 << 20
#define idb_CANT_FIND_VERPATH 1 << 21
#define idb_RES22             1 << 22
#define idb_CANT_CO           1 << 23
#define idb_CANT_CHECKIN      1 << 24
#define idb_CANT_INSTALL_CV   1 << 25
#define idb_CANT_CANCEL_CO    1 << 26
#define idb_RES27             1 << 27
#define idb_RES28             1 << 28
#define idb_RES29             1 << 29
#define idb_RES30             1 << 30 
#define idb_RES31             1 << 31

/*------------------*
 * idb config types *
 *------------------*/

#define idb_CONFIG_STATIC     1
#define idb_CONFIG_BY_TIME    2
#define idb_CONFIG_BY_PRIMARY 3
#define idb_CONFIG_DYNAMIC    4 

#endif
