/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: The interutils.h header file                                    */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: interutls.h,v 1.1.1.1 1997/02/20 19:17:22 vipulX Exp $ */
/* $Log: interutls.h,v $
 * Revision 1.1.1.1  1997/02/20  19:17:22  vipulX
 * DMServer
 * */

#ifndef  _H_INTERUTLS
#define  _H_INTERUTLS

#include "dmserver.h"

extern char theLibName[2048];		/* global variable in fwinters.c */
extern int interCheckArg(int input, int output);
extern void interPutInfoInMsg(DMSINFO *info, int startArg);
extern void iDMS_reset_theLibName();

#endif
