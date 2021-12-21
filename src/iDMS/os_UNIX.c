/*
############# Intel Corporation Confidential information. #############
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
# $Id: os_UNIX.c,v 1.1.2.10 1998/01/07 19:31:17 mgatlin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

/**********************************************************************
 *
 * Name:	os_UNIX.c
 * Written by:  M. Gatlin, mgatlin@ichips.intel.com
 * Date:        05/05/97
 * Description:	Operating System-specific functionality used by Coral.
 * 
 * Exportable functions in this package:
 *
 * Time Functions               Description
 * ---------------------------  ---------------------------------------
 * osGetSystemTime              Gets current time, returns +/- offset 
 * osParseSystemTime            Converts time string into time_t value
 * voFastTime                   Gets current time.
 *
 * Memory Functions             Description
 * ---------------------------  ---------------------------------------
 * osMalloc			Allocates memory / records allocation
 * osRealloc                    Reallocates memory / records allocation
 * osFree			Frees memory / records deallocation
 * osGetMemUsed			Returns amount of memory allocated
 * osGetSysMemUsed		Returns amount of system memory used
 * voMemoryUsed                 Returns amount of memory allocated
 *
 * File Functions               Description
 * ---------------------------  ---------------------------------------
 * osFileExists                 Checks if file exists
 * osCreateDir			Creates the directory
 * osCreateFile                 Creates the file
 * osFileIsDir                  Checks if file is directory
 * osFileIsReadable             Checks if file is readable
 * osFileIsWritable             Checks if file is writable
 * osFileIsRW                   Checks if file is both R/W 
 * osFileIsRO                   Checks if file is R/O
 * osFileIsExecutable           Checks if file is executable
 * osFileSize                   Gets file size (in bytes)
 * osGetDirChar                 Gets directory separator character
 * osGetFileOwner               Gets the owner of a file.   
 *
 * Error Functions              Description
 * ---------------------------  ---------------------------------------
 * osDisplayInfoMsg             Displays an informational message
 * osBuildWarnMsg               Creates warning message in msg buffer
 * osDisplayErrMsg              Displays error message and aborts
 * osDeleteWarnMsg              Deletes warning message from msg buffer     
 * osDisplayWarnMsg             Force display of message in msg buffer
 *
 * System Functions             Description
 * ---------------------------  ---------------------------------------
 * osCopyFile                   Copies a file.
 * osCopyString                 Copies a string (allocates storage).
 * osSimplifyPath               Make a relative path an absolute path.
 * osRemoveFile                 Removes a file (NOT A DIRECTORY!)
 * osRenameFile                 Renames a file
 * osSystem			Makes a call to the native OS.
 * osExit                       Exits program.
 * osGetProcessID               Gets process ID information.
 * osGetUserID                  Gets user ID information.
 * osGetUsername                Gets username information.
 * osCreateSymLink		Creates a Symbolic link
 * voSimplifyFileName           Returns a simplified file name.
 *********************************************************************/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <malloc.h>
#include <time.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef abs
#undef abs
#endif
#ifndef OH_MACH_LINUX
#include <macros.h>
#else
#include <errno.h>
#endif
#include <stdarg.h>

#include <os_functions.h>

#define TRUE  1
#define FALSE 0

#define SUCCESS 0
#define FAILURE -1

#define BUFSZ    2048
/* need to change from PATH_MAX to PATH_MAX_L because it's 
   define in /usr/include/linux/limits.h */
#define PATH_MAX_L 1023
#define NAME_MAX 255

/* Local variables */

static tMemInfo *mMemRecPtr = NULL;	/* Memory allocation tracking */
static char	mMsgBuffer[BUFSZ];	/* Message Buffer */
static char	mTmpBuffer[BUFSZ];	/* Temporary Message Buffer */

/* Local prototypes */

static unsigned long osauxUpdateMemUsed(size_t);
static int           osauxCreateMemRecord(void*, size_t);
static void          osauxDeleteMemRecord(void*);

static struct passwd *osauxGetPasswordInfo(uid_t inUid); 

/****
 ****
 ****    T I M E   F U N C T I O N S 
 ****
 ****/


/******************************************************************************
 * Name:		osGetSystemTime             
 * Description:		Gets the time from the system, applies offset and 
 *                      returns value. An offset of '0' (zero) returns the 
 *                      current time.
 * Parameters:		inOffset       
 * Returns:		Current date/time +/- offset (time_t)                  
 * Global data:		NONE      
 * Call Assumptions:	
 */
time_t osGetSystemTime(time_t inOffset)
{
   return(time(0)+inOffset);
} /* osGetSystemTime */



/******************************************************************************
 * Name:		osParseSystemTime             
 * Description:		Reads a text string in the format
 *
 *                      WWW MMM DD HH:MM:SS YYYY\n\0  (26 chars including NULL)
 *
 *                      and converts it to an equivalent time_t value.
 *
 * Parameters:		inTimeString       
 * Returns:		date/time (time_t)                  
 * Global data:		NONE      
 * Call Assumptions:
 *	
 * Assumes that the <time.h> header file contains declarations of all relevant
 * functions and externals, including the tm structure, which includes the 
 * following members:
 *
 *         int tm_sec;      seconds after the minute - [0,61]
 *         int tm_min;      minutes after the hour - [0,59]
 *         int tm_hour;     hours - [0,23]
 *         int tm_mday;     day of month - [1,31]
 *         int tm_mon;      month of year - [0,11]
 *         int tm_year;     years since 1900
 *         int tm_wday;     days since Sunday - [0,6]         - IGNORED
 *         int tm_yday;     days since January 1 - [0,365]    - IGNORED
 *         int tm_isdst;    daylight savings time flag        - NEGATIVE VALUE
 */
time_t osParseSystemTime(const char* inTimeString)
{
   struct tm tmTimeStruct;
   char*     pCurStr;
   char      sStrBuf[26];        /* size of valid portion of inTimeString */
   
   strcpy(sStrBuf,inTimeString); /* no destructive side-effects allowed   */
   
   /* Get Day of Week - IGNORED */
   pCurStr = strtok(sStrBuf," ");  

   /* Get Month of Year */
   pCurStr = strtok(NULL," ");
   if(!strcmp(pCurStr,"Jan")) 
      tmTimeStruct.tm_mon = 0;
   else if(!strcmp(pCurStr,"Feb"))
      tmTimeStruct.tm_mon = 1;
   else if(!strcmp(pCurStr,"Mar"))
      tmTimeStruct.tm_mon = 2;   
   else if(!strcmp(pCurStr,"Apr"))
      tmTimeStruct.tm_mon = 3;      
   else if(!strcmp(pCurStr,"May"))
      tmTimeStruct.tm_mon = 4;      
   else if(!strcmp(pCurStr,"Jun"))
      tmTimeStruct.tm_mon = 5;      
   else if(!strcmp(pCurStr,"Jul"))
      tmTimeStruct.tm_mon = 6;      
   else if(!strcmp(pCurStr,"Aug"))
      tmTimeStruct.tm_mon = 7;      
   else if(!strcmp(pCurStr,"Sep"))
      tmTimeStruct.tm_mon = 8;      
   else if(!strcmp(pCurStr,"Oct"))
      tmTimeStruct.tm_mon = 9;      
   else if(!strcmp(pCurStr,"Nov"))
      tmTimeStruct.tm_mon = 10;      
   else if(!strcmp(pCurStr,"Dec"))
      tmTimeStruct.tm_mon = 11;
   else
      tmTimeStruct.tm_mon = -1;  /* Assign a default value */ 
   
   /* Get Day of Month, and assign to tm_mday */
   pCurStr = strtok(NULL," ");  
   tmTimeStruct.tm_mday = atoi(pCurStr);
  
   /* Get Hour of Day */
   pCurStr = strtok(NULL,":");  
   tmTimeStruct.tm_hour = atoi(pCurStr);

   /* Get Minutes of Hour */
   pCurStr = strtok(NULL,":");
   tmTimeStruct.tm_min  = atol(pCurStr);
   
   /* Get Seconds of Minute*/
   pCurStr = strtok(NULL," ");  
   tmTimeStruct.tm_sec  = atol(pCurStr);
  
   /* Get Year */
   pCurStr = strtok(NULL,"\n");  
   tmTimeStruct.tm_year = atol(pCurStr) - 1900; /* requires years since 1900 */

   /* Set negative value to force mktime to determine daylight savings time  */
   tmTimeStruct.tm_isdst = -1;

   return(mktime(&tmTimeStruct));       /* mktime returns -1 if invalid date */
} /* osParseSystemTime */


/****
 ****
 ****    M E M O R Y   F U N C T I O N S 
 ****
 ****/


/******************************************************************************
 * Name:		osMalloc             
 * Description:		Allocate memory.       
 * Parameters:		NONE
 * Returns:		Memory allocated (size_t)                
 * Global data:		NONE      
 * Call Assumptions:	NONE
 */
void* osMalloc(size_t inMemRequested)
{
   void* pNewMem;
   
   pNewMem = malloc(inMemRequested);
   if (pNewMem)
      osauxCreateMemRecord(pNewMem,inMemRequested);      

   return(pNewMem);	
} /* osMalloc */


/******************************************************************************
 * Name:		osRealloc             
 * Description:		Reallocate memory.       
 * Parameters:		NONE
 * Returns:		Memory allocated (size_t)                
 * Global data:		NONE      
 * Call Assumptions:	NONE
 */
void* osRealloc(void*  inMemPtr,
                size_t inMemRequested)
{
   void* pNewMem;
   
   pNewMem = realloc(inMemPtr, inMemRequested);
   if (pNewMem) {
      osauxDeleteMemRecord(inMemPtr);               /* Delete old record */
      osauxCreateMemRecord(pNewMem,inMemRequested); /* Create new record */     
   }

   return(pNewMem);	
} /* osRealloc */


/******************************************************************************
 * Name:		osFree
 * Description:		Frees Memory.       
 * Parameters:		NONE
 * Returns:		NONE
 * Global data:		NONE      
 * Call Assumptions:	NONE
 */
void osFree(void* inMemPtr)
{
   osauxDeleteMemRecord(inMemPtr);
   free(inMemPtr);
   
   return;
} /* osFree */


/******************************************************************************
 * Name:		osGetMemUsed             
 * Description:		Gets memory used.       
 * Parameters:		NONE
 * Returns:		Memory used (ptrdiff_t)                
 * Global data:		NONE      
 * Call Assumptions:	NONE
 */
unsigned long osGetMemUsed()
{
   return(osauxUpdateMemUsed(0));	
} /* osGetMemUsed */


/******************************************************************************
 * Name:		osauxUpdateMemUsed             
 * Description:		Updates memory used, based upon calls to osMalloc and
 *                      osFree.       
 * Parameters:		NONE
 * Returns:		Current memory used               
 * Global data:		NONE      
 * Call Assumptions:	NONE
 */
static unsigned long osauxUpdateMemUsed(size_t inMemDelta)
{
   static unsigned long svTotalMemoryUsed = 0; /* Encapsulated system variable */

   svTotalMemoryUsed += inMemDelta;
   return((unsigned long) svTotalMemoryUsed);	
} /* osGetMemUsed */


/******************************************************************************
 * Name:		osauxCreateMemRecord             
 * Description:		Creates a memory allocation record.       
 * Parameters:		NONE
 * Returns:		               
 * Global data:		mMemRecPtr      
 * Call Assumptions:	NONE
 */
static int osauxCreateMemRecord(void*  inMemPtr, 
                                size_t inMemSize)
{
   tMemInfo *MemTemp;
   
   MemTemp = (tMemInfo *) malloc(sizeof(tMemInfo)); /* Create the record   */
   if (MemTemp != NULL) {                           /* Populate the record */
      MemTemp->pStart = inMemPtr;
      MemTemp->Size   = inMemSize;
      MemTemp->pNext  = mMemRecPtr;
      mMemRecPtr      = MemTemp;
      
      osauxUpdateMemUsed(inMemSize);
   }

      return (MemTemp != NULL);                     /* Return status       */
} /* osCreateMemRecord */


/******************************************************************************
 * Name:		osauxDeleteMemRecord             
 * Description:		Deletes a memory allocation record.       
 * Parameters:		NONE
 * Returns:		               
 * Global data:		mMemRecPtr      
 * Call Assumptions:	NONE
 */
static void osauxDeleteMemRecord(void* inMemPtr)
{
   tMemInfo *MemTemp, 
            *MemPrev;

   MemPrev = NULL;
   for (MemTemp = mMemRecPtr; MemTemp != NULL; MemTemp = MemTemp->pNext) {
      if (MemTemp->pStart == inMemPtr) {
         if (MemPrev == NULL) 
            mMemRecPtr = MemTemp->pNext;
         else
            MemPrev->pNext = MemTemp->pNext;
         break;
      }
      MemPrev = MemTemp;
   }
   
   if (MemTemp != NULL) {
      osauxUpdateMemUsed(MemTemp->Size * -1);  /* Negative number for update */
      free(MemTemp);
   }
   
   return; 
} /* osDeleteMemRecord */


/******************************************************************************
 * Name:		osGetSysMemUsed             
 * Description:		Gets system memory used.       
 * Parameters:		NONE
 * Returns:		Memory used (unsigned long)                
 * Global data:		NONE      
 * Call Assumptions:	NONE
 */
unsigned long osGetSysMemUsed()
{
   struct mallinfo MallInfoData;

   MallInfoData = mallinfo();

   return(MallInfoData.uordblks);	
} /* osGetSysMemUsed */



/****
 ****
 ****    F I L E   F U N C T I O N S 
 ****
 ****/

/******************************************************************************
 * Name:		osFileExists             
 * Description:		Checks whether a file exists or not.       
 * Parameters:		inFileName - The full pathname of the file.
 * Returns:		TRUE if file exists, FALSE otherwise           
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
boolean osFileExists(char* inFileName)
{ 
  int res;
  res=access(inFileName, F_OK);
  if (!res)
    return (TRUE);
  else
    return (FALSE);
}

/******************************************************************************
 * Name:                osCreateDir
 * Description:         Creates a new directory.
 * Parameters:          inFileName - The full pathname of the dir.
 * Returns:             TRUE if file is created successfully, FALSE otherwise
 * Global data:         NONE
 * Call Assumptions:    NONE
 */
boolean osCreateDir(char *inFileName)
{

  int res;
  res=mkdir(inFileName, 0x1ff);
  return (res < 0)?FALSE:TRUE;
}


/******************************************************************************
 * Name:		osCreateFile             
 * Description:		Creates an new file.       
 * Parameters:		inFileName - The full pathname of the file.
 * Returns:		TRUE if file is created successfully, FALSE otherwise           
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
boolean osCreateFile(char *inFileName)
{
  int ret;
  ret = creat(inFileName, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
  if(ret < 0) {
    return FALSE;
  } else {
    close(ret);
    return TRUE;
  }
}


/******************************************************************************
 * Name:		osFileIsDir             
 * Description:		Checks whether a file is a directory or not.       
 * Parameters:		inFileName - The full pathname of the file.
 * Returns:		TRUE if file is a directory, FALSE otherwise           
 * Global data:		NONE
 * Call Assumptions:	inFileName has been simplified.
 */
boolean osFileIsDir(char* inFileName)
{ 
   struct stat StatBuf;
   stat(inFileName, &StatBuf);
   if(S_ISDIR(StatBuf.st_mode))
      return (TRUE);
   else
      return (FALSE);
}


/******************************************************************************
 * Name:		osFileIsReadable             
 * Description:		Checks whether a file is readable or not.       
 * Parameters:		inFileName - The full pathname of the file.
 * Returns:		TRUE if file is readable, FALSE otherwise           
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
boolean osFileIsReadable(char* inFileName)
{ 

  int res;
  res=access(inFileName, R_OK);
  if(!res)
    return (TRUE);
  else
    return (FALSE);
}


/******************************************************************************
 * Name:		osFileIsWritable
 * Description:		Checks whether a file is writable or not.       
 * Parameters:		inFileName - The full pathname of the file.
 * Returns:		TRUE if file is writable, FALSE otherwise           
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
boolean osFileIsWritable(char* inFileName)
{ 
  int res;
  res=access(inFileName, W_OK);
  if(!res)
    return (TRUE);
  else
    return (FALSE);
}

/******************************************************************************
 * Name:		osFileIsRW
 * Description:		Checks whether a file is readable and writable.       
 * Parameters:		inFileName - The full pathname of the file.
 * Returns:		TRUE if file is readable/writable, FALSE otherwise           
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
boolean osFileIsRW(char *inFileName)
{
  int res;
  res=access(inFileName, (R_OK|W_OK));
  return res ? FALSE : TRUE;
}


/******************************************************************************
 * Name:		osFileIsRO
 * Description:		Checks whether a file is readonly.       
 * Parameters:		inFileName - The full pathname of the file.
 * Returns:		TRUE if file is readonly, FALSE otherwise           
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
boolean osFileIsRO(char *inFileName)
{
   if (access(inFileName, R_OK))
      return FALSE; /* no read */
   return access(inFileName, W_OK) ? TRUE : FALSE; /* OK if NO write */
}


/******************************************************************************
 * Name:		osFileIsExecutable
 * Description:		Checks whether a file is executable or not.       
 * Parameters:		inFileName - The full pathname of the file.
 * Returns:		TRUE if file is executable, FALSE otherwise           
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
boolean osFileIsExecutable(char* inFileName)
{ 
  int res;
  res=access(inFileName, (X_OK));
  return res ? FALSE : TRUE;
}


/******************************************************************************
 * Name:		osFileSize
 * Description:		Gets the size of a file in bytes.       
 * Parameters:		inFileName - The full pathname of the file.
 * Returns:		SIZE if file is accessible, -1 otherwise           
 * Global data:		NONE
 * Call Assumptions:	inFileName has been simplified.
 */
long osFileSize(char* inFileName)
{ 
   struct stat StatBuf;
   stat(inFileName, &StatBuf);
   return (StatBuf.st_size);
}


/******************************************************************************
 * Name:		osGetDirChar
 * Description:		Gets directory separator character.       
 * Parameters:		NONE.
 * Returns:		Directory separator character.           
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
char    osGetDirChar()
{
   return '/';
}


/******************************************************************************
 * Name:		osGetFileOwner
 * Description:		Gets the owner of a file.       
 * Parameters:		inFileName - The full pathname of the file.
 * Returns:		Userid of file owner.           
 * Global data:		NONE
 * Call Assumptions:	inFileName has been simplified.
 */
char* osGetFileOwner(char* inFileName)
{ 
   struct stat StatBuf;
   stat(inFileName, &StatBuf);
   return (osauxGetPasswordInfo(StatBuf.st_uid)->pw_name);
}


/****
 ****
 ****   S Y S T E M   F U N C T I O N S 
 ****
 ****/

/******************************************************************************
 * Name:		osCopyFile
 * Description:		Copies inFileSource onto inFileDest.       
 * Parameters:		inFileSource - full pathname of the file copied from.
 *                      inFileDest   - full pathname of the file copied to.
 * Returns:		0 if file is copied successfully, -1 otherwise           
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
int osCopyFile(char* inFileSource, char* inFileDest)
{ 
   char    CommandBuf[BUFSZ];
   boolean fFirstFileOK,
           fSecondFileOK;
   int fStatus = -1;
   int     f;
   /* Verify that first file exists and is not a directory */
   if(osFileExists(inFileSource) && !osFileIsDir(inFileSource))
      fFirstFileOK = TRUE;
      
   /* Verify that if second file exists, it's not a directory */
   if (osFileExists(inFileDest))
   {   
      if (!osFileIsDir(inFileDest))
         fSecondFileOK = TRUE;
      else
         fSecondFileOK = FALSE;
   }
   else
      fSecondFileOK = TRUE;

   if(fFirstFileOK && fSecondFileOK) {

    f = fork();
    if (f == -1) return (FAILURE);
    if(f){
	  waitpid(f,&fStatus,0);
	} else {
      execl("/bin/cp", "cp",
        (getenv("DMSERVER_NO_PRESERVE_OWNERSHIP") ? "--preserve=mode,timestamps" :  "--preserve=mode,ownership,timestamps"),
         "-L", "-H", "-f", inFileSource, inFileDest, NULL);
	}
       //return(osFileExists(inFileDest) ? SUCCESS : FAILURE);  /* Successful copy */
  }
   return (WEXITSTATUS(fStatus)? FAILURE : SUCCESS);
}
 

/******************************************************************************
 * Name:		osCopyString
 * Description:		Copies inString to a newly allocated string space.       
 * Parameters:		inString - String to be copied.
 * Returns:		Returns a pointer to new string of it is copied 
 *                      successfully, returns VOID otherwise.
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
char* osCopyString(char* inString)
{  
   return(strdup(inString));
}


/******************************************************************************
 * Name:		osSimplifyPath
 * Description:		Returns the absolute pathname - strips ~, ./, ../ and
 *                      symbolic links.       
 * Parameters:		inPathname - path to be simplified.
 * Returns:		Returns absolute path if command is successful, NULL 
 *                      otherwise.
 * Global data:		NONE
 * Call Assumptions:	Last element in the path is assumed to be the filename.
 */

/*------------------------------------------------------------------------------------
 * No longer your grandfather's simplify Path function
 * 
 * rewritten by Sorin on 1/20/2010 to use the now readily available realpath()
 * 
 * If function is called with 2 parameters, the function will not blindly remove
 * the number of chars from inPath given by the second one
 *
 * For example: osSimplifyPath("/a/b/c/file", "ff") will not blindly remove 2 chars
 * thus creating half dirs like "/a/b/c/fi"
 *
 * On the other hand, if you do call it with osSimplifyPath("/a/b/c/ffff", "ffff")
 * it will return "/a/b/c"
 * 
 *------------------------------------------------------------------------------------*/


char* osSimplifyPath(char* inPathname, char* inBasepath)
{
   if (!inPathname) {
      return 0;
   }

   static char ret_buff[PATH_MAX+1]; /* realpath() needs a second parameter */
   static char tmp_buff[PATH_MAX+1]; /* if we need to process the input */
   static char tmp2[PATH_MAX+1];     /* if we detect ~ we need to save the user someplace */

   char *ip = tmp_buff;
   strcpy(tmp_buff, inPathname);

   /*-----------------------------*/
   /* leading and trailing spaces */
   /*-----------------------------*/

   char *end = ip + strlen(ip)-1;

   while (((*end == ' ') || (*end == '\t')) && (end >= ip)) {
      *end = '\0';
      end --;
   }

   while ((*ip == ' ') || (*ip == '\t')) {
      ip ++;
   } 

   /*---------------------------------*/ 
   /* get rid of the trailing slashes */
   /*---------------------------------*/

   char *ls = strrchr(ip, '/');
   if (ls) {
      if (*(ls+1) == '\0') { /* trailing slash */

        /* remove all trailing slashes like "/a/b/c///" becomes "/a/b/c" */
        
         while ((*ls == '/') && (ls >= ip)) {
            *ls = '\0';
             ls --; 
         }
      }
   }

   /*-------------------------------------------------*/
   /* expand ~ outside realpath since it does not do it/
   /*-------------------------------------------------*/


   if (ip[0] == '~') {
      /* let see if this is something like ~[/$] or it is ~user[/$] */
      char *beg = ip+1;
      char *end = ip+1;
      while ((*end != '\0') && (*end != '/')) { 
         end++; 
      }

      char *user = tmp2;
      user[0] = '\0';

      if (end == beg) {
         char *login_str = getlogin();
         /* if this fails, let's look in the password file */
         if (!login_str){
            struct passwd *pwd = getpwuid(geteuid());
            if (pwd)
              login_str = pwd->pw_name;
           }
           if (!login_str)
              strcpy(user, "unknown");
           else
              strcpy(user, login_str);
      }
      else {
         int ulen = end-beg;
         strncpy(user, beg, ulen);
         user[ulen] = '\0';
      }
      struct passwd *pwd = getpwnam(user); /* no such user */
      if (!pwd) {
         return 0;
      }
      strcpy(tmp2, pwd->pw_dir);
      strcat(tmp2, end);
      ip = tmp2;
   }

   /* see if the second parameter is null or empty */

     int just_call_it = (!inBasepath)?1:0;

   if (! just_call_it) {
      just_call_it = (strcmp(inBasepath, "") == 0);
   }

   if (just_call_it) {
      char *rc = realpath(ip, ret_buff);
      return rc;
   }

   /*-----------------------------------------------------*/
   /* see if we need to strip file name                   */
   /*   if we are here, inBasepath is not an empty string */
   /*-----------------------------------------------------*/

   ls = strrchr(ip, '/'); /* if found, we know that this is not at the very end of the path */

   if (ls) {
      if (strcmp(ls+1, inBasepath) == 0) {
         /* strip all slashes in between the path and the file name - most likely one ... */
          while ((*ls == '/') && (ls >= ip)) {
             *ls = '\0';
             ls --;
          }
      }
   } 

   return realpath(ip, ret_buff);

}
   
/*-----------------------------------------------------------------------
 * osRemoveFile 
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars: 
 *    Global vars:
 *
 * Uses:
 *    Global var: 
 *
 *
 * Description: In spite the name, it removes a file.
 *              Uses fopen to see if the file is not a directory and not
 *              to erase the entire world if somebody gives a mistaken
 *              parameter.
 *              Calls the ANSI remove() function. The weird undef below is
 *              because one of the multiple header files we are including
 *              above manages to redefine remove -- and yes, I checked,
 *              there are no other calls to remove in this file.
 *
 *              Does NOT recursivelly erase a directory
 *
 *                                             Sorin -- 08/12/97
 *-----------------------------------------------------------------------*/


#ifdef remove
#undef remove
#endif

int osRemoveFile(const char *fileName)
{
  struct stat _sbuff;
  int ret = -1;

  ret = lstat(fileName,&_sbuff); /* do not attempt to delete dir */
  if(S_ISDIR(_sbuff.st_mode) || ret) return -1;

  /* make writable */
  ret = chmod(fileName, _sbuff.st_mode|S_IRUSR|S_IWUSR);
  if(ret) return -1;
  
  return unlink(fileName);
}

/*-----------------------------------------------------------------------
 * osRenameFile
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description: Renames src file to new (dest) name.
 *              Returns 0 for success and -1 for failure 
 *
 *                                             Sorin -- 08/12/97
 *-----------------------------------------------------------------------*/

int osRenameFile(const char *src, const char *dest)
{
  int res;
  res= rename(src, dest);
  return res;
  
}


/******************************************************************************
 * Name:		osSystem
 * Description:		Generates a system call.       
 * Parameters:		inCommand - command to be executed
 * Returns:		Returns 0 if command is successful, an error code
 *                      otherwise.
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
int osSystem(char* inCommand)
{
  int res;
  res=system(inCommand);
  return res;

}


/******************************************************************************
 * Name:		osExit
 * Description:		Exits program.       
 * Parameters:		Exit status.
 * Returns:		NONE
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
int osExit(int inExitStatus)
{
   exit(inExitStatus);
}


/******************************************************************************
 * Name:		osGetProcessID
 * Description:		Gets process ID from system.       
 * Parameters:		NONE
 * Returns:		Process ID.
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
pid_t osGetProcessID(void)
{
   return(getpid());
}


/******************************************************************************
 * Name:		osGetUserID
 * Description:		Gets user ID from system.       
 * Parameters:		NONE
 * Returns:		User ID.
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
uid_t osGetUserID(void)
{
   return(getuid());
}


/******************************************************************************
 * Name:		osauxGetPasswordInfo
 * Description:		Gets password file info from system.       
 * Parameters:		NONE
 * Returns:		Password information struct.
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
static struct passwd *osauxGetPasswordInfo(uid_t inUid)
{
   return(getpwuid(inUid));
} 


/******************************************************************************
 * Name:		osGetUsername
 * Description:		Gets username info from system.       
 * Parameters:		NONE
 * Returns:		Pointer to Username string.
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
char *osGetUsername(void)
{
   return(osauxGetPasswordInfo(osGetUserID())->pw_name);
} 

/******************************************************************************
 * Name:                osCreateSymLink
 * Description:         Creates symlink (man symlink)
 * Parameters:          path1 path2 
 * Returns:             TRUE if file is created successfully, FALSE otherwise
 * Global data:         NONE
 * Call Assumptions:    NONE
 */
boolean osCreateSymLink(char *path1, char *path2)
{
  int res;
  res= symlink(path1, path2);
  return (res < 0)? FALSE:TRUE;
}


/****
 ****
 ****   E R R O R   F U N C T I O N S 
 ****
 ****/
 
 
/******************************************************************************
 * Name:		osDisplayInfoMsg
 * Description:		Displays a message.       
 * Parameters:		
 * Returns:		NONE
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
int osDisplayInfoMsg(char *inFmtString, ...)
{
   va_list	vlArgs;
   
   va_start(vlArgs, inFmtString);
   
   fprintf(stderr, "*INFO*: ");
   vfprintf(stderr, inFmtString, vlArgs);
   fprintf(stderr, "\n");   
   
   va_end(vlArgs);
   
   return(TRUE);
}

/******************************************************************************
 * Name:		osBuildWarnMsg
 * Description:		Creates a warning in the msg buffer.       
 * Parameters:		
 * Returns:		NONE
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
int osBuildWarnMsg(char *inFmtString, ...)
{
   va_list	vlArgs;
   
   va_start(vlArgs, inFmtString);

   vsprintf(mMsgBuffer, inFmtString, vlArgs);
   va_end(vlArgs);
   
   return(TRUE);
}


/******************************************************************************
 * Name:		osDisplayErrMsg
 * Description:		Displays an error message and aborts.       
 * Parameters:		
 * Returns:		NONE
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
void osDisplayErrMsg(char *inFmtString, ...)
{
   va_list	vlArgs;
   
   va_start(vlArgs, inFmtString);
   
   fprintf(stderr, "*ERROR*: ");
   vfprintf(stderr, inFmtString, vlArgs);
   fprintf(stderr, "\n");
   
   va_end(vlArgs);
   
   exit(TRUE);
}


/******************************************************************************
 * Name:		osDeleteWarnMsg
 * Description:		Deletes warning message from msg buffer       
 * Parameters:		
 * Returns:		NONE
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
char* osDeleteWarnMsg(void)
{
   strcpy(mTmpBuffer,mMsgBuffer);
   memset(mMsgBuffer,'\0',BUFSZ);   
   return(mTmpBuffer);
}


/******************************************************************************
 * Name:		osDisplayWarnMsg
 * Description:		Force display of message in msg buffer.       
 * Parameters:		
 * Returns:		NONE
 * Global data:		NONE
 * Call Assumptions:	NONE
 */
void osDisplayWarnMsg(void)
{
   sprintf(mTmpBuffer, "*WARNING*: %s\n", mMsgBuffer);
   fprintf(stderr, mTmpBuffer);
   return;
}

/******************************************************************************
 * Name:		voSimplifyFileName
 * Description:		Returns the absolute pathname - strips ~, ./, ../ and
 *                      symbolic links.       
 * Parameters:		inPathname - path to be simplified.
 * Returns:		Returns absolute path if command is successful, NULL 
 *                      otherwise.
 * Global data:		NONE
 * Call Assumptions:	Last element in the path is assumed to be the filename.
 */
char* voSimplifyFileName(const char *inPathName, const char *inBasePath)
{
  return (osSimplifyPath((char*)inPathName, (char*)inBasePath));
}


/******************************************************************************
 * Name:		voMemoryUsed
 * Description:		Gets memory used.       
 * Parameters:		NONE
 * Returns:		Memory used (ptrdiff_t)                
 * Global data:		NONE      
 * Call Assumptions:	NONE
 */
unsigned long voMemoryUsed()
{
   return(osGetMemUsed());	
} /* voMemoryUsed */


/******************************************************************************
 * Name:		voFastTime
 * Description:		Gets the current time from the system, 
 * Parameters:		NONE
 * Returns:		Current date/time (time_t)                  
 * Global data:		NONE      
 * Call Assumptions:	NONE
 */
time_t voFastTime()
{
   return(osGetSystemTime(0));
} /* voFastTime */

/* this is a temp solution to fix the synchronicity
   hanging after making system call!
 */
int linux_system (char *command) {
    int pid, status;

    if (command == 0)
        return 1;
    pid = fork();
    if (pid == -1)
        return -1;
    if (pid == 0) {
        char *argv[4];
        argv[0] = "sh";
        argv[1] = "-c";
        argv[2] = command;
        argv[3] = 0;
        execv("/bin/sh", argv);
        exit(127);
    }
        do {
            if (waitpid(pid, &status, 0) == -1) {
                if (errno != EINTR)
                    return -1;
            } else
                return status;
        } while(1);
}

int isRegularFile(const char* path){
  struct stat s;
  if (lstat(path,&s) < 0) return 0;
  return S_ISREG(s.st_mode);
}
