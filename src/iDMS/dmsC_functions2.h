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
# $Id: dmsC_functions2.h,v 1.1.2.7 1997/11/06 18:30:38 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef _DMS_FUNCTIONS2_H
#define _DMS_FUNCTIONS2_H

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
//        Date: 04/14/97
//
//        Authors: Sorin Blebea
//
//        File: dms_functions2 -- prototypes for the functions used
//                                          internally by iDMS.
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>

#include <dmsC_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Special internal idms use */

int check_for_buff_space(char **Buff, int* lb, int space);
char *set_string_in_buff(char **Buff, int* lb, char *content);
char *append_string_to_buff(char **Buff, int *lb, char *content);
int dmsC_deallocate_vars();

/* string manipulation */

char *dmsPutBackSlashBeforeQuote(char *inString, int allocate_new);
char *dmsFormFileName(char *dir, char *cName, char *vName, int call_get_ext);

/* global vars related */

int matchToolName(const char *tvNameList, const char *tName);
int matchvName(const char *tvNameList, const char *vName);
int matchToolvName(const char *tvNameList, const char *tName,
                   const char *vName);
int match_for_tool_n_version(const INFO* policy, int i, const char *tname,
                             const char *vname);

int findPolicyEntry(const INFO* policy, int policyCount,
                    const char *tName, const char *vName);

/* general string processing */

void dmsSeparateByWhatever(char c, char *is, char *o1, char *o2);
void dmsSeparateByDot(char *inString, char *output1, char *output2);
void dmsSeparateByColon(char *inString, char *output1, char *output2);
void dmsSeparateBySpace(char *inString, char *output1, char *output2);
int  dmsMatchExpression(char *expressionList, char *pattern);
int  is_wild_exp(const char *exp);   

#ifdef __cplusplus
  }
#endif

#endif
