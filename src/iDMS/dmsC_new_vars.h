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
# $Id: dmsC_new_vars.h,v 1.1.2.16 1997/12/12 23:12:55 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

#ifndef _DMSC_NEW_VARS_H
#define _DMSC_NEW_VARS_H

/*----------------------------------------------------------------------------*
 * Created by Sorin to store the new global variables                         *
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <dmsC_defines.h>
#include <dmsC_types.h>
#include <fwierr.h>

/* The idea is to #define IDMS_MAIN in fwidms.yacc for now */ 

#ifdef EXTERNAL
#undef EXTERNAL
#endif

#ifdef INITIAL_VALUE
#undef INITIAL_VALUE
#endif

#ifdef IDMS_MAIN
#   define EXTERNAL
#   define INITIAL_VALUE(x) x
#else
#   define EXTERNAL extern
#   define INITIAL_VALUE(x)
#endif

/*-----------------------*
 * And now the variables *
 *-----------------------*/

EXTERNAL char *dmsAppName INITIAL_VALUE(=0);
EXTERNAL FILE *idmsLogFilePtr INITIAL_VALUE(=0);

/*.............................*
 * New stuff for user temp dir *
 *.............................*/

EXTERNAL int  dms_tmp_usr_dir_attempted INITIAL_VALUE(=0);

EXTERNAL char dms_tmp_dir[1024] INITIAL_VALUE(="/tmp");

/*.......* 
 * Flags * 
 *.......*/

EXTERNAL int dmsDoLogAfterAll INITIAL_VALUE(=0);
EXTERNAL int debugDmsOn INITIAL_VALUE(=0);
EXTERNAL int dmsFindUnmanaged INITIAL_VALUE(=1);

EXTERNAL int messageHeader INITIAL_VALUE(=0); /* Set to 1 by idmb - YACK! */
EXTERNAL int forceReplaceCvInConfig INITIAL_VALUE(=1);

EXTERNAL int idmsNotInitialized INITIAL_VALUE(=1);

EXTERNAL int disable_obsolete_messages INITIAL_VALUE(=0);

/* after a hiatus of more than a decade, I am back -- Sorin 04/16/08 */

EXTERNAL int debug_idb INITIAL_VALUE(=0);

EXTERNAL int turbo_dmserver INITIAL_VALUE(=0);
EXTERNAL int twin_turbo INITIAL_VALUE(=0);

/*.........*
 * General *
 *.........*/

EXTERNAL char *userName INITIAL_VALUE(=0);

/*...........*
 * Reporting *
 *...........*/

EXTERNAL int dmsLastErrorNumber INITIAL_VALUE(=0);

/*------------------*
 * TO BE ELIMINATED *
 *------------------*/

EXTERNAL DMSINFO dmsLastCVInfo;
EXTERNAL int dmsKeepTmpCopy INITIAL_VALUE(=0);


/*----------------*
 * PARSER RELATED *
 *----------------*/

EXTERNAL INFO toolName;  /* stores the current tool's name -- obsolete */
EXTERNAL char *dmsPathFileName INITIAL_VALUE(=0);

/* This ain't right -- should be changed soon */

#ifdef IDMS_MAIN
   char action[10][5] = {"XXX","R/W","R/O","W/L","W/D","T/C","???","DIR",
                         "???","^$?"
                        };
#else
   extern char action[10][5];
#endif


EXTERNAL INFO libpath[BUFSIZE];
EXTERNAL INFO tconfig[BUFSIZE];
EXTERNAL INFO worklib[BUFSIZE];
EXTERNAL INFO rwpaths[BUFSIZE];
EXTERNAL INFO workdir[BUFSIZE];
EXTERNAL INFO var[BUFSIZE];

EXTERNAL int libpathCount INITIAL_VALUE(=0);
EXTERNAL int tconfigCount INITIAL_VALUE(=0);
EXTERNAL int worklibCount INITIAL_VALUE(=0);
EXTERNAL int rwpathsCount INITIAL_VALUE(=0);
EXTERNAL int workdirCount INITIAL_VALUE(=0);
EXTERNAL int varCount INITIAL_VALUE(=0);

/* flags, etc. - parser related */

EXTERNAL int dmsIsOn INITIAL_VALUE(= 0);
EXTERNAL int dmsLineNumber INITIAL_VALUE(= 0);
EXTERNAL int toolCheck INITIAL_VALUE(= 1);
EXTERNAL char dmsStack[SUPERBUFSIZE];

EXTERNAL int errorCount INITIAL_VALUE(=0);

EXTERNAL int incCount INITIAL_VALUE(=0);
EXTERNAL int includeDepth INITIAL_VALUE(=0);
EXTERNAL int maxDepth INITIAL_VALUE(=0);
EXTERNAL int appendType INITIAL_VALUE(=0);

EXTERNAL char include[MAXINCLUDEDEPTH][256];
EXTERNAL char resultBuf[SUPERBUFSIZE];

/* config related */

EXTERNAL char dmsConfName[BUFSIZE];

/* Miscellaneous remains of the dark idms past */

EXTERNAL char theLibName[BUFSIZE] INITIAL_VALUE(="*");
EXTERNAL char theConfName[BUFSIZE] INITIAL_VALUE(="default");

#endif
