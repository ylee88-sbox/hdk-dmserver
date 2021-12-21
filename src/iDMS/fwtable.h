#ifndef FWTABLE_H
#define FWTABLE_H

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
# $Id: fwtable.h,v 1.1 1997/12/12 17:25:25 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
/*
#Revision 1.1  93/01/13  14:05:29  dhejmad
#Initial revision
#
*/


# define IDMS_ATTRIBUTE_TABLENAME "iDMSAttributeControlTable"
# define IDMS_ATTRIBUTE_TABLE_COLUMNS 3

# define IDMS_VIEWTYPE_VIEWNAME_TABLE "iDMSIntelDataFormatToNameTable"
# define IDMS_VIEWTYPE_VIEWNAME_TABLE_COLUMNS 3

# define IDMS_REGISTRY_TABLE "iDMSIntelRegistryTable"
# define IDMS_REGISTRY_TABLE_COLUMNS 2

# define IDMS_EXTTYPE_EXTNAME_TABLE "iDMSIntelExtTable"
# define IDMS_EXTTYPE_EXTNAME_TABLE_COLUMNS 3

# define IDMS_VIEWNAME_EXT_TABLE "iDMSViewNameToExtTable"
# define IDMS_VIEWNAME_EXT_TABLE_COLUMNS 2

# define IDMS_WRITE_VIEWNAME_TABLE "iDMSWriteViewTypeToNameTable"
# define IDMS_WRITE_VIEWNAME_TABLE_COLUMNS 3

# define IDMS_USERTRIGGER_TABLE "iDMSUserTriggerTable"
# define IDMS_USERTRIGGER_TABLE_TCL "iDMSUserTriggerTable_tcl"
# define IDMS_USERTRIGGER_TABLE_COLUMNS 3

# define IDMS_GENERATED_ATTR_TABLE "iDMSGeneratedAttributeTable"
# define IDMS_GENERATED_ATTR_TABLE_COLUMNS 3

#define userTriggerFileEnvVarName "IDMS_USERTRIGGERFILE"
#define userTriggerTclFileEnvVarName "IDMS_USERTRIGGERFILE_TCL"
#define userTriggerDisableEnvVarName "IDMS_DISABLEUSERTRIGGER"


# define IDMS_ATTRIBUTE_NULLIFY_COLUMN 3
# define IDMS_ATTRIBUTE_INHERIT_COLUMN 2
# define IDMS_ATTRIBUTE_INHERIT_VALUE "1"
# define IDMS_ATTRIBUTE_NO_INHERIT "0"
# define IDMS_ATTRIBUTE_INHERIT_NULL "2"

# define IDMS_ATTRIBUTE_NULLIFY_VALUE "idmNull"

/* error code indicating end of table */
# define idmsNoRow -1

typedef struct idmTableColumnEntry {  
    char *valueList;
    struct idmTableColumnEntry *nextColumn;
} idmTableColumnEntry;

typedef struct idmTableRowEntry {  
    struct idmTableColumnEntry *columns;
    struct idmTableRowEntry *nextRow;
} idmTableRowEntry;

typedef struct idmTable {
    char *name;
    struct idmTableRowEntry *table;
    struct idmTable *nextTable;
} idmTable;


/* hack to avoid renaming the yywrap comman */
//#define ytablewrap() yywrap()

#endif

