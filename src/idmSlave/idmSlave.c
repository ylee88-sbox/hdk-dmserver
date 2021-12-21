/*
############## Intel Corporation Confidential information. ############
#                                                                     #
#              Intel  confidential		                      #
#                                                                     #
# This listing is supplied under the terms of a  license  agreement   #
# with Intel Corporation and may not be copied nor disclosed except   #
# in accordance with the terms of the agreement.                      #
#                                                                     #
############## Intel Corporation Confidential information. ############

#######################################################################
# RCS information:                                                    #
# $Id: idmSlave.c,v 1.1.14.4 1998/05/08 21:46:12 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
/* 
#Revision 1.1  92/09/11  16:31:40  dhejmad
#Initial revision
#
*/

/* This is the source code for idmSlave process - 
** it serves mainly to encapsulate the idms - so as
** to make the functionality available from Opus
*/

#define voUnistd_h
#define voRandom_h
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <signal.h>
#include <setjmp.h>
#include "attr.h"
#include "fwidms.h"
#include "fwimsg.h"
extern char * idb_get_startup();
#define CHECKOUT_NAME          "Checkout"
#define CHECKIN_NAME           "Checkin"
#define SET_ATTRIBUTE_NAME     "SetAttr"
#define GET_ATTRIBUTE_NAME     "GetAttr"
#define DELETE_ATTRIBUTE_NAME  "DelAttr"
#define SET_SEARCHPATH_NAME    "SetPath"
#define SET_TOOL_ENTRY_NAME    "SetTool"
#define REPARSE_PATH_NAME      "Reparse"
#define GET_VERSION_NAME       "Version"
#define UNLOCK_NAME            "UnCheckout"
#define IS_ALIVE_NAME	       "Alive"
#define GET_WORKDIR            "WorkDir"
#define CREATE_CELLVIEW        "CreateCV"
#define CREATE_FILE            "CreateUxF"
#define READONLY_CV            "ReadCVType"
#define LOAD                   "Load"
#define EDIT                   "Edit"
#define READorEDIT             "ReadEdit"
#define SAVE                   "Save"
#define UPDATE                 "Update"
#define INSTALL_FILE           "Install"
#define INFO                   "Info"
#define GET_WORKLIB            "WorkLib"
#define PURGE                  "Purge"
#define REPORT                 "Report"
#define PRINTPATH              "PrintPath"
#define READONLY               "ReadOnly"
#define LASTINFO               "LastInfo"
#define QUIT_NAME              "Quit"
#define LIST_ATTR_LIB          "ListAttrLib"
#define CREATE_CONFIG          "CreateConfig"
#define REMOVE_CONFIG          "RemoveConfig"
#define LIST_CONFIG_LIB        "ListConfigLib"
#define LIST_CONFIG_CV         "ListConfigCV"
#define DIFF_CONFIG            "DiffConfig"
#define CREATE_CONFIG_COMMENT  "CreateConfigComment"
#define CREATE_CONFIG_INST     "CreateConfigInst"
#define EXPORT_CONFIG          "ExportConfig"
#define INSERT_CONFIG          "InsertConfig"

/* for slave to update opus directly */
#define SkillCmd 3
#define SkillRes 4
#define INPUT_STRING_SIZE 8191

extern Boolean globalAttrDisableFlag;
extern int ListAttrCode;
extern char ListAttrLibName[1024];
extern char ListAttrVersionName[1024];
extern char ListAttrLibPathName[1024];
#ifndef OH_MACH_LINUX
extern  "C" void U_STACK_TRACE(int);
#endif
/* global variable to store the input string */
char inputBuffer[INPUT_STRING_SIZE];
char rstring[8000];

/*
** Moving returnval from readOnlyCellView to global place to fix the problem
** seen while HP porting ... Shesha Feb 10 '94
*/
char returnval[2048];

/* global pointer which acts as a reference to next char to 
** be read 
*/
char *bufferPtr;
char *globalReturnPtr;

/* procedure  to get token from inputBuffer and place 
** it into a string that the procedure creates -
** deallocation happens in the calling procedure
** a sample string could be 
** ":7:SetAttr:1:#:5:cell0:5:tview:1:#:5:test1:3:str:9:opewew we"
** so the algo is simple, always start at a colon, and leave the pointer
** at a colon (ex last when it is at NULL). 
** take the int off the front - convert to ascii, allocate a string 
** and then copy the string itself.
*/
void packageString(char **s) 
{ 
    int i, length;
    char tokenLength[256];
    int tokenCounter;
	char *tempString;

    /* if the global pointer is at null flag error */
    if (bufferPtr == NULL) {
		*s = NULL;
		fprintf(stderr,"Unexpected End of Input\n"); 
		fflush(stderr);
		return;
    }
    
    /* check to see its a colon and move one byte */
    if (*bufferPtr != ':') {
		*s = NULL;
		fprintf(stderr,"Unexpected char in Input %c\n", *bufferPtr); 
		fflush(stderr);
		return;
    }
    
    bufferPtr += 1;
    
    tokenLength[0] = '\0';
    tokenCounter = 0;
    /* get the int off from the front of the string */
    while ((bufferPtr) && (*bufferPtr != ':')) {
		tokenLength[tokenCounter++] = *bufferPtr;
		bufferPtr += 1;
    }
    tokenLength[tokenCounter] = '\0';
    
    /* get the length of token,  position ptr to start, allocate
     * storage for token
     */
    length = atoi(tokenLength);
    bufferPtr += 1;
    tempString = (char *) malloc(length+1);
    
    /* get the token */
    for(i=0;i<length;++i) {
		tempString[i] = *bufferPtr;
		bufferPtr += 1;
    }
    
    tempString[length] = '\0';
    
    /* if the string is a "#" then return NULL */
    if (strcmp(tempString,"#") == 0) {
		free(tempString);
		*s = NULL;
    }
	else {
		*s = tempString;
	}
}

/* here the idea is simple - get a string from the above procedure
** and convert it to an int and pass is back
*/

void packageint(int *s) 
{ 
    char *intstring;

    packageString(&intstring);
  
    if (intstring != NULL)
	*s = atoi(intstring);
    
    free(intstring);
}

void resetListAttrVars()
{
   ListAttrCode=0;
   sprintf(ListAttrLibName,"");
   sprintf(ListAttrVersionName,"");
   sprintf(ListAttrLibPathName,"");
}

char *returnListAttrVars()
{
   if(strlen(ListAttrLibName)==0) sprintf(ListAttrLibName,"#");
   if(strlen(ListAttrVersionName)==0) sprintf(ListAttrVersionName,"#");
   if(strlen(ListAttrLibPathName)==0) sprintf(ListAttrLibPathName,"#");
   sprintf(rstring,"%s %d %s %s %s",
      "list",ListAttrCode,ListAttrLibName,
             ListAttrVersionName,ListAttrLibPathName);
   return(rstring);
}

char *setAttribute()
{
    char *libname, *cellname;
    char *viewname, *versionname;
    char *attrname, *attrtype, *attrvalue;
    int i;

    /* get each argument and package */
    packageString(&libname);
    packageString(&cellname);
    packageString(&viewname);
    packageString(&versionname);
    packageString(&attrname);
    packageString(&attrtype);
    packageString(&attrvalue);
    
    i = dmsCSetAttribute(libname,cellname,viewname,versionname,
		     attrname,attrtype,attrvalue);

    /* deallocate memory */
    free(libname);
    free(cellname);
    free(viewname);
    free(versionname);
    free(attrname);
    free(attrtype);
    free(attrvalue);

    return((i==idmsSuccess) ? "t" : "f");
}

char *getAttribute()
{
    char *libname, *cellname;
    char *viewname, *versionname;
    char *attrname, *attrtype, *attrvalue;
    
    if (globalReturnPtr != NULL)
		free(globalReturnPtr);

    globalReturnPtr = NULL;

    /* get each argument and package */
    packageString(&libname);
    packageString(&cellname);
    packageString(&viewname);
    packageString(&versionname);
    packageString(&attrname);
    
    resetListAttrVars();
    if (dmsCGetAttribute(libname,cellname,viewname,versionname,
			attrname,&attrtype,&attrvalue) == idmsSuccess) {
		globalReturnPtr = (char *)
				malloc(strlen(attrtype)+strlen(attrvalue)+5);
		sprintf(globalReturnPtr,"%s %s", attrtype, attrvalue);
    }
	else {
		if (strcmp(attrtype,"list") == 0) {
			globalReturnPtr=returnListAttrVars();
		}
	}
    
    free(libname);
    free(cellname);
    free(viewname);
    free(versionname);
    free(attrname);
    
    return((globalReturnPtr == NULL) ? "f" : globalReturnPtr);
}

char *deleteAttribute()
{
    char *libname, *cellname;
    char *viewname, *versionname;
    char *attrname;
    int i;

    /* get each argument and package */
    packageString(&libname);
    packageString(&cellname);
    packageString(&viewname);
    packageString(&versionname);
    packageString(&attrname);
    
    resetListAttrVars();
    i = dmsCDeleteAttribute(libname,cellname,viewname,versionname,attrname);
	if (i != idmsSuccess) {
		globalReturnPtr=returnListAttrVars();
	}
    
    free(libname);
    free(cellname);
    free(viewname);
    free(versionname);
    free(attrname);
    
    return((i== idmsSuccess) ? "t" : globalReturnPtr);
}

char *checkOut()
{
    char *libname, *cellname;
    char *viewname, *versionname;

    char *path;

    packageString(&libname);
    packageString(&cellname);
    packageString(&viewname);
    packageString(&versionname);

    /* if  libname is given call different idms procedure */
    if ((libname == NULL) || (strlen(libname) == 0)) {
	if (dmsCCheckoutCellView(cellname,viewname,&path) != idmsSuccess)
	    path = "f";
    }
    else {
	if (dmsCCheckoutCellViewByLib(libname,cellname,viewname,
	     versionname, &path) != idmsSuccess)
	    path = "f";
    }
    free(libname);
    free(cellname);
    free(viewname);
    free(versionname);
    
    return(path);
}
   

char *checkIn()
{
    char *libname, *cellname;
    char *viewname, *versionname;
    int advflag;
    char *logmsg;
    char *path;
    int i;

   
    packageString(&libname);
    packageString(&cellname);
    packageString(&viewname);
    packageString(&versionname);
    packageint(&advflag);
    packageString(&logmsg);
    
    /*  since path is not filled  */

    if ((libname == NULL) || (strlen(libname) == 0)) 
	i = dmsCCheckinCellView(cellname,viewname,"",advflag,
			logmsg,&path);
    else 
	i = dmsCCheckinCellViewByLib(libname,cellname,viewname,versionname,"",
				    advflag,logmsg,&path);
    
    free(libname);
    free(cellname);
    free(viewname);
    free(versionname);
    free(logmsg);
    
    return((i==idmsSuccess) ? "t" : "f");
}

/* cancel checkout - simple call */
char *cancelCheckout()
{
    char *libname, *cellname;
    char *viewname, *versionname;
    char *path;
    int i;

    packageString(&libname);
    packageString(&cellname);
    packageString(&viewname);
    packageString(&versionname);

    /* if libname is not given ask idms to do it */
    
    if ((libname == NULL) || (strlen(libname) == 0)) 
	i = dmsCCancelCheckoutCellView(cellname,viewname,&path);
    else 
	i = dmsCCancelCheckoutCellViewByLib(libname,
			   cellname,viewname,versionname,&path);
    
    free(libname);
    free(cellname);
    free(viewname);
    free(versionname);
    
    return((i == idmsSuccess) ? "t" : "f");
}

   
/* the idea here is to make sure that opus' search path
** has this process' search path in the front
*/
/********************************************************
* not necessary in 4.4			-sh 1/20/00
char *setSearchPath()
{
     char *tmp;
     char *token;
     char buffer[1024];
     int tempLength;
     int packetNum;

     if (globalReturnPtr != NULL)
	free(globalReturnPtr);

     globalReturnPtr = strdup(dmGetLibPath());
     tempLength = strlen(globalReturnPtr);

     token = globalReturnPtr;
     packetNum = 1;
     while (tempLength > 2000) {
	sprintf(buffer,"idmCollectString(list(%d \"",packetNum);
        (void) write(SkillCmd, buffer, strlen(buffer));
	(void) write(SkillCmd, token, 2000);
	token += 2000;
	while (*token != ' ') {
		(void)write(SkillCmd, token, 1);
		++token;
	}
	sprintf(buffer,"\"))");
        (void) write(SkillCmd, buffer, strlen(buffer));
	memset(buffer,0,1024);
        (void) read(SkillRes,buffer,1024);
	if (strstr(buffer, "t") == NULL) {
		free(globalReturnPtr);
     		globalReturnPtr = NULL;
     		return("f");
	}
	tempLength = strlen(token);
	++packetNum;
     }
	
     sprintf(buffer,"idmCollectString(list(%d \"",packetNum);
     (void) write(SkillCmd, buffer, strlen(buffer));
     (void) write(SkillCmd, token, strlen(token));
     sprintf(buffer,"\"))");
     (void) write(SkillCmd, buffer, strlen(buffer));
     memset(buffer,0,1024);
     (void) read(SkillRes,buffer,1024);
     if (strstr(buffer, "t") == NULL) {
	free(globalReturnPtr);
     	globalReturnPtr = NULL;
     	return("f");
     }

     free(globalReturnPtr);
     globalReturnPtr = NULL;
     return("t");
}
********************************************************/


/* simply set the toolname and cause opus to change its 
** search path
*/
char *setTool()
{
    char *toolname;
    
    packageString(&toolname);
    (void) dmsCSetToolName(toolname);
    free(toolname);

    return("t");
}

/* reparse the dms.pth file and cause opus to reset its
** search path
*/
char *reparse()
{
	char *fileName;
	int result;

	packageString(&fileName);
	/* if fileName is NULL call dmsCParsePath, 
	** else supply the FileName */
	result = (fileName ? 
		dmsCParseThePath(fileName) : 
		dmsCParsePath() );

	if (fileName) free(fileName);
    return((result==idmsSuccess) ?"t":"f");
}

char *getWorkingDir()
{
    char *workingdir;
    char *viewname;
    int i;


    packageString(&viewname);
    i = dmsCGetWorkingDir(viewname,&workingdir);
    free(viewname);
   
    return((i==idmsSuccess) ? workingdir : "f");
}

char *getWorkingLib()
{
    char *workinglib;
    char *viewname;
    int i;

    packageString(&viewname);
    i = dmsCGetWorkingLib(viewname,&workinglib);
    free(viewname);

    return((i == idmsSuccess) ? workinglib : "f");
}

char *createCellView()
{
    int i;
    char *cellname,*viewname;
    char *path;

    packageString(&cellname);
    packageString(&viewname);
    
    i = dmsCCreateCellView(cellname,viewname,&path);
    
    free(cellname);
    free(viewname);
    
    return((i==idmsSuccess) ? path : "f");
}

char *createFile()
{
    char *cellname, *viewname;
    char *path;
    int i;

    packageString(&cellname);
    packageString(&viewname);
    
    i = dmsCCreateUnixFile(cellname,viewname,&path);

    free(cellname);
    free(viewname);
    
    return((i==idmsSuccess) ? path : "f");
}

char *readOnlyCellView()
{
    int i;
    char *cellname, *viewtype;
    char *viewname, *path;

    packageString(&cellname);
    packageString(&viewtype);
    
    i = dmsCReadonlyCellViewType(cellname,viewtype,&viewname,&path);

    free(cellname);
    free(viewtype);
    
    if ( i == idmsSuccess) {
	sprintf(returnval,"%s %s",path,viewname);
	return(returnval);
    }
    return("f");
}

char *edit()
{
    char *cellname,*viewname;
    char *libname, *versionname;
    int i;
    char *path;

    packageString(&libname);
    packageString(&cellname);
    packageString(&viewname);
    packageString(&versionname);
    
    if ((libname == NULL) || (strlen(libname) == 0))
	i = dmsCEditCellView(cellname,viewname, &path);
    else 
	i = dmsCEditCellViewByLib(libname,cellname,viewname,versionname,&path);

    free(libname);
    free(cellname);
    free(viewname);
    free(versionname);

    return((i==idmsSuccess) ? path : "f");
}

char *readEdit()
{
    int i;
    char *cellname, *viewname;
    char *path;
    
    packageString(&cellname);
    packageString(&viewname);
    
    i = dmsCReadOrEditByPolicy(cellname,viewname,&path);

    free(cellname);
    free(viewname);

    return((i==idmsSuccess) ? path : "f");
}

char *save()
{
    int i;
    char *cellname, *viewname;
    char *path;
    
    packageString(&cellname);
    packageString(&viewname);
    
    i = dmsCSaveCellView(cellname,viewname,&path);

    free(cellname);
    free(viewname);
    
    return((i==idmsSuccess) ? path : "f");
}

char *load()
{
    int i;
    char *cellname, *viewname;
    char *path;
    
    packageString(&cellname);
    packageString(&viewname);
    
    i = dmsCLoadCellView(cellname,viewname,&path);

    free(cellname);
    free(viewname);
    
    return((i==idmsSuccess) ? path : "f");
}

char *update()
{
    char *libname;
    char *cellname, *viewname;
    char *log, *path;
    int adv;
    int i;

    packageString(&libname);
    packageString(&cellname);
    packageString(&viewname);
    packageint(&adv);
    packageString(&log);
   
    if ((libname == NULL) || (strlen(libname) == 0))
	i = dmsCUpdateCellView(cellname,viewname,"",adv,
			       log,&path);
    else 
	i = dmsCUpdateCellViewByLib(libname,cellname,viewname,"",
				    "",adv,log,&path);
    
    free(libname);
    free(cellname);
    free(viewname);
    free(log);

    return((i == idmsSuccess) ? path : "f");
}

char *info()
{
    char *cellname, *viewname;
    DMSINFO *info;
    int search;
    int retint;

    packageint(&search);
    packageString(&cellname);
    packageString(&viewname);
    
    retint = dmsCInfoCellView(search, cellname, viewname, &info);
    sprintf(rstring,"%d %s %s %s %s %s %s %d %s",
	    retint, info->libName, info->cellName, 
	    info->viewName, info->verName, info->coUser, 
	    info->fullPathName, info->location, info->libPath);

    return(rstring);
}

char *lastInfo()
{
    
    DMSINFO *info;
    int retint;

    retint = dmsCGetLastCellViewInfo(&info);
    sprintf(rstring,"%d %s %s %s %s %s %s %s", 
	    	info->location, info->libName, info->cellName, 
	    	info->viewName, info->verName, info->coUser, 
	    	info->fullPathName, info->libPath);
    return(rstring);
}

char *install()
{
    char *cellname, *viewname;
    char *log, *path;
    int i;

    packageString(&cellname);
    packageString(&viewname);
    packageString(&log);
   
    i = dmsCInstallCellView(cellname,viewname,"",log,&path);

    free(cellname);
    free(viewname);
    free(log);

    return((i==idmsSuccess) ? path : "f");
}

char *purge()
{
    char *cellname, *viewname;
    char *path;
    int i;
    
    packageString(&cellname);
    packageString(&viewname);
    
    i = dmsCPurgeCellView(cellname, viewname, &path);
    
    free(cellname);
    free(viewname);
    
    return((i==idmsSuccess) ? "t" : "f");
}

char *readOnly()
{
    char *cellname, *viewname;
    char *path;
    int i;
    
    packageString(&cellname);
    packageString(&viewname);
    
    i = dmsCReadonlyCellView(cellname, viewname, &path);
    
    free(cellname);
    free(viewname);
    
    return((i==idmsSuccess) ?  path : "f" );
}    

char *printpath()
{
	char *viewName;
	char *retVal;

	packageString(&viewName);

	retVal = dmsCPrintPath(viewName);
		
	free(viewName);

	return(retVal);
}

char *listAttrLib()
{
   char *cellName,*viewName,*versionName,*attrName;
   int op;
   int retint;
   char *at,*av;

   packageint(&op);
   packageString(&cellName);
   packageString(&viewName);
   packageString(&versionName);
   packageString(&attrName);
   
   ListAttrCode=0;
   sprintf(ListAttrLibName,"");
   sprintf(ListAttrVersionName,"");
   sprintf(ListAttrLibPathName,"");

   if(op==1) { /* set */
      retint=dmsCSetAttribute(NULL,cellName,viewName,versionName,attrName,
                              "list",NULL);
   }
   if(op==2) { /* get */
      retint=dmsCGetAttribute(NULL,cellName,viewName,versionName,attrName,
                              &at,&av);
   }
   if(op==3) { /* delete */
      retint=dmsCDeleteAttribute(NULL,
                                 cellName,viewName,versionName,attrName);
   }

   if(strlen(ListAttrLibName)==0) sprintf(ListAttrLibName,"#");
   if(strlen(ListAttrVersionName)==0) sprintf(ListAttrVersionName,"#");
   if(strlen(ListAttrLibPathName)==0) sprintf(ListAttrLibPathName,"#");
   sprintf(rstring,"%d %s %s %s",
            ListAttrCode,ListAttrLibName,
            ListAttrVersionName,ListAttrLibPathName);

   free(attrName);
   free(versionName);
   free(viewName);
   free(cellName);
   return(rstring);
}


char *createConfig()
{
    char *libName, *confName;
    int retValue;

    packageString(&libName);
    packageString(&confName);

    retValue = dmsCCreateConfig(libName, confName);

    free(libName);
    free(confName);

    return((retValue != FALSE) ? "t" : "f");
}


char *removeConfig()
{
    char *libName, *confName;
    int retValue;

    packageString(&libName);
    packageString(&confName);

    retValue = dmsCRemoveConfig(libName, confName);

    free(libName);
    free(confName);

    return((retValue != FALSE) ? "t" : "f");
}


char *listConfigCV()
{
   char *libName;
   char *cellName;
   char *viewName;
   char *versionName;
   char *configs;
   void *libID;

   packageString(&libName);
   packageString(&cellName);
   packageString(&viewName);
   packageString(&versionName);
   

   configs = dmsCGetConfigsOfCV(libName, cellName, viewName,
                 versionName);

   free(libName);
   free(cellName);
   free(viewName);
   free(versionName);

   if(configs) {
       (void)strcpy(rstring, configs);
       free(configs);
       return(rstring);
   } else {
       return(NULL);
   }
}

char *diffConfig()
{
   char *libName1;
   char *configName1;
   char *libName2;
   char *configName2;
   char *oFile;
   int retValue;

   packageString(&libName1);
   packageString(&configName1);
   packageString(&libName2);
   packageString(&configName2);
   packageString(&oFile);

   retValue = dmsDiffConfig(libName1, configName1, libName2, configName2,
                  oFile);

   free(libName1);
   free(configName1);
   free(libName2);
   free(configName2);
   free(oFile);

   return((retValue != FALSE) ? "t" : "f");
}


char *createConfigComment()
{
   char *libName;
   char *configName;
   char *comment;
   char *append;
   int  retValue;
   Boolean appendV;

   packageString(&libName);
   packageString(&configName);
   packageString(&comment);
   packageString(&append);

   appendV = append[0] == 'Y' ? TRUE : FALSE;
   retValue = dmsCCreateConfigComment(libName, configName,
                 comment, (char *)appendV);

   free(libName);
   free(configName);
   free(comment);
   free(append);

   return((retValue != FALSE) ? "t" : "f");
}


char *createConfigInst()
{
   char *libName;
   char *configName;
   char *instName;
   int  retValue;

   packageString(&libName);
   packageString(&configName);
   packageString(&instName);
   
   retValue = dmsCCreateConfigInstance(libName, configName, instName);

   free(libName);
   free(configName);
   free(instName);

   return ((retValue != FALSE) ? "t" : "f");
}


char *exportConfig()
{
   char *libName;
   char *configName;
   char *toWhere;
   int  retValue;

   packageString(&libName);
   packageString(&configName);
   packageString(&toWhere);

   retValue = dmsCExportConfig(libName, configName, toWhere);

   free(libName);
   free(configName);
   free(toWhere);

   return ((retValue != FALSE) ? "t" : "f");
}


char *insertConfig()
{
   char *libName;
   char *configName;
   char *toConfName;
   int  retValue;

   packageString(&libName);
   packageString(&configName);
   packageString(&toConfName);

   retValue = dmsCInsertConfig(libName, configName, toConfName);

   free(libName);
   free(configName);
   free(toConfName);

   return ((retValue != FALSE) ? "t" : "f");
}


char *listConfigLib()
{
   char *libName;
   char *libConfigs;

   packageString(&libName);
   
   libConfigs=dmsCListConfig(libName);

   free(libName);

   if(libConfigs) {
       (void)strcpy(rstring, libConfigs);
       free(libConfigs);
       return(rstring);
   } else {
       return(NULL);
   }
}


/* all that the main process does is login to 
** the dms and then wait in a listening loop
** every query is expected to be in the 
** form 
** 		key arguments
** the main loop strips the key and stores it,
** then based on the key hads over the rest of the 
** string to an appropriate handler 
*/

void slaveSegVHandler(int dummy)
{
FILE* fileptr = fopen("/tmp/idmSlave.stack.log","w");
int newdesc = fileno(fileptr);
   signal(SIGSEGV,SIG_IGN);
   fprintf(stderr,"SEGV in idmSlave\n");
   fprintf(stderr,"slave is aborting with exit(3)\n");
   fprintf(stdout,"slave:SEGV in idmSlave:endSlave\n");
   fflush(stdout);
   fprintf(stderr,"calling exitDMS\n");
   (void) dmsCPIExitDMS();
   fprintf(stderr,"calling exit(3)\n");
dup2(newdesc,1);
dup2(newdesc,2);
#ifndef OH_MACH_LINUX
U_STACK_TRACE(0);
#endif
fclose(fileptr);
   exit(3);
}

int main(int argc, char *argv[])
{
    char *token;
    char *returnVal;
    char buffer[50],tmpString[1024];
    char * cdslibfile;
     FILE *fileFp;
    int ct = 0, i;

    if (dmsCPIInitDMS("opus", argv[0], 2) != idmsSuccess) {
		fprintf(stderr,"Error in parsing dms.pth file \n");
		fprintf(stderr,"%s\n", dmsCReportMessage());
		fprintf(stderr,"Fix problem, kill and restart slave \n");
	}

   cdslibfile = idb_get_startup(); 
    /* tell the master that the slave is ready */
    fprintf(stdout,"slave:ready %s:endSlave",cdslibfile);
    fflush(stdout);

    signal(SIGSEGV,slaveSegVHandler);

    /* now do forever */

    for (;;) {
	if (fgets(inputBuffer,INPUT_STRING_SIZE,stdin)==NULL){
	    if (ct>=2){
		fileFp=fopen("/tmp/idmSlave.dead.log","w");
		fprintf(fileFp,"I'm exiting.\n");
		fclose(fileFp);
		exit(0);
	    }
	    else ct++;
	}
	else ct = 0;
	
	/* set the global buffer ptr to the start of input */
	bufferPtr = inputBuffer;
	packageString(&token);

	if (token == NULL) {
	    fprintf(stderr,"Empty String Received\n");
	    fflush(stderr);
	    continue;
	}

	if (strcasecmp(token,CHECKOUT_NAME) == 0) 
	    returnVal = checkOut();
	else if (strcasecmp(token,CHECKIN_NAME) == 0) 
	    returnVal = checkIn();
	else if (strcasecmp(token,SET_ATTRIBUTE_NAME) == 0)
	    returnVal = setAttribute();
	else if (strcasecmp(token,GET_ATTRIBUTE_NAME) == 0)
	    returnVal = getAttribute();
	else if (strcasecmp(token,DELETE_ATTRIBUTE_NAME) == 0)
	    returnVal = deleteAttribute();
	/***************************************************
	else if (strcasecmp(token,SET_SEARCHPATH_NAME) == 0)
	    returnVal = setSearchPath();
	***************************************************/
	else if (strcasecmp(token,SET_TOOL_ENTRY_NAME) == 0)
	    returnVal = setTool();
	else if (strcasecmp(token,REPARSE_PATH_NAME) == 0)
	    returnVal = reparse();
	else if (strcasecmp(token,UNLOCK_NAME) == 0)
	    returnVal = cancelCheckout();
	else if (strcasecmp(token,GET_VERSION_NAME) == 0)
	    returnVal = dmsCVersionNumber();
	else if (strcasecmp(token,IS_ALIVE_NAME) == 0) {
	    strcpy(buffer,"true");
	    returnVal = buffer;
	}
	else if (strcasecmp(token,GET_WORKDIR) == 0) 
	    returnVal = getWorkingDir();
	else if (strcasecmp(token,CREATE_CELLVIEW) == 0) 
	    returnVal = createCellView();
	else if (strcasecmp(token,CREATE_FILE) == 0) 
	    returnVal = createFile();
	else if (strcasecmp(token,READONLY_CV) == 0) 
	    returnVal = readOnlyCellView();
	else if (strcasecmp(token,LOAD) == 0) 
	    returnVal = load();
	else if (strcasecmp(token,EDIT) == 0) 
	    returnVal = edit();
	else if (strcasecmp(token,READorEDIT) == 0) 
	    returnVal = readEdit();
	else if (strcasecmp(token,SAVE) == 0) 
	    returnVal = save();
	else if (strcasecmp(token,UPDATE) == 0) 
	    returnVal = update();
	else if (strcasecmp(token,INSTALL_FILE) == 0) 
	    returnVal = install();
	else if (strcasecmp(token,INFO) == 0) 
	    returnVal = info();
	else if (strcasecmp(token, GET_WORKLIB) == 0) {
	    returnVal = getWorkingLib();
	}
	else if (strcasecmp(token, PURGE) == 0) {
	    returnVal = purge();
	}
	else if (strcasecmp(token, REPORT) == 0) {
	    returnVal = dmsCReportMessage();
	}
	else if (strcasecmp(token, PRINTPATH) == 0) {
	    returnVal = printpath(); 
	}
	else if (strcasecmp(token, READONLY) == 0) {
	    returnVal = readOnly();
	}
	else if (strcasecmp(token,LASTINFO) == 0) {
	    returnVal = lastInfo();
	}
	else if (strcasecmp(token,QUIT_NAME) == 0) {
	    (void) dmsCPIExitDMS();
	    exit(0);
	}
	else if (strcasecmp(token,LIST_ATTR_LIB) == 0) {
	    returnVal = listAttrLib();
	}
	else if (strcasecmp(token,CREATE_CONFIG) == 0) {
	    returnVal = createConfig();
	}
	else if (strcasecmp(token,REMOVE_CONFIG) == 0) {
	    returnVal = removeConfig();
	}
	else if (strcasecmp(token,LIST_CONFIG_LIB) == 0) {
	    returnVal = listConfigLib();
	}
	else if (strcasecmp(token,LIST_CONFIG_CV) == 0) {
	    returnVal = listConfigCV();
	}
	else if (strcasecmp(token,DIFF_CONFIG) == 0) {
	    returnVal = diffConfig();
	}
	else if (strcasecmp(token,CREATE_CONFIG_COMMENT) == 0) {
	    returnVal = createConfigComment();
	}
	else if (strcasecmp(token,CREATE_CONFIG_INST) == 0) {
	    returnVal = createConfigInst();
	}
	else if (strcasecmp(token,EXPORT_CONFIG) == 0) {
	    returnVal = exportConfig();
	}
	else if (strcasecmp(token,INSERT_CONFIG) == 0) {
	    returnVal = insertConfig();
	}
	else {
	    /* the default case */
            returnVal = "f";
	    fprintf(stderr,"Invalid Command %s \n", token);
	    fflush(stderr);
	}
	/* now write the returnVal to stdout
	fprintf(stdout,"slave:%s:endSlave\n",returnVal);
	fflush(stdout);
	*/
	/* not useful but keep it anyway */
	/* Fix for dmsSkillPrintPath not able to send long DMSPATH beyond some 4k */
	fprintf(stdout,"slave:");
	fflush(stdout);
	for (i=0;i<strlen(returnVal);i+=256) {
	  strncpy(tmpString,returnVal+i,256);
	  tmpString[256]=0;
          fprintf(stdout,"%s",tmpString);
	  fflush(stdout);
	}
	fprintf(stdout,":endSlave\n");
	fflush(stdout);
	free(token);
    }
    exit(0);
}

