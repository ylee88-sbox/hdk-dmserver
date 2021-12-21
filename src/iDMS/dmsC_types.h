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
# $Id: dmsC_types.h,v 1.1.2.10 1997/11/10 22:07:57 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef _IDMS__TYPES_H
#define _IDMS__TYPES_H

/*----------------------------------------------------------------------------*
 * Created by Sorin to store the types
 *----------------------------------------------------------------------------*/


#define RW_opt 1
#define RO_opt 2
#define WORKDIR_opt 4
#define UNIXDIR_opt 7
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
typedef char* String;
typedef void* voidStar;
#endif
typedef struct elem {
  int opt;
  char *dirLib;
  struct elem *next;
  char *absPath;
} OPERATION;


typedef struct head {
  char *name;
  struct elem *next;
} INFO;


typedef struct dms_new_info{
  int  location;
  char *libPath;
  char *libName;
  char *cellName;
  char *viewName;
  char *verName;
  char *coUser;
  char *fullPathName;

/* New fields from here */

  char *searchPath;
  char *pathConfName;
  char *toolConfName;

} DMS_NEW_INFO;

#endif
