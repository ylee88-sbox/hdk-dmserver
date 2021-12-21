/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: For hash table implementation to take care of proper response   */
/*            from DMserver for multiple clients for dmsCGetLastCellViewInfo()*/
/*            and dmsCReportMessage() INTERS PIs.                             */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: hashUtils.h,v 1.1.1.1 1997/02/20 19:17:21 vipulX Exp $ */
/* $Log: hashUtils.h,v $
 * Revision 1.1.1.1  1997/02/20  19:17:21  vipulX
 * DMServer
 * */

#ifndef  _H_HASHUTILS
#define  _H_HASHUTILS

#include "dmserver.h"
#include <stdio.h>
#include <stdlib.h>

#define TRUE	1
#define FALSE	0
#define HASH_TABLE_SIZE 	13
#define CLIENT_ID_SIZE  	64
#define REP_MESSAGE_SIZE	30720

typedef struct _INFO_NODE *INFO_NODE_PTR;
typedef struct _INFO_NODE{
	char 	       client_id[CLIENT_ID_SIZE];
	char 	       rep_message[REP_MESSAGE_SIZE];
	DMSINFO        dm_info_struct;
	INFO_NODE_PTR  next_node;
	INFO_NODE_PTR  prev_node;
} INFO_NODE;

extern int hashCreate(char *);
extern int hashDelete(char *);
extern int hashSetReportMsg(char *);
extern int hashSetLastCellViewInfo(char *);
extern char *hashGetReportMsg(char *);
extern int hashGetLastCellViewInfo(char *, DMSINFO **);
extern char* dmsCReportMessage();	
#endif
