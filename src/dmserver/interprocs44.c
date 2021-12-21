
/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: All the handler procedures. Each procedure essentially executes */
/*            the inters call with the appropriate arguments from the global  */
/*            structure and writes return values back to the global structure */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: interprocs.c,v 1.1.1.1 1997/02/20 19:17:22 vipulX Exp $ */
/* $Log: interprocs.c,v $
 * Revision 1.1.1.1  1997/02/20  19:17:22  vipulX
 * DMServer
 * */

#include "dmserver.h"
#include "interprocs.h"
#include "interutls.h"
#include "hashUtils.h"
#include "../p2pitc/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

extern inter_msg interMsg;
extern char *senderId;
static char *interBadArg = "Incorrect Number of Arguments";
char inter_General_Buffer[INTERS_TOOL_LENGTH];
extern int sendBigMsg;

/* begin : added on 11/13/96 for removing warnings */
extern char * dmsCGetPolicyViewList(char *tool, char *policy);
extern char * dmsCGetSearchPath(void);
extern char * dmsCPrintPath(char *vName);
extern char * dmsCVersionNumber(void);
/* end : added on 11/13/96 for removing warnings */

dm_CALLBACK interCallbacks[INTERS_NUM_COMMANDS] = {
	checkOut, 		/* ITC_CHECKOUT 	0  */
	checkIn,   		/* ITC_CHECKIN  	1  */
	setAttribute,		/* SET_ATTRIBUTE 	2  */
 	getAttribute,		/* GET_ATTRIBUTE 	3  */
	deleteAttribute,        /* DELETE_ATTRIBUTE 	4  */
	setSearchPath,          /* SET_SEARCH_PATH  	5  */
	setToolName, 	        /* SET_TOOL_NAME	6  */
	reparse, 		/* REPARSE  		7  */
	version,		/* GET_VERSION          8  */
	cancelCheckout,		/* CANCEL_CHECKOUT      9  */
	intersInit,             /* PING                 10 */
	getWorkingDir,          /* GET_WORKDIR 		11 */
	createCellView, 	/* CREATE_CELLVIEW  	12 */
	createFile,             /* CREATE_FILE      	13 */
	readOnlyCVType, 	/* READONLY_CV_TYPE  	14 */
	load,			/* LOAD 		15 */
	edit,			/* EDIT  		16 */
	readEdit,		/* READorEDIT 		17 */
	save,                   /* SAVE  		18 */
	infoCellView,		/* INFO_CELLVIEW      	19 */
	getWorkingLib,		/* GET_WORKLIB   	20 */
	purge,			/* PURGE 		21 */
	report, 		/* REPORT 		22 */
	printpath, 		/* PRINTPATH 		23 */
	readOnlyCV,		/* READONLY  		24 */
	lastInfo,		/* LASTINFO  		25 */
	install, 		/* ITC_INSTALL 		26 */
	update,                 /* UPDATE      		27 */
	getSearchPath,		/* GET_SEARCH_PATH 	28 */
	getPolicyVwList,	/* GET_POLICY_VWLIST 	29 */
	intersExit,		/* QUIT    		30 */
	setAttributeList,	/* SET_ATTRIBUTE_LIST   31 */ 
	getAllAttributes,	/* GET_ALL_ATTRIBUTES   32 */ 
	setMultipleViews,	/* SET_MULTIPLE_VIEWS   33 */ 
	getMultipleViews,	/* GET_MULTIPLE_VIEWS   34 */ 
	getToolName,	        /* GET_TOOL_NAME        35 */ 
	printIdmsLog,           /* PRINT_INTERS_LOG     36 */ 
	getLibraryList,         /* GET_LIBRARY_LIST     37 */
	getCheckoutVersion ,    /* GET_CHECKOUT_VERSION 38 */
	getCellList,		/* GET_CELL_LIST        39 */
	getViewList,		/* GET_VIEW_LIST        40 */
	getViewCellViewList,    /* GET_VIEWCELLVIEW_LIST    41 */
	getCellCellViewList,    /* GET_CELLCELLVIEW_LIST    42 */
	getVersionList,         /* GET_VERSION_LIST         43 */
	getExtensionTableList,  /* GET_EXTENSIONTABLE_LIST  44 */
	delete,                 /* DELETE                   45 */
        getCdslibName,          /* GETCDSLIBNAME            46 */
	getLibPath,	       	/* GET_LIB_PATH		    47 */
        getCellViewsByLib,      /* GET_CELLVIEWS_BYLIB      48 */
        getCellViewsForCache,   /* GET_CELLVIEWS_FORCACHE   49 */
        getAllCellViews         /* GET_CELLVIEWS_BYLIB      50 */
};
#if 0
/* -------------------------- BEGIN : AFS support -------------------------*/
/* The code for AFS support has been taken from the MTEX sources (Ty Tang)
 * originally written by Johny Srouji
 */

/* 
 * Name             : ascii2Token 
 * Call Assumptions : 
 * Description      : Converts a stream of bytes into an array of AFS token
 * Parameters       : <tokenBuffer> - Bytes buffer containing the ASCII
 *                                    format of the AFS token
 *                    <tokenRecord> - AFS tokens constructed from byte stream
 * Returns          : N/A
 * Global Data      : 
 * Algorithm        :
 */
void
ascii2Token(char *tokenBuffer, union afs_token_record *tokenRecord)
{
    char tmp[10240];      /* temporary buffer */
    int  index;         /* counter var.     */
    
    for( tmp[2]='\0', index=0; index<(2*sizeof(struct ktc_principal)) 
             && *tokenBuffer != '\0'; index++, tokenBuffer += 2 ) {
      tmp[0] = *tokenBuffer;
        tmp[1] = *(tokenBuffer + 1);
        tokenRecord->ascii_tkt[index] = (int)strtoul(tmp, NULL, 16);
    } /* for */
    
        /* convert token */
    memcpy(tmp, tokenBuffer, 20);
    tokenBuffer += 20;
    tmp[20] = '\0';
    tokenRecord->token.afstoken.startTime = (int)strtoul(tmp, NULL, 16);
    
    memcpy(tmp, tokenBuffer, 20);
    tokenBuffer += 20;
    tmp[20] = '\0';
    tokenRecord->token.afstoken.endTime = (int)strtoul(tmp, NULL, 16);
  
    for( tmp[2]='\0', index=0; index<sizeof(struct ktc_encryptionKey) 
             && *tokenBuffer!='\0'; index++, tokenBuffer += 2 ) {
        tmp[0] = *tokenBuffer;
        tmp[1] = *(tokenBuffer + 1);
        tokenRecord->token.afstoken.sessionKey.data[index] = (int)strtoul(tmp, NULL, 16);
    } /* for */
  
    memcpy(tmp, tokenBuffer, 20);
    tokenBuffer += 20;
    tmp[20] = '\0';
    tokenRecord->token.afstoken.kvno = (int)strtoul(tmp, NULL, 16);
    
    memcpy(tmp, tokenBuffer, 20);
    tokenBuffer += 20;
    tmp[20] = '\0';
    tokenRecord->token.afstoken.ticketLen = (int)strtoul(tmp, NULL, 16);
  
    for( tmp[2]='\0', index=0; index<MAXKTCTICKETLEN && *tokenBuffer!='\0'; 
         tokenBuffer += 2, index++ ) {
        tmp[0] = *tokenBuffer;
        tmp[1] = *(tokenBuffer + 1);
        tokenRecord->token.afstoken.ticket[index] = (int)strtoul(tmp, NULL, 16);
    } /* for */
}   

/* 
 * Name             : tokensToKernel
 * Call Assumptions : 
 * Description      : Converts the ASCII token string to AFS tokens and pushes 
 *                    the tokens into the kernel.
 * Parameters       : <tokenBuffer> - ASCII byte stream containing the user's
 *                    AFS tokens.
 * Returns          : N/A
 * Global Data      : 
 * Algorithm        :
 */
void
tokensToKernel(char *tokenBuffer)
{
    union afs_token_record tokenRecord; /* Temp struct for ktc_SetToken() */
    
#ifndef _WIN32
    setpag();
#endif
    
    for( ; *tokenBuffer != '\0'; tokenBuffer += TKTLEN ) {
            /* Return -1 if the length of tokenString is NOT a multiple of TKTLEN */
        if ( strlen(tokenBuffer) < TKTLEN ) {
	    /*
            Debug(0, "tokensToKernel: -E- malformed AFS token:\n%s", 
                  tokenBuffer);
		  */
            return;
        } /* if */
    
        /*Debug(2, "tokensToKernel: -I- AFS token is \n%s", tokenBuffer);
	*/
    
            /* Convert the token string into AFS tokens */
        ascii2Token(tokenBuffer, &tokenRecord);
        
        if ( ktc_SetToken(&tokenRecord.token.service, 
#ifdef _WIN32
                          &tokenRecord.token.client, &tokenRecord.token.afstoken, 0) )
#else 
                          &tokenRecord.token.afstoken, &tokenRecord.token.client, 0) )
#endif
        {
	    /*
            Debug(1, "tokensToKernel: ktc_SetToken() fails (%d)", errno);
	    */
        }
    } /* for */

    return; 
}
#endif
/* -------------------------- END-- : AFS support -------------------------*/
void hash_hashSetReportMsg() {

  if(!hashSetReportMsg(senderId))
     printf("hashERROR: Report Msg not set for client: %s\n",senderId);
}

void hash_hashSetLastCellViewInfo() {

  if(!hashSetLastCellViewInfo(senderId))
     printf("hashERROR: Last CellView info not set for client: %s\n",senderId);
}

void hash_hashSetReportMsgAndLastCVInfo() {
   
  (void) hash_hashSetReportMsg();
  (void) hash_hashSetLastCellViewInfo();
}

void lastInfo() {
  DMSINFO *info;

  if (interCheckArg(1,8) != intersSuccess)
	return;
  interMsg.status = hashGetLastCellViewInfo(senderId,&info);
  (void) interPutInfoInMsg(info,2);
}

void getPolicyVwList() {
  char *list;

  if (interCheckArg(3,1) != intersSuccess)
	return;
  list = dmsCGetPolicyViewList(interMsg.arguments[2].val.stArg,interMsg.arguments[3].val.stArg); 
  interMsg.status = (list ? intersSuccess : intersError);
  interMsg.arguments[4].type = INTER_STRING_TYPE;
  interMsg.arguments[4].val.stArg = list;
}
	
void getSearchPath() {
  char *path;
  
  if (interCheckArg(1,1) != intersSuccess)
	return;
  path = dmsCGetSearchPath();
  interMsg.status = (path ? intersSuccess : intersError);
  interMsg.arguments[2].type = INTER_STRING_TYPE;
  interMsg.arguments[2].val.stArg = path;
}

void getWorkingDir() {
  char *workingdir=NULL;

  if (interCheckArg(2,1) != intersSuccess)
	return;
  interMsg.status=dmsCGetWorkingDir(interMsg.arguments[2].val.stArg, &workingdir); 
  interMsg.arguments[3].type = INTER_STRING_TYPE;
  interMsg.arguments[3].val.stArg = workingdir;

  (void) hash_hashSetReportMsg();
}

void getWorkingLib() {
  char *workinglib=NULL;

  if (interCheckArg(2,1) != intersSuccess)
	return;
  interMsg.status=dmsCGetWorkingLib(interMsg.arguments[2].val.stArg, &workinglib); 
  interMsg.arguments[3].type = INTER_STRING_TYPE;
  interMsg.arguments[3].val.stArg = workinglib;

  (void) hash_hashSetReportMsg();
}

/****  This code is unsafe for the environment when host type is LINUX.
void modifyVars(void) {
        char **a= environ;
        char *value;
        char *newvalue;

        while (*a){
                * Replace for HOSTTYPE & SETUP_HOSTYPE *
                if ( strstr(*a,"HOSTTYPE") || strstr(*a,"SETUP_HOSTYPE") ) {
                        value = strstr(*a,"=");
                        strcpy(value+1,"nt_4.0");
                }
                else if ( strstr(*a,"CAD_UTILS") ) {
                        value = NULL;
                        * Modify the value if hp700_ux100 appears *
                        value = strstr(*a,"hp700_ux100");
                        if ( value ) {
                                newvalue=value;
                                strncpy(newvalue,"nt_4.0",6);
                                newvalue+=6;
                                value+=strlen("hp700_ux100");
                                while(*value) *newvalue++=*value++;
                                *newvalue='\0';
                        }

                }
                a++;
        }
}
*****/

void intersInit() {
  struct passwd *pwEntry;
  FILE *fp;
  char line[10240], line2[10240];
  char *start,*first,*next;
  char *dollar, *slash;
  char *value;
  int length;
  int equal;
  int i;
  char *tempstore;
  struct stat statbuf;
  int len1;
  char setupfile[256];	/* adding setup filename 1.0.a7 */
  FILE *sffp;		/* setup file file pointer */

  static char dmspath[255];

  /* Added chdir() on 1/13/97 */
  /* added dmspath on 6/25/97 -- on Sunil/Balaji's rec. */
  /* Agruments got here are 2 - username */
  /*                        3 - unixDir  */
  /*                        4 - setupfil */
  /*                        5 - appName  */
  /*                        6 - dmspath  */
  /*                        7 - afsTokens*/
  /* arg 5 will be handled in dmserver.c */

  /* 1.0.a7 - 6/5/98 - Changed the working model a lil bit -- now we check
   * to see if a file called $DMSPATH.setupenv exists and if it does - then
   * after all the variables have been sourced --- source this file - the 
   * format for this file var=value - so we can do a straight putenv.
   * This file will be generated by Unified setup dmsCompiler_new.pl
   */

  pwEntry = getpwnam(interMsg.arguments[2].val.stArg);
  /* fprintf(stderr,"The uid is %d\n",pwEntry->pw_uid); */
  /* we don't want anybody to be running as the big guy */
  if ( geteuid() == 0) {
	pwEntry = getpwnam("nobody");
	if (pwEntry != NULL)
	  setuid(pwEntry->pw_uid);
	else {
	  ErrorLog(EXIT,"This server will never run as root\n");
          exit(1); 
        }

  }
  if((geteuid()==0)) 
     exit(-1);
  /* fprintf(stderr,"The euid now is %d\n",geteuid()); */

  /* 1.0.a8 - 7/13/98 - Added support for AFS tokens */
  if (interMsg.arguments[7].val.stArg && interMsg.arguments[7].val.stArg[0]) {
     //tokensToKernel(interMsg.arguments[7].val.stArg);
   }

  /* 1.0.a8 END - 7/13/98 - Added support for AFS tokens */

  chdir(interMsg.arguments[3].val.stArg);

  /* parse the dms.pth file and do a setenv()/putenv() for args */
  stat(interMsg.arguments[4].val.stArg, &statbuf);
  fp = fopen(interMsg.arguments[4].val.stArg,"r");
  if (fp != NULL) {
	while (statbuf.st_size > ftell(fp)) {
		fscanf(fp,"%[^\n]",line);
		length=strlen(line);
		/* fseek(fp,length,SEEK_CUR); */
		fseek(fp,1,SEEK_CUR);
		/* Get lines & discard lines not having setenv statements */
		/* lines are of the form setenv name value */
		start=strstr(line,"setenv");
		if ( start != NULL ) {
			/* Now we have a value to be set in the process, env */
			/* move to end of setenv and before name */
			equal = 0; start += 6;
			while ( (*start == ' ')||(*start == '\t') )
				start++;
			first=next=start;
			while(*next) {
				if ( (*next == ' ')||(*next == '\t') ){
					if (equal == 0){
						*first='=';equal=1;
						first++;
					} /* if -equal */
				next++;
				} /* if - next */
				else {
					*first = *next; first++; next++;
				}
			} /* while - next */
			*first = '\0';
			/* Adding for replacements of $ */
			while(dollar=strchr(start,'$')) {
				for (slash=dollar; *slash; slash++) {
					if (*slash == '/') {
						*slash='\0';
						break;
					} /* if -slash */
				} /* for - slash */
				len1=dollar-start;
				strncpy(line2,start,len1);
				dollar++;
				value=getenv(dollar);
				sprintf(line2+len1,"%s/%s",value,slash+1);
				strcpy(start,line2);
			}
			/* End adding for replacement of dollar */
			/* allocate memory for this and set in environ */
			tempstore=strdup(start);
			/* fprintf(stderr,"Putting string %s\n",tempstore);*/
			putenv(tempstore);
			tempstore=NULL;
		} /* if - start */
	} /* while eof */
    fclose(fp);
  } /* if - fp */
  /* fclose(fp); */
  /* added the following lines to put dmspath in the environment */
  sprintf(dmspath,"DMSPATH=%s",interMsg.arguments[6].val.stArg);
  putenv(dmspath);

  /* 1.0.a7 6/5/98 - Additions begin */
  /* Create the new setup source filename */
  sprintf(setupfile,"%ssetupenv",interMsg.arguments[6].val.stArg);
  stat(setupfile, &statbuf);
  sffp=fopen(setupfile,"r");
  if ( sffp != NULL ) {
	while (statbuf.st_size > ftell(sffp)) {
		fscanf(sffp, "%[^\n]",line);
		fseek(sffp,1,SEEK_CUR);
		length = strlen(line);
		tempstore=strdup(line);
		putenv(tempstore);
	}
  	fclose(sffp);
  }
  /* 1.0.a7 6/5/98 - Additions end */

  /* 1.0.a8 7/14/98 - Additions start */
  /* We need to change the hp700_ux100 to nt_4.0 */
  /* modifyVars(); */
  /* 1.0.a8 7/14/98 - Additions end */

  if(hashCreate(senderId))
     interMsg.status=intersSuccess;
  else {
     interMsg.status=intersNotInit; 
     printf("hashERROR: Could not ceate hash NODE for client: %s\n",senderId);
  }
}

void intersExit() {
  if(hashDelete(senderId))
     interMsg.status=intersSuccess;
  else {
     interMsg.status=intersFailure;
     printf("hashERROR: Could not delete hash NODE for client: %s\n",senderId);
  }
}

void reparse() {
  if (interCheckArg(1,0) != intersSuccess)
	return;
  interMsg.status = dmsCParsePath();

  (void) hash_hashSetReportMsg();
}

void printpath() {
  char *path=NULL;

  if (interCheckArg(2,1) != intersSuccess)
	return;
  path = dmsCPrintPath(interMsg.arguments[2].val.stArg);
  interMsg.status = (path ? intersSuccess : intersError);
  interMsg.arguments[3].type = INTER_STRING_TYPE;
  interMsg.arguments[3].val.stArg = path;

  (void) hash_hashSetReportMsg();
}

void version() {
  char *ver;

  if (interCheckArg(1,1) != intersSuccess)
	return;
  ver = dmsCVersionNumber();
  interMsg.status = (ver ? intersSuccess : intersError);
  interMsg.arguments[2].type = INTER_STRING_TYPE;
  interMsg.arguments[2].val.stArg = ver;
}

void report() {
  char *rep;

  if (interCheckArg(1,1) != intersSuccess)
	return;
  rep = hashGetReportMsg(senderId);

  interMsg.status = (rep ? intersSuccess : intersError);
  interMsg.arguments[2].type = INTER_STRING_TYPE;
  interMsg.arguments[2].val.stArg = rep;
}

void readOnlyCVType() {
  if (interCheckArg(5,2) != intersSuccess)
	return;
  interMsg.arguments[6].type=interMsg.arguments[7].type=INTER_STRING_TYPE;
  interMsg.status = dmsCReadonlyCellViewTypeByLib(
			interMsg.arguments[2].val.stArg, /* libName */
			interMsg.arguments[3].val.stArg, /* cellName */
			interMsg.arguments[4].val.stArg, /* viewType */
			interMsg.arguments[5].val.stArg, /* version */
			&interMsg.arguments[6].val.stArg, /* return vwName */
			&interMsg.arguments[7].val.stArg  /* return path */
  ); 
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void readOnlyCV(void) {
  if (interCheckArg(5,1) != intersSuccess)
	return;
  interMsg.arguments[6].type=INTER_STRING_TYPE;
  interMsg.status = dmsCReadonlyCellViewByLib(
			interMsg.arguments[2].val.stArg, /* libName */
			interMsg.arguments[3].val.stArg, /* cellName */
			interMsg.arguments[4].val.stArg, /* viewType */
			interMsg.arguments[5].val.stArg, /* version */
			&interMsg.arguments[6].val.stArg  /* return path */
   );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void setAttribute() {
  if (interCheckArg(8,0) != intersSuccess)
	return;
  interMsg.status = dmsCSetAttribute(
			interMsg.arguments[2].val.stArg, /* libName */
			interMsg.arguments[3].val.stArg, /* cellName */
			interMsg.arguments[4].val.stArg, /* viewType */
			interMsg.arguments[5].val.stArg, /* version */
			interMsg.arguments[6].val.stArg, /* attrName*/
			interMsg.arguments[7].val.stArg, /* attrtype*/
			interMsg.arguments[8].val.stArg  /* attrval*/
   );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void getAttribute() {
  if (interCheckArg(6,2) != intersSuccess)
	return;
  interMsg.arguments[7].type=interMsg.arguments[8].type=INTER_STRING_TYPE;
  interMsg.status = dmsCGetAttribute(
			interMsg.arguments[2].val.stArg, /* libName */
			interMsg.arguments[3].val.stArg, /* cellName */
			interMsg.arguments[4].val.stArg, /* viewType */
			interMsg.arguments[5].val.stArg, /* version */
			interMsg.arguments[6].val.stArg, /* attrName*/
			&interMsg.arguments[7].val.stArg, /* attrtype return*/
			&interMsg.arguments[8].val.stArg  /* attrval return */
   );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void createFile() {
  if (interCheckArg(3,1) != intersSuccess)
	return;
  interMsg.arguments[4].type=INTER_STRING_TYPE;
  interMsg.status = dmsCCreateUnixFile(
			interMsg.arguments[2].val.stArg, /* cellName */
			interMsg.arguments[3].val.stArg, /* viewName*/
			&interMsg.arguments[4].val.stArg /* path return*/
  );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void purge() {
  if (interCheckArg(3,1) != intersSuccess)
	return;
  interMsg.arguments[4].type=INTER_STRING_TYPE;
  interMsg.status = dmsCPurgeCellView(
			interMsg.arguments[2].val.stArg, /* cellName */
			interMsg.arguments[3].val.stArg, /* viewName*/
			&interMsg.arguments[4].val.stArg /* path return*/
  );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void checkOut() {
  if (interCheckArg(5,1) != intersSuccess)
	return;
  interMsg.arguments[6].type=INTER_STRING_TYPE;
  interMsg.status = dmsCCheckoutCellViewByLib(
			interMsg.arguments[2].val.stArg, /* lib */
			interMsg.arguments[3].val.stArg, /* cell */
			interMsg.arguments[4].val.stArg,/* view */
			interMsg.arguments[5].val.stArg, /* version */
			&interMsg.arguments[6].val.stArg /* path return*/
  );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void checkIn() {
  if (interCheckArg(8,1) != intersSuccess)
	return;
  interMsg.arguments[9].type=INTER_STRING_TYPE;
  interMsg.status = dmsCCheckinCellViewByLib(
			interMsg.arguments[2].val.stArg, /* lib */
			interMsg.arguments[3].val.stArg, /* cell */
			interMsg.arguments[4].val.stArg,/* view */
			interMsg.arguments[5].val.stArg, /* version */
			interMsg.arguments[6].val.stArg, /* children */
			interMsg.arguments[7].val.intArg, /* advance */
			interMsg.arguments[8].val.stArg, /* log */
			&interMsg.arguments[9].val.stArg /* path return*/
  );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void install() {
  if (interCheckArg(6,1) != intersSuccess)
	return;
  interMsg.arguments[7].type=INTER_STRING_TYPE;
  interMsg.status = dmsCInstallCellViewByLib(
			interMsg.arguments[2].val.stArg, /* lib */
			interMsg.arguments[3].val.stArg, /* cell */
			interMsg.arguments[4].val.stArg, /* view */
			interMsg.arguments[5].val.stArg, /* children */
			interMsg.arguments[6].val.stArg, /* log */
			&interMsg.arguments[7].val.stArg /* path return*/
  );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void createCellView() {
  if (interCheckArg(3,1) != intersSuccess)
	return;
  interMsg.arguments[4].type=INTER_STRING_TYPE;
  interMsg.status = dmsCCreateCellView(
			interMsg.arguments[2].val.stArg, /* cell */
			interMsg.arguments[3].val.stArg, /* view */
			&interMsg.arguments[4].val.stArg /* path return*/
  );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void edit() {
  if (interCheckArg(5,1) != intersSuccess)
	return;
  interMsg.arguments[6].type=INTER_STRING_TYPE;
  interMsg.status = dmsCEditCellViewByLib(
			interMsg.arguments[2].val.stArg, /* lib*/
			interMsg.arguments[3].val.stArg, /* cell */
			interMsg.arguments[4].val.stArg, /* view */
			interMsg.arguments[5].val.stArg, /* ver */
			&interMsg.arguments[6].val.stArg /* path return*/
  );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void readEdit() {
  if (interCheckArg(5,1) != intersSuccess)
	return;
  interMsg.arguments[6].type=INTER_STRING_TYPE;
  interMsg.status = dmsCReadOrEditByPolicyByLib(
			interMsg.arguments[2].val.stArg, /* lib */
			interMsg.arguments[3].val.stArg, /* cell */
			interMsg.arguments[4].val.stArg, /* view */
			interMsg.arguments[5].val.stArg, /* version */
			&interMsg.arguments[6].val.stArg /* path return*/
  );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void save() {
  if (interCheckArg(3,1) != intersSuccess)
	return;
  interMsg.arguments[4].type=INTER_STRING_TYPE;
  interMsg.status = dmsCSaveCellView(
			interMsg.arguments[2].val.stArg, /* cell */
			interMsg.arguments[3].val.stArg, /* view */
			&interMsg.arguments[4].val.stArg /* path return*/
  );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void load() {
  if (interCheckArg(3,1) != intersSuccess)
	return;
  interMsg.arguments[4].type=INTER_STRING_TYPE;
  interMsg.status = dmsCLoadCellView(
			interMsg.arguments[2].val.stArg, /* cell */
			interMsg.arguments[3].val.stArg, /* view */
			&interMsg.arguments[4].val.stArg /* path return*/
  );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void cancelCheckout() {
  if (interCheckArg(5,1) != intersSuccess)
	return;
  interMsg.arguments[6].type=INTER_STRING_TYPE;
  interMsg.status = dmsCCancelCheckoutCellViewByLib(
			interMsg.arguments[2].val.stArg, /* libName */
			interMsg.arguments[3].val.stArg, /* cellName */
			interMsg.arguments[4].val.stArg, /* viewName */
			interMsg.arguments[5].val.stArg, /* version */
			&interMsg.arguments[6].val.stArg /* path return*/
   );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void deleteAttribute() {
  if (interCheckArg(6,0) != intersSuccess)
	return;
  interMsg.status = dmsCDeleteAttribute(
			interMsg.arguments[2].val.stArg, /* libName */
			interMsg.arguments[3].val.stArg, /* cellName */
			interMsg.arguments[4].val.stArg, /* viewName */
			interMsg.arguments[5].val.stArg, /* version */
			interMsg.arguments[6].val.stArg  /* attrName*/
   );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void update() {
  if (interCheckArg(8,1) != intersSuccess)
	return;
  interMsg.arguments[9].type=INTER_STRING_TYPE;
  interMsg.status = dmsCUpdateCellViewByLib(
			interMsg.arguments[2].val.stArg, /* libName */
			interMsg.arguments[3].val.stArg, /* cellName */
			interMsg.arguments[4].val.stArg, /* viewName */
			interMsg.arguments[5].val.stArg, /* version */
			interMsg.arguments[6].val.stArg, /* children */
			interMsg.arguments[7].val.intArg, /* advance */
			interMsg.arguments[8].val.stArg, /* log */
			&interMsg.arguments[9].val.stArg /* path return*/
  );
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void setAttributeList() {
  if (interCheckArg(8,0) != intersSuccess)
	return;

  interMsg.status = dmsCSetAttributeList(
			interMsg.arguments[2].val.stArg, /* libNameList */
			interMsg.arguments[3].val.stArg, /* cellNameList */
			interMsg.arguments[4].val.stArg, /* viewTypeList */
			interMsg.arguments[5].val.stArg, /* versionList */
			interMsg.arguments[6].val.stArg, /* attrNameList*/
			interMsg.arguments[7].val.stArg, /* attrtypeList*/
			interMsg.arguments[8].val.stArg  /* attrvalList*/
   );
}

void getAllAttributes() {
  if (interCheckArg(5,3) != intersSuccess)
	return;

  interMsg.arguments[6].type=interMsg.arguments[7].type=INTER_STRING_TYPE;
  interMsg.arguments[8].type=INTER_STRING_TYPE;
  interMsg.status = dmsCGetAllAttributes(
			interMsg.arguments[2].val.stArg,  /* libNameList */
			interMsg.arguments[3].val.stArg,  /* cellNameList */
			interMsg.arguments[4].val.stArg,  /* viewTypeList */
			interMsg.arguments[5].val.stArg,  /* versionList */
			&interMsg.arguments[6].val.stArg, /* atNameList return*/
			&interMsg.arguments[7].val.stArg, /* atTypeList return*/
			&interMsg.arguments[8].val.stArg  /* atValList return */
   );
}

void setMultipleViews() {
  if (interCheckArg(2,0) != intersSuccess)
	return;

  interMsg.status = dmsCSetMultipleViews(
			interMsg.arguments[2].val.stArg  /* viewList */
   );
}

void getMultipleViews() {
  if (interCheckArg(1,1) != intersSuccess)
	return;

  interMsg.arguments[2].type = INTER_STRING_TYPE;
  interMsg.status = dmsCGetMultipleViews(
  			&interMsg.arguments[2].val.stArg  /* viewList */
  );
}

void infoCellView() {
  DMSINFO *info;

  if (interCheckArg(4,8) != intersSuccess)
	return;
  interMsg.status = dmsCInfoCellView(
			interMsg.arguments[2].val.intArg, /* searchLibOnly */
			interMsg.arguments[3].val.stArg,  /* cellName      */
			interMsg.arguments[4].val.stArg,  /* viewType      */
			&info				/* Info - DMSINFO*/
  );
  (void) interPutInfoInMsg(info,5);
  (void) hash_hashSetReportMsgAndLastCVInfo();
}

void setToolName() {
  if (interCheckArg(2,0) != intersSuccess)
	return;

  interMsg.status = dmsCSetToolName(
			interMsg.arguments[2].val.stArg  /* toolName */
   );
}

void getToolName() {


  if (interCheckArg(1,1) != intersSuccess)
	return;
  interMsg.arguments[2].type = INTER_STRING_TYPE;
  interMsg.status = dmsCGetToolName(inter_General_Buffer)==0; /* toolName */
  interMsg.arguments[2].val.stArg = inter_General_Buffer;
}

void printIdmsLog() {
  if (interCheckArg(2,0) != intersSuccess)
	return;

  interMsg.status = dmsCPrintIdmsLog(
			interMsg.arguments[2].val.stArg  /* logMessage */
   );
}

void setSearchPath() {
  /* No iDMS PI to implement this!!! */
}

void getLibraryList() {
if (interCheckArg(1,1) != intersSuccess)
	return;
  interMsg.arguments[2].type = INTER_STRING_TYPE;
  interMsg.status = 0;
  interMsg.arguments[2].val.stArg = dmsGenLibNameList("*","*", NULL);
}

void getCheckoutVersion() 
{
static char locker[1024]="";
static char path[1024]="";
char* checkoutVer;
int noError;
*locker=0;
*path=0;


fprintf (stderr,"Trying to get checkout version\n");	
if (interCheckArg(4,2) != intersSuccess)
	return;

/* dmsFindLib(
 		"",
		interMsg.arguments[2].val.stArg,
 		interMsg.arguments[3].val.stArg,&noError);
	  		
 if (noError == TRUE) {
i*/
 interMsg.arguments[5].type = INTER_STRING_TYPE;
 interMsg.arguments[6].type = INTER_STRING_TYPE;
  interMsg.status= dmCGetVersionAndLocker(
	  		interMsg.arguments[2].val.stArg,
	  		interMsg.arguments[3].val.stArg,
	  		interMsg.arguments[4].val.stArg,
	  		locker,path,0,&checkoutVer);
	  		
	  		interMsg.arguments[5].val.stArg = locker;
	  		interMsg.arguments[6].val.stArg = path;
	  		
/*	 }*/
}
		


void getCellList() {

  char* cellList="";
  if (interCheckArg(2,1) != intersSuccess)
	return;

 interMsg.status= dmsGenCells(interMsg.arguments[2].val.stArg,&cellList);
 interMsg.arguments[3].type = INTER_STRING_TYPE;
  interMsg.arguments[3].val.stArg = cellList;
}

void getViewList() {

  char* cellList="";
  if (interCheckArg(2,1) != intersSuccess)
	return;

 interMsg.status=dmsGenViews(interMsg.arguments[2].val.stArg,&cellList);
 interMsg.arguments[3].type = INTER_STRING_TYPE;
  interMsg.arguments[3].val.stArg = cellList;
}

void getCellCellViewList() {

  char* cellList="";
  if (interCheckArg(3,1) != intersSuccess)
	return;
    interMsg.status= dmsGenCellCellViews(interMsg.arguments[2].val.stArg,
    		    interMsg.arguments[3].val.stArg,
  		    &cellList);
    interMsg.arguments[4].type = INTER_STRING_TYPE;  
    interMsg.arguments[4].val.stArg = cellList;

  
 } 
void getViewCellViewList() {

  char* cellList="";
  if (interCheckArg(3,1) != intersSuccess)
	return;
    interMsg.status=dmsGenViewCellViews(interMsg.arguments[2].val.stArg,
    		    interMsg.arguments[3].val.stArg,
  		    &cellList);
    interMsg.arguments[4].type = INTER_STRING_TYPE;  
    interMsg.arguments[4].val.stArg = cellList;

  
 } 






void getVersionList() {

  char* cellList="";
  if (interCheckArg(4,1) != intersSuccess)
	return;
    interMsg.status=dmsGenVersions(interMsg.arguments[2].val.stArg,
    		    interMsg.arguments[3].val.stArg,
                    interMsg.arguments[4].val.stArg,
  		    &cellList);
    interMsg.arguments[5].type = INTER_STRING_TYPE;  
    interMsg.arguments[5].val.stArg = cellList;
	
  
	} 

void getExtensionTableList() {

  char* extList="";
  if (interCheckArg(3,1) != intersSuccess)
     return;
     interMsg.status=idmGetViewName(interMsg.arguments[2].val.stArg,interMsg.arguments[3].val.stArg,&extList);
     interMsg.arguments[4].type = INTER_STRING_TYPE;  
     interMsg.arguments[4].val.stArg = extList;
     
     } 


/******* For all 3 deletes: delete cell, cellview, cellversion *******/

void delete() {
  int which_delete = 0;

  if (interCheckArg(6,1) != intersSuccess)
    return;
  if(!strcmp(interMsg.arguments[2].val.stArg, "cv"))
    which_delete = 1;
  else if(!strcmp(interMsg.arguments[2].val.stArg, "c"))
    which_delete = 2;
  else if(!strcmp(interMsg.arguments[2].val.stArg, "cvv"))
    which_delete = 3;
  else 
    return; 

  if(!strcmp(interMsg.arguments[3].val.stArg,"") || !interMsg.arguments[3].val.stArg){
  interMsg.arguments[7].type=INTER_STRING_TYPE;
  interMsg.status = dmsCDeleteByLib( 
       which_delete,                    /* delete arg */ 
       "*",                             /* libName    */
       interMsg.arguments[4].val.stArg, /* cellName   */
       interMsg.arguments[5].val.stArg, /* viewName   */
       interMsg.arguments[6].val.stArg, /* verName    */
       &interMsg.arguments[7].val.stArg /* path return*/
       ); 
  } else {  
  interMsg.arguments[7].type=INTER_STRING_TYPE;
  interMsg.status = dmsCDeleteByLib( 
       which_delete,                    /* delete arg */ 
       interMsg.arguments[3].val.stArg, /* libName    */
       interMsg.arguments[4].val.stArg, /* cellName   */
       interMsg.arguments[5].val.stArg, /* viewName   */
       interMsg.arguments[6].val.stArg, /* verName    */
       &interMsg.arguments[7].val.stArg /* path return*/
       );
  }

  (void) hash_hashSetReportMsgAndLastCVInfo();
}

/***** get cds.lib fileName *****/

void getCdslibName() {

  char* fileName="";
 
  if (interCheckArg(1,1) != intersSuccess)
	return;
    interMsg.status=dmsCGetCdslibFileName(&fileName);
    interMsg.arguments[2].type = INTER_STRING_TYPE;  
    interMsg.arguments[2].val.stArg = fileName;
}

void getLibPath() {
  char* libPath="";

  if (interCheckArg(3,1) != intersSuccess)
        return;
  interMsg.status = dmsCGetLibPath(interMsg.arguments[2].val.stArg, &libPath); 
  interMsg.arguments[4].val.stArg = libPath;
  (void) hash_hashSetReportMsg();
}

void getCellViewsByLib() {
  char* outfile="";
  if (interCheckArg(3,1) != intersSuccess)
        return;
    interMsg.status=dmsGenCellViewsByLib(interMsg.arguments[2].val.stArg,
                    interMsg.arguments[3].val.stArg,
                    &outfile); 
    interMsg.arguments[4].type = INTER_STRING_TYPE;
    interMsg.arguments[4].val.stArg = outfile;

    sendBigMsg = 4;
}


void getCellViewsForCache() {
  char* outfile="";
  if (interCheckArg(4,1) != intersSuccess)
        return;
    interMsg.status=dmsGenCellViewsForCache(
                        interMsg.arguments[2].val.stArg, /* cellName */
                        interMsg.arguments[3].val.stArg, /* viewType */
                        interMsg.arguments[4].val.stArg, /* version */
                        &outfile);  /* return outfile */
    interMsg.arguments[5].type = INTER_STRING_TYPE;
    interMsg.arguments[5].val.stArg = outfile;

    sendBigMsg = 5;
}

void getAllCellViews() {
  char* outfile="";
  if (interCheckArg(3,1) != intersSuccess)
        return;
    interMsg.status=dmsGenAllCellViews(
                        interMsg.arguments[2].val.stArg, /* viewName */
                        interMsg.arguments[3].val.stArg, /* completeInfo? */
                        &outfile);  /* return outfile */
    interMsg.arguments[4].type = INTER_STRING_TYPE;
    interMsg.arguments[4].val.stArg = outfile;

    sendBigMsg = 4;
}

