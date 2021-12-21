#ifndef DMSC_CADENCE_SPEC_H
#define DMSC_CADENCE_SPEC_H

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
# $Id: dmsC_Cadence_spec.h,v 1.1.2.6 1997/11/13 19:42:49 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

/**********************************************************************
 *
 * Name:	dmsC_Cadence_spec.h
 * Authors:     Sorin Blebea
 * Date:        07/18/97
 * Description: Cadence spec idms functions
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

/* This file is not self sufficient because Cadence's header files
   are NOT!!! protected against multiple inclusion */

#include <stdlib.h>

#ifdef __cplusplus
   extern "C" {
#endif

/*============*
 * Prototypes *
 *============*/
/**
int dmsCRemoveCVFromConfig(dmLibId libID, char *confName, char *cName,
                           char *vName);
int dmsCAddCVToConfig(dmLibId libID, char *confName, char *cName,
                      char *vName, char *verName, int bind);
**/
int dmsCGetLibIdByLib(char *lName, char *cName, char *vName, char *viewType,
                      ddId *libID);

/* REM: check this func*/
int dmsCGetLibId(char *cName, char *vName, char *viewType, ddId *libID);

/* Time */

int dmsCGetCheckInTime(int versionID); 

/* CDBA */
int dmsCCdbaReadCellView(char *cName, char *vName, int *timeStamp,
                         ddId *cdbaLibID);
int dmsCCdbaReadCellViewWriteLib(char *cName, char *vName, int *timeStamp,
                                 ddId *cdbaLibID);
int dmsCCdbaLockCellView(char *cName, char *vName, char *viewType,
                         ddId *cdbaLibID);
int dmsCCdbaLockCellViewForId(char *cName, char *vName, char *viewType,
                              ddId *cdbaLibID, char *cdbaVerID);
int dmsCCdbaGetLibId(char *cName, char *vName, char *viewType,
                     ddId *cdbaLibID);
int dmsCCdbaWriteCellView(char *cName, char *vName, char *viewType,
                          char *children, int advance, char *log, 
                          ddId *cdbaLibID);

#ifdef __cplusplus
   }
#endif

#endif  
