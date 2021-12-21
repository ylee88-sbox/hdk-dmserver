/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: timer.c a set of funcs. for multimedia timers under NT under    */
/*            UNIX it uses the standard alarm() call and installs a signal    */
/*            handler for SIGALRM.                                            */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: timer.c,v 1.1.1.1 1997/02/20 19:17:58 vipulX Exp $ */
/* $Log: timer.c,v $
 * Revision 1.1.1.1  1997/02/20  19:17:58  vipulX
 * P2PITC
 * */
  
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#else
#include <signal.h>
#endif /* WIN32 */

#define TIMER_PRIVATE
#include <p2pitc/timer.h>
#include <p2pitc/log.h>

#ifdef WIN32
static TIMECAPS tc; 
static UINT     wTimerRes; 

/* call this one befor using any timer services */
void InitTimer(void)
{
	/* determine min and max resolution supported by timer services */
	if(timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) { 
		/* Error; can't continue. */
		ErrorLog(EXIT,"Error in timeGetDevCaps");
	} 
	wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION),tc.wPeriodMax); 
	timeBeginPeriod(wTimerRes);		/* establish min timer resolution */
}

/* end the timer */
void EndTimer(void)
{
 	timeEndPeriod(wTimerRes); 
}

  
/* 
** To start the timer event, specify the amount of time before 
** the callback occurs, the required resolution, the address of
** the callback function, and user data to supply with the 
** callback. 
*/
  
UINT SetTimerCallback(NPSEQ npSeq,           /* Sequencer data */ 
                      UINT msInterval)       /* Event interval */ 
{ 
    npSeq->wTimerID = timeSetEvent( 
         msInterval,           /* Delay                        */ 
         wTimerRes,            /* Resolution (global variable) */ 
         (LPTIMECALLBACK) retryFunction, /* Callback function   */ 
         (DWORD) npSeq,               /* User data             */ 
         TIME_ONESHOT);               /* Event type (one-time) */ 
 
    if (npSeq->wTimerID != 0) 
        return TIMERR_NOCANDO; 
    else 
        return TIMERR_NOERROR; 
} 
 
  
/* Before freeing cancel outstanding timers. */

void DestroyTimer(NPSEQ npSeq) 
{ 
    if (npSeq->wTimerID) {      /* If timer event is pending */ 
        timeKillEvent(npSeq->wTimerID);  /* Cancel the event */ 
        npSeq->wTimerID = 0; 
    } 
}

#else /* not WIN32 - UNIX */
/* InitTimer and EndTimer are not required under UNIX */
void InitTimer(void)
{
}
void EndTimer(void)
{
}
void SetTimerCallback(NPSEQ npSeq,unsigned int msInterval)
{
	/* install signal handler for SIGALRM */
	/* used sigaction so that I dont need to install it again */
	struct sigaction action;
	action.sa_handler = retryFunction;
	sigemptyset( &(action.sa_mask) );
	/* I would have used SA_RESTART but strangely it doesnt seem to 
	   be there */
	action.sa_flags = 0;

	if (sigaction (SIGALRM, &action,NULL) != 0) {
		ErrorLog(EXIT,"Sigaction failed");
	}

	/* use setitimer or alarm() for this ; in this app. alarm() will 
	 * suffice - remember to convert milliseconds to seconds 
	 */
	alarm(msInterval/1000);
}
#endif /* WIN32 */
