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
# $Id: fwusertrigger.c,v 1.1.2.3.4.1.10.7 1997/12/12 22:18:26 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
/*
#Revision 1.1  93/01/04  11:11:48  dhejmad
#Initial revision
#
*/
#include "fwidms.h"
#include "attr.h"
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "fwtable.h"
#include "fwimsg.h"
#include <time.h>
#include <sys/types.h>
#include <idb_functions.h>

#ifndef DISABLE_SYNCHRONICITY
 #include <scapi.h>
#endif

/*----------------------*
 * Functions prototypes *
 *----------------------*/

#include <dmsC_reports.h>
#include <dmsC_internal_functions.h>


/* global variable to hold the name of the trigger
** function, the return value of the trigger
*/
Boolean globalSkillResponse = 0;
static char *tclTriggerName=0;
static char *skillTriggerName=0;

/* to hold the value of whether we are in nullgt */
int idmsNullGT = 0;

extern Boolean globalAttrDisableFlag;

/*------------------------------------------------------------------------------------------------------

First of all, following variable should be external not a copy and
get set from fwtrigger.c

Secondly, we do not use Cadence pretriggers any more.

For whatever reasons, when compiled optimized is set to 0 and unoptimized set to 1 

so that's why the code "failed to execute pretriggers" in the -O2 mode.

Disabling permanently. If later we still need the triggers, will add another variable to enable them
or revert the code.


Sorin 05/07/08
------------------------------------------------------------------------------------------------------*/


/*again, change this to extern if needed and take the 1 out! */
static Boolean globalUserTriggerDisableFlag = 1;

extern int debugDmsOn;

/* internal procedure called by all dms event procedure
** which will cause the appropriate even to be done
*/

static char skillStringToEval[10000];
static char tclStringToEval[10000];

Boolean idmExecuteUserTrigger(char *event, Boolean preTrigger, char *libName,
                              char *cellName, char *viewName,
                              char *versionName, char *path)
{
	idmTable tablePtr;
	int skillTrigReadStatus, tclTrigReadStatus;
        HRESULT dssRetHandle;
        char *dssRet;

        if (debugDmsOn){
          dmsPrintIdmsLog("<<*C-CALL*>> - idmExecuteUserTrigger(%s, %d, %s, %s, %s, %s, %s) - flag = %d\n",
                          event,  preTrigger, libName, cellName, viewName, versionName, path, globalUserTriggerDisableFlag);
        }

        /* if the triggers are disabled simply return success */
        if (globalUserTriggerDisableFlag)
                return(TRUE);

        int nullgt = 0;
        if (idmsNullGT == 1) {
           nullgt = 1;
        }

	/* the function will be in either the second (pre) or third
	** (post) column */

        int column = 2;
        if (! preTrigger) { 
           column = 3;
        }

	/* allocate enough space */
        if (!skillTriggerName) {
          if (!(skillTriggerName = (char *)malloc(BUFFER_LENGTH)))
            return 0;
 	}
        if (!tclTriggerName) {
          if (!(tclTriggerName = (char *)malloc(BUFFER_LENGTH)))
            return 0;
        }

	skillTrigReadStatus = idmGetColumn(IDMS_USERTRIGGER_TABLE,event,column,
							&skillTriggerName);
	tclTrigReadStatus = idmGetColumn(IDMS_USERTRIGGER_TABLE_TCL,event,column,
							&tclTriggerName);

	/* if table does not exist OR key does not exist simply return true */
	if(skillTrigReadStatus != idmsSuccess && tclTrigReadStatus != idmsSuccess){
           if (debugDmsOn){
             dmsPrintIdmsLog("*IDMS DEBUG* - nothing in table\n");
           }
	   return(TRUE);
        }

	/* now that we have the user trigger name simply execute the function
	** provided the function is NOT a "-" 
	*/

        if (debugDmsOn){
             dmsPrintIdmsLog("*IDMS DEBUG* - skillTriggerName: %s, tclTriggerName: %s\n", skillTriggerName, tclTriggerName);
        }

	if (strcmp(skillTriggerName,"") == 0)
	  skillTrigReadStatus = FALSE;
	else
	  skillTrigReadStatus = TRUE;

	if (strcmp(tclTriggerName,"") == 0)
	  tclTrigReadStatus = FALSE;
	else
	  tclTrigReadStatus = TRUE;

	/* the 4.3 code was doing this */
	if(!skillTrigReadStatus && !tclTrigReadStatus)
	  return(TRUE);

	if(skillTrigReadStatus) {
	  sprintf(skillStringToEval,"dssSkillEval {%s(\"%d\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\")}", 
		skillTriggerName, nullgt, cellName, viewName, path, libName, versionName);
	} 
	if(tclTrigReadStatus) {
	  sprintf(tclStringToEval,"%s \"%d\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"", 
		tclTriggerName, nullgt, cellName, viewName, path, libName, versionName);
	} 

	globalSkillResponse = FALSE;

	if(idmsNullGT == 1 || idmsNullGT == 2) {

	  if(tclTrigReadStatus) {
	    dssRetHandle = sapiExecuteCmd(tclStringToEval, &dssRet);
	    globalSkillResponse = SUCCEEDED(dssRetHandle)? TRUE : FALSE;
            if (debugDmsOn){
               dmsPrintIdmsLog("*IDMS DEBUG* - returned %d for call: %s\n",  globalSkillResponse, tclStringToEval);
            }
	  }

	  if(skillTrigReadStatus) {
	    dssRetHandle = sapiExecuteCmd(skillStringToEval, &dssRet);
	    globalSkillResponse = SUCCEEDED(dssRetHandle)? TRUE : FALSE;
            if (debugDmsOn){
               dmsPrintIdmsLog("*IDMS DEBUG* - returned %d for call: %s\n",  globalSkillResponse, skillStringToEval);
            }
	  }
	  
	}

	return(globalSkillResponse);
}


/* const strings to be evaled to init trigs */
char *tclEvalStr1 = "dssSkillEval { unless(boundp('dssvInit) load(strcat(getShellEnvVar(\"SYNC_DIR\") \"/cds/skill/dssInit.il\")))}";

char *tclEvalStr2 = "dssSkillEval { unless(boundp('dssTrigRegistered) load(strcat(getShellEnvVar(\"SYNC_DIR\") \"/cds/skill/dssTrigger.il\")))}";

char *tclEvalStr3 = "dssSkillEval { unless(boundp('dssTrigPID) && dssTrigPID dssTrigStartIPCChan())}";

/* char *tclEvalStr4 = "trigger enable skilltrigger"; */
char *tclEvalStr4 = "dssSkillEval {dssCmd(\"trigger create skilltrigger -require type {preObject postObject} -require command {ci co} -tcl_script {dssSkillTrig}\")}";

/* procedure called at initialization to load the 
** user defined skill file - or cause it to be loaded in
** opus
*/
Boolean idmInitUserTriggers(int nullgt) {
    char *skillStringToEval, *tclStringToEval;
    char *skillFile , *tclFile;
    char *dssRet;
    int bufferLength;
    HRESULT dssRetHandle;
    HRESULT dssRetHandleTcl;
    skillStringToEval = tclStringToEval = NULL;

    if (globalUserTriggerDisableFlag)
	return(FALSE);

	/* set the nullGT var to TRUE/FALSE for 
	** all idms modules 
	*/
	idmsNullGT = nullgt;

    /* check if the env variable to disable user 
    ** triggers is set */
    if (dmsAppGetEnv(userTriggerDisableEnvVarName)) {
	globalUserTriggerDisableFlag = TRUE;
	return(FALSE);
    }
    
    skillFile = dmsAppGetEnv(userTriggerFileEnvVarName);

    if(skillFile) {
      bufferLength = strlen(skillFile) + 50;
      skillStringToEval = (char *) malloc(bufferLength);
      sprintf(skillStringToEval,"dssSkillEval {load(\"%s\")}", skillFile);
    }

    tclFile = dmsAppGetEnv(userTriggerTclFileEnvVarName);

    if(tclFile) {
      bufferLength = strlen(tclFile) + 10;
      tclStringToEval = (char *) malloc(bufferLength);
      sprintf(tclStringToEval,"run %s", tclFile);
    }
    
    if(skillStringToEval == NULL && tclStringToEval == NULL) {
	globalUserTriggerDisableFlag = TRUE;
	return(FALSE);
    }
    
    /* it is a non graphic session or a skill based ipc 
       (former rws idmsNullGT = 0)
    */
    if(idmsNullGT == 0 || idmsNullGT == 1 || idmsNullGT == 2) { 
      dssRetHandle = sapiExecuteCmd(tclEvalStr1, &dssRet);

      dssRetHandle = (SUCCEEDED(dssRetHandle)) ? sapiExecuteCmd(tclEvalStr2, &dssRet): dssRetHandle;

      dssRetHandle = (SUCCEEDED(dssRetHandle)) ? sapiExecuteCmd(tclEvalStr3, &dssRet): dssRetHandle;

      dssRetHandle = (SUCCEEDED(dssRetHandle)) ? sapiExecuteCmd(tclEvalStr4, &dssRet): dssRetHandle;

      if(skillStringToEval) {
      dssRetHandle = (SUCCEEDED(dssRetHandle)) ? 
	  sapiExecuteCmd(skillStringToEval, &dssRet): dssRetHandle;
      }

      if(tclStringToEval) {
      dssRetHandleTcl = sapiExecuteCmd(tclStringToEval, &dssRet);
      }

      globalSkillResponse = (SUCCEEDED(dssRetHandle)||SUCCEEDED(dssRetHandleTcl)) ? TRUE:FALSE;
    }
    else
      globalSkillResponse = FALSE;	

    if(skillStringToEval) free(skillStringToEval);
    if(tclStringToEval) free(tclStringToEval);

    if (!globalSkillResponse)
	globalUserTriggerDisableFlag = TRUE;

    return(globalSkillResponse);
}

int idmGetWord(char *word, char *buffer, char seperator)
{
    int i = 0, j = 0;
    
    while ((buffer[i] != seperator) && (buffer[i] != '\0')) {
	word[i] = buffer[i];
	++i;
    }
    word[i] = '\0';
    
    if (buffer[i] == seperator) {
	++i;
	while (buffer[i] != '\0')
	    buffer[j++] = buffer[i++];
	buffer[j] = '\0';
    }
    else 
	buffer[0] = '\0';

    return(strlen(word));
}

/* this procedure is similar to a user trigger except 
** it is specific and allows only attributes to be set
*/
Boolean idmSetGeneratedAttribute(char *event, char *libName, char *cellName,
                                 char *viewName, char *versionName)
{

	idmTable *tablePtr;
	Boolean done = FALSE;
	int row = 1;
	char *contents, *token;
	Boolean error = FALSE;
	char toolName[256];
	char userName[256];
	int retval;
	char tempContents[1024], tempWord[1024];
	time_t thisTime;

	/* if attribute PI is disabled simply return */
	if (globalAttrDisableFlag)
		return(TRUE);

	/* look to see if the table is present in the global file
	** if not return */
	if (dmsCGetTable(IDMS_GENERATED_ATTR_TABLE,&tablePtr) != idmsSuccess) 
		return(TRUE);

	/* look at the table for the existence of this event and view */
	while (!done) {
		if (dmsCGetTableEntry(tablePtr,row,&contents) != idmsSuccess) 
			return(TRUE);
		token = strtok(contents," ");
		if (token!=NULL) {
			if (strcmp(event, token) == 0) {
				/* check for viewName */
				token = strtok(NULL," ");
				if (strcmp(token, viewName) == 0) 
					done = TRUE;
			}
		}
		row++;
	}
	
	/* we have a list of attributes which we want generated 
	** get each one by one and simply set them using the standard call
	** that way we integrate with all existing attribute control features
	*/
	/** NOTE - I do not use strtok because I was having some
	** inexplicable problems
	*/
	token = strtok(NULL," ");
	strcpy(tempContents,token);
	while (idmGetWord(tempWord,tempContents,':')) {
		/* depending on what the attribute place a value and type 
		** the possible attributes are dirty, toolname, username, time 
		*/
	    if (strcmp(tempWord,"dirty") == 0) {
			retval = dmsCSetAttribute(NULL, cellName, viewName,
			       versionName, "dirty", "boolean", "TRUE");
		}
	    else if (strcmp(tempWord,"toolname") == 0) {
			dmsCGetToolName(toolName);
			retval = dmsCSetAttribute(NULL, cellName, viewName,
				versionName, "toolname", "string", toolName);
		}
	    else if (strcmp(tempWord,"username") == 0) {
		cuserid(userName);
		retval = dmsCSetAttribute(NULL, cellName, viewName,
				 versionName, "username", "string", userName);
		}
	    else if (strcmp(tempWord,"time") == 0) {
		(void) time(&thisTime);
		
		retval = dmsCSetAttribute(NULL, cellName, viewName,
			  versionName, "time", "string", ctime(&thisTime));
		}
	    else 
		error = TRUE;

		if (retval != idmsSuccess)
			error = TRUE;

	}

	return(!error);
}
