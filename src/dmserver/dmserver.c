/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-2003 Intel Corporation. All Rights Reserved.          */
/*   Program: DMS SERVER PROGRAM                                              */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: dmserver.c,v 1.5 1997/04/08 16:45:20 vipulX Exp $ */
/* $Log: dmserver.c,v $
 * Revision 1.5  1997/04/08  16:45:20  vipulX
 * Changed because of WaitForListenConnection()
 *
 * Revision 1.4  1997/02/21  22:38:41  vipulX
 * Removed SetDebugOn
 *
 * Revision 1.3  1997/02/21  22:30:50  vipulX
 * Fixed dup2()
 *
 * Revision 1.2  1997/02/21  19:01:49  vipulX
 * Modified so that  stdout & stderr go to /tmp/dmserver.pid
 *
 * Revision 1.1.1.1  1997/02/20  19:17:21  vipulX
 * DMServer
 * */

#ifdef __cplusplus
extern "C" { 
#endif

#include <stdio.h> 
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dmserver.h"
#include "interp2p.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h> 

#define TIME_OUT 65535
char *senderId;
char *groupName=0;
SOCKET lsock, csock,crsock,cwsock;
int port;
extern inter_msg	interMsg;
extern p2p_msg	msgBuffer;
extern dm_CALLBACK interCallbacks[INTERS_NUM_COMMANDS];
extern void hash_hashSetReportMsg(); 
extern void iDMS_reset_theLibName(); 
extern char *dmsCReportMessage();
extern int  dmsCPIInitDMS (char *appName, char *bootName, int nullgtMode);
extern void dmsPrintIdmsLog(char* a);
extern void dmsCPIExitDMS(); 
extern int sendBigMsg;
extern char * bigBufferString;

#ifdef __cplusplus
} 
#endif

/* global variables */
char globalToolName[INTERS_TOOL_LENGTH];
inter_msg interMsg;
int inter_itc_init;
int dmserver_log=0;


typedef void  Sigfunc(int);
Sigfunc * signal(int sigNum, Sigfunc * handler) {
	struct sigaction act, oact;

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;

	if (sigaction(sigNum,&act,&oact) < 0)
		return (SIG_ERR);
	return (oact.sa_handler);
}

void ProcessMsg(inter_msg *interMsg);

/* when our child dies off */
void reaper(int signo)
{
	int pid, stat;
	waitpid(pid, &stat,WNOHANG);
}
/* 
** called when a SIGSEGV or SIGDANGER occurs
** sends a message to its clients telling them to 
** terminate
*/
void terminateHandler(int signo) 
{

	DebugLog("dmserver: Terminating because of  signal %d\n",signo);
        close(csock);
	(void) dmsCPIExitDMS();	
	exit(1);
}
void terminateHandler_noclean(int signo)
{

        DebugLog("dmserver: Terminating because of  signal %d\n",signo);
        close(csock);
        exit(1);
}

/* 
** the main function simply decodes the arguments passed in.
** the main program initialized the various handlers to take care 
** segv
*/
int main(int argc, char *argv[]) 
{
  int argcCounter;
  int msgLength;
  int fd;
  char logs[256];
  int pid;
  char *umask_env;
  mode_t umask_val;
  int len = 0;

  globalToolName[0] = '\0';
  inter_itc_init = 0;
  lsock=csock=crsock=cwsock=0;
  port=0;
  dmserver_log=0;
  /* get input arguments if any */
  for(argcCounter=1; argcCounter<=argc-1 ;++argcCounter) {
    if (strcmp(argv[argcCounter],"-trace")==0)
      SetDebugOn();
    else
      if (strcmp(argv[argcCounter],"-serverlog")==0)
        dmserver_log=1;
      else 	
        if (strcmp(argv[argcCounter],"-read")==0)
          crsock=atoi(argv[++argcCounter]);
        else
          if (strcmp(argv[argcCounter],"-write")==0)
            cwsock=atoi(argv[++argcCounter]);
          else
            if (strcmp(argv[argcCounter],"-port")==0)
              port=atoi(argv[++argcCounter]);
            else
	      if (strcmp(argv[argcCounter],"-groupName")==0) {
		groupName=strdup(argv[++argcCounter]);
              }
	      else
	        if (strcmp(argv[argcCounter],"-testinit")==0){
		  printf("Testing dmsCPIInitDMS: globalToolName=%s, locationName=%s\n",argv[argcCounter+1], argv[argcCounter+2]);
		  if (dmsCPIInitDMS(argv[argcCounter+1],argv[argcCounter+2],1) != intersSuccess){
		    printf("failed to call dmsCPIInitDMS\n");
	          }else
		    printf("called dmsCPIInitDMS successfully!\n");
	        } else
                  DebugLog("dmserver: Invalid option");
  }
  
  if (cwsock == crsock) {
    switch(fork()) {
    case -1 :
      ErrorLog(0, "Process table full. Cannot fork  dmserver \n");
      exit(3);
    case 0  :
      setsid();
      umask(0);
      close(0);
      sprintf(logs,"/tmp/dmserver.%d",getpid());
      fd = open(dmserver_log?logs:"/dev/null",O_WRONLY|O_CREAT,0666);
      close(1);dup2(fd,1);
      close(2);dup2(fd,2);
      fflush(stderr);
      fflush(stdout);
      if (signal(SIGCHLD,SIG_IGN) == SIG_ERR) 
        dmsPrintIdmsLog("dmserver: Could not set SIGCHLD Handler\n");
      chdir("/");
      break;
    default :
      exit(0);
    }
    
  }
  
  /* initialize all the signal handlers */
  if (signal(SIGSEGV, terminateHandler) == SIG_ERR) 
    dmsPrintIdmsLog("dmserver: Could not set SIGSEGV Handler\n");
  if (signal(SIGBUS, terminateHandler) == SIG_ERR) 
    dmsPrintIdmsLog("dmserver: Could not set SIGSEGV Handler\n");
  if (signal(SIGPIPE,SIG_IGN) == SIG_ERR)
    dmsPrintIdmsLog("dmserver: Could not set SIGPIPE Handler\n");
  if (signal(SIGQUIT,SIG_IGN) == SIG_ERR)
    dmsPrintIdmsLog("dmserver: Could not set SIGQUIT Handler\n");
 

  if (cwsock==crsock) {
    WaitForListenConnection(&lsock, port ? port : P2P_PORT);
    if (!lsock) {
      dmsPrintIdmsLog("dmserver already running. Goodbye....");
      exit(1);
    }
    for(;;) {
      pid=0;
      DebugLog("Waiting for Connection");
      WaitForConnection( &lsock, &csock);
      pid = fork();
      if ( pid == -1){
        fprintf(stderr,"Cannot  fork dmserver  for new connection. errorno %d\n",errno);
      }
      
      if ( pid != 0) {
	close(csock);
	continue;
      }
      /* DO MODIFICATIONS HERE in dmserver */
      /* In interprocs.c/intersinit() before calling dmsCPIInitDMS() */
      /* 1. GET user name so that and do setuid() -do this later */
      /* 2. GET dms.pth file pathname */
      /* 3. parse dms.pth file and */
      /* 4. remove comments and do setenv for rest */
      
      /* Modifications in dmclient / dmsCPIInitDMS() */
      /* 1. getenv var P2PITC_USER */
      /* 2. getenv var P2PITC_SETUPFILE */
      /* 3. Fill in username, unixDir, setupfile, appName, dmspath */
      
      close(lsock);
      inter_itc_init = 2;
      crsock=csock;
      cwsock=csock;
      /* initialize all the signal handlers */
      if (signal(SIGSEGV, terminateHandler_noclean) == SIG_ERR) 
        dmsPrintIdmsLog("dmserver: Could not set SIGSEGV Handler\n");
      if (signal(SIGBUS, terminateHandler_noclean) == SIG_ERR) 
        dmsPrintIdmsLog("dmserver: Could not set SIGSEGV Handler\n");
      if (signal(SIGCHLD,terminateHandler) == SIG_ERR) 
        dmsPrintIdmsLog("dmserver: Could not set SIGCHLD Handler\n");
      if (signal(SIGPIPE,terminateHandler) == SIG_ERR)
        dmsPrintIdmsLog("dmserver: Could not set SIGPIPE Handler\n");
      if (signal(SIGQUIT,terminateHandler) == SIG_ERR)
        dmsPrintIdmsLog("dmserver: Could not set SIGQUIT Handler\n");
      break;
    }
  }  
  else {
    DebugLog("Running in Pipe Mode Read %d Write %d\n",crsock,cwsock);
    inter_itc_init = 2;
    WaitForListenConnection(0,0);
  }
  /* Set the umask for the DM server if specified */
  umask_env=getenv("DMSERVER_UMASK");
  if ( umask_env != NULL ) {
    umask_val=strtol(umask_env, NULL, 8); /* always assume octal numbers */
    umask(umask_val);
    printf("dmserver umask set to %o\n", umask_val);
  }
  while(inter_itc_init) {
    if ( WaitForMsg(crsock,TIME_OUT) != P2P_SUCCESS ){
      ErrorLog(0,"Error waiting for message\n");
      if (!csock) {
	DebugLog("Invalid operation while waiting");
	exit(1);
      }
      continue;
    }
    if ( ReceiveMsg(crsock, &msgBuffer, sizeof(msgBuffer),TIME_OUT)
	 != P2P_SUCCESS ) {
      EndConnection(crsock);
      inter_itc_init = 0;
      if (!csock) 
	exit(1);
      continue;
    }
    if ( ExtractMsg( &msgBuffer, &interMsg ) != P2P_SUCCESS )
      continue;
    /* Replace sender by something better */
    /* fill up senderId with meaningful stuff */
    ProcessMsg(&interMsg);
    if ( inter_itc_init != 1) {
      if (!*globalToolName) {
        strcpy(globalToolName,interMsg.arguments[5].val.stArg);
        
      }

      if (dmsCPIInitDMS(globalToolName,
			argv[0],1) != intersSuccess) {
	dmsPrintIdmsLog("dmserver: Could not Initialize dms\n");
        fprintf(stderr,"%s", dmsCReportMessage());
        inter_itc_init =1;
      } else {
        inter_itc_init = 1;
      }
      (void) hash_hashSetReportMsg();

    }
    
    BuildMsg( &msgBuffer, &interMsg );
    msgLength = ntohl(msgBuffer.length);

    if ( SendMsg(cwsock, &msgBuffer, msgLength,TIME_OUT) != P2P_SUCCESS ) {
      if(bigBufferString != 0) { 
        free(bigBufferString);
        bigBufferString = 0;
      }
      sendBigMsg = 0;
      continue;
    }

    if(bigBufferString != 0) {
      free(bigBufferString);
      bigBufferString = 0;
    }
    sendBigMsg = 0;
  }
  
  exit(0);
}


/* 
** The processing of the message involves, unpacking the message
** and calling the appropriate handler based on the opNum.
** The handler will set the return values if any and then
** return an appropriate code - this procedure will then
** package the return values, set the status and return
*/

void ProcessMsg(inter_msg *interMsg)
{
	senderId = interMsg->senderId;
	/* start unpacking the message */
	interMsg->valid = 0; interMsg->status = -1; interMsg->stString = 0; /*NULL;*/
	iDMS_reset_theLibName();
	sendBigMsg = 0;
	if(INTERS_NUM_COMMANDS > (interMsg->arguments[1].val.intArg))
	  interCallbacks[interMsg->arguments[1].val.intArg]();
		
}

















