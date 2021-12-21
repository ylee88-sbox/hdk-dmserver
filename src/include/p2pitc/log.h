/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: log.h - this file contains the APIs to generate error and debug */
/*            log files. this would be used for NT-UNIX-ITC                   */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: log.h,v 1.1.1.1 1997/02/20 19:17:57 vipulX Exp $ */
/* $Log: log.h,v $
 * Revision 1.1.1.1  1997/02/20  19:17:57  vipulX
 * P2PITC
 * */

#ifndef _LOG_H
#define _LOG_H


#define EXIT 1
void SetDebugOn(void);
int ErrorLog(int , char *);
void DebugLog(char *fmt, ...);

#endif /* _LOG_H */
