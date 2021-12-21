/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: dmserver.h the dmserver header file & internal procs.           */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: dmserver.h,v 1.2 1997/04/08 21:23:28 vipulX Exp $ */
/* $Log: dmserver.h,v $
 * Revision 1.2  1997/04/08  21:23:28  vipulX
 * Changes for removing dmserver.h dependency from p2p
 *
 * Revision 1.1.1.1  1997/02/20  19:17:22  vipulX
 * DMServer
 * */

#ifndef  _H_DMSERVER
#define  _H_DMSERVER

#include <p2pitc/interp2p.h>

/* macros  */

#define ITC_CHECKOUT           0
#define ITC_CHECKIN	       1
#define SET_ATTRIBUTE          2
#define GET_ATTRIBUTE          3
#define DELETE_ATTRIBUTE       4
#define SET_SEARCH_PATH	       5
#define SET_TOOL_NAME          6
#define REPARSE                7
#define GET_VERSION            8
#define CANCEL_CHECKOUT        9
#define PING                   10
#define GET_WORKDIR            11
#define CREATE_CELLVIEW        12
#define CREATE_FILE            13 
#define READONLY_CV_TYPE       14
#define LOAD                   15
#define EDIT                   16
#define READorEDIT             17
#define SAVE                   18
#define INFO_CELLVIEW          19
#define GET_WORKLIB            20
#define PURGE                  21
#define REPORT                 22
#define PRINTPATH              23
#define READONLY               24
#define LASTINFO               25
#define ITC_INSTALL            26
#define UPDATE                 27
#define GET_SEARCH_PATH        28
#define GET_POLICY_VWLIST      29
#define QUIT                   30
#define SET_ATTRIBUTE_LIST     31
#define GET_ALL_ATTRIBUTES     32
#define SET_MULTIPLE_VIEWS     33
#define GET_MULTIPLE_VIEWS     34
#define GET_TOOL_NAME          35
#define PRINT_INTERS_LOG       36
#define GET_LIBRARY_LIST       37
#define GET_CHECKOUT_VERSION   38
#define GET_CELL_LIST          39
#define GET_VIEW_LIST          40
#define GET_VIEWCELLVIEW_LIST      41
#define GET_CELLCELLVIEW_LIST      42
#define GET_VERSION_LIST      43
#define GET_EXTENSIONTABLE_LIST  44
#define DELETE			45
#define GETCDSLIBNAME		46
#define GET_LIB_PATH	        47
#define GET_CELLVIEWS_BYLIB     48
#define GET_CELLVIEWS_FORCACHE  49
#define GET_ALL_CELLVIEWS       50

/* Total number of commands - add 1 to the last */

#define INTERS_NUM_COMMANDS    51



/* attribute pi error codes */
# define intersFailure                         105
# define intersBadparameter                    106
/* no error */
#define intersSuccess                          0

/* DMS Error */
#define intersNotInit                          109


typedef void (*dm_CALLBACK)();
	
#define INTERS_TOOL_LENGTH 1000
#define INTERS_CHECKTIMEOUT 10

/* AFS related definations etc 07/13/98 */
/* Fixed variables definitions */
#define  MAXCELL 5
#define  TKTLEN (4 * sizeof(struct ktc_principal) + 4 * 20 + 2 * \
                 sizeof(struct ktc_encryptionKey) + 2 * MAXKTCTICKETLEN)

#endif





