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
# $Id: attr.h,v 1.1.14.1 1997/07/30 17:18:30 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
/* 
#Revision 1.1  93/01/13  14:05:25  dhejmad
#Initial revision
#
*/

# define PATHLENGTH 200
# define NAMELENGTH 1024
# define IDMS_SEARCHPATHLENGTH 1024

# define iDMS_HIERARCHY_SEPERATOR_CHAR '/'
# define iDMS_HIERARCHY_SEPERATOR_STRING "/"

# define iDMS_INHERITVALUE 1
# define iDMS_INHERITNOVALUE 2
# define iDMS_NOINHERIT 0
# define iDMS_BOOLEAN 'b'
# define iDMS_STRING  's'
# define iDMS_FLOAT 'f'
# define iDMS_INTEGER 'i'
# define iDMS_TIME 't'
# define iDMS_LIST 'l'
# define iDMS_HIER 'h'

# define iDMSNULLFLOATVALUE -1.0
# define iDMSNULLINTVALUE -1
# define iDMSNULLTIMEVALUE (long) -1
# define iDMSNULLSTRINGVAL "idmNullified"

#define  iDMSMappingKeywordEnvVar "IDMS_MAPPINGKEYWORD"
# define iDMS_SHADOW_PROPNAME "iDMS_newVersionProp"
# define iDMS_SHADOW_DIRNAME  ".iDMS"
# define iDMS_SHADOW_LIBPREFIX "Ux"
# define iDMS_SHADOW_VIEWTYPE "schematic"
# define iDMS_SHADOW_DIRPERMISSIONS \
    (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)


#define CREATE_IT 1
#define NO_CREATE 0
#define IDMS_TRIGGER_PRIORITY 513



#define iDMS_MaxElementsInList 2000
#define iDMS_ListDelimiter ":"
#define iDMS_EmptyListSymbol "#"



