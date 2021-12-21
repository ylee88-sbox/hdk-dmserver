/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: This is ntsock.h header file. It includes various public defn   */
/*            for UNIX-NT-ITC samples.                                        */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: ntsock.h,v 1.1.1.1 1997/02/20 19:17:57 vipulX Exp $ */
/* $Log: ntsock.h,v $
 * Revision 1.1.1.1  1997/02/20  19:17:57  vipulX
 * P2PITC
 * */

#ifndef _NTSOCK_H
#define _NTSOCK_H

/* we want to run on both NT + UNIX - don't we */
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

typedef struct sockaddr SOCKADDR;				/* and now the typedefs */
typedef struct sockaddr *PSOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in *PSOCKADDR_IN;
typedef struct in_addr IN_ADDR;
typedef struct in_addr *PIN_ADDR;
typedef struct hostent HOSTENT;
typedef struct hostent *PHOSTENT;
typedef struct linger LINGER;
typedef struct linger *PLINGER;


#endif /* WIN32 */


#define BLOCK 0									/* Socket blocking mode */
#define NBLOCK 1								/* Socket non-blocking mode */

int WinSockInitialize(void);
char * WinSockError(void);
void ChangeSockMode(SOCKET s, int mode);
int GetRecvLength(SOCKET s);
int CheckForWrite(SOCKET s);
int GetRecvLengthForBigMsg(SOCKET s);
#endif /* _NTSOCK_H */
