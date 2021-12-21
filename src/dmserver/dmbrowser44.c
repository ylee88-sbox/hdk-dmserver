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
# $Id: dmbrowser.h  0.1 1998/07/29 19:21:08 jdelcan Exp $
#                                                                     #
#######################################################################
#######################################################################
*/


/* INCLUDES */

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <string.h>
/*******************#include <varargs.h> *****************************/
#include <stdarg.h>
#include <stdlib.h>
/* for directory access */
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

#define voRandom_h
#define voUnistd_h
#define BFSZ 2048
#define MAXBUFSIZE  70720

/* INCLUDES */
 #if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
 #include "itkDB.h"
 #else
#define ddId void*
extern ddId dmsLibID;
#endif
#define RW_opt 1
#define RO_opt 2
#define WORKDIR_opt 4
#define UNIXDIR_opt 7

/* redefine structs */
typedef struct elem {
  int opt;
  char *dirLib;
  struct elem *next;
  char *absPath;
} OPERATION;

typedef struct head {
  char *name;
  struct elem *next;
} INFO;

/* DMSINFO definition for dmsCInfoCellView()
   0: not exist 1: workdir 2: unixdir 3: RW 4: RO 5: create
*/
typedef struct dmsInfo {
  int  location; 
  char libPath[1024];
  char libName[1024];
  char cellName[1024];
  char viewName[1024];
  char verName[1024];
  char coUser[1024];
  char fullPathName[1024];
} DMSINFO;

extern int debugDmsOn;

extern INFO rwpaths[];
extern int rwpathsCount;
extern INFO toolName;
extern DMSINFO dmsLastCVInfo;
extern char *dmsGetWorkingDir(char *vName, char *dmsWorkingDir);
char *dmCGenObjRel(char*library, char*name, int isCellName);
char*  osSimplifyPath(char* inPathName, char* inBasePath);
char *dmsFormFileName(char *dir, char *cName, char *vName, int call_get_ext);
/************
 This function checks if this path is listed as a RO or RW library in the
 rwpath in dms.pth file.
 Returns: 3 if it is listed as a RW library.
          4 if it is listed as a RO library.
 else returns 0.
*************/
int dmsIsRWLib(const char *libName, const char *vName)
{
  struct elem *ptr;
  int i;
  char tmp[100]="";
  char *tmp1;

  i = findPolicyEntry(rwpaths, rwpathsCount, toolName.name, vName);
  if (debugDmsOn)
    dmsPrintIdmsLog("*IDMS DEBUG* %d.rwpaths %s:\n", i+1, rwpaths[i].name);
  ptr = rwpaths[i].next;
  while (ptr){
    switch (ptr->opt%10){
    case UNIXDIR_opt:
    case WORKDIR_opt:
      break;
    case RW_opt:
      /*** Need to check dirLib string before '.'
           Config could be part of lib string. (lib.LATEST)  ***/
      strcpy(tmp, ptr->dirLib);
      tmp1 = strchr(tmp, '.');
      if(!tmp1) {
        if(!strcmp(ptr->dirLib, libName))
          return 3;
      } else {
        *tmp1 = '\0';
        if(!strcmp(tmp, libName))
          return 3;
      }
      break;
    case RO_opt:
      /*** Need to check dirLib string before '.'
           Config could be part of lib string.  (lib.LATEST)  ***/
      strcpy(tmp, ptr->dirLib);
      tmp1 = strchr(tmp, '.');
      if(!tmp1) {
        if(!strcmp(ptr->dirLib, libName))
          return 4;
      } else {
        *tmp1 = '\0';
        if(!strcmp(tmp, libName))
          return 4;
      }
      break;
    default:
      break;
    }
    ptr=ptr->next;
  }
  return 0;
}



/*******
 This function checks if this path is listed in the rwpath in dms.pth file.
 In GenViewCellView, we don't want to get the cells with view = vname if the library 
 is not in rwpath for this view.
*************/
int dmsIsViewInRWPath(const char *libName, const char *vName)
{
  struct elem *ptr;
  int i;
  char tmp[100]=""; 
  char *tmp1;

  i = findPolicyEntry(rwpaths, rwpathsCount, toolName.name, vName);
  if (debugDmsOn)
    dmsPrintIdmsLog("*IDMS DEBUG* %d.rwpaths %s:\n", i+1, rwpaths[i].name);
  ptr = rwpaths[i].next;
  while (ptr){
    switch (ptr->opt%10){
    case UNIXDIR_opt:
    case WORKDIR_opt:
      break;
    case RW_opt:
    case RO_opt:
      /* need to check dirLib string before '.'  Config could be part of lib string.  (lib.LATEST)  */
      strcpy(tmp, ptr->dirLib);
      tmp1 = strchr(tmp, '.');
      if(!tmp1) {
        if(!strcmp(ptr->dirLib, libName))
	  return 1; 
      } else {
        *tmp1 = '\0';
	if(!strcmp(tmp, libName))
	  return 1; 
      }     
      break;
    default:
      break;
    }
    ptr=ptr->next;
  }
  return 0;
}

/***************
 * Parameters:  fdir = directory to search
 *              vname    = viewname to search
 *              getLocker: == 0 -> return cellname.
 *                         == 1 -> return complete cell info.
 *			   == 2 -> return complete cell info
 *                                              + header "UD/WD <dirpath> "
 *
 * Description: Appends to the buffer all cellviews found in the given 
 *              directory.  
 * 
 * Notes: this does not support any library cell checked out in workdir.
 *        will always return cell with no library information. 
 */
int dmsGetCompleteDirInfos(char*fdir,char*vname,int getLocker, char** output)
{
  struct dirent *dp = 0;
#ifndef OH_MACH_LINUX
  struct __dirdesc *dirp = 0;
#else
  struct __dirstream *dirp = 0;
#endif
  char *tmp2=0;
  char *wkdir=0;
  char *full_path=0;
  char tmp[BFSZ];
  char filename[BFSZ];
  char eList[BFSZ];
  char _extbuff[BFSZ];
  char fileBuffer[BFSZ];
  char *eNameList = 0; 
  int max = BFSZ;
  int size = 0;
  char * tmpcellviewList=0;
  
  *output = 0;
  if(!fdir || !vname)
    return -1;

  if(!osFileIsDir(fdir)) 
    return -1;

  /* allocate size to some big number (BUFSIZE) first */
  tmpcellviewList = (char*)malloc(max);
  *tmpcellviewList=0;
   
  if(getLocker==2) { /* dir path in there. */
    wkdir = dmsGetWorkingDir(vname, 0);
    if(wkdir) {
      size = strlen(wkdir) -1 ;
      if (wkdir[size] =='/')
        wkdir[size] = '\0';
    }
    if(!strcmp(wkdir, osSimplifyPath(fdir,0))) {
      sprintf(fileBuffer, "%s %s","WD", fdir);
    } else {
      sprintf(fileBuffer, "%s %s","UD", fdir);
    }
    strcpy(tmpcellviewList, fileBuffer);
    strcat(tmpcellviewList, " ");
    size = size + strlen(fileBuffer) + 1;
  }

  if(idmGetExtName(toolName.name, vname, &eNameList)) {
    if(eNameList){
      strcpy(eNameList,vname);
    } 
  }
  strcpy(tmp, eNameList);
  
  dirp = opendir(fdir);
  if(dirp) {
    while((dp = readdir(dirp)) != 0) {
      if(strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..") && (dp->d_name[0] != '.')) {
        /* Check to see if it has matching viewtype. */
        strcpy(filename, dp->d_name);
        tmp2 = strchr(filename, '.');
        if(tmp2) {
          *tmp2 = '\0'; /* filename is the cellname now. */
          tmp2++;
	  /* Loop thru extension list. */
	  strcpy(eList, tmp);
          eNameList = eList;
          dmsSeparateByColon(eNameList,_extbuff, eNameList);
          while(strcmp(_extbuff,"")) {
	    if(!strcmp(_extbuff, tmp2)) {
	      /* Matching vname extension -> add */
	      if(!getLocker) {
                strcpy(fileBuffer, filename);
              } else {
	        full_path = dmsFormFileName(fdir, filename, _extbuff, 1);
                sprintf(fileBuffer, "%s:%s:%s:%s:%s",filename,_extbuff,"",full_path,"NONE");
              }
    	      if ((strlen(fileBuffer)+1+size) > max) {
                max = 2*size + strlen(fileBuffer)+1;
      		tmpcellviewList = realloc(tmpcellviewList, max);
      		if(!tmpcellviewList)
        	  return -1;
       	      }
    	      strcat(tmpcellviewList, fileBuffer);
              strcat(tmpcellviewList, " ");
              size = size + strlen(fileBuffer) + 1;
	      break;
	    }
            dmsSeparateByColon(eNameList, _extbuff, eNameList);
	  }
        }
      }
    }
    (void) closedir(dirp);
  }
  
  *output = tmpcellviewList;
  return 0;
 
}

/*
 *              completeInfo: == 0 -> return cellname.
 *                            == 3 -> return complete cell info.
 *                            == 2 -> return complete cell info
 *                                              + header "UD/WD <dirpath> "
 */ 
int dmsGetCompleteLibInfos(char*libName,char*viewName,int completeInfo, char**cellviewList)
{
   int size=0;
   char cellName[128];
   char * tmpPtr = 0;
   char * tmpcellList=0;
   char * tmp=0;
   char * tmpcellviewList=0;
   int ret = 0;

   /* Error Checking on libNames or if library is not in rwpaths
      for this viewname.*/
   if (!libName || !dmsIsViewInRWPath(libName, viewName) ) {
     *cellviewList = 0;
     return -1; 
   }
   ret = dmsIsRWLib(libName, viewName);

#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE)) 
   tmpcellList = dmCGenObjRel(libName, viewName, completeInfo);    
   if(!tmpcellList) {
     return -1;
   }
   else{ /* allocate tmpcellviewList to the (size of tmpcellList)+2 */
     size = strlen(tmpcellList);
     /* add RW or RO up front to indicate the library type */
     tmpcellviewList=(char*)malloc(size+5);
     *tmpcellviewList=0;
   }
   if(completeInfo == 2) {
     if(ret == 3) 
       strcpy(tmpcellviewList, "RW");
     else if(ret == 4)
       strcpy(tmpcellviewList, "RO");
     else 
       return -1;
   }
   strcat(tmpcellviewList," "); 
   strcat(tmpcellviewList,tmpcellList);
 
#else   
   libID= ddGetObj(libName,NULL,NULL,NULL,NULL,"r");
   if (!libID) {
     return -1;
   }
   size = 0;
   printf("FEATURE NOT SUPPORTED\n");
   
#endif
   *cellviewList=tmpcellviewList;
   return 0;
}


int dmsGenCells(char * libName, char ** cellList) {
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
   char * tmpcellList;
   ddId libID;
   ddId cellId;
   
   ddObjRelState* pState;

   
   int size=0;
 
   libID= ddGetObj(libName,NULL,NULL,NULL,NULL,"r");
   if (!libID) {
     return -1;
   }

   pState=ddStartGenObjRel(libID,ddObjChildRelType);
   while (cellId=ddGenObjRel(pState)) {
     size+= strlen(ddGetObjName(cellId))+1;
   }
   ddStopGen(pState);

   tmpcellList = (char*)malloc(size+2);
   *tmpcellList=0;


   pState=ddStartGenObjRel(libID,ddObjChildRelType);
   while (cellId=ddGenObjRel(pState)) {
     strcat(tmpcellList,ddGetObjName(cellId));
     strcat(tmpcellList," ");
   }
   
   *cellList= tmpcellList;
   ddStopGen(pState);
#endif
   return 0;
  
}

int dmsGenViews(char * libName, char **viewList)
{
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
  ddObjRelState* pState;
  ddObjRelState* pState1;

  ddId libID;
  ddId cellId;
  ddId viewId;
  
  char* tmpviewList;
  int size;
  
  size=0;
  
  libID= ddGetObj(libName,NULL,NULL,NULL,NULL,"r");
  if (!libID) {
    return -1;
  }
  pState=ddStartGenObjRel(libID,ddObjChildRelType);
  while (cellId=ddGenObjRel(pState)) {
    pState=ddStartGenObjRel(cellId,ddObjChildRelType);
    while (viewId=ddGenObjRel(pState)) {
      size+=strlen(ddGetObjName(cellId))+1;
    }
    ddStopGen(pState1);
    break;
  }
  
  ddStopGen(pState);
  tmpviewList = (char*)malloc(size+2);
  *tmpviewList=0;
  
  pState=ddStartGenObjRel(libID,ddObjChildRelType);
  while (cellId=ddGenObjRel(pState)) {
    pState=ddStartGenObjRel(cellId,ddObjChildRelType);
    while (viewId=ddGenObjRel(pState)) {
      strcat(tmpviewList,ddGetObjName(cellId));
      strcat(tmpviewList," ");
    }
    ddStopGen(pState1);
    
    break;
  }
  
   
  ddStopGen(pState);
  *viewList = tmpviewList;
#endif
  return 0;
}



/* DmBrowser calls this function to get a list of cells with the specific viewName.
   cellviewList format is:
        "cName1:locker1 cName2:locker2"
 */ 
int  dmsGenViewCellViews(libName,viewName,cellviewList) 
char* libName;
char* viewName;
char** cellviewList;
{
   char locker[1024];
   char fp[1024];
   char verid[1024];
   char* checkoutVer;
   int size=0;
   char cellName[128];
   char * tmpPtr = 0;
   char * tmpcellList=0;
   char * tmpcellviewList=0;

#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
   ddObjRelState* pState;
   ddObjRelState* pState1;
   ddId libID;
   ddId cellId;
   ddId viewId;
   int firstcell=0;
   int noError;
   char* tmpviewList;
#endif   

  
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE)) 
   if (!libName) {
     return -1;
   }
   /* return if this library is not in rwpaths for this viewname. */
   if ( !dmsIsViewInRWPath(libName, viewName) ) {
     *cellviewList = 0;
     return -1; //return 0;
   }
    
   tmpcellList = dmCGenObjRel(libName, viewName, 0);    
   if(!tmpcellList)
     return -1;
   else{ /* allocate tmpcellviewList to the (size of tmpcelList)+(14* size of tmpcelList) */
     size = 15*strlen(tmpcellList);
     tmpcellviewList=(char*)malloc(size+2);
     *tmpcellviewList=0;
   }
   
   while (tmpcellList && (strcmp(tmpcellList,""))) {
     if(tmpPtr = strrchr(tmpcellList, ' '))
       *tmpPtr = '\0';
     else
       *tmpcellList = '\0';
     if(tmpPtr = strrchr(tmpcellList, ' ')) {
       *tmpPtr++;
       strcpy(cellName,tmpPtr);
       *tmpPtr = '\0';
     } else {
       strcpy(cellName,tmpcellList);
       *tmpcellList = '\0';
     }
     strcat(tmpcellviewList,cellName);
     strcat(tmpcellviewList," ");
     
     if(!tmpcellList)
       break;
   }
#else   
   libID= ddGetObj(libName,NULL,NULL,NULL,NULL,"r");
   if (!libID) {
     return -1;
   }
   /* return if this library is not in rwpaths for this viewname. */
   if ( !dmsIsViewInRWPath(libName, viewName) ) {
     *cellviewList = 0;
     return -1; // return 0;
   }
   
   pState=ddStartGenObjRel(libID,ddObjChildRelType);
   while (cellId=ddGenObjRel(pState)) {
     pState1=ddStartGenObjRel(cellId,ddObjChildRelType);
     while (viewId=ddGenObjRel(pState1)) {
       if (!strcmp(ddGetObjName(viewId),viewName))
         size+=strlen(ddGetObjName(cellId))+14;
     }
     ddStopGen(pState1);
   }
   ddStopGen(pState);

   tmpcellviewList=(char*)malloc(size+2);
   *tmpcellviewList=0;
   
   
   pState=ddStartGenObjRel(libID,ddObjChildRelType);
   while (cellId=ddGenObjRel(pState)) {
     pState1=ddStartGenObjRel(cellId,ddObjChildRelType);
     while (viewId=ddGenObjRel(pState1)) {
       if (!strcmp(ddGetObjName(viewId),viewName)) {
         strcat(tmpcellviewList,ddGetObjName(cellId));
         strcat(tmpcellviewList,":");
         *locker=0;
         *verid=0;
         strcpy(cellName,ddGetObjName(cellId));
         
         dmCGetVersionAndLocker(libName,cellName,viewName,
                                NULL,locker,fp,1,&checkoutVer);
         strcat(tmpcellviewList,*locker?locker:"0");
         strcat(tmpcellviewList," ");
       }
     }
     ddStopGen(pState1);
     }
   ddStopGen(pState);
#endif
   *cellviewList=tmpcellviewList;

   return 0;
}



int  dmsGenCellCellViews(libName,cellName,cellviewList) 
char* libName;
char* cellName;
char** cellviewList;
{
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
  ddObjRelState* pState1;
  ddId viewId;
  ddId libID;
  ddId cellId;
  char * tmpcellviewList=0;
  int size=0;
  libID= ddGetObj(libName,NULL,NULL,NULL,NULL,"r");
  if (!libID) {
    return -1;
  }
  cellId=ddGetObj(libName,cellName,NULL,NULL,NULL,"r");
  pState1=ddStartGenObjRel(cellId,ddObjChildRelType);
  while (viewId=ddGenObjRel(pState1)) {
    size+=strlen(ddGetObjName(viewId))+2;
  }

  ddStopGen(pState1);
  tmpcellviewList=(char*)malloc(size+2);
  *tmpcellviewList=0;
  pState1=ddStartGenObjRel(cellId,ddObjChildRelType);
  while (viewId=ddGenObjRel(pState1)) {
    strcat(tmpcellviewList,ddGetObjName(viewId));
    strcat(tmpcellviewList," ");
  }
  ddStopGen(pState1);
  
  *cellviewList=tmpcellviewList;
#endif
  return 0;
  
}


int dmsGenVersions(libName,cName,vName,versionList) 
     char* libName;
     char* cName;
     char* vName;
     char** versionList;
{

  static char buffer[1024];
  dmsCGetAllVersions(libName,cName,vName,buffer);
  /* printf ("get versions %s %s %s result %s\n",libName,cName,vName,buffer); */   

  *versionList=buffer;
  return 0;
}






void convertUnixToWin(char **unixName, int location)
{
}

int dmsGetViewName(toolName,viewName,extList)
  char* toolName;
char* viewName;
char** extList;
{
 return idmGetViewName(toolName,viewName,extList);
}

void dmsCPIRestartDMS() 
{
  return;
}

/*
  Generate all complete infos of cellviews in the given library with
  the given viewname.
*/
int dmsGenCellViewsByLib(char*libName,char*viewName, char**cellviewList)
{
   return dmsGetCompleteLibInfos(libName, viewName, 2, cellviewList); 
}

/*
  Generate all complete infos of cellviews in the given library with
  the given viewname.
*/
int dmsGenCellViewsForCache(char*cellName,char*viewName,char*version,char**outfile)
{
  DMSINFO *info;
  int returnValue;
  char *fp=NULL;
  char *vview=NULL;
  char *dmsWorkingDir=NULL;
  char *tmp= NULL;
 
  /* cellName, viewName are required */
  if(!viewName||!cellName||!version||!strcmp(viewName,"")||!strcmp(cellName,""))  
    return -1;
 
  returnValue=dmsCReadonlyCellViewTypeByLib("*",cellName,viewName,version,&vview,&fp);
  if(returnValue != 0) { /* Failed */
    return -1;
  }
  /* dmsCGetLastCellViewInfo(&info); */
  
  
  if(dmsLastCVInfo.location >2) { /* RW/RO library */
    dmsGenCellViewsByLib(dmsLastCVInfo.libName, viewName, outfile);
  } else {
    if(dmsLastCVInfo.location ==1){ /* WorkDir */
      dmsWorkingDir = strdup(dmsGetWorkingDir(viewName, 0));
    } else { /* UnixDir */
      dmsWorkingDir = strdup(dmsLastCVInfo.fullPathName);
      if(tmp = strrchr(dmsWorkingDir, '/')){
        *tmp = '\0';
      } else {
  /* printf("ERROR!!! Cannot find char / in UnixDir with fullPath %s\n", info->fullPathName); */
        return -1;
      }
    }
    dmsGetCompleteDirInfos(dmsWorkingDir, viewName, 2, outfile);
    free(dmsWorkingDir);      
  }
  return 0;
}

/**  
  *  completeInfo = 0:  return only cellnames.
  *               = 1:  return complete cell information.
 **/
int dmsGenAllCellViews(char*viewName, int completeInfo, char**outfile)
{ 
  struct elem *ptr;
  int i;
  char tmp[BFSZ]="";
  char *tmp1;
  int max = MAXBUFSIZE;
  int size = 0;
  char * tmpcellviewList=0;
  char * ret = 0;
  
  *outfile = 0;
  
  /* allocate size to some big number first */
  tmpcellviewList = (char*)malloc(max);
  *tmpcellviewList=0;

  i = findPolicyEntry(rwpaths, rwpathsCount, toolName.name, viewName);
  if (debugDmsOn)
    dmsPrintIdmsLog("*IDMS DEBUG* %d.rwpaths %s:\n", i+1, rwpaths[i].name);
  ptr = rwpaths[i].next;
  while (ptr){
    switch (ptr->opt%10){
    case UNIXDIR_opt:
    case WORKDIR_opt:
      strcpy(tmp, ptr->dirLib);
      dmsGetCompleteDirInfos(tmp, viewName, completeInfo, &ret);
      if (ret && ((strlen(ret)+1+size) > max)) {
        max = 2*size + strlen(ret)+1;
        tmpcellviewList = realloc(tmpcellviewList, max);
        if(!tmpcellviewList)
          return -1;
      }
      strcat(tmpcellviewList, ret);
      strcat(tmpcellviewList, " ");
      size = size + strlen(ret) + 1;
      break;
    case RW_opt:
    case RO_opt:
      strcpy(tmp, ptr->dirLib);
      tmp1 = strchr(tmp, '.');
      if(tmp1) {
        *tmp1 = '\0';
      }
      if(!completeInfo)
        dmsGetCompleteLibInfos(tmp, viewName, 4, &ret);
      else
        dmsGetCompleteLibInfos(tmp, viewName, 3, &ret);
      if (ret && ((strlen(ret)+1+size) > max)) {
        max = 2*size + strlen(ret)+1;
        tmpcellviewList = realloc(tmpcellviewList, max);
        if(!tmpcellviewList)
          return -1;
      }
      if(ret) {
        strcat(tmpcellviewList, ret);
        strcat(tmpcellviewList, " ");
        size = size + strlen(ret) + 1;
      }
      break;
    default:
      break;
    }
    ptr=ptr->next;
    free(ret);
    ret = 0;
  }
  *outfile = tmpcellviewList;
  return 0;
}
