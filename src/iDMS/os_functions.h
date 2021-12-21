#ifndef OS_FUNCTIONS_H
#define OS_FUNCTIONS_H

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
# $Id: os_functions.h,v 1.1.2.7 1997/08/25 21:31:49 mgatlin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

/**********************************************************************
 *
 * Name:	os_UNIX.h
 * Written by:  M. Gatlin, mgatlin@ichips.intel.com
 * Date:        04/28/97
 * Description: operating system-specific routines - header file 
 *
 *********************************************************************/

#if (defined(_PROTOTYPES) || defined(__STDC__) || defined(__cplusplus)) && !defined(_NO_PROTO)
#undef  PARAMS
#define PARAMS(args) args
#else
#undef  PARAMS
#define PARAMS(args) ()
#endif
#ifdef __cplusplus
  extern "C" {
#endif

#include <time.h>
#include <unistd.h>

/******************************* TYPEDEFS ****************************/

typedef struct MEMINFO
{
   struct MEMINFO *pNext;
   void           *pStart;
   size_t          Size;

} tMemInfo;

typedef int boolean;

/************************* FUNCTION PROTOTYPES ***********************/

/****
 ****
 ****    T I M E   F U N C T I O N S 
 ****
 ****/

 time_t   osGetSystemTime   PARAMS((time_t inOffset));
 time_t   osParseSystemTime PARAMS((const char* inTimeString));
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
 time_t   voFastTime        PARAMS((void));   
#endif
    
/****
 ****
 ****    M E M O R Y   F U N C T I O N S 
 ****
 ****/

 void*         osMalloc        PARAMS((size_t inMemRequested));
 void*         osRealloc       PARAMS((void* inMemPtr, size_t inMemRequested));
 void          osFree          PARAMS((void* inMemPtr));
 unsigned long osGetMemUsed    PARAMS((void));
 unsigned long osGetSysMemUsed PARAMS((void));
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
 unsigned long voMemoryUsed    PARAMS((void));
#endif
    
/****
 ****
 ****    F I L E   F U N C T I O N S 
 ****
 ****/

 boolean osFileExists       PARAMS((char* inFileName));
 boolean osCreateFile       PARAMS((char* inFileName));
 boolean osCreateDir	    PARAMS((char* inFileName));
 boolean osFileIsDir        PARAMS((char* inFileName));
 boolean osFileIsReadable   PARAMS((char* inFileName));
 boolean osFileIsWritable   PARAMS((char* inFileName));
 boolean osFileIsExecutable PARAMS((char* inFileName));
 boolean osFileIsRW         PARAMS((char *inFileName));
 boolean osFileIsRO         PARAMS((char *inFileName));
 long    osFileSize         PARAMS((char* inFileName));
 char    osGetDirChar       PARAMS((void)); 
 char*   osGetFileOwner     PARAMS((char* inFileName));
 
/****
 ****
 ****   S Y S T E M   F U N C T I O N S 
 ****
 ****/

 int           osCopyFile         PARAMS((char* inFileSource, char* inFileDest));
 char*         osCopyString       PARAMS((char* inString));

 char*         osSimplifyPath     PARAMS((char* inPathName, char* inBasePath));
 int           osRemoveFile       PARAMS((const char *fileName));
 int           osRenameFile       PARAMS((const char *src, const char *dest));

 int           osSystem           PARAMS((char* inCommand));
 int           osExit             PARAMS((int inExitStatus));

 pid_t         osGetProcessID     PARAMS((void));
 uid_t         osGetUserID        PARAMS((void));
 char*         osGetUsername      PARAMS((void));
 boolean       osCreateSymLink    PARAMS((char *path1, char *path2));
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
 char*         voSimplifyFileName PARAMS((const char *inPathName, const char *inBasePath));
#ifdef OH_MACH_LINUX
 int 	       linux_system       PARAMS((char *command));
#endif
#endif
    
/****
 ****
 ****   E R R O R   F U N C T I O N S 
 ****
 ****/

 int     osDisplayInfoMsg(char *inFmtString, ...);
 int     osBuildWarnMsg(char *inFmtString, ...);
 void    osDisplayErrMsg(char *inFmtString, ...);

 char*   osDeleteWarnMsg  PARAMS((void));
 void    osDisplayWarnMsg PARAMS((void));


int isRegularFile(const char* path);

#ifdef __cplusplus
  }
#endif

#endif /* ifndef OS_FUNCTIONS_H */
