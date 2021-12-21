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
# $Id: dmsC_reports.c,v 1.1.2.1 1997/08/08 23:06:17 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

/*
//----------------------------------------------------------------------------
//            (C) Copyright Intel Corporation, 1994
//
//        Licensed material --  Program property of Intel
//        Corporation All Rights Reserved
//
//        This program is the   property of Intel  and is
//        furnished  pursuant  to  a    written   license
//        agreement.  It may not  be used, reproduced, or
//        disclosed  to others  except in accordance with
//        the terms and conditions of that agreement.
//
//        Intel Design Technology Copyright reserved 1994
//        Date: 08/07/97 
//
//        Authors: Sorin Blebea
//
//        File: dmsC_reports.c -- reporting functions used internally by idms
//                  
//        Whenever possible they were implemented in ANSI C (translated from
//        an ancient K&R very bad written C).
//        All the functions in this file should pass a code review.
//        Some of them are weird because of the huge amount of bad 
//        legacy code inherited.
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/types.h>
#include <sys/time.h>
#include <pwd.h>

#ifdef HPPA
#include <unistd.h>
#endif

/* Variable # of params -- different for C/C++ */

#include <stdarg.h>

#include <dmsC_defines.h>
#include <dmsC_internal_functions.h>
#include <dmsC_reports.h>
#include <dmsC_functions2.h>
#include <fwierr.h>
#include <fwimsg.h>

#include <idb_functions.h>

/*-----------------------*
 * Global variables used *
 *-----------------------*/

#include <dmsC_new_vars.h>
#include <dmsC_new.h>

/*------------------*
 * Static variables *
 *------------------*/

static char *_tmpBuff1 = 0;
static int  _LtmpBuff1 = 0;
static int  _log_already_failed = 0;


/*---------------------------*
 * Internally used functions * 
 *---------------------------*/

/*-----------------------*
 * Functions definitions *
 *-----------------------*/

/*-----------------------------------------------------------------------
 * dmsGenErrorCode 
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars: 
 *    Global vars:
 *
 * Uses:
 *    Global var: dmsLastErrorNumber 
 *
 *
 * Description: Unnecessary function used by the code all over the place,
 *              should disappear, but for that all its calls need to be
 *              eliminated.
 *-----------------------------------------------------------------------*/

int dmsGenErrorCode(int noError)
{
  return (noError)?idmsSuccess:dmsLastErrorNumber;
} 


/*-----------------------------------------------------------------------
 * dmsFormPrintPath 
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars: The last message buffer is modifed !!
 *    Global vars:  
 *
 * Uses:
 *    Global var: 
 *
 *
 * Description: Stores the entire dmspath to be printed in the last message
 *             variable.
 *-----------------------------------------------------------------------*/


static void dmsPrintPolicy(int i, int j, const char *name, const INFO *policy)
{
   struct elem *ptr;

   dmsCVarAppendToLastMessage(1024, " \n%d.%s (%s)\n", j, name, policy[i].name);
   ptr = policy[i].next;
   while (ptr) {
      dmsCVarAppendToLastMessage(1024, "\t%s (%s)\n", action[ptr->opt%10], 
                                 ptr->dirLib);
      ptr=ptr->next;
   }
}


static void print_info_table(const INFO *table, int table_count,
               const char *prompt, const char *tName, const char *vName) 
{
   int i;
   int j = 1;

   /* preserving the past */

   dmsCVarAppendToLastMessage(1024, " \n%s Entry:\n", prompt);
   if (strcmp(vName,"*") && strcmp(tName,"*")) {
      i = findPolicyEntry(table, table_count, tName, vName);
      dmsPrintPolicy(i, j, prompt, table);
   } 
   else {
      for (i = table_count-1; i>=0; i--){
         if (match_for_tool_n_version(table, i, tName, vName)) {
           dmsPrintPolicy(i, j, prompt, table);
           j++;
         }
      }
   }
}



void dmsFormPrintPath(char *tName, char *vName)
{
   int i;
   struct elem *ptr;

   dmsCSetLastMessage("");
   dmsCVarAppendToLastMessage(1024, " \nDMSPATH: %s\nTool Name: %s\n",
                              dmsPathFileName, tName);

   if (debugDmsOn) { /* VARIABLE */
      dmsCVarAppendToLastMessage(1024, " \nvariable Count=%d\n", varCount);
      for (i=0; i<varCount; i++) {
         dmsCVarAppendToLastMessage(1024, " \n%d.variable %s:\n", i+1, 
                                    var[i].name);
         ptr=var[i].next;
         while (ptr) {
            dmsCVarAppendToLastMessage(1024, "\t%s %2d (%s)\n",
               action[ptr->opt%10], ptr->opt, ptr->dirLib);
	    ptr=ptr->next;
         }
      }
   }

   /* THIS LOOKS WRONG BUT LETS LEAVE IT SO FOR NOW */

   if (!strcmp(vName,"*") || !strcmp(tName, "*")) {
      for (i=0; i<incCount; i++) { /* INCLUDE */
         dmsCVarAppendToLastMessage(1024, " \n%d.include (%s)\n", i+1,
                                    include[i]);
      }
   }

   print_info_table(libpath, libpathCount, "libpath", tName, vName);
   print_info_table(tconfig, tconfigCount, "tconfig", tName, vName);
   print_info_table(worklib, worklibCount, "worklib", tName, vName);
   print_info_table(workdir, workdirCount, "workdir", tName, vName);
   print_info_table(rwpaths, rwpathsCount, "rwpaths", tName, vName); 
}

 
/*-----------------------------------------------------------------------
 * dmsReportMsgIfAny
 *
 * No static/global vars affected.
 *
 * Description:
 *   Just prints two messages at a time instead of one and it also clears
 *   the content of the buffers. Not at all necessary but needed at least
 *   for now because its removal will cause lots of code changes.
 *   Does not print if string empty or null pointer.
 *
 *-----------------------------------------------------------------------*/


void dmsReportMsgIfAny(char *message, char *outString)
{
   if (message){
      if (strlen(message) > 0){
          dmsPrintResult(stderr, "%s",message);
          strcpy(message,"");
          if (outString){
             if (strlen(outString) > 0){
                dmsPrintResult(stderr,"%s\n",outString);
                strcpy(outString,"");
             }
          }
          dmsPrintResult(stderr," \n");
      } 
   }
}


/*-----------------------------------------------------------------------
 * dmsPrintIdmsLog 
 *
 * VARIABLE # OF ARGUMENTS
 *
 * Modifies:
 *    Static vars:  _tmpBuff1, _LtmpBuff1, _log_already_failed
 *    Global var:   idmsLogFilePtr
 *
 * Uses:
 *    Global var: dmsAppName, dmsDoLogAfterAll
 *
 *
 * Description: writes in the log file. Opens it if possible. Ignores it
 * if DMSLOGFILE env var not set uses "."
 * NOTE that DMSLOGFILE is actually the *DIRECTORY NAME*
 *-----------------------------------------------------------------------*/

void dmsPrintIdmsLog(char *format, ...)
{
   int rc = check_for_buff_space(&_tmpBuff1, &_LtmpBuff1,
            strlen(dmsAppName) + strlen("DMSPATH") + 1);

   char *idmsLogFileName;
   time_t ct;
   char *envp;

   va_list ptr;

  /* basically this means that DMSLOGFILE is not set */

   if (!dmsDoLogAfterAll)
       return;

   if (!idmsLogFilePtr && !_log_already_failed) {
     /* Create iDMS log file pointer for log */
     envp = dmsAppGetEnv("DMSLOGFILE");
     idmsLogFileName = (char *)malloc(strlen(envp)+1 +1 + 
                       strlen("iDMS.log.") + 10);
     if (envp)
       sprintf(idmsLogFileName,"%s/iDMS.log.%d", envp, getpid());
     else
       sprintf(idmsLogFileName,"./iDMS.log.%d",getpid());

     if (idmsLogFilePtr = fopen(idmsLogFileName, "w")) {
        ct=time(0);
        fprintf(idmsLogFilePtr,"========== iDMS LOG ==========\n");
        fprintf(idmsLogFilePtr,"Version: %s\n", DMSVERSION);
        fprintf(idmsLogFilePtr,"User Name: %s\n", getpwuid(getuid())->pw_name);
        fprintf(idmsLogFilePtr,"Process ID: %d\n", getpid());
        strcpy(_tmpBuff1,dmsAppName);
        strcat(_tmpBuff1,"DMSPATH");
        if (getenv(_tmpBuff1))
           fprintf(idmsLogFilePtr, "%sDMSPATH: %s\n", dmsAppName,
              getenv(_tmpBuff1));
        if (getenv("DMSPATH"))
           fprintf(idmsLogFilePtr, "DMSPATH: %s\n", getenv("DMSPATH"));
        fprintf(idmsLogFilePtr,"Time: %s",ctime(&ct));
        fprintf(idmsLogFilePtr,"==============================\n");
        fflush(idmsLogFilePtr);
    
     }
     else{ 
         _log_already_failed = 1;
        fprintf(stderr, "\n*IDMS ERROR* -- Cannot open log file %s -- will use stderr!\n",
                idmsLogFileName);
        idmsLogFilePtr = stderr;
     }   
     free(idmsLogFileName);
  }

  va_start(ptr, format);

  vfprintf(idmsLogFilePtr, format, ptr);
  va_end(ptr);
  fflush(idmsLogFilePtr);
}


/*-----------------------------------------------------------------------
 * dmsPrintResult
 *
 * VARIABLE # OF ARGUMENTS
 *
 * Modifies:
 *    Static vars:  _tmpBuff1, _LtmpBuff1
 *
 * Uses:
 *    define MAXBUFSIZE.
 *
 * Description: prints to the specified file. Basically a fprintf after the
 *    function was modified. Left alive because it might have some merit
 *    when we move to NT.
 *
 * Bad function, Bad function, ... it can overflow if the user really 
 * wants. It is still here because of the ancient idms code.
 *-----------------------------------------------------------------------*/

void dmsPrintResult(FILE *port, char *format, ...)
{
  int rc = check_for_buff_space(&_tmpBuff1, &_LtmpBuff1, MAXBUFSIZE);
  va_list         ptr;
  va_start(ptr, format);

  vsprintf(_tmpBuff1, format, ptr);

  va_end(ptr);

  fprintf(port, _tmpBuff1);
  fflush(port);
}


/*-----------------------------------------------------------------------
 * dmsFormMessage 
 *
 * Parameters:
 *    type   - type of message as described in fwimsg.h
 *    number - index in table -- if number == 0, reports last dm vendor message
 *    va_dcl - list to be passed to vsprintf 
 *
 * Modifies:
 *    Static vars: 
 *    Global vars:
 *                dmsLastErrorNumber
 *
 * Uses:
 *    Global var: 
 *                messageHeader
 *
 *
 * Description:  The function formally known as "DMSMESSAGE"
 *               Reports 0 if failure, non zero otherwise
 *
 *-----------------------------------------------------------------------*/

static char *_dmsF_localBuff = 0;
static int  _len__dmsF_localBuff = 0;
int dmsFormMessage(const char * cfunction, const char * cfile, const int cline, int type, int number, ...)
{
  char *lm_pnt, *tmp, *dmv_pnt;
  va_list         ptr;
  va_start(ptr, number);
  if (debugDmsOn) {
    dmsPrintIdmsLog("*IDMS DEBUG* %s %s %d\n", cfunction, cfile, cline);
    fprintf(stderr, "*IDMS DEBUG* %s %s %d\n", cfunction, cfile, cline);
  }
  

  /* Sets last error number */

  dmsLastErrorNumber = number;
  lm_pnt = (char *)dmsCGetLastMessage();
     /* this will alloc. space also if necessary */

  if (number == 0){ /* changed from original code, never seem to be used */    
     dmsCSetLastMessage(idb_get_cdn_vendor_message());
     idb_set_cdn_vendor_message("");
     if (debugDmsOn)
     dmsPrintIdmsLog("*IDMS DEBUG* %s\n", dmsCGetLastMessage());
      return 1;
  }

  check_for_buff_space(&_dmsF_localBuff, &_len__dmsF_localBuff, MAXBUFSIZE);

  if (messageHeader)
     strcpy(_dmsF_localBuff, dmsMessage[type]);
  else 
     _dmsF_localBuff[0] = '\0';

  tmp = _dmsF_localBuff;
  strcat(tmp, dmsMessage[number]);
  vsprintf(lm_pnt, tmp, ptr);
  va_end(ptr);

  /* Now add also the message from DM vendor if necessary */

  if (type!=OKTYPE) {
    if (*(dmv_pnt = (char *)idb_get_cdn_vendor_message()) != 0) /* there is a message */
      strcat(lm_pnt, idb_get_cdn_vendor_string());
      strcat(lm_pnt, dmv_pnt);
  }
  idb_set_cdn_vendor_message("");
  idb_get_record_dm_vendor_string(0);
  if (debugDmsOn)
     dmsPrintIdmsLog("*IDMS DEBUG* %s\n",dmsCGetLastMessage());
  return 1;
}
