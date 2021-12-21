/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: interp2p.h                                                      */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: interp2p.h,v 1.2 1997/04/08 21:26:39 vipulX Exp $ */
/* $Log: interp2p.h,v $
 * Revision 1.2  1997/04/08  21:26:39  vipulX
 * Changes for removing dmserver.h dependency
 *
 * Revision 1.1.1.1  1997/02/20  19:17:57  vipulX
 * P2PITC
 * */

#ifndef _INTERP2P_H
#define _INTERP2P_H

#include "p2p.h"

/* DMSINFO definition for dmsCInfoCellView() */
#define INTER_UNDEF_TYPE (-1)
#define INTER_STRING_TYPE 1
#define INTER_INT_TYPE 2
#define INTER_BYTE_TYPE 3

# define intersError                           107

typedef struct dmsInfo {
  int  location; /* 0: not exist 1: workdir 2: unixdir 3: RW 4: RO 5: create */
  char libPath[1024];
  char libName[1024];
  char cellName[1024];
  char viewName[1024];
  char verName[1024];
  char coUser[1024];
  char fullPathName[1024];
} DMSINFO;

#define INTERS_MAXARGS 20
#define SENDER_LENGTH 256
typedef struct {
  int type;
  union {
    int intArg;
    char *  stArg;
  } val;
} interArg;

typedef struct {
  int type;
  union {
    int intArg;
    int  stArg;
  } val;
} interArg64;
typedef struct {
  /* admin stuff */
  int valid;
  int status;
  int stString;
  
  /* the message stuff */
  int num_inputargs;
  int num_outputargs;
  interArg arguments[INTERS_MAXARGS];
  
  /* handle to the global message */
  char senderId[SENDER_LENGTH];
} inter_msg;

typedef struct {
  /* admin stuff */
  int valid;
  int status;
  int stString;
  
  /* the message stuff */
  int num_inputargs;
  int num_outputargs;
  interArg64 arguments[INTERS_MAXARGS];
  
  /* handle to the global message */
  char senderId[SENDER_LENGTH];
} inter_msg64;

#define P2P_LENGTH (256*2000)
#define INTER_MSG_SIZE sizeof(inter_msg)

typedef struct {
  int length;
#ifdef __LP64__
  inter_msg64 interMsg;
#else
  inter_msg   interMsg;
#endif
  char p2pStrings[P2P_LENGTH];
} p2p_msg;

void interCommunicate(char *caller);
void interPackageInfo();
void interSet_N_StrArg(int numInput, int numOutput, int opNum, ...);
int ExtractMsg( p2p_msg *msgBuffer, inter_msg *interMsg);
int BuildMsg(p2p_msg *msgBuffer, inter_msg *interMsg);

#endif /* _INTERP2P_H */
