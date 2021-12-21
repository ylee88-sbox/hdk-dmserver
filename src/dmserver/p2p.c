/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: p2p.c - contains the P2P ITC APIs for NT-UNIX-ITC uses winsock  */
/*            on NT and BSD sockets on Unix.                                  */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: p2p.c,v 1.4 1997/04/30 21:50:21 vipulX Exp $ */
/* $Log: p2p.c,v $
 * Revision 1.4  1997/04/30  21:50:21  vipulX
 *  fixed NT's small TCP bufer
 *
 * Revision 1.3  1997/04/21  19:29:30  vipulX
 * Fixed bugs in 1.0.a1
 *
 * Revision 1.2  1997/04/08  16:43:13  vipulX
 * modified for opus communication
 *
 * Revision 1.1.1.1  1997/02/20  19:17:58  vipulX
 * P2PITC
 * */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <p2pitc/p2p.h>
#include <p2pitc/ntsock.h>
#include <p2pitc/timer.h>
#include <p2pitc/log.h>
// I _would_ have used OH_COMP_MSC, but this !@#!@#$ (ectoplasm, bashibosuk)
// module does not depend on Boo

#ifdef OH_MACH_LINUX
#define	R_OK	4		/* Test for read permission.  */
#define	W_OK	2		/* Test for write permission.  */
#define	X_OK	1		/* Test for execute permission.  */
#define	F_OK	0		/* Test for existence.  */
extern int access __P ((__const char *__name, int __type));
#endif
/* a dirty fix for select on HP-UX but no other way out */
#ifdef WIN32
#define FIX_IT
#else
#define FIX_IT   
#endif

/* 
** This API set would be used by both client and server. To determine which is
** using this set, we set a flag 
*/
static int sockInit = 0;
int user = INVALID_USER;
static int ackPending = 0;

char * bigReceivedString = 0;
char * bigBufferString = 0;
int sendBigMsg = 0;
int secondMsgSize = 0;

/* 
** Open socket in passive mode and wait for initiation requests from clients.
** 1.0.a1 added port # as arg
*/
void WaitForListenConnection(SOCKET *lsock, int port)
{
  SOCKADDR_IN myaddr_in;		/* for local socket address */
  int optlvl = SOL_SOCKET;	/* option setting level */
  int optname = SO_REUSEADDR;	/* option name */
  int optval = 1;			/* setting options for SO_REUSEADDR */
  int optlen = sizeof (optval);	/* sizeof option value */


  if (!port) {
    user=SERVER;
    return;
  }

  /* check if this library has been called before */
  if (user == INVALID_USER){
    user = SERVER;
    /* Initialize the winsock */
    if ( WinSockInitialize() != 0) {
      ErrorLog(EXIT,"Error initializing winsock\n");
    }
  }
  memset ((char *)&myaddr_in, 0, sizeof(SOCKADDR_IN));
  myaddr_in.sin_family = AF_INET;
  myaddr_in.sin_addr.s_addr = INADDR_ANY;
  myaddr_in.sin_port = ntohs((short)port);
  *lsock = socket (AF_INET, SOCK_STREAM, 0);
  if (*lsock == INVALID_SOCKET) {
    DebugLog(WinSockError());
  }
  if ( setsockopt(*lsock, optlvl, optname, (char *)&optval, 
                  optlen) == SOCKET_ERROR ) {
    ErrorLog(EXIT, WinSockError());
    *lsock=0;
    return;
  }
  if (bind(*lsock, (PSOCKADDR)&myaddr_in, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
    ErrorLog(EXIT, WinSockError());
    *lsock=0;
    return;
  }
  if (listen(*lsock, 1) == SOCKET_ERROR) {
    ErrorLog(EXIT, WinSockError());
    *lsock=0;
   
 }
}

/* 
** Wait for new connections 
*/
void WaitForConnection( SOCKET *lsock, SOCKET *csock)
{
  SOCKADDR_IN peeraddr_in;	/* for peer (client) address */
  int addrlen;
  int optlvl = SOL_SOCKET;	/* option setting level */
  int optname = SO_LINGER;	/* option name */
  struct linger optval;		/* setting options for SO_LINGER */
  int optlen = sizeof (optval);	/* sizeof option value */

  optval.l_onoff = 1;		/* turn linger on */
  optval.l_linger = 0;		/* but let's not xmit after close */

  addrlen = sizeof(SOCKADDR_IN);
 restart:
  *csock = accept(*lsock, (PSOCKADDR)&peeraddr_in, &addrlen);
  if ( *csock == SOCKET_ERROR) {
    if (WSAEINTR == errno)
      goto restart;
    ErrorLog(EXIT,WinSockError());
  }
  if ( setsockopt(*csock, optlvl, optname, (char *)&optval, optlen) 
       == SOCKET_ERROR ) {
    ErrorLog(EXIT, WinSockError());
  }
}
/*
** Waits for Async connection under Windows NT environment
** not available under Unix - 1.0.a1
*/
#ifdef WIN32
void WaitForAsyncConnection(SOCKET *lsock, HWND hWnd)
{
  if (WSAAsyncSelect(*lsock,hWnd,WSA_ACCEPT,FD_ACCEPT) > 0 )
    ErrorLog(EXIT,WinSockError());
}
#endif /* WIN32 */

/*
** Accepts async connection after a previous call to WSAAsyncSelect
** not available under Unix - 1.0.a1
*/
#ifdef WIN32
void AcceptAsyncConnection(SOCKET *lsock, SOCKET *csock, HWND hWnd)
{

  SOCKADDR_IN peeraddr_in;        /* for peer address */
  int addrlen;

  addrlen = sizeof(SOCKADDR_IN);
 restart:
  *csock = accept(*lsock, (PSOCKADDR)&peeraddr_in, &addrlen);
  if ( *csock == SOCKET_ERROR) {
    if (WSAEINTR == errno)
      goto restart;
    ErrorLog(EXIT,WinSockError());
  }
	
  /* Set Async read mode on csock */
  if (WSAAsyncSelect(*csock,hWnd,WSA_READ,FD_READ) > 0 )
    ErrorLog(EXIT,WinSockError());
}
#endif /* WIN32 */

/* 
** Waits for the "send" from client. The intention is not to hang
** around in Read . 
*/
int WaitForMsg( SOCKET csock, int timeOut)
{
	
  fd_set read_fds;

  FD_ZERO(&read_fds);		/* Clear the fd_set */
  FD_SET(csock, &read_fds);
  do {
    errno=0;
    if ( select(csock+1, FIX_IT &read_fds, NULL, NULL, NULL) == SOCKET_ERROR) {
      ErrorLog (0, "Problem in WaitForMsg");
    }
  } while ( errno==EINTR);
  return P2P_SUCCESS;
}

/* 
** Initiates the connection for the client to the server
** 1.0.a1 added port # as arg
*/
int InitiateConnection(char *serverNamep, SOCKET *csock, int port)
{
  SOCKADDR_IN peeraddr_in;	/* for peer socket address */
  int addrlen;				/* address length */
  SOCKADDR_IN	myaddr_in;		/* local address */
  PHOSTENT hp;				/* pointer to host info for remote host */
  fd_set write_fds;			/* write fd_set to check in connect */
  struct timeval tv;			/* timevalue for connect timeout */
  int value,len;				/* length and value for SO_RECVBUF */
  char* serverName;
  
  if (serverNamep) {
    serverName=strdup(serverNamep);
    serverName=strtok(serverName,":");
    while(serverName){
      

      if (user != SERVER)
        user = CLIENT;			/* we are on a client system */
      InitTimer();				/* Initialize the timer system */
      
      /* Initialize the winsock */
      if ( WinSockInitialize() != 0) {
        ErrorLog(EXIT,"Error initializing winsock");
      }
      hp = gethostbyname (serverName);
      if (hp == NULL) {
        ErrorLog(EXIT, WinSockError());
        serverName=strtok(NULL,":");
        *csock=0;
        continue;
      }
      memset ((char *)&peeraddr_in, 0, sizeof(SOCKADDR_IN));
      memset ((char *)&myaddr_in, 0, sizeof(SOCKADDR_IN));
      peeraddr_in.sin_family = AF_INET;
      peeraddr_in.sin_addr.s_addr = ((PIN_ADDR)(hp->h_addr))->s_addr;
      peeraddr_in.sin_port = ntohs((short)port);
      
      *csock = socket (AF_INET, SOCK_STREAM, 0);
      if (*csock == INVALID_SOCKET) {
        ErrorLog(EXIT, WinSockError());
        *csock=0;
        break;
      }
	  
      FD_ZERO(&write_fds);		/* Clear the fd_set */
      FD_SET(*csock, &write_fds);
      tv.tv_sec = 1;				/* set time out values */
      tv.tv_usec = 0;
#if 0 
#ifdef WIN32
      ChangeSockMode(*csock, NBLOCK);	/* Put socket in non-blocking mode */
#endif /* WIN32 */
#endif
      /* changed as suggested by Anat - now returns P2P_FAILURE 1.0.a1 */
      if (connect(*csock, (PSOCKADDR)&peeraddr_in, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
        if ( WSAGetLastError() != WSAEWOULDBLOCK ) {
          ErrorLog(0, WinSockError());
          serverName=strtok(NULL,":");
          *csock=0;
          continue;
          
        }
        /* Socket might block so give a time limit of 1 sec */
        if ( select((*csock)+1, NULL, FIX_IT &write_fds, NULL, &tv) == SOCKET_ERROR) {
          ErrorLog(EXIT, WinSockError()); 
        }
        if (! FD_ISSET(*csock, &write_fds)) {
          DebugLog("connect() timed out");
          exit(1);
        } 
      }
      /* begin additions to fix NT's small buffer length */
      len=4;
      getsockopt(*csock,SOL_SOCKET,SO_RCVBUF, (char *)&value, &len);
      if (value < 65536) {
        value = 65536;
        setsockopt(*csock,SOL_SOCKET,SO_RCVBUF, (char *)&value, len);
    }
    /* end additions to fix NT's small buffer length */
#if 0
#ifndef WIN32
      ChangeSockMode(*csock, NBLOCK);	/* Put socket in non-blocking mode */
#endif /* WIN32 */
#endif
      addrlen = sizeof (SOCKADDR_IN);
      if (getsockname(*csock,(PSOCKADDR)&myaddr_in,&addrlen) == SOCKET_ERROR) {
        ErrorLog(EXIT,WinSockError());
        *csock=0;
      }
      
      break;
    }
  }
  else {
    return initiatePipes(csock) ?P2P_SUCCESS : P2P_FAILURE;
  }
  DebugLog("Connected to Server");
  return *csock?P2P_SUCCESS:P2P_FAILURE;
  
}


/* 
** Sends the message to peer . Should be used after connection has been 
** established and the data to be sent, has been placed in the required 
** message format. 
*/
int SendMsg(SOCKET csock, void *msgBuffer, int msgLength, int timeOut)
{
  /* initialize the timer guys it's a global in timer.h */
  int len=0;
  nSeq.wTimerID = 0;
  nSeq.nAckPending = 0;
  nSeq.nRetryCount = 0;
  

  DebugLog("Sending Message");
  if (CheckForWrite(csock)) {
    
    len=write(csock, msgBuffer, msgLength);
    if (len==-1) 
      len=send(csock, msgBuffer, msgLength,0);
    if (len != msgLength) {
      ErrorLog(0,WinSockError());
      return P2P_FAILURE;
    }

    /* Send second time if it is for large message.*/ 
    if((sendBigMsg != 0) && bigBufferString) {
      msgLength = strlen(bigBufferString)+1;
      len=write(csock, bigBufferString, msgLength);
      if (len==-1)
        len=send(csock, bigBufferString, msgLength,0);
      if (len != msgLength) {
        ErrorLog(0,WinSockError());
        return P2P_FAILURE;
      }
    }

    /* mark that we have an Ack Pending */
    /* only in a client do we actually time out & expect a reply */
    if ( user == CLIENT ){
      nSeq.nAckPending = 1;
#if 0
      SetTimerCallback(&nSeq, timeOut);	/* start the timer */
#endif
    }
  }
  else {
    DebugLog("send() timeout");
    return P2P_FAILURE;
  }
  return P2P_SUCCESS;
}

/* 
** Receives the message in the buffer. A timeout value can be specified which 
** can be 0 , making the connection blocking. 
*/
int ReceiveMsg(SOCKET csock, void *msgBuffer, int msgLength, int timeOut)
{

  int len=0;
  int recv_length=0;

  DebugLog("Calling Get Receive Length");

  if (sendBigMsg == 0) /* regular single msg communication */
    recv_length = GetRecvLength(csock);
  else if (sendBigMsg == 1) {
    /* 1st receive of large msg communication. Get the msgBuffer header */
    recv_length = GetRecvLengthForBigMsg(csock);
    sendBigMsg = 0;
  } else /* 2nd receive of large msg communication. Get the string */
    recv_length = secondMsgSize;
  
  if (recv_length == 0)
    return P2P_FAILURE;

  if(sendBigMsg != 0) {
    secondMsgSize = 0; /* reset */ 
    if(!msgBuffer) {   /* error cking */
      nSeq.nAckPending = 0;
      return P2P_FAILURE;
    } 
    while ( len < recv_length ) {
      DebugLog("Receiving Message");
      len += read(csock,(char*)msgBuffer+len,recv_length);
      if (len==-1) {
        len = recv(csock,msgBuffer,recv_length,0);
      } 
    }
  }  else {
    /* regular receive */
    *((int*)msgBuffer)=recv_length;

    while ( len < recv_length ) {
      DebugLog("Receiving Message");
      len += read(csock,(char*)msgBuffer+len+sizeof(int),recv_length);
      if (len==-1) {
        len = recv(csock,msgBuffer,recv_length,0);
      }
    } 
  }

  /* mark that we got back our ack */
  nSeq.nAckPending = 0;
  return P2P_SUCCESS;
}


/* Ends the communication between client and server. */
void EndConnection(SOCKET sock)
{
  DebugLog("Closing connection");
  if (sockInit) {
    ErrorLog(0,"Terminating Child process\n");
#ifndef WIN32
    kill(sockInit,9);
    wait4(sockInit,NULL,0,NULL);
    sockInit=0;
#else
    //	  _kill(sockInit,9);
#endif 
  }
  closesocket(sock);
#ifdef WIN32
  /* Implicit assumption - no server on NT - if a server needs
     to be created -- check that WSACleanup is called after  
     closing listen socket */
  WSACleanup();
#endif /* WIN32 */
  EndTimer();
}





#ifdef WIN32
void PASCAL retryFunction(UINT a , UINT b,DWORD c, DWORD d, DWORD e)
#else /* not win32 - UNIX */
  void retryFunction(int signal)
#endif /* WIN32 */
{
}
#ifndef WIN32
char*
findFullPath(char*execName)
{
  static char buffer[1024];
  char * dmpath=getenv("DMSERVER_BIN_DIR");
  char * dmenv=getenv("DMSERVER_EXE_NAME");
  char * path=getenv("PATH");
  char * tok=NULL;
  char * pathDup;
  char * dmexe=NULL; 

  path=dmpath?dmpath:path;
  dmexe=dmenv?dmenv:execName;
  if(dmexe && !strcmp(dmexe, "")) /* empty DMSERVER_EXE_NAME */
    dmexe = execName;
  if (!path) {
    return NULL;
  }
  path=strdup(path);
  tok=strtok(path,":");
  while(tok) {
    sprintf (buffer,"%s/%s",tok,dmexe); /* execName);*/
    if (!access(buffer, X_OK ) )       {
      free(path);
      return buffer;
    }
    tok=strtok(NULL,":");
  }
  free(path);
  
  return NULL;
  

}
#endif
int initiatePipes(int * hpipe) {

#ifndef WIN32
  pid_t pid;
  
  char hstr[64];
  char histr[64];
  char * args[8];
  char * dmserverFp;
  
  
  int cpipe[2];
  int rpipe[2];
  dmserverFp=findFullPath("dmserver");
  if (!dmserverFp) {
    ErrorLog(0,"Failed to locate dmserver executable\n");
    return 0;
  }
  

  if(pipe( cpipe, 256 ) == -1 )
    return 0;
  if(pipe( rpipe, 256 ) == -1 )
    return 0;
  pid=fork();
  if (pid==-1) {
    ErrorLog(0,"Failed to fork dmserver process\n");
  }
  else 
    if (pid) {
      sockInit=pid;
      hpipe[0]=rpipe[0];
      hpipe[1]=cpipe[1];
      close(rpipe[1]);
      close(cpipe[0]);
      user=CLIENT;
      return 1;
    }
  /* If there is an argument, this must be the spawned process. */
    else{
    
      sprintf(hstr,"%d",cpipe[0]);
      sprintf(histr,"%d",rpipe[1]);
      close(cpipe[1]);
      close(rpipe[0]);
    
    
      args[0] = dmserverFp;
      args[1] = "-read";
      args[2] = hstr;
      args[3] = "-write";
      args[4]= histr;
      args[5]= NULL;
      args[6] = NULL;
      if (execvp(args[0],args)==-1) {
        ErrorLog(0,"Failure to run the dmserver process\n");
        close(hpipe[0]);
        close(hpipe[1]);
        exit(1);
      }
    }
       
#else
  return 0;         
#endif  
  
}
