/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: ntsock.c - for NT socket related stuff                          */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: ntsock.c,v 1.1.1.1 1997/02/20 19:17:58 vipulX Exp $ */
/* $Log: ntsock.c,v $
 * Revision 1.1.1.1  1997/02/20  19:17:58  vipulX
 * P2PITC
 * */

#include <stdio.h>
#ifndef WIN32
#include <sys/ioctl.h>
#endif
#ifdef WIN32
#include <errno.h>
#endif
#include <p2pitc/ntsock.h>
#include <p2pitc/timer.h>
#include <p2pitc/log.h>

/* a dirty fix for select on HP-UX but no other way out */
#ifdef WIN32
#define FIX_IT
#else
#define FIX_IT   
#endif

#ifdef WIN32
static struct errListTag {
  unsigned int code;
  char * Msg;
} ErrList[] = {
  { WSAEACCES, "Permission denied." },
  { WSAEADDRINUSE, "Address already in use." },
  { WSAEADDRNOTAVAIL, "Cannot assign requested address." },
  { WSAEAFNOSUPPORT, "Address family not supported by protocol family." },
  { WSAEALREADY, "Operation already in progress." },
  { WSAECONNABORTED, "Software caused connection abort." },
  { WSAECONNREFUSED, "Connection refused." },
  { WSAECONNRESET, "Connection reset by peer." },
  { WSAEDESTADDRREQ, "Destination address required." },
  { WSAEFAULT, "Bad address." },
  { WSAEHOSTDOWN, "Host is down." },
  { WSAEHOSTUNREACH, "No route to host." },
  { WSAEINPROGRESS, "Operation now in progress." },
  { WSAEINTR, "Interrupted function call." },
  { WSAEINVAL, "Invalid argument." },
  { WSAEISCONN, "Socket is already connected." },
  { WSAEMFILE, "Too many open files." },
  { WSAEMSGSIZE, "Message too long." },
  { WSAENETDOWN, "Network is down." },
  { WSAENETRESET, "Network dropped connection on reset." },
  { WSAENETUNREACH, "Network is unreachable." },
  { WSAENOBUFS, "No buffer space available." },
  { WSAENOPROTOOPT, "Bad protocol option." },
  { WSAENOTCONN, "Socket is not connected." },
  { WSAENOTSOCK, "Socket operation on non-socket." },
  { WSAEOPNOTSUPP, "Operation not supported." },
  { WSAEPFNOSUPPORT, "Protocol family not supported." },
  { WSAEPROCLIM, "Too many processes." },
  { WSAEPROTONOSUPPORT, "Protocol not supported." },
  { WSAEPROTOTYPE, "Protocol wrong type for socket." },
  { WSAESHUTDOWN, "Cannot send after socket shutdown." },
  { WSAESOCKTNOSUPPORT, "Socket type not supported." },
  { WSAETIMEDOUT, "Connection timed out." },
  { WSAEWOULDBLOCK, "Resource temporarily unavailable." },
  { WSAHOST_NOT_FOUND, "Host not found." },
  { WSANOTINITIALISED, "Successful WSAStartup not yet performed." },
  { WSANO_DATA, "Valid name, no data record of requested type." },
  { WSANO_RECOVERY, "This is a non-recoverable error." },
  { WSASYSNOTREADY, "Network subsystem is unavailable." },
  { WSATRY_AGAIN, "Non-authoritative host not found." },
  { WSAVERNOTSUPPORTED, "WINSOCK.DLL version out of range." },
  { WSAEDISCON, "Graceful shutdown in progress." },
  { 0, "Unknown WinSock Error" }
};
#endif /* WIN32 */
	

/* Initialize the winsock library */
int WinSockInitialize(void)
{
#ifdef WIN32 
  WORD wVersionReq;
  WSADATA wsaData;

  wVersionReq = MAKEWORD(1,1);
  return( WSAStartup(wVersionReq,&wsaData) );
#else
  return 0;
#endif /* WIN32 */
}


/* Fakes WSAGetLastError() for UNIX */
#ifndef WIN32
int WSAGetLastError()
{
  return errno;
}
#endif /* WIN32 */

/* Would return an error message string */
char * WinSockError(void)
{
#ifdef WIN32
  int i=0;
  unsigned int code = WSAGetLastError();		/* the error code for winsock errs */
	
  while ( (ErrList[i].code != 0) && ( ErrList[i].code != code) ) i++;
  return ( ErrList[i].Msg );
#else 
  return ((char*)strerror(errno));
#endif /* WIN32*/
}

/* To put the socket in Block/Non - Block mode */
void ChangeSockMode(SOCKET s, int mode)
{
  long ioctlCmd;				/* command to ioctl to put in NB mode */
  unsigned long ioctlArg;		/* argument to ioctl for Non Block mode */

  ioctlCmd = FIONBIO;
  ioctlArg = mode;
	
  if ( ioctlsocket(s,ioctlCmd, &ioctlArg) == SOCKET_ERROR) {
    ErrorLog(EXIT, WinSockError());
  }
}

/* Gets the amount of data available on the socket 
 * for reading  returns 0 if error - close on peer
 * returns -1 if full count not got
 * returns count on read
 */
int GetRecvLength(SOCKET s)
{
  int ioctlCmd;				/* command to ioctl to check data */
  unsigned int ioctlArg;		/* argument to get datasize */
  fd_set read_fds;			/* read fd_set to check in read */
  struct timeval tv;			/* timevalue for connect timeout */
  int	peekLength;		/* peek at first 4 bytes */
  int	peekLengthN;		/* peek at first 4 bytes */



  /* check to see if descriptor is ready for reading */
  FD_ZERO(&read_fds);			/* Clear the fd_set */
  FD_SET(s, &read_fds);
  while (1){ 
   errno=0;
   if ( select(s+1,FIX_IT &read_fds, NULL, NULL, NULL) == SOCKET_ERROR) {
     if (errno!=EINTR){
       ErrorLog(EXIT, WinSockError());
       break;
     } else {
	continue;
     } 
   }
   break;
  } 
  if (! FD_ISSET(s, &read_fds)) {
    return (0);
  } 
	
  if (read(s,&peekLengthN, 4) != 4 ) {
    ErrorLog(0,WinSockError());
  }

  ioctlCmd = FIONREAD;
  ioctlArg = 0;
  if ( ioctlsocket(s,ioctlCmd, &ioctlArg) == SOCKET_ERROR) {
    ErrorLog(EXIT, WinSockError());
  }
  if (ioctlArg == 0 )
    return (ioctlArg);
  peekLength = ntohl(peekLengthN)-sizeof(int);
  //peekLength=ioctlArg;

  if (peekLength == 0)
    return -1;
  if ( ioctlArg > peekLength) {
    return -1;
  }
        
  else
    return (peekLength);
}

/* Checks if it's possible to write to 
 * the specified socket 
 */
int CheckForWrite(SOCKET s)
{
  fd_set write_fds;			/* write fd_set to check writes */
  struct timeval tv;			/* timevalue for select timeout */


  /* check to see if descriptor is ready for writing */
  FD_ZERO(&write_fds);			/* Clear the fd_set */
  FD_SET(s, &write_fds);
  tv.tv_sec = 1;				/* set time out values */
  tv.tv_usec = 0;
  while (1){
   errno=0;
   if ( select(s+1, NULL, FIX_IT &write_fds, NULL, &tv) == SOCKET_ERROR) {
     if (errno!=EINTR){
       ErrorLog(EXIT, WinSockError());
       break;
     } else {
        continue;
     }
   }
   break;
  }
  if (! FD_ISSET(s, &write_fds)) {
    return 0;
  } 
  return 1;
}

/* Works the same as GetRecvLength except 
 * it allows ( ioctlArg > peekLength) since
 * it can send 2 msg in pipe during 1 run. 
 */
int GetRecvLengthForBigMsg(SOCKET s)
{
  int ioctlCmd;                         /* command to ioctl to check data */
  unsigned int ioctlArg;                /* argument to get datasize */
  fd_set read_fds;                      /* read fd_set to check in read */
  struct timeval tv;                    /* timevalue for connect timeout */
  int   peekLength;             /* peek at first 4 bytes */
  int   peekLengthN;            /* peek at first 4 bytes */

  /* check to see if descriptor is ready for reading */
  FD_ZERO(&read_fds);                   /* Clear the fd_set */
  FD_SET(s, &read_fds);
  while (1){ 
   errno=0;
   if ( select(s+1,FIX_IT &read_fds, NULL, NULL, NULL) == SOCKET_ERROR) {
     if (errno!=EINTR){
       ErrorLog(EXIT, WinSockError());
       break;
     } else {
        continue;
     } 
   }
   break;
  } 
  if (! FD_ISSET(s, &read_fds)) {
    return (0);
  } 

  if (read(s,&peekLengthN, 4) != 4 ) {
    ErrorLog(0,WinSockError());
  }

  ioctlCmd = FIONREAD;
  ioctlArg = 0;
  if ( ioctlsocket(s,ioctlCmd, &ioctlArg) == SOCKET_ERROR) {
    ErrorLog(EXIT, WinSockError());
  }
  if (ioctlArg == 0 ) {
    return (ioctlArg);
  }
  peekLength = ntohl(peekLengthN)-sizeof(int);
  if (peekLength == 0) {
    return -1;
  }
  else {
    return (peekLength);
  }
}

