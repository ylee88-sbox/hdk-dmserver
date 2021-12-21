/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: timer.h APIs for multimedia timers under NT for UNIX-NT-ITC     */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: timer.h,v 1.1.1.1 1997/02/20 19:17:57 vipulX Exp $ */
/* $Log: timer.h,v $
 * Revision 1.1.1.1  1997/02/20  19:17:57  vipulX
 * P2PITC
 * */

#ifndef _TIMER_H
#define _TIMER_H

#define TARGET_RESOLUTION 1000  /* 1 second accuracy. */ 
#define TIME_OUT 5000			/* time out value in ms */
#define RETRY_COUNT 3			/* Max # of retries to be done */

typedef struct {
	unsigned int wTimerID;		/* timer id filled in by system */
	short nAckPending;			/* 0 = not 1 = pending ack */
	short nRetryCount;			/* # of retries dones */ 
}NSEQ, *NPSEQ;

void InitTimer(void);
void EndTimer(void);
#ifdef WIN32
void DestroyTimer(NPSEQ );
UINT SetTimerCallback(NPSEQ , UINT ); 
void PASCAL retryFunction(UINT , UINT ,DWORD , DWORD , DWORD ); 
#else /* not win32 - UNIX */
void DestroyTimer(NPSEQ );
void SetTimerCallback(NPSEQ , unsigned int ); 
void retryFunction(int); 
#endif /* WIN32 */

#ifndef TIMER_PRIVATE
extern
#endif 
NSEQ nSeq;						/* Sequence id */

#endif /* _TIMER_H */
