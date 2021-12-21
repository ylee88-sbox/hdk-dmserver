/*
############## Intel Corporation Confidential information. ############
#                                                                     #
#              Intel  confidential		                      #
#                                                                     #
# This listing is supplied under the terms of a  license  agreement   #
# with Intel Corporation and may not be copied nor disclosed except   #
# in accordance with the terms of the agreement.                      #
#                                                                     #
############## intel Corporation Confidential information. ############

#######################################################################
# RCS information:                                                    #
# $Id: fwidmb.c,v 1.1.4.8.6.33 1997/11/14 23:04:55 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
/*
#Revision 1.34  1993/08/16  16:46:08  cchiu
#Make exist cellview take cellList and viewList
#
#Initial revision
#
*/

/* INCLUDES */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>
/* CADENCE INCLUDES */
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
#include <itkDB.h>
#endif
/* IDMS INCLUDES */
#include <idmb_variables.h>
#include <fwierr.h>
#include <fwimsg.h>
#include <dmsC.h>
#include <dmsC_new.h>
#include <idb_functions.h>
#include <dmsC_new.h>
#include <table_functions.h>
#include <os_functions.h>
#include <dmsC_new_vars.h>
#include <dmsC_status.h>
#include <idb_functions.h>
#include <errno.h>
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
#include <dmsC_Cadence_spec.h>
#else
#include <pwd.h>
#include <sys/types.h>
#endif

/* Prototypes */

#include <dmsC_parser_proto.h>
#include <idmb_internal_functions.h>
#include <dmsC_reports.h>

/*--------------------------------------------------------------------*
 * Static variables -- most of them wrongly stored in fwidms.c before *
 *--------------------------------------------------------------------*/

struct timeval tp;
struct timezone tzp;
long sec_in, usec_in, sec_out, usec_out;

extern char theToolName[BUFSIZE],idmbCmdBuffer[BUFSIZE];
extern int idmbOneLineCommandMode;
extern FILE*fptr;

/* to make several lex work together */
#undef   NLSTATE
#undef   BEGIN
#undef   ECHO
#undef   REJECT
#undef   output
#undef   input
#undef   unput
#undef   wreturn
#undef   woutput
#undef   winput
#undef   wunput

/* MACROS */
#define STDPORT stdout /* redirect outputs not nedded by opus server mode */

#define QUERYHEADER1    " \n  LIBNAME   CELLNAME   VIEWNAME  VERSION CI_OWNER  CO_USER LAST CI_TIME/CO_TIME"
#define QUERYHEADER2    "\n---------- ---------- ---------- ------- -------- -------- --------------------\n"
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
#define EXECUTEWILD(ffunction,ffile,fline,function,mode,nextLibOnList) \
  {\
    dmsFormMessage(ffunction,ffile,fline,OKTYPE,NOCVMATCH,cName,vName,toolName.name);\
    if (debugDmsOn) { \
      fprintf(stdout, "(%s)%s:%d tmpLibName:%s\n",ffunction,ffile,fline,tmpLibName);\
      fflush(stdout);\
    }\
    if (is_wild_exp(tmpLibName))\
      dmsGenLibNameList(tmpLibName,vName,libNameList);\
    else\
      strcpy(libNameList,tmpLibName);\
    dmsSeparateBySpace(libNameList,theLibName,libNameList);\
    if (debugDmsOn) { \
      fprintf(stdout, "(%s)%s:%d theLibName:%s\n",ffunction,ffile,fline,theLibName);\
      fflush(stdout);\
    }\
    while(strcmp(theLibName,"")) {\
      if (debugDmsOn) printf("Library Name: %s\n",theLibName);\
      dmsSeparateByDot(theLibName,theLibName,tmpString);\
      if (dmsLibID=idb_open_library(theLibName,NULL,mode)) {\
        dmsSetLastLibName(theLibName);\
      } else {\
        idb_capture_dm_vendor_warning();\
        if (!(dmsLibID=idb_open_library(theLibName, NULL,  "r"))) {\
	  idb_capture_dm_vendor_warning();\
	  dmsFormMessage(ffunction, ffile, fline, WARNING,CANTREADLIB,theLibName);\
	} else\
          dmsFormMessage(ffunction, ffile, fline, WARNING,CANTWRITELIB,theLibName,\
				idb_get_library_directory(dmsLibID,"r"));\
	idmbReportMsgIfAny(dmsCGetLastMessage(),"");\
	goto nextLibOnList;\
      }\
      if (!is_wild_exp(cName)&&!is_wild_exp(vName)) {\
        strcpy(tmpCell,cName);\
        strcpy(tmpView,vName);\
        strcpy(tmpCell2,cName);\
        strcpy(tmpView2,vName);\
        if (debugDmsOn) { \
         printf(" Cell Name: %s\n",cName);\
         printf(" View Name: %s\n",vName);\
	} \
        while (strcmp(tmpCell2,"")) {\
          dmsSeparateBySpace(tmpCell2,cName,tmpCell2);\
          while (strcmp(tmpView2,"")) {\
            dmsSeparateBySpace(tmpView2,vName,tmpView2);\
    	    if (!idb_find_cellview(dmsLibID,cName,vName)) {\
	      idb_capture_dm_vendor_warning();\
              break;\
	    }\
	    if (!function) {\
	      count++;\
	    }\
	    idmbReportMsgIfAny(dmsCGetLastMessage(),pp);\
          }\
          strcpy(tmpView2,tmpView);\
        }\
        strcpy(cName,tmpCell);\
        strcpy(vName,tmpView);\
      } else\
      if (is_wild_exp(cName)&&!is_wild_exp(vName)) {\
        strcpy(tmpCell,cName);\
	strcpy(tmpView,vName);\
	strcpy(tmpView2,vName);\
	while (strcmp(tmpView2,"")) {\
	  dmsSeparateBySpace(tmpView2,vName,tmpView2);\
	  if (gsPtr=ddStartGenObjRel(dmsLibID, ddObjChildRelType)) {\
	    while (ddObj = ddGenObjRel(gsPtr)) {\
	      if (ddGetObj(NULL, NULL, vName, NULL, ddObj, "r")) {\
	        strcpy(cName,ddGetObjName(ddObj));\
	        if (!dmsMatchExpression(tmpCell,cName)) continue;\
	        if (!function) {\
	          count++;\
		}\
	        idmbReportMsgIfAny(dmsCGetLastMessage(),pp);\
	      }\
	    }\
	    ddStopGen(gsPtr);\
	  } else\
	    idb_capture_dm_vendor_warning();\
	}\
	strcpy(cName,tmpCell);\
	strcpy(vName,tmpView);\
      } else if (!is_wild_exp(cName)&&is_wild_exp(vName)) {\
        strcpy(tmpCell,cName);\
	strcpy(tmpView,vName);\
        strcpy(tmpCell2,cName);\
        while (strcmp(tmpCell2,"")) {\
	  dmsSeparateBySpace(tmpCell2,cName,tmpCell2);\
	  if (cellID=ddGetObj(NULL,cName,NULL,NULL,dmsLibID,"r")) {\
	    gsPtr=ddStartGenObjRel(cellID,ddObjChildRelType);\
	    while (ddObj=ddGenObjRel(gsPtr)) {\
	      if (ddGetObj(NULL, NULL, NULL, NULL, ddObj, "r")) {\
	        strcpy(vName,ddGetObjName(ddObj));\
		if(debugDmsOn) { \
	          fprintf(fptr, "(%s)%s:%d View:%s\n", ffunction, ffile, fline, vName); fflush(fptr);\
	        } \
	        if (!dmsMatchExpression(tmpView,vName)) continue;\
	        if (!function) {\
	          count++;\
		}\
	        idmbReportMsgIfAny(dmsCGetLastMessage(),pp);\
	      }\
	    }\
	    ddStopGen(gsPtr);\
	  } else\
	    idb_capture_dm_vendor_warning();\
	}\
	strcpy(cName,tmpCell);\
	strcpy(vName,tmpView);\
      } else if (is_wild_exp(cName)&&is_wild_exp(vName)) {\
        strcpy(tmpCell,cName);\
        strcpy(tmpView,vName);\
	gsPtr=ddStartGenObjRel(dmsLibID,ddObjChildRelType);\
	while (cellID=ddGenObjRel(gsPtr)) {\
	  strcpy(cName,ddGetObjName(cellID));\
	  if(debugDmsOn) { \
	    fprintf(fptr, "(%s)%s:%d Cell:%s\n", ffunction, ffile, fline, cName); fflush(fptr);\
	  } \
	  if (!dmsMatchExpression(tmpCell,cName)) continue;\
	  gsPtr1 = ddStartGenObjRel(cellID,ddObjChildRelType);\
	  while (ddObj = ddGenObjRel(gsPtr1)) {\
	    strcpy(vName,ddGetObjName(ddObj));\
	    if(debugDmsOn) { \
	      fprintf(fptr, "(%s)%s:%d View:%s\n", ffunction, ffile, fline, vName); fflush(fptr);\
	    } \
	    if (!dmsMatchExpression(tmpView,vName)) continue;\
	    if (!function) {\
	      count++;\
	    }\
	    idmbReportMsgIfAny(dmsCGetLastMessage(),pp);\
	  }\
	  ddStopGen(gsPtr1);\
	}\
	ddStopGen(gsPtr);\
        strcpy(cName,tmpCell);\
        strcpy(vName,tmpView);\
      }\
      nextLibOnList: dmsSeparateBySpace(libNameList,theLibName,libNameList);\
    }\
    if (count==0) {\
      idmbReportMsgIfAny(dmsCGetLastMessage(),pp);\
    }\
    strcpy(outString,"O.K.");\
    dmsFormMessage(ffunction, ffile, fline, OKTYPE,TOTALWILD,count);\
  }
#else
#define EXECUTEWILD(ffunction,ffile,fline,function,mode,nextLibOnList) \
  {\
    dmsFormMessage(ffunction, ffile, fline, OKTYPE,NOCVMATCH,cName,vName,toolName.name);\
    if (debugDmsOn) { \
      fprintf(stdout, "(%s)%s:%d tmpLibName:%s\n", ffunction, ffile, fline, tmpLibName);\
      fflush(stdout);\
    }\
    if (is_wild_exp(tmpLibName))\
      dmsGenLibNameList(tmpLibName,vName,libNameList);\
    else\
      strcpy(libNameList,tmpLibName);\
    dmsSeparateBySpace(libNameList,theLibName,libNameList);\
    if (debugDmsOn) { \
      fprintf(stdout, "(%s)%s:%d theLibName:%s\n", ffunction, ffile, fline, theLibName);\
      fflush(stdout);\
    }\
    while(strcmp(theLibName,"")) {\
      if (debugDmsOn) printf("Library Name: %s\n",theLibName);\
      dmsSeparateByDot(theLibName,theLibName,tmpString);\
      if (dmsLibID=idb_open_library(theLibName, NULL, mode)) {\
        dmsSetLastLibName(theLibName);\
      } else {\
        idb_capture_dm_vendor_warning();\
        if (!(dmsLibID=idb_open_library(theLibName, NULL,  "r"))) {\
	  idb_capture_dm_vendor_warning();\
	  dmsFormMessage(ffunction, ffile, fline, WARNING,CANTREADLIB,theLibName);\
	} else\
          dmsFormMessage(ffunction, ffile, fline, WARNING,CANTWRITELIB,theLibName,\
				idb_get_library_directory(dmsLibID,"r"));\
	idmbReportMsgIfAny(dmsCGetLastMessage(),"");\
	goto nextLibOnList;\
      }\
      if (!is_wild_exp(cName)&&!is_wild_exp(vName)) {\
        strcpy(tmpCell,cName);\
        strcpy(tmpView,vName);\
        strcpy(tmpCell2,cName);\
        strcpy(tmpView2,vName);\
        if (debugDmsOn) { \
         printf(" Cell Name: %s\n",cName);\
         printf(" View Name: %s\n",vName);\
	} \
        while (strcmp(tmpCell2,"")) {\
          dmsSeparateBySpace(tmpCell2,cName,tmpCell2);\
          while (strcmp(tmpView2,"")) {\
            dmsSeparateBySpace(tmpView2,vName,tmpView2);\
    	    if (!idb_find_cellview(dmsLibID,cName,vName)) {\
	      idb_capture_dm_vendor_warning();\
              break;\
	    }\
	    if (!function) {\
	      count++;\
	    }\
	    idmbReportMsgIfAny(dmsCGetLastMessage(),pp);\
          }\
          strcpy(tmpView2,tmpView);\
        }\
        strcpy(cName,tmpCell);\
        strcpy(vName,tmpView);\
      } else\
      if (is_wild_exp(cName)&&!is_wild_exp(vName)) {\
        strcpy(tmpCell,cName);\
	strcpy(tmpView,vName);\
        tmpRetList = idb_gen_obj_rel((char*)dmsLibID, (char*)vName, 0,0);\
        while (tmpRetList && (strcmp(tmpRetList,""))) {\
          if(tmpPtr = strrchr(tmpRetList, ' '))\
            *tmpPtr = '\0';\
          else\
            *tmpRetList = '\0';\
          if(tmpPtr = strrchr(tmpRetList, ' ')) {\
            *tmpPtr++;\
            strcpy(cName,tmpPtr);\
            *tmpPtr = '\0';\
          } else {\
            strcpy(cName,tmpRetList);\
            *tmpRetList = '\0';\
          }\
          if(debugDmsOn) { \
	    fprintf(fptr, "(%s)%s:%d Cells:%s\n", ffunction, ffile, fline, cName); fflush(fptr);\
          } \
          if (!dmsMatchExpression(tmpCell,cName)) continue;\
          if (!function) {\
            count++;\
          }\
          idmbReportMsgIfAny(dmsCGetLastMessage(),pp);\
          if(!tmpRetList)\
            break;\
        }\
        strcpy(cName,tmpCell);\
	strcpy(vName,tmpView);\
      } else if (!is_wild_exp(cName)&&is_wild_exp(vName)) {\
        strcpy(tmpCell,cName);\
        strcpy(tmpView,vName);\
        tmpRetList = idb_gen_obj_rel((char*)dmsLibID, (char*)cName, 1,0);\
        while (tmpRetList && (strcmp(tmpRetList,""))) {\
          if(tmpPtr = strrchr(tmpRetList, ' '))\
            *tmpPtr = '\0';\
          else\
            *tmpRetList = '\0';\
          if(tmpPtr = strrchr(tmpRetList, ' ')) {\
            *tmpPtr++;\
            strcpy(vName,tmpPtr);\
            *tmpPtr = '\0';\
          } else {\
            strcpy(vName,tmpRetList);\
            *tmpRetList = '\0';\
          }\
          if(debugDmsOn) { \
	    fprintf(fptr, "(%s)%s:%d View:%s\n", ffunction, ffile, fline, vName); fflush(fptr);\
          } \
          if (!dmsMatchExpression(tmpView,vName)) continue;\
          if (!function) {\
            count++;\
          }\
          idmbReportMsgIfAny(dmsCGetLastMessage(),pp);\
          if(!tmpRetList)\
            break;\
        }\
        strcpy(cName,tmpCell);\
	strcpy(vName,tmpView);\
      } else if (is_wild_exp(cName)&&is_wild_exp(vName)) {\
        strcpy(tmpCell,cName);\
        strcpy(tmpView,vName);\
        tmpRetList = idb_gen_obj_rel((char*)dmsLibID, (char*)vName, 0,0);\
        while (tmpRetList && (strcmp(tmpRetList,""))) {\
          if(tmpPtr = strrchr(tmpRetList, ' '))\
            *tmpPtr = '\0';\
          else\
            *tmpRetList = '\0';\
          if(tmpPtr = strrchr(tmpRetList, ' ')) {\
            *tmpPtr++;\
            strcpy(cName,tmpPtr);\
            *tmpPtr = '\0';\
          } else {\
            strcpy(cName,tmpRetList);\
            *tmpRetList = '\0';\
          }\
          if(debugDmsOn) { \
	    fprintf(fptr, "(%s)%s:%d Cells:%s\n", ffunction, ffile, fline, cName); fflush(fptr);\
          } \
          if (!dmsMatchExpression(tmpCell,cName)) continue;\
          tmpRetList2 = idb_gen_obj_rel((char*)dmsLibID, (char*)cName, 1,0);\
          while (tmpRetList2 && (strcmp(tmpRetList2,""))) {\
            if(tmpPtr2 = strrchr(tmpRetList2, ' '))\
              *tmpPtr2 = '\0';\
            else\
              *tmpRetList2 = '\0';\
            if(tmpPtr2 = strrchr(tmpRetList2, ' ')) {\
              *tmpPtr2++;\
              strcpy(vName,tmpPtr2);\
              *tmpPtr2 = '\0';\
            } else {\
              strcpy(vName,tmpRetList2);\
              *tmpRetList2 = '\0';\
            }\
            if(debugDmsOn) { \
              fprintf(fptr, "(%s)%s:%d View:%s\n", ffunction, ffile, fline, vName); fflush(fptr);\
            } \
            if (!dmsMatchExpression(tmpView,vName)) continue;\
            if (!function) {\
              count++;\
            }\
            idmbReportMsgIfAny(dmsCGetLastMessage(),pp);\
            if(!tmpRetList2)\
              break;\
          }\
          if(!tmpRetList)\
            break;\
        }\
        strcpy(cName,tmpCell);\
        strcpy(vName,tmpView);\
      }\
      nextLibOnList: dmsSeparateBySpace(libNameList,theLibName,libNameList);\
    }\
    if (count==0) {\
      idmbReportMsgIfAny(dmsCGetLastMessage(),pp);\
    }\
    strcpy(outString,"O.K.");\
    dmsFormMessage(ffunction, ffile, fline, OKTYPE,TOTALWILD,count);\
  }
#endif

#define INPUTSETPATH(selection) \
        count=0;\
        if (!dmsGetUserInput(selection,theToolName,tmpLibName,theConfName,cName,vName,verName)) break;\
        strcpy(theLibName,tmpLibName);

#define ABORTCMD(ffunction,ffile,fline) \
        {\
	   dmsFormMessage(ffunction, ffile, fline, WARNING,BREAKBYSEMICOL);\
	   return(FALSE);\
        }

#define STOPCMD(ffunction,ffile,fline) \
        {\
	   dmsFormMessage(ffunction, ffile, fline, WARNING,BREAKBYSEMICOL);\
	   break;\
        }

/* PROCEDURES */


/* MOVED FROM fwidms.c */


int matchRWPATHLibName(char *lName, char *vName) {
  struct elem *ptr;
  char tmp1[BUFSIZE],tmp2[BUFSIZE],*p,*pp;
  int i;

  if (!strcmp(lName,"*")) return(TRUE);

  i = findPolicyEntry(rwpaths, rwpathsCount, toolName.name, vName);

  if (debugDmsOn)
    dmsPrintIdmsLog("*IDMS DEBUG* %d.rwpaths %s:\n", i+1, rwpaths[i].name);
  ptr=rwpaths[i].next;
  while (ptr) {
    if (debugDmsOn)
      dmsPrintIdmsLog("*IDMS DEBUG* \t%s %2d (%s)\n",action[ptr->opt%10],ptr->opt,ptr->dirLib);
    switch (ptr->opt%10) {
    case 1:
    case 2:
      if (!strcmp(ptr->dirLib,"")) return(TRUE);
      strcpy(tmp1,ptr->dirLib);
      p=strtok(tmp1," ");
      while (p) {
	strcpy(tmp2,p);
	p=strtok('\0'," ");
	pp=strtok(tmp2,".");
	if (!strcmp(pp,lName)) return(TRUE);
      }
      break;
    }
    ptr=ptr->next;
  }
  return(FALSE);
}

/* LOCAL FUNCTIONS */

char dmsToUpper(command)
char *command;
{
  if (command[0]>='a') command[0]=toupper(command[0]);
  return(command[0]);
}


void
getTimeIn()
{
  gettimeofday(&tp, &tzp);
  sec_in=tp.tv_sec;
  usec_in=tp.tv_usec;
}


void
getTimeOut()
{
  gettimeofday(&tp, &tzp);
  sec_out=tp.tv_sec;
  usec_out=tp.tv_usec;
  idmbPrintResult(fptr,"Execution time=%.2f seconds.\n", (sec_out - sec_in) + (usec_out - usec_in) / 1000000.0);
  idmbPrintResult(fptr,"\n");
}


int dmsReadInput(char *prompt,char *defaultVal,char *input)
{
  char ch,inputVal[BUFSIZE],tmpString[BUFSIZE];
  int  length;

  if (idmbOneLineCommandMode||idmbGraphicMode) {
    dmsSeparateByWhatever('|',idmbCmdBuffer,input,idmbCmdBuffer);
    if (!strcmp(input,"")) strcpy(input,defaultVal);
    return(TRUE);
  }

  if (strcmp(defaultVal,""))
    sprintf(tmpString,"%s (%s) ",prompt,defaultVal);
  else
    sprintf(tmpString,"%s ",prompt);
  fprintf(stdout,"%s",tmpString);

  fflush(stdout);

  ch=' ';
  length=0;
  while (ch!='\n') {
    ch=getc(stdin);
    if (feof(stdin)|| ferror(stdin)) {
      inputVal[0]='\0';
      exit(0);
    }
    
    inputVal[length]=ch;
    length++;
  }
  inputVal[length-1]='\0';

  getTimeIn();

  if (strcmp(inputVal,"")&&(inputVal[0]!=';')) {
    strcpy(input,inputVal);
  } else {
    strcpy(input,defaultVal);
    if (inputVal[0]==';')
      return(FALSE);
  }
  return(TRUE);
} /* dmsReadInput */
  

int dmsCreateAttribute(char *cName,char *vName,char *verName,
			char *pName,char *propType,char *propVal,int *noError)
{
  struct elem *ptr;
  int i;
  char *verIdPath;
  char fp[BUFSIZE];
  i = findPolicyEntry(rwpaths, rwpathsCount, toolName.name, vName);

  ptr=rwpaths[i].next;
  while (ptr) {
    switch (ptr->opt%10) {
    case 1: /* RW */
      if (!dms_find_lib_id("w", ptr->dirLib, cName, vName, noError, fp))
         break;
      verIdPath = idb_find_version_and_locker(dmsLibID, cName, vName, verName, 
						NULL, fp, (int)NULL, NULL);
      idb_capture_dm_vendor_warning();
      if (verIdPath) {
	if (!dmsCSetAttributeList(dmsGetLastLibName(),cName,vName,verName,
							pName,propType,propVal))
	  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,CREATEPROPOK,pName,dmsGetLastLibName(),cName,vName,
									verName);
	else
	  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTCRTPROP,pName,dmsGetLastLibName(),cName,vName,
									verName);
	return(0);
      }
      break;
    case 2: /* RO */
      if (!dms_find_lib_id("r", ptr->dirLib, cName, vName, noError,fp))
         break;
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,CRTPROPINROLIB,dmsGetLastLibName(),cName,vName);
      return(FALSE);
      break;
    }
    ptr=ptr->next;
  }
  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,FILENOTFOUND,cName,vName,verName,dmsConfName);
  return(dmsLastErrorNumber);
} /* dmsCreateAttribute */


int dmsShowAttribute(char *cName,char *vName,char *verName,int *noError)
{
  struct elem *ptr;
  int i;
  char *verIdPath;
  char fp[BUFSIZE];
  char nTmp[BUFSIZE],tTmp[BUFSIZE],vTmp[BUFSIZE],*nList=NULL,*tList=NULL,*vList=NULL;
  String rPath=NULL,wPath=NULL;
  i = findPolicyEntry(rwpaths, rwpathsCount, toolName.name, vName);

  ptr=rwpaths[i].next;
  while (ptr) {
    switch (ptr->opt%10) {
    case 1: /* RW */
    case 2: /* RO */
      if (!dms_find_lib_id("r", ptr->dirLib, cName, vName, noError, fp))
         break;
      verIdPath = idb_find_version_and_locker(dmsLibID, cName, vName, verName, 
						NULL, fp, (int)NULL, NULL);
      idb_capture_dm_vendor_warning();
      if (verIdPath) {
	if (!dmsCGetAllAttributes(dmsGetLastLibName(),cName,vName,verName,
							&nList,&tList,&vList)) {
	  if (nList&&tList&&vList) {
	    dmsSeparateByColon(nList,nTmp,nList);
	    while (strcmp(nTmp,"")) {
	      dmsSeparateByColon(tList,tTmp,tList);
	      dmsSeparateByColon(vList,vTmp,vList);
	      idmbPrintResult(fptr,"Property Name: [%s]\tType: [%s]\tValue: [%s]\n",nTmp,tTmp,vTmp);
	      dmsSeparateByColon(nList,nTmp,nList);
	    }
	  }
	  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,EXAMINEPROP,dmsGetLastLibName(),cName,vName,verName);
	}
	return(0);
      }
      break;
    }
    ptr=ptr->next;
  }
  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,FILENOTFOUND,cName,vName,verName,dmsConfName);
  return(dmsLastErrorNumber);
} /* dmsShowAttribute */

int dmsDeleteAttribute(char *cName,char *vName,char *verName,
						char *pName,int *noError)
{
  struct elem *ptr;
  int i;
  char *verIdPath;
  char fp[BUFSIZE];
  i = findPolicyEntry(rwpaths, rwpathsCount, toolName.name, vName);

  ptr=rwpaths[i].next;
  while (ptr) {
    switch (ptr->opt%10) {
    case 1: /* RW */
    case 2: /* RO */
      if (!dms_find_lib_id("r", ptr->dirLib, cName, vName, noError, fp))
         break;
      verIdPath = idb_find_version_and_locker(dmsLibID, cName, vName, verName, 
						NULL, fp, (int)NULL, NULL);
      idb_capture_dm_vendor_warning();
      if (verIdPath) {
	if (!dmsCDeleteAttribute(dmsGetLastLibName(),cName,vName,verName,pName))
	  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,DELETEPROP,pName,dmsGetLastLibName(),cName,vName,verName);
	else
	  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTDELPROP,pName,dmsGetLastLibName(),cName,vName,verName);
	return(0);
      }
      break;
    }
    ptr=ptr->next;
  }
  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,FILENOTFOUND,cName,vName,verName,dmsConfName);
  return(dmsLastErrorNumber);
} /* dmsDeleteAttribute */

/*********************************************************************
REM: Was being used by TEXT vTypes in 4.3.4 in dmsCFetchCellView. 
     Not being used anywhere else.			-sh 6/13/00
int dmsCopyWithHeaderLog(source,destination)
char *source;
char *destination;
{
  char tmpString[BUFSIZE],awkFile[BUFSIZE];
  FILE *fp;
  sprintf(awkFile,"%s/%s/rcsHeaderLog.awk",ircsDirectory,DMSVERSION);
  if (osFileExists(awkFile)&&(voFileLength(source)>0)) {
    if (strcmp(source,destination)) {
      sprintf(tmpString,"awk -f %s hLine=\"%s\" aLine=\"%s\" dLine=\"%s\" vLine=\"%s\" sLine=\"%s\" kLine=\"%s\" lLine=\"%s\" rLine=\"%s\" cLine=\"%s\" %s > %s"
,awkFile,rcsHeader,rcsAuthor,rcsDate,rcsVersion,rcsSource,rcsLocker,rcsLog,rcsRev,rcsComment,source,destination);
    } else {
      sprintf(tmpString,"awk -f %s hLine=\"%s\" aLine=\"%s\" dLine=\"%s\" vLine=\"%s\" sLine=\"%s\" kLine=\"%s\" lLine=\"%s\" rLine=\"%s\" cLine=\"%s\" %s > %s_tmp\nmv -f %s_tmp 
%s",awkFile,rcsHeader,rcsAuthor,rcsDate,rcsVersion,rcsSource,
rcsLocker,rcsLog,rcsRev,rcsComment,source,destination,destination,source);
    }
    return(!system(tmpString));
  } else {
    if (strcmp(source,destination))
      return(!voCopyFile(source,destination));
    else
      return(TRUE);
  }
} * dmsCopyWithHeaderLog *
*********************************************************************/



int dmsCFetchCellView(char *cName,char *vName,char *verName,char *toWhere,char **FPN)
{
  int noError;
  char tmpString[BUFSIZE],destName[BUFSIZE];
  void *cdbaLibID;
  if (!strcmp(toWhere,"")) {
    dmsGetWorkingDir(vName,dmsWorkingDir);
    sprintf(destName,"%s/%s.%s",dmsWorkingDir,cName, idmGetExtension(vName, 0));
  } else
    sprintf(destName,"%s/%s.%s",toWhere,cName, idmGetExtension(vName, 0));

  if (osFileExists(destName)) { /* rename if exist */
    sprintf(tmpString,"%s.bak",destName);
    if (osFileExists(tmpString)) { /* make old backup copy writeable */
      chmod(tmpString,0644);
    }
    sprintf(tmpString,"mv -f %s %s%s",destName,destName,".bak");
    system(tmpString);
    sprintf((char *)dmsCGetLastMessage(), "*IDMS WARNING* EXISTING FILE RENAME TO BE %s.bak",destName);
    idmbReportMsgIfAny(dmsCGetLastMessage(),"");
  }
  dmsKeepTmpCopy=TRUE;
  dmsReadCellView(0,0,DONT_CREATE,cName,vName,verName,BIN,dmsBuf,&cdbaLibID,&noError,0);
  dmsKeepTmpCopy=FALSE;
  if (noError) {
      if (!strncmp(dmsBuf,"/tmp/",5)) {
	sprintf(tmpString,"cp %s %s",dmsBuf,destName);
	noError=(!system(tmpString));
      } else {
	noError=!osCopyFile(dmsBuf,destName);
      }
    if (noError) {
      /*chmod(destName,292);*/ /* mode Fr--r--r-- */
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,FETCHOK,dmsGetLastLibName(),cName,vName,verName,destName);
    } else
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTFETCH,dmsGetLastLibName(),cName,vName,verName,destName);
  }
  *FPN=dmsBuf;
  return(dmsGenErrorCode(noError));
} /* dmsCFetchCellView */


int dmsCDiffVersions(char *cName,char *vName,char *verName1,char *verName2,char *option)
{
  int noError;
  char tmpString[BUFSIZE],FPN1[BUFSIZE],FPN2[BUFSIZE];
  void *cdbaLibID;

  /* produce copy of first version */
  dmsKeepTmpCopy=TRUE;
  dmsReadCellView(1,0,DONT_CREATE,cName,vName,verName1,BIN,FPN1,cdbaLibID,&noError,0);
  dmsKeepTmpCopy=FALSE;
  if (!noError) return(dmsGenErrorCode(noError));

  /* produce copy of second version */
  dmsKeepTmpCopy=TRUE;
  dmsReadCellView(1,0,DONT_CREATE,cName,vName,verName2,BIN,FPN2,cdbaLibID,&noError,0);
  dmsKeepTmpCopy=FALSE;
  if (!noError) return(dmsGenErrorCode(noError));

  sprintf(tmpString,"diff %s %s %s",option,FPN1,FPN2);
  system(tmpString);

  if (!strcmp(verName1,"")) strcpy(verName1,"current");
  if (!strcmp(verName2,"")) strcpy(verName2,"current");
  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,DIFFOK,dmsGetLastLibName(),cName,vName,verName1,verName2);

  return(dmsGenErrorCode(noError));
} /* dmsCDiffVersions */


int dmsGenCellViewFromFile(FILE *fp,char *pathName,char *cName,char *vName)
{
  char tmpString[BUFSIZE];
  char ch;
  int  i,j,n,flag,count;

  if (feof(fp)) return(FALSE);

  flag=0;
  j=0;
  while (flag == 0) {
    ch=getc(fp);
    n=0;
    while ((ch != '\n') && (!feof(fp))) {
      if ((isalpha(ch))||(isdigit(ch))||(ch=='.')||(ch=='_')||(ch=='/')||(ch=='-')) {
	tmpString[n]=ch;
	if (ch == '.')
	  j=n;
	n++;
      }
      ch=getc(fp);
    }
    if (feof(fp))
      flag=1;
    else {
      tmpString[n]='\0';
      strcpy(vName, &tmpString[j + 1]);
      tmpString[j]='\0';
      for (i=strlen(tmpString)-1;i>=0;i--) {
	if (tmpString[i]=='/') {
	  strcpy(cName,&tmpString[i+1]);
	  strncpy(pathName,tmpString,i+1);
	  pathName[i+1]='\0';
	  if (strlen(cName)>0&&strlen(vName)>0)
	    return(TRUE);
	  else
	    continue;
	}
      }
      strcpy(pathName,"./");
      strcpy(cName,tmpString);
      if (strlen(cName)>0&&strlen(vName)>0)
	return(TRUE);
      else
	continue;
    }
  }
  return(FALSE);
}


int dmsImportCellView()
{
  char iFileName[BUFSIZE],cName[BUFSIZE],vName[BUFSIZE],libName[BUFSIZE];
  char pathName[BUFSIZE];
  char *FPN;
  FILE *fp;
  int  count;

  count=0;
  if (!dmsReadInput("Give The Library Name:","workLib",theLibName))
    ABORTCMD(__FUNCTION__, __FILE__, __LINE__);
  if (!dmsReadInput("Give File of CellView List:","cellview.list",iFileName))
    ABORTCMD(__FUNCTION__, __FILE__, __LINE__);

  if (!(dmsLibID=idb_open_library(theLibName, NULL, "r"))) {
      idb_capture_dm_vendor_warning();
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTREADLIB,theLibName);
      return(FALSE);
  }
  if ((fp=fopen(iFileName, "r")) == NULL) {
    idmbPrintResult(fptr,"Cannot open file %s\n", iFileName);
    return(FALSE);
  }

  getTimeIn();

  while(dmsGenCellViewFromFile(fp,pathName,cName,vName)) {
    sprintf(dmsWorkingDir, "%s%s.%s", pathName, cName, vName);

    if(!dmsCInstallCellViewByLib(theLibName,cName,vName,NULL,"Imported from idmb.",&FPN)) {
      idmbPrintResult(fptr,"IMPORT CV: %s.%s FROM %s to %sO.K.\n",cName,vName,dmsWorkingDir, FPN);
      count++;
    } else {
      idmbPrintResult(fptr,"IMPORT CV: %s.%s FROM %s ERROR\n",cName,vName,dmsWorkingDir);
      idmbReportMsgIfAny(dmsCGetLastMessage(),"");
    }

  }

  fclose(fp);
  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,IMPORTCOUNT,count,iFileName,theLibName);
  return(count);
} /* dmsImportCellView */


int dmsExportCellView()
{
  char iFileName[BUFSIZE],tmpString[BUFSIZE],cName[BUFSIZE],vName[BUFSIZE],libName[BUFSIZE];
  char toWhere[BUFSIZE],pathName[BUFSIZE],FPN[BUFSIZE],*fileName;
  FILE *fp;
  int  count;

  count=0;
  if (!dmsReadInput("Give The Library Name:","workLib",theLibName))
    ABORTCMD(__FUNCTION__, __FILE__, __LINE__);
  if (!dmsReadInput("Give File of CellView List:","cellview.list",iFileName))
    ABORTCMD(__FUNCTION__, __FILE__, __LINE__);
  if (!dmsReadInput("Give the Directory to Export to:",".",FPN))
    ABORTCMD(__FUNCTION__, __FILE__, __LINE__);

  if (!(dmsLibID=idb_open_library(theLibName, NULL, "r"))) {
    idb_capture_dm_vendor_warning();
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTREADLIB,theLibName);
    return(FALSE);
  }

  if ((fp=fopen(iFileName, "r")) == NULL) {
    idmbPrintResult(fptr,"Cannot open file %s\n", iFileName);
    return(FALSE);
  }

  struct stat s;
  if(-1 == stat(FPN, &s)) {
    if(errno == ENOENT) {
      idmbPrintResult(fptr,"Directory to export:%s not exist\n",FPN);
      return(FALSE);
    } 
  } else if (access(FPN, W_OK) != 0){
     idmbPrintResult(fptr,"Write permission denied on %s\n",FPN);
     return(FALSE);
  } 
  getTimeIn();
  if(!idb_simplify_file_name(FPN,(const char *)NULL))
   strcpy(toWhere,"");
  else
   strcpy(toWhere,idb_simplify_file_name(FPN,(const char *)NULL));

  while(dmsGenCellViewFromFile(fp,pathName,cName,vName)) {
    if(dmsCFetchCellView(cName,vName,"",toWhere,&fileName)) {
      idmbReportMsgIfAny(dmsCGetLastMessage(),"");
      idmbPrintResult(fptr,"EXPORT CV: %s.%s TO %s ERROR\n \n",cName,vName,toWhere);
    } else {
      idmbPrintResult(fptr,"EXPORT CV: %s.%s TO %s O.K.\n \n",cName,vName,toWhere);
      count++;
    }
  }

  fclose(fp);
  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,EXPORTCOUNT,count,iFileName,theLibName);
  return(count);
} /* dmsExportCellView */


void dmsDeleteCell(char *cName, char *vName,int *noError)
{
  void *ddObj;
  char intelCVName[BUFSIZE];
 
  if (debugDmsOn) printf("\nDeleting %s.%s ", cName, vName);
  sprintf(intelCVName, "%s.%s", cName, vName);
  ddObj = ddGetObj(NULL, cName, vName, intelCVName, dmsLibID, "r");
  if (*noError=ddDeleteObj(ddObj)) {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,DELETEOK,cName,vName,dmsGetLastLibName());
    *noError=TRUE;
  } else {
    idb_capture_dm_vendor_warning();
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,DELETEFAIL,cName,vName,dmsGetLastLibName());
    *noError=FALSE;
  }
} /* dmsDeleteCell */
 

void dmsDeleteCellView(char *cName,char *vName,int *noError)
{
  struct elem *ptr;
  int i;
  char tmp[BUFSIZE];
  String rPath=NULL,wPath=NULL;
  char fp[BUFSIZE];
  *noError=FALSE;

  i = findPolicyEntry(rwpaths, rwpathsCount, toolName.name, vName);

  ptr=rwpaths[i].next;
  while (ptr) {
    switch (ptr->opt%10) {
    case 1: /* RO */
    case 2: /* RW */
      if (!dms_find_lib_id("w", ptr->dirLib, cName, vName, noError, fp))
         break;
      dmsDeleteCell(cName,vName,noError);
      break;
    }
    if (*noError) break;
    ptr=ptr->next;
  }
  return;
}


int dmsCDeleteCellView(char *cName,char *vName)
{
  int noError;
  dmsDeleteCellView(cName,vName,&noError);
  return(dmsGenErrorCode(noError));
} /* dmsCDeleteCellView */


int dmsSkillQueryCellView(char *cName,char *vName,char *verName,
						char *qCreator,char *qLocker)
{
  char *verIdPath;
  char lastTime[BUFSIZE], creator[BUFSIZE],locker[BUFSIZE],version[BUFSIZE];
  char *fpath = 0;
  char *new_ver = NULL;
  struct stat StatBuf;
  time_t last_t;
  struct passwd *pwEntry=NULL;
 
  dmsCSetLastMessage(""); 
  /* strcpy(creator, "UNKNOWN");		 REM: not implemented */
  fpath = idb_find_cellview(dmsLibID, cName, vName);
  verIdPath = idb_find_version_and_locker(dmsLibID, cName, vName, verName, 
                                          locker, fpath, TRUE, &new_ver);
  /* get the new version - latest version here */ 
  if(new_ver) {
    strcpy(version, new_ver);
  }
  if (!verIdPath) return(TRUE); /* no such version */

  if (!dmsMatchExpression(qLocker,locker)&&!strcmp(locker,NOBODY)) return(TRUE);

  /** if (!dmsMatchExpression(qCreator,creator)) return(TRUE); **/

  /* strcpy(lastTime,"UNKNOWN");		 REM: not implemented */
  /* CI_User - get the file owner of this file */
  stat(fpath, &StatBuf);
  pwEntry = getpwuid(StatBuf.st_uid);
  if(pwEntry) {
    strcpy(creator,(pwEntry->pw_name?pwEntry->pw_name:"UNKNOWN"));
  } else {
    strcpy(creator, "UNKNOWN");
  }
  last_t = StatBuf.st_atime;
  ctime(&last_t);
  strcpy(lastTime, ctime(&last_t));
  lastTime[24]='\0';

  sprintf((char *)dmsCGetLastMessage(),"%10s %10s %10s %7s %8s %8s %20s",dmsGetLastLibName(),cName,vName,version,creator,locker,lastTime);

  return(0);
} /* dmsSkillQueryCellView */



int dmsShowLog(char *cName,char *vName,int *noError)
{
  struct elem *ptr;
  int i;
  char allVersions[BUFSIZE];
  char *token;
  char fp[BUFSIZE];
  idmbPrintResult(fptr,QUERYHEADER1);
  idmbPrintResult(fptr,QUERYHEADER2);

  i = findPolicyEntry(rwpaths, rwpathsCount, toolName.name, vName);

  ptr=rwpaths[i].next;
  while (ptr) {
    switch (ptr->opt%10) {
    case 1: /* RW */
    case 2: /* RO */
      if (!dms_find_lib_id("r", ptr->dirLib, cName, vName, noError,fp))
         break;
      idb_get_all_versions(dmsLibID,cName,vName, allVersions);
      token = strtok(allVersions, " ");
      while(token) {
	/* REM: is dmsSkillQueryCellView necessary ? */
	/* dmsSkillQueryCellView(cName,vName,token,"*","*"); */

	/* idb_evaluate_string("verID~>logMessage",1); */
	idmbReportMsgIfAny(dmsCGetLastMessage(),"");
	idmbPrintResult(fptr,"   LOG: %s\n", "NOTIMPLEMENTED");
	idmbPrintResult(fptr,"CONFIG: %s\n\n",dmsCGetConfigsOfCV(theLibName,cName,vName,
									token));
	token = strtok(NULL, " ");
      }
      return(0);		/* REM: why ? */
      break;
    }
    ptr=ptr->next;
  }
  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,NOTINANYLIB,cName,vName,dmsConfName);
  return(dmsLastErrorNumber);
} /* dmsShowLog */


int dmsGetUserInput(char *select,char *tName,char *lName,char *fName,
						char *cName,char *vName,char *verName)
{
  if (select[0]=='1') {
    if (!dmsReadInput("Tool Name:",tName,tName)) ABORTCMD(__FUNCTION__, __FILE__, __LINE__);
  }

  dmsCSetToolName(tName);

  if (select[1]=='1') {
    if (!dmsReadInput("Lib Name:",lName,lName))  ABORTCMD(__FUNCTION__, __FILE__, __LINE__);
  }

  if (select[2]=='1') {
    if (!dmsReadInput("Config Name (name/default/NONE):","default",fName))  ABORTCMD(__FUNCTION__, __FILE__, __LINE__);
    if (!strcmp(fName,NOBODY)) strcpy(fName,"");
  } else strcpy(fName,"default");

  if (select[3]=='1') {
    if (!dmsReadInput("Cell Name:","*",cName)) ABORTCMD(__FUNCTION__, __FILE__, __LINE__);
  }

  if (select[4]=='1') {
    if (!dmsReadInput("View Name:","*",vName)) ABORTCMD(__FUNCTION__, __FILE__, __LINE__);
  }

  if (select[5]=='1') {
    if(!dmsReadInput("Version Name (pi.si/current):","current",verName)) ABORTCMD(__FUNCTION__, __FILE__, __LINE__);
  } else strcpy(verName,"");
  if (!strcmp(verName,"current")) strcpy(verName,"");

  idmbPrintResult(fptr,"\n");
  fflush(fptr);

  getTimeIn();

  return(TRUE);
} /* dmsGetUserInput */

/******
void dmsCCheckAllLib(char *libName,char *libNameList)
{
  char tmpList[MAXBUFSIZE],tmpName[BUFSIZE];

  dmsGenLibNameList(libName,"*",libNameList);
  strcpy(tmpList,libNameList);
  
  while (strcmp(tmpList,"")) {
    dmsSeparateBySpace(tmpList,tmpName,tmpList);
    idmbPrintResult(fptr,"\nLIBRARY: %s\n",tmpName);
    dmsLibID=dmOpenLib(tmpName,NULL,NULL,"w");
    dmCheck(dmsLibID);
  }
}
****************/


int dmsBCWriteCellView(char *cName,char *vName,int advance,char *log,char *config,char **FPN)
{
  char tmp[BUFSIZE],locker[BUFSIZE];
  char *rPath=NULL;
  int  noError;
  char *fpath = 0;
  DMSINFO tmpCVInfo;
  static char _ibuff[BUFSIZE];

  fpath = idb_find_cellview(dmsLibID, cName, vName);
  strcpy(locker,"");
  rPath = idb_find_version_and_locker(dmsLibID, cName, vName, NULL, locker,
         fpath, GET_LAST_IF_NOT_LOCKED, NULL);

  if (!strcmp(locker,userName)) {
   /**   if ((noError = dmsCExecutePreTrigger("checkin", "", cName, vName, "", ""))
           != idmsSuccess)
         return noError;
   **/
    dmsWriteCellView(0,0,cName,vName,"",BIN,log,config, _ibuff, &dmsLibID,&noError);
    *FPN = _ibuff;
   if (noError)
      dmsCSetGeneratedAttribute("checkin", idb_get_last_library_name(),
                                cName, vName, dmsGetLastVerName());
   /**
   dmsCExecutePostTrigger("checkin", idb_get_last_library_name(), cName, vName,
                          dmsGetLastVerName(), _ibuff);
   **/
    return(dmsGenErrorCode(noError));
  } else {
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,SKIPNOLOCK,dmsGetLastLibName(),cName,vName);
    *FPN=dmsBuf;
    return(dmsLastErrorNumber);
  }
} /* dmsBCWriteCellView */


void idmbExecuteCommand(char *command)
{
  char libNameList[MAXBUFSIZE]; 
  char cName[BUFSIZE],vName[BUFSIZE],verName[BUFSIZE],tmpCell[BUFSIZE],tmpView[BUFSIZE];
  char tmpCell2[BUFSIZE],tmpView2[BUFSIZE],log[BUFSIZE];
  char config[BUFSIZE],byType[BUFSIZE], byVer[BUFSIZE];
  char tmpString[BUFSIZE],outString[MAXBUFSIZE],*p=NULL, *pp=NULL;
  char tmpLibName[BUFSIZE]="*",qCreator[BUFSIZE],qLocker[BUFSIZE];
  char pName[BUFSIZE],propType[BUFSIZE],propVal[BUFSIZE];
  int  timeStamp,noError,count,advance,location;
  void *gsPtr, *gsPtr1;
  void *ddObj;
  time_t bTime,eTime;
  int bind;
  DMSINFO *info;
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
  char *tmpRetList=0;
  char *tmpRetList2=0;
  char *tmpPtr=0;
  char *tmpPtr2=0;
#endif
  dmsReadInput("Command:","",command);

  switch(dmsToUpper(command)) {
/**
 **
 ** [V] - View Library Database
 **
 **/
  case 'V':
    INPUTSETPATH("110111");
    if (!dmsReadInput("CI Owner Name:","*",qCreator)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
    if (!dmsReadInput("CO User Name (name/locked/NONE):","*",qLocker)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
    
    idmbPrintResult(fptr,QUERYHEADER1);
    idmbPrintResult(fptr,QUERYHEADER2);
    
    EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsSkillQueryCellView(cName,vName,verName,qCreator,qLocker),"r",NL1); 
    break;
/**
 **
 ** [M] - Misc utility
 **
 **/
  case 'M':
    if (!dmsReadInput("[M]emory usage/[U]nix command/[E]dit dms.pth:","m",command)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
    switch(dmsToUpper(command)) {
    case 'M':
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,MEMORYUSED,voMemoryUsed());
      break;
    case 'U':
      if (!dmsReadInput("UNIX command:","",outString)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      system(outString);
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,UNIXCMDOK,outString);
	break;
    case 'E':
      strcpy(outString,"vi $DMSPATH");
      system(outString);
      dmsCParsePath();
      break;
    }
    break;
/**
 **
 ** [P] - Property management
 **
 **/
  case 'P':
    if (!dmsReadInput("[C]reate/[L]ist/[R]emove:","l",command)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
    INPUTSETPATH("110111");
    
    if (!matchRWPATHLibName(theLibName,vName)) {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,LIBNOTONRWPATHS,theLibName,theToolName,vName);
      break;
    }
    switch(dmsToUpper(command)) {
    case 'L':
      EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsShowAttribute(cName,vName,verName,&noError),"r",NL11);
      break;
    case 'C':
      if (!dmsReadInput("Property Name:","propName",pName)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      if (!dmsReadInput("Property Type(String/Integer/Boolean/Time):","s",propType)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      switch(dmsToUpper(propType)){
      case 'S':
	if (!dmsReadInput("Property String Value:","string",propVal)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
	break;
      case 'I':
	if (!dmsReadInput("Property Integer Value:","0",propVal)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
	break;
	/*
	  case 'F':
	  if (!dmsReadInput("Property Float Value:","0.0",propVal)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
	  break;
	  */
      case 'B':
	if (!dmsReadInput("Property Boolean Value:(t/f)","t",propVal)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
	break;
      case 'T':
	bTime=voFastTime();          /* to current time */
	idb_capture_dm_vendor_warning();
	strcpy(tmpString,ctime(&bTime));
	tmpString[24]='\0';
	if (!dmsReadInput("Property Value:",tmpString,propVal)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
	break;
      default:
	idmbPrintResult(fptr,"Type Not Supported Yet!!!\n");
	break;
      }
      EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsCreateAttribute(cName,vName,verName,pName,propType,propVal,&noError),"w",NL12);
      break;
    case 'R':
      if (!dmsReadInput("Property Name:","propName",pName)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsDeleteAttribute(cName,vName,verName,pName,&noError),"w",NL13);
      break;
    }
    break;
/**
 **
 ** [Y] - Config management
 **
 **/
  case 'Y':
    if (!dmsReadInput("[L]ist/[C]reate/[T]ext/l[O]ad/[D]elete config:\nadd[S]ub/remo[V]eSub/create[I]nstance/co[M]ment config:\n[A]dd/[R]emove/di[F]f/i[N]sert/e[X]port cellview:","l",command)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
    if ((dmsToUpper(command)=='A')||(dmsToUpper(command)=='R')) {
      INPUTSETPATH("111111");
    } else if (dmsToUpper(command)=='L'||dmsToUpper(command)=='O') {
      INPUTSETPATH("110000");
    } else {
      INPUTSETPATH("111000");
    }
    
    getTimeIn();
    dmsLibID=idb_open_library(theLibName, NULL, "r");
    if (!dmsLibID) {
      idb_capture_dm_vendor_warning();
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,CANTREADLIB,theLibName);
    }
    
    switch(dmsToUpper(command)) {
    case 'L':
      noError=(int)dmsCListConfig(theLibName);
      break;
    case 'T':
      sprintf(log,"%s.ascii",theConfName);
      if (!dmsReadInput("Give File Name To Dump To:",log,log))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      if (!dmsReadInput("Expand Sub-configurations(Y/N):","no",tmpString))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      if (dmsToUpper(tmpString)=='Y') advance=TRUE; else advance=FALSE;
      noError=dmsCDumpConfig(theLibName,theConfName,log,advance);
      break;
    case 'O':
      sprintf(tmpString,"config.ascii");
      if (!dmsReadInput("Give File Name To Load From:",tmpString,tmpString))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      noError=dmsCLoadConfig(theLibName,tmpString);
      break;
    case 'F':
      if (!dmsReadInput("Give Library Name of Configuration to Diff:",theLibName,tmpLibName))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      if (!dmsReadInput("Give Configuration Name to Diff:",theConfName,tmpString))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      if (!dmsReadInput("Give File Name To Output To:","",log))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      noError=dmsDiffConfig(theLibName,theConfName,tmpLibName,tmpString,log);
      break;
    case 'N':
      if (!dmsReadInput("Give Configuration Name To Insert To:",".",tmpString))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      noError=dmsCInsertConfig(theLibName,theConfName,tmpString);
      break;
    case 'X':
      if (!dmsReadInput("Give the Directory to Export to:",".",tmpString))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      noError=dmsCExportConfig(theLibName,theConfName,tmpString);
      break;
    case 'C':
      noError=dmsCCreateConfig(theLibName,theConfName);
      break;
    case 'D':
      noError=dmsCRemoveConfig(theLibName,theConfName);
      break;
    case 'A':
      if (!dmsReadInput("Binding Type ([S]tatic/Most Recent by [T]ime/Most Recent by [P]rimary Index):","s",byType)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      switch(dmsToUpper(byType)) {
      case 'S':
      case 'T':
      case 'P':
	bind=0;
	break;
      }
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
      EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsCAddCVToConfig(dmsLibID,theConfName,cName,vName,verName,bind),"w",NL10);
#endif
      break;
    case 'R':
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
      EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsCRemoveCVFromConfig(dmsLibID,theConfName,cName,vName),"w",NL15);
#endif
      break;
    case 'S':
      if (!dmsReadInput("Give Library Name of Sub-configuration:",theLibName,tmpLibName))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      if (!dmsReadInput("Give Sub-configuration Name:",theConfName,tmpString))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      noError=dmsCAddSCToConfig(theLibName,theConfName,tmpLibName,tmpString);
      break;
    case 'V':
      if (!dmsReadInput("Give Library Name of Sub-configuration:",theLibName,tmpLibName))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      if (!dmsReadInput("Give Sub-configuration Name:",theConfName,tmpString))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      noError=dmsCRemoveSCFromConfig(theLibName,theConfName,tmpLibName,tmpString);
      break;
    case 'I':
      sprintf(tmpString,"%s.instance",theConfName);
      if (!dmsReadInput("Give Configuration Instance Name:",tmpString,tmpString))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      noError=dmsCCreateConfigInstance(theLibName,theConfName,tmpString);
      break;
    case 'M':
      sprintf(log,"Created by %s",userName);
      if (!dmsReadInput("Give Configuration Comment:",log,log))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      if (!dmsReadInput("Append to Previous Comment(Y/N):","no",tmpString))
	STOPCMD(__FUNCTION__, __FILE__, __LINE__);
      if (dmsToUpper(tmpString)=='Y') advance=TRUE; else advance=FALSE;
      noError=dmsCCreateConfigComment(theLibName,theConfName,log,(char *)advance);
      break;
    }
    break;
/**
 **
 ** [I] - Import files into a library
 **
 **/
  case 'I':
    INPUTSETPATH("100000");
    dmsImportCellView();
    break;
/**
 **
 ** [X] - Export files out of a library
 **
 **/
  case 'X':
    INPUTSETPATH("100000");
    dmsExportCellView();
    break;
/**
 **
 ** [S] - DMS.PATH management
 **
 **/
  case 'S':
    if (!dmsReadInput("[S]earch path/[W]orkdir/work[L]ib/[R]eparse/[P]rint result:","p",command)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
    switch(dmsToUpper(command)) {
    case 'S':
      INPUTSETPATH("100000");
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,SEARCHPATH,toolName.name,dmsCGetSearchPath());
      break;
    case 'P':
      INPUTSETPATH("100010");
      dmsCPrintPath(vName);
      break;
    case 'R':
      dmsCParsePath();
      break;
    case 'W':
      INPUTSETPATH("100010");
      dmsCGetWorkingDir(vName,&pp);
      strcpy(outString,pp);
      break;
    case 'L':
      INPUTSETPATH("100010");
      dmsCGetWorkingLib(vName,&pp);
      strcpy(outString,pp);
      break;
    }
    break;
/**
 **
 ** [G] - Read/Lock files based on policy
 **
 **/
  case 'G':
    INPUTSETPATH("111111");
    
    EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsCReadOrEditByPolicyByLib(theLibName,cName,vName,verName,&pp),"w",NL4);
    /*2.0 EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsBCGetCellView(cName,vName,verName,outString),"w",NL4);*/
    
    break;
/**
 **
 ** [L] - Checkout files to workdir
 **
 **/
  case 'L':
    INPUTSETPATH("111111");
    
    EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsCCheckoutCellViewByLib(theLibName,cName,vName,verName,&pp),"w",NL5);
    
    break;
/**
 **
 ** [C] - Cancel checkout of files
 **
 **/
  case 'C':
    INPUTSETPATH("110111");
    
    EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsCCancelCheckoutCellViewByLib(theLibName,cName,vName,verName,&pp),"w",NL3);
    /*2.0 EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsCUnLockCellView(cName,vName,outString),"w",NL3);*/
    
    break;
/**
 **
 ** [E] - Find physical location of files
 **
 **/
  case 'E':
    INPUTSETPATH("110110");
    if (!strcmp(tmpLibName,"*")&&!is_wild_exp(cName)&&!is_wild_exp(vName)) { /* for Unix files to be found */
      strcpy(tmpCell,cName);
      strcpy(tmpView,vName);
      strcpy(tmpCell2,cName);
      strcpy(tmpView2,vName);
      while (strcmp(tmpCell2,"")) {
	dmsSeparateBySpace(tmpCell2,cName,tmpCell2);
	while (strcmp(tmpView2,"")) {
	  dmsSeparateBySpace(tmpView2,vName,tmpView2);
	  dmsCReadonlyCellViewByLib("*",cName,vName,verName,&pp);
	  location=dmsCGetLastCellViewInfo(&info);
	  if (location==idmsCvInWorkdir||location==idmsCvInUnixdir) {
	    count++;
	    idmbReportMsgIfAny(dmsCGetLastMessage(),pp);
	  }
	}
	strcpy(tmpView2,tmpView);
      }
      strcpy(cName,tmpCell);
      strcpy(vName,tmpView);
    }
    EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsCReadonlyCellViewByLib(theLibName,cName,vName,verName,&pp),"r",NL7);
    
    break;
/**
 **
 ** [F] - Fetch files to workdir for readonly
 **
 **/
  case 'F':
    INPUTSETPATH("111111");

    EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsCFetchCellView(cName,vName,verName,"",&pp),"r",NL8);
    
    break;
/**
 **
 ** [W] - Checkin files from working directory
 **
 **/
  case 'W':
    INPUTSETPATH("110111");

    if (!dmsReadInput("Log Message:","Checkin from idmb.",log)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
    if (!dmsReadInput("Configuration:",NOBODY,config)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
    if (!strcmp(config,NOBODY)) strcpy(config,"");
    if (!dmsReadInput("Advance Primary Index(Y/N):","no",tmpString)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
    if (dmsToUpper(tmpString)=='Y') advance=TRUE; else advance=FALSE;
    
    EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsBCWriteCellView(cName,vName,advance,log,config,&pp),"w",NL9);
    
    break;
/**
 **
 ** [Z] - Make diff between two versions of files
 **
 **/
  case 'Z':
    INPUTSETPATH("110111");
    if (!dmsReadInput("Make diff with version (pi.si/current):","current",byVer)) STOPCMD(__FUNCTION__, __FILE__, __LINE__);
    if (!strcmp(byVer,"current")) strcpy(byVer,"");
    
    EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsCDiffVersions(cName,vName,verName,byVer,"-b"),"r",NL14);
    
    break;
/**
 **
 ** [R] - Log history of files
 **
 **/
  case 'R':
    INPUTSETPATH("110110");
    EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsShowLog(cName,vName,&noError),"r",NL31);
    break;
/**
 **
 ** [N] - Install a new file in worklib
 **
 **/
  case 'N':
    INPUTSETPATH("100110");
    dmsCInstallCellView(cName,vName,"",userName,&pp);
    strcpy(outString,pp);
    break;
/**
 **
 ** [D] - Delete files
 **
 **/
  case 'D':
    INPUTSETPATH("110110");
    EXECUTEWILD(__FUNCTION__, __FILE__, __LINE__, dmsCDeleteCellView(cName,vName),"w",NL2);
    break;
/**
 **
 ** [Q] - Quit iDMB
 **
 **/
  case 'Q':
    dmsCPIExitDMS();
    idmbReportMsgIfAny(dmsCGetLastMessage(),"");

    if (idmbGraphicMode) {
      idmbPrintResult(fptr, "QUIT");
    }
    exit(0);

    break;
/**
 **
 ** Misc Commands 
 **
 **/
  case '$': /* message is cleared after reported, so not useful from idmb */
    strcpy(outString,dmsCReportMessage());
    break;
  case '%':
    dmsCRefreshAllLib(outString);
    break;
  case '^':
    idmbPrintResult(fptr,dmsCVersionNumber());
    break;
  default:
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING,UNKNOWNCMD,command);
    break;
  }
  
  /* some clean up for next command */
  strcpy(theLibName,"");
  strcpy(theConfName,"");
  strcpy(tmpLibName,"");
  strcpy(cName,"");
  strcpy(vName,"");
  strcpy(verName,"");
  strcpy(config,"");

  fflush(fptr);
  idmbReportMsgIfAny(dmsCGetLastMessage(),outString);
  getTimeOut();
  idmbPrintResult(fptr,"^^^^^^^^^^^^^^^^^ %c ^^^^^^^^^^^^^^^^^\n",command[0]);
  fflush(fptr);
  dmsCRefreshAllLib(outString); /* auto refresh every run */
  if (!idmbGraphicMode) dmsReadInput("PRESS ENTER TO CONTINUE","",outString);
  strcpy(outString,"");
  pp=NULL;
} /* idmbExecuteCommand */



void idmbMsgFromDFII()
{
  char command[BUFSIZE],inBuffer[BUFSIZE]="";
  char *ptr1;

  while(TRUE) {
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
    	  fgets(inBuffer, sizeof(inBuffer), stdin);
#else
	  gets(inBuffer);
#endif
	  if(!strcmp(inBuffer,"IDMB_GO_DO_IT")) {
	    fprintf(fptr, "%s", idmbCmdBuffer);
	    fflush(fptr);
	    idmbExecuteCommand(command);
	    strcpy(idmbCmdBuffer,"");
	  } else if (strstr(inBuffer,";")) {
	    strcpy(idmbCmdBuffer,"");
	  } else if(strcmp(inBuffer, "")) {
	    strcat(idmbCmdBuffer,inBuffer);
	    strcat(idmbCmdBuffer,"|");
	  }
  }
}

