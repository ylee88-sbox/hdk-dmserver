/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: File to hold general purpose procedures not belonging in the    */
/*            other modules !!                                                */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: interutls.c,v 1.1.1.1 1997/02/20 19:17:22 vipulX Exp $ */
/* $Log: interutls.c,v $
 * Revision 1.1.1.1  1997/02/20  19:17:22  vipulX
 * DMServer
 * */
#include <string.h>
#include "interutls.h"

extern inter_msg interMsg;

static char *interBadArg = "Incorrect number of arguments for procedure";

int 
interCheckArg(int input, int output) {

    if ((interMsg.num_outputargs != output) || (interMsg.num_inputargs != input)) {
 	interMsg.num_outputargs = 0;
	interMsg.status = intersBadparameter;
	interMsg.stString = (int)interBadArg;
	return(intersError);
    }
    return(intersSuccess);
}

void 
interPutInfoInMsg(DMSINFO *info, int startArg) {

    interMsg.arguments[startArg].type = INTER_INT_TYPE;
    interMsg.arguments[startArg++].val.intArg = info->location;
    interMsg.arguments[startArg].type = INTER_STRING_TYPE;
    interMsg.arguments[startArg++].val.stArg = info->libPath;
    interMsg.arguments[startArg].type = INTER_STRING_TYPE;
    interMsg.arguments[startArg++].val.stArg = info->libName;
    interMsg.arguments[startArg].type = INTER_STRING_TYPE;
    interMsg.arguments[startArg++].val.stArg = info->cellName;
    interMsg.arguments[startArg].type = INTER_STRING_TYPE;
    interMsg.arguments[startArg++].val.stArg = info->viewName;
    interMsg.arguments[startArg].type = INTER_STRING_TYPE;
    interMsg.arguments[startArg++].val.stArg = info->verName;
    interMsg.arguments[startArg].type = INTER_STRING_TYPE;
    interMsg.arguments[startArg++].val.stArg = info->coUser;
    interMsg.arguments[startArg].type = INTER_STRING_TYPE;
    interMsg.arguments[startArg++].val.stArg = info->fullPathName;
}

void
iDMS_reset_theLibName() {
	strcpy(theLibName, "*");	/* Reinitialize this INTERS variable */
}
