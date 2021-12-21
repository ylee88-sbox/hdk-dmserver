/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1997   Intel Corporation. All Rights Reserved.             */
/*   Program: opusp2p.c                                                       */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Sunil Bhasin          sbhasin@scdt.intel.com           5-6677            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1997/04/03                                                         */
/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <p2pitc/opusp2p.h>

/* 
** BuildOpusMsg builds a message for transmission 
*/
int BuildOpusMsg(OpusMsg *msg, char *fnString, char *argString)
{
	int length;

	strcpy(msg->fnString,fnString);
	strcpy(msg->argString,argString);
	length = sizeof(int)+S_STR_SIZE+strlen(argString)+1;
	msg->length = ntohl(length);

	return length;
}

/*
** RegisterFunction - takes in a pointer to old func list - first time use NULL
** string - a string identifier
** function - function pointer
** return value - pointer to new func list 
*/
funcListType * RegisterFunction(funcListType *funcList, char *string, void (*function)(char *) )
{
	funcListType *funcNode;

	funcNode = (funcListType *)malloc(sizeof (funcListType) );
	/* Register the name etc */
	funcNode->function = function;
	strcpy(funcNode->string, string);
	funcNode->next = funcList;
	return funcNode;
}

/* 
** Executes the function corresponding to the opus msg 
*/
void ExecuteFunction(funcListType *funcList, OpusMsg *msg)
{
	funcListType *funcNode;

	funcNode = funcList;
	while (funcNode != NULL) {
		if (!strcmp(msg->fnString, funcNode->string) ) {
			funcNode->function(msg->argString);
			return;
		}
		funcNode = funcNode->next;
	}
	fprintf(stderr, "function <%s> not found\n", msg->fnString);
}
