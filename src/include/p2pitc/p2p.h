/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: p2p.h - the prototypes of P2P ITC API for NT-UNIX-ITC           */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: p2p.h,v 1.2 1997/04/08 16:43:14 vipulX Exp $ */
/* $Log: p2p.h,v $
 * Revision 1.2  1997/04/08  16:43:14  vipulX
 * modified for opus communication
 *
 * Revision 1.1.1.1  1997/02/20  19:17:57  vipulX
 * P2PITC
 * */

#ifndef _P2P_H
#define _P2P_H

#ifdef WIN32									/* the NT code */
#include <winsock.h>
#else											
#include <sys/types.h>							/* the UNIX includes */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>	
#include <sys/time.h>
#include <errno.h>
extern int errno;								/* the externs */
#ifdef OH_MACH_LINUX
extern const char * const sys_errlist[];
#else
extern char *sys_errlist[];
#endif
#define SOCKET int								/* the defines */
#define closesocket close
#define ioctlsocket ioctl
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define WSAEWOULDBLOCK EWOULDBLOCK
#define WSAEINTR EINTR
#endif /* WIN32 */

#define P2P_SUCCESS 1
#define P2P_FAILURE 0

#define P2P_PORT 4000

#define INVALID_USER 0
#define CLIENT 1
#define SERVER 2

#ifdef WIN32
#define WSA_ACCEPT	WM_USER+0
#define WSA_READ	WM_USER+1
#endif /* WIN32 */


void WaitForListenConnection(SOCKET *lsock, int port);
void WaitForConnection( SOCKET *lsock, SOCKET *csock);
#ifdef WIN32
void WaitForAsyncConnection(SOCKET *lsock, HWND hWnd);
void AcceptAsyncConnection(SOCKET *lsock, SOCKET *csock, HWND hWnd);
#endif /* WIN32 */
int WaitForMsg( SOCKET csock, int timeOut);
int InitiateConnection(char *serverName, SOCKET *csock,int port);
int SendMsg(SOCKET csock, void *msgBuffer, int msgLength, int timeOut);
int ReceiveMsg(SOCKET csock, void *msgBuffer, int msgLength, int timeOut);

void EndConnection(SOCKET sock);

#endif /* _P2P_H */
