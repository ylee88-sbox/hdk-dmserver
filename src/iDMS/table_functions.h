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
# $Id: table_functions.h,v 1.1.2.5 1997/08/13 23:06:53 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

/* Prototypes for functions in table_functions.c */

#include <fwtable.h>

const idmTable* getGlobalTablePtr();
idmTable* setGlobalTablePtr(const idmTable* value);

int setTableLineNumber(int num);
int getTableLineNumber();
int incrTableLineNumber();
int incrTableErrors();
int getNoTableErrors();
void initTableParserVars();

int idmReadGlobalFile();
int idmGetColumn(char *table, char *key, int column, char **val);
int idmGetViewName(char *toolName, char *viewType, char **viewNameList);
int idmSetViewName(char *toolName, char *viewType, char *viewNameList);
char *idmGetExtension(char *viewName, char *ext);
void idmPrintGlobalTable();
int idmCheckColumn(char *name, int columns);
int idmCheckDefinedTables();
int idmGetWriteViewName(char *toolName, char *viewType, char **viewName);
int idmGetExtName(char *toolName, char *extType, char **extNameList);
char *idmGetPattern(char *viewName, char *pattern_buff);

