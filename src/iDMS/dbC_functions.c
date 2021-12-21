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
# $Id: dbC_functions.c,v 1.1.2.2 1998/01/13 23:03:38 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/


#include <dbC.h>
#include <idb_functions.h>

Boolean dbCCreateLibrary(const char *libName, const char *path,         
                          const char *workGroup, const char *userGroup)
{
   int ret_code = FALSE;
   if (idb_create_library(libName, path, workGroup, userGroup))
      ret_code = TRUE;
   idb_capture_dm_vendor_warning();
   return ret_code;
}

