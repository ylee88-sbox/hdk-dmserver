/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: log.c - contains the APIs to generate error and debug log files */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: log.c,v 1.1.1.1 1997/02/20 19:17:57 vipulX Exp $ */
/* $Log: log.c,v $
 * Revision 1.1.1.1  1997/02/20  19:17:57  vipulX
 * P2PITC
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define PATH_MAX MAX_PATH
#else  /* ! WIN32 */
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif /* PATH_MAX */
#endif /* ! WIN32 */

#include <p2pitc/log.h>

FILE *fpError;
FILE *fpDebug;

int debug = 0;

void SetDebugOn(void)
{
  debug = 1;
}

int ErrorLog(int Action, char *message)
{
  int save_errno = errno;
  static FirstTime = 1;
  long timevar;
  char *tmp_pth;
  char *tmp_tn;
  char Log_path[PATH_MAX];
  int dpid;

  if (debug) {
    if (FirstTime) {
      
      /* Check for write access - Problem reported by Bharat Krishna
       * as part of Seico v1 (Nike) tool. Lets check the current dir.
       * then check /tmp and finally env variable TEMP for writing
       * in the  log file - made same changes for Debug Log too */
#ifndef OH_MACH_WIN32
      dpid=getpid();
#else
      dpid=_getpid();
#endif
      FirstTime = 0;
      tmp_pth=getenv("WORK_AREA_ROOT_DIR");
      tmp_tn=getenv("P2PITC_TOOLNAME");
      sprintf(Log_path,"%s/%s/P2PErr.log.%d",tmp_pth?tmp_pth:".",
              tmp_tn  ?tmp_tn :".",dpid);
      fpError = fopen(Log_path,"w+");
      if (fpError == NULL) {
        sprintf(Log_path,"/tmp/P2PErr.log.%d",dpid);
        fpError = fopen(Log_path,"w+");
        if (fpError == NULL) {
          tmp_pth=getenv("TEMP");
          if (tmp_pth != NULL) {
            sprintf(Log_path,"%s/P2PErr.log.%d",tmp_pth,dpid);
            fpError = fopen(Log_path,"w+");
          } else {
            fprintf(stderr,"Cannot write P2PError.log.%d in %s\n",dpid,Log_path);
            fpError = stderr;
          }
        }
      }
    }
    
  
    time(&timevar);
    fprintf(fpError,"%24s:%s\n",ctime(&timevar),message);
    fflush(fpError);
    
  }
  
  if (Action==EXIT){
    fprintf(stderr,"Connection lost to server process. Most likely the server process has terminated unexpectedly.\nMessage was: %s\n", message ? message : "");
    fflush(stderr);
  }
  errno = save_errno;
  return (Action);
}

void DebugLog(char *fmt, ...)
{
  static FirstTime = 1;
  long timevar;
  char date[25];
  char message[256];
  va_list argptr;
  char *tmp_pth;
  char *tmp_tn;
  char Log_path[PATH_MAX];
  int dpid;
  
  if (!debug)
    return;
  if (FirstTime) {
#ifndef OH_MACH_WIN32
    dpid=getpid();
#else
    dpid=_getpid();
#endif
    
    FirstTime = 0;
    tmp_pth=getenv("WORK_AREA_ROOT_DIR");
    tmp_tn=getenv("P2PITC_TOOLNAME");
    sprintf(Log_path,"%s/%s/P2PDbg.log.%d",tmp_pth?tmp_pth:".",
            tmp_tn  ?tmp_tn :".",dpid);
    
    fpDebug = fopen(Log_path,"w+");
    
    if (fpDebug == NULL) {
      sprintf(Log_path,"/tmp/P2PDbg.log.%d",dpid);
      fpDebug = fopen(Log_path,"w+");
      if (fpDebug == NULL) {
        tmp_pth=getenv("TEMP");
        if (tmp_pth != NULL) {
          sprintf(Log_path,"%s/P2PDbg.log.%d",tmp_pth,dpid);
          fpDebug = fopen(Log_path,"w+");
        } else{
          fprintf(stderr,"Cannot write P2PDebug.log.%d in %s\n",dpid,Log_path);
          fpDebug = stdout;
        }
        
      }
    }
  }
 
  
  va_start(argptr,fmt);
  vsprintf(message,fmt,argptr);
  va_end(argptr);
  
  time(&timevar);
  sprintf(date,"%23s",ctime(&timevar));
  fprintf(fpDebug,"%s:%s\n",date,message);
  fflush(fpDebug);
}
