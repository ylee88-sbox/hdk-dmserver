/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1997   Intel Corporation. All Rights Reserved.             */
/*   Program: opusp2p.h                                                       */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Sunil Bhasin          sbhasin@scdt.intel.com           5-6677            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1997/04/03                                                         */
/*----------------------------------------------------------------------------*/
#ifndef _OPUSP2P_H
#define _OPUSP2P_H

#define S_STR_SIZE 80
#define B_STR_SIZE (1024*10)

/* structure used for registering a function */
typedef struct funcListType{
	char string[80];
	void (*function)();
	struct funcListType *next;
}funcListType; 

/* the format of the message sent across */
typedef struct OpusMsg {
	int length;
	char fnString[S_STR_SIZE];
	char argString[B_STR_SIZE];
}OpusMsg;

int BuildOpusMsg(OpusMsg *msg, char *fnString, char *argString);

funcListType * RegisterFunction(funcListType *funcList, char *string, void (*function)(char *) );

void ExecuteFunction(funcListType *funcList, OpusMsg *msg);

#endif /* _OPUSP2P_H */
