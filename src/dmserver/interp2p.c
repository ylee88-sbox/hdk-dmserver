/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: interp2p.c - this file contains the conversion routines between */
/*            inter and p2p messages and some common utility routines.        */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: interp2p.c,v 1.3 1997/05/05 17:23:54 vipulX Exp $ */
/* $Log: interp2p.c,v $
 * Revision 1.3  1997/05/05  17:23:54  vipulX
 * Added fix for bug (NT4.0) in BuildMsg
 *
 * Revision 1.2  1997/04/08  21:26:38  vipulX
 * Changes for removing dmserver.h dependency
 *
 * Revision 1.1.1.1  1997/02/20  19:17:57  vipulX
 * P2PITC
 * */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include  <process.h>
#endif
#include <p2pitc/interp2p.h>
#include <p2pitc/timer.h>
#include <p2pitc/log.h>

extern SOCKET crsock;
extern SOCKET cwsock;
extern int user;

p2p_msg msgBuffer;
inter_msg interMsg;
DMSINFO globalInfo;

extern int sendBigMsg;
extern int secondMsgSize;
extern char * bigReceivedString; 
extern char * bigBufferString;

void
interSet_N_StrArg(int numInput, int numOutput, int opNum, ...)
{
	unsigned int i;
	va_list argptr;

	va_start(argptr,opNum);

	interMsg.status = -1;
	interMsg.stString = 0;
	interMsg.num_inputargs = numInput;
	interMsg.num_outputargs = numOutput;
	interMsg.arguments[1].type = INTER_INT_TYPE;
	interMsg.arguments[1].val.intArg = opNum;
	for(i=2;i<=numInput;++i) {
		interMsg.arguments[i].type = INTER_STRING_TYPE;
		interMsg.arguments[i].val.stArg = va_arg(argptr, char *);
	}
	interMsg.valid = 1;
	va_end(argptr);
}

void interPackageInfo() {
        int i=interMsg.num_inputargs+1;
        if (interMsg.status==intersError) // Added by Jose to avoid crash when the server dies
          return;
        globalInfo.location = interMsg.arguments[i++].val.intArg;
        strncpy(globalInfo.libPath,interMsg.arguments[i++].val.stArg,1023);
        strncpy(globalInfo.libName,interMsg.arguments[i++].val.stArg,1023);
        strncpy(globalInfo.cellName,interMsg.arguments[i++].val.stArg,1023);
        strncpy(globalInfo.viewName, interMsg.arguments[i++].val.stArg,1023);
        strncpy(globalInfo.verName, interMsg.arguments[i++].val.stArg,1023);
        strncpy(globalInfo.coUser, interMsg.arguments[i++].val.stArg,1023);
        strncpy(globalInfo.fullPathName, interMsg.arguments[i++].val.stArg,1023);
}

void interCommunicate(char *caller) {

	int msgLength ;
	int timeOut = TIME_OUT;
        int bigMsg = 0;

	if (interMsg.valid == 0){
		/* Signal error */
		DebugLog(caller);
		return;
	}

 	DebugLog("Calling Build msg from interComm");
	BuildMsg(&msgBuffer, &interMsg);
	DebugLog("Ret frm Build msg from interComm");
	/*        get time out -- where ? */
  	msgLength = ntohl(msgBuffer.length);
	SendMsg(cwsock, &msgBuffer, msgLength, timeOut);
	/* Need  to shut down the connection */
  	msgLength = sizeof(msgBuffer);
        if(!strcmp(caller,"dmsGenCellViewsByLib") || !strcmp(caller,"dmsGenCellViewsForCache") ||
	   !strcmp(caller,"dmsGenAllCellViews")) { 
            sendBigMsg = 1;  /* set for 1st receive of header msg */
            bigMsg = 1;
        }
	if ( ReceiveMsg(crsock, &msgBuffer, msgLength, timeOut) != P2P_SUCCESS ) {
		DebugLog("Timeout occurred in");
		DebugLog(caller);
		interMsg.status = intersError;
	}
	else {
          	if(!strcmp(caller,"dmsGenCellViewsByLib") || !strcmp(caller,"dmsGenAllCellViews")) { 
            	  sendBigMsg = 4; /* return output is 4 in argument. */
              	} else if(!strcmp(caller,"dmsGenCellViewsForCache")) {
            	  sendBigMsg = 5; /* return output is 5 in argument. */
	        }
            	DebugLog("Calling Extrct msg from interComm");
            	ExtractMsg(&msgBuffer, &interMsg);
            	DebugLog("Ret frm Extrct msg from interComm");
          	
		if(bigMsg != 0) { 
	    	  if(bigReceivedString) {
	      	    free(bigReceivedString);
	            bigReceivedString = 0;
	          }
 	          bigReceivedString = (char*)malloc(secondMsgSize*sizeof(char));

	    	  if ( ReceiveMsg(crsock, bigReceivedString, msgLength, timeOut) != P2P_SUCCESS ) {
                    DebugLog("Timeout occurred in");
                    DebugLog(caller);
                    interMsg.status = intersError;
                  } else {
	 	    if(bigReceivedString)  interMsg.arguments[sendBigMsg].val.stArg = bigReceivedString;
	            else interMsg.arguments[sendBigMsg].val.stArg = 0;
	       	  }
	          sendBigMsg = 0;
          	} 
	}
}


/* 
** Extract the various data from the message received from peer.
** For all arguments, convert to host byte order.
** For all string arguments, convert the offset to actual pointers.
** Clear inter_msg
** Copy p2p_msg.interMsg to inter_msg 
 */
int ExtractMsg( p2p_msg *msgBuffer, inter_msg *interMsg)
{
	unsigned int i;
	unsigned int allArgs;
	char *startPtr;

	/* the interMsg has its parts in NBO */
	memset(interMsg,0,sizeof(*interMsg));
#ifdef __LP64__
        interMsg->valid=msgBuffer->interMsg.valid;
        interMsg->status=msgBuffer->interMsg.status;
        interMsg->num_inputargs=msgBuffer->interMsg.num_inputargs;
        interMsg->num_outputargs=msgBuffer->interMsg.num_outputargs;
        strcpy(interMsg->senderId,msgBuffer->interMsg.senderId);
        for ( i = 0 ; i <INTERS_MAXARGS; i++) {
          interMsg->arguments[i].type=msgBuffer->interMsg.arguments[i].type;
          interMsg->arguments[i].val.intArg = msgBuffer->interMsg.arguments[i].val.intArg;
        }
        
#else        
	memcpy( interMsg,&(msgBuffer->interMsg),sizeof(*interMsg));
#endif
	/* start converting to host byte order */
	interMsg->valid = ntohl(interMsg->valid);
	interMsg->status = ntohl(interMsg->status);

	/* interMsg->stString = blah - blah */
	interMsg->num_inputargs = ntohl(interMsg->num_inputargs);
	interMsg->num_outputargs = ntohl(interMsg->num_outputargs);
	
	allArgs = interMsg->num_inputargs + interMsg->num_outputargs;
	for(i = 1; i <= allArgs; i++){
		interMsg->arguments[i].type = ntohl(interMsg->arguments[i].type);
		interMsg->arguments[i].val.intArg = 
			ntohl(interMsg->arguments[i].val.intArg);
	  if((sendBigMsg != 0) && (i == sendBigMsg)) {
	    /* for multi-(large) msg communication, extract 2nd msg size. */
	    secondMsgSize = interMsg->arguments[i].val.intArg;
	  }
	}

	/* interMsg is in Host order now; Convert all offsets to pointers */
	/* put in starting offset */
	startPtr = msgBuffer->p2pStrings;
	for ( i = 1 ; i<= allArgs; i++) {
		if (interMsg->arguments[i].type == INTER_STRING_TYPE ) {
			if ( interMsg->arguments[i].val.intArg != -1 ) {
				interMsg->arguments[i].val.stArg =
					startPtr + interMsg->arguments[i].val.intArg;
			}
			else {
				interMsg->arguments[i].val.stArg = NULL;
			}
		}
	}
	/***** Changed *****/
#if 0
	if (user == CLIENT) {
		if ( msgBuffer->interMsg.status != 0)
			interMsg->stString = msgBuffer->interMsg.stString;
	}
#endif
	/***** Changed *****/
	return P2P_SUCCESS;
}

/* 
** Builds the entire message from the various components and makes 
** it ready for sending to the "other end" . Till now we just have
** pointers to strings which are valid only locally to send across
** network - we need to put in the actual strings one way to do 
** this would be to allocate a huge buffer, put interMsg in it and put
** in the extra args after it and the pointers holding relative loc
** from start of our area; ie after interMsg
 */
int BuildMsg(p2p_msg *msgBuffer, inter_msg *interMsg)
{
   unsigned int i; /* running var */
   unsigned int np = 0;
   unsigned int tnp = 0;
   unsigned int allArgs;
    pid_t pid;
    unsigned int length;
    char hname[SENDER_LENGTH];
    char*           ms=NULL;
    unsigned int             len= 0;

	/* fix for NT 4.0 - user value not set to client properly */
#ifdef WIN32
	user = CLIENT;
#endif
	/* fill in senderId */
	if (user == CLIENT) {
  		gethostname((char *)&hname,SENDER_LENGTH );
#ifdef WIN32
 		pid = _getpid();
#else
		pid = getpid();
#endif
		sprintf((char *)&(interMsg->senderId),"%s%d",hname,pid);
	}
	memset(msgBuffer,0,sizeof(*msgBuffer));
#ifdef __LP64__
        msgBuffer->interMsg.valid=interMsg->valid;
        msgBuffer->interMsg.status=interMsg->status;
        msgBuffer->interMsg.num_inputargs=interMsg->num_inputargs;
        msgBuffer->interMsg.num_outputargs=interMsg->num_outputargs;
        strcpy(msgBuffer->interMsg.senderId,interMsg->senderId);
        for ( i = 0 ; i <INTERS_MAXARGS; i++) {
          msgBuffer->interMsg.arguments[i].type=interMsg->arguments[i].type;
          if (interMsg->arguments[i].type == INTER_STRING_TYPE ) {
            if (interMsg->arguments[i].val.stArg==NULL || interMsg->arguments[i].val.stArg=="") {
              msgBuffer->interMsg.arguments[i].val.stArg = 0;
            }
          } else {
            msgBuffer->interMsg.arguments[i].val.intArg = interMsg->arguments[i].val.intArg;
          }
        }
#else  
	memcpy(&(msgBuffer->interMsg), interMsg,sizeof(*interMsg));
#endif
	/* server man */
	/* make status string as the first guy */
	/***** Changed *****/
	/* This changes piece "fixes" all args for xmission */
	allArgs = interMsg->num_inputargs + interMsg->num_outputargs;
        
        for ( i = 1 ; i <= allArgs; i++) {
          if (msgBuffer->interMsg.arguments[i].type == INTER_INT_TYPE ) {
            msgBuffer->interMsg.arguments[i].type = htonl (INTER_INT_TYPE);
            msgBuffer->interMsg.arguments[i].val.intArg = 
              htonl(msgBuffer->interMsg.arguments[i].val.intArg);
          }
          
          /* if the arg is a string modify the stuff as above */
          else {
            /* copy the string from it's original location to nextStringPtr */
            /* checking for nulls on the way */
            if ( ( interMsg->arguments[i].val.stArg != NULL ) && 
                 ( interMsg->arguments[i].val.stArg != "" ) ) {
              if ( (user == SERVER)  || ( i <= interMsg->num_inputargs ) ) {
                tnp = np + strlen(interMsg->arguments[i].val.stArg) + 1;
	        if((sendBigMsg != 0) && (i == sendBigMsg)) {
                  bigBufferString = (char*)malloc(tnp*sizeof(char));
                  if(!bigBufferString) {
                    *interMsg->arguments[i].val.stArg=0;
                  } else {
                    strcpy(&(msgBuffer->p2pStrings[0])+np,"");
                    strcpy(bigBufferString, interMsg->arguments[i].val.stArg);
                  }

                } else if (tnp >= P2P_LENGTH) {
                  fprintf(stderr,"Message overflow: %s\n",interMsg->arguments[i].val.stArg); 
                  strcpy(&(msgBuffer->p2pStrings[0])+np,"");
                  *interMsg->arguments[i].val.stArg=0;
                } else {
                  strcpy(&(msgBuffer->p2pStrings[0])+np, interMsg->arguments[i].val.stArg);
                }
              }

	     if((sendBigMsg == i) && bigBufferString) { 
	      len=strlen(bigBufferString)+1;
	      msgBuffer->interMsg.arguments[i].val.intArg = htonl (len);
	     } else
              msgBuffer->interMsg.arguments[i].val.intArg = htonl (np);
            }
            else {
              msgBuffer->interMsg.arguments[i].val.intArg = htonl(-1);
            }
            
            /* put it's offset in the arguments.val.intArg field - in NBO */
            msgBuffer->interMsg.arguments[i].type = htonl (INTER_STRING_TYPE);
            
            /* find out length of string just copied and move nextStringPtr	*/
            /* by that amt + 1 so that nextStringPtr points to a free loc. */

            if ( (user == SERVER) || ( i <= interMsg->num_inputargs ) ) 
	      if((sendBigMsg != 0) && bigBufferString) {
		np = np + 1;	
	      } else {

#ifdef WIN32
              if ( interMsg->arguments[i].val.stArg != NULL)
                np = np + strlen(interMsg->arguments[i].val.stArg) + 1;
#else
            if ( ( interMsg->arguments[i].val.stArg == NULL) ||
                 (interMsg->arguments[i].val.stArg == "") )
              np = np + 1;
            else
              np = np + strlen(interMsg->arguments[i].val.stArg) + 1;

#endif
            }
          }
	}
        
	/* Convert what all we can to Network byte order */
	msgBuffer->interMsg.num_inputargs = htonl(interMsg->num_inputargs);
	msgBuffer->interMsg.num_outputargs = htonl(interMsg->num_outputargs);
	msgBuffer->interMsg.valid = htonl(interMsg->valid);
	msgBuffer->interMsg.status = htonl(interMsg->status);
 	/* I cal. len as 1. len field + interMsg + size of strings */
	length = sizeof (msgBuffer->length) + sizeof(msgBuffer->interMsg) + np;
 	msgBuffer->length = htonl(length);
	/* stString NOT YET DONE client status string to null */
	return P2P_SUCCESS;
}


