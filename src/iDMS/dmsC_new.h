#ifndef DMSC_NEW_H
#define DMSC_NEW_H

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
# $Id: dmsC_new.h,v 1.1.2.30 1998/01/13 23:03:43 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

/**********************************************************************
 *
 * Name:	dmsC_new.c
 * Authors:     Sorin Blebea
 * Date:        07/18/97
 * Description: New dmsC api functions
 *********************************************************************/

#if (defined(_PROTOTYPES) || defined(__STDC__) || defined(__cplusplus)) && !defined(_NO_PROTO)
#undef  PARAMS
#define PARAMS(args) args
#define CONST const
#else
#undef  PARAMS
#define PARAMS(args) ()
#define CONST 
#endif

#include <stdlib.h>
#include <fwtable.h>
#include <dmsC_defines.h>

#ifdef __cplusplus
  extern "C" {
#endif

/*============*
 * Prototypes *
 *============*/

/*--------------*
 * Initializing *
 *--------------* */

/* following functions are defined in dmsC_API_1.c */

CONST char *dmsCInitDmsPath(CONST char *appName, CONST char *bootName,
                            int nullgtMode);
Boolean dmsCInitVendorDM(CONST char *dmsBootName);
void coralCDisableObsoleteMessages();
Boolean dmsCCheckEnvSettings(char **message);
/**
Boolean dmsCRegisterTriggers(int mode);
Boolean dmsCRegisterUserTriggers(int mode);
**/
Boolean dmsCParseDmsPathFile(CONST char *filename);

/*-----------*
 * Reporting *
 *-----------*/

CONST char *dmsCGetLastMessage PARAMS((void));
CONST char *dmsCSetLastMessage PARAMS((CONST char *new_message));
CONST char *dmsCSetLastMessageSize PARAMS ((int size));
CONST char *dmsCAppendToLastMessage PARAMS((CONST char *to_append));
char *dmsCGetWarnPrompt PARAMS((void));
char *dmsCSetWarnPrompt PARAMS((char *s));
int dmsCGetLastErrorCode PARAMS((void));
int dmsCGetLastStatusCode PARAMS((void));
CONST char *dmsCGetLastVendorMessage PARAMS((void));

/*-------------------------*/
/* Variable # of arguments */
/*-------------------------*/

   void dmsCPrintWarning(char *format, ...);
   void dmsCPrintError(char *format, ...);
   CONST char *dmsCVarAppendToLastMessage(int extra_size, char *format, ...);

char *dmsCGetToolName PARAMS((char *tname));
char *dmsCSetToolName PARAMS(( CONST char *newname));

/*-------------------------*
 * TABLE Related functions *
 *-------------------------*/

int dmsCGetTable(char *tableName, idmTable **tablePtr);
int dmsCGetTableEntry(idmTable *tablePtr, int rowNumber, char **tableEntry);

/*------------*
 * DM related *
 *------------*/

int dmsCRefreshAllLib(char *outString);
/** long coralCGetCheckInTime(void *pver_id); **/
int dmsCPathInitDMS(char *appName, char *bootName, char *dmsPath, 
                    int nullgtMode);
void *coralCGetLibIdByLib(char *lName, char *cName, char *vName,
                          char *viewType, int *error);
void *coralCGetLibId(char *cName, char *vName, char *viewType, int *error);

/* coralC */
/**
int coralCRemoveCVFromConfig(void *plib_id, char *confName, char *cName,
                             char *vName);
int coralCAddCVToConfig(void *plib_id, char *confName, char *cName,
                        char *vName, char *verName, int bind);
**/

/*-----------------*
 * TRIGGER RELATED *
 *-----------------*/

/**
int dmsCExecutePreTrigger(char *triggName, char *libName, char *cName,
                          char *vName, char *verName, char *FPN);

int dmsCExecutePostTrigger(char *triggName, char *libName, char *cName,
                           char *vName, char *verName, char *FPN); 
**/

/*------------*
 * ATTRIBUTES *
 *------------*/

int dmsCSetGeneratedAttribute(char *event, char *libName, char *cellName,
                              char *viewName, char *versionName);
#ifdef __cplusplus
  } 
#endif


#endif  
