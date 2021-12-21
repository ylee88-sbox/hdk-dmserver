#ifndef DBC_H
#define DBC_H

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
# $Id: dbC.h,v 1.1.2.2 1998/01/13 23:03:37 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

/**********************************************************************
 *
 * Name:	dbC.h
 * Authors:     Sorin Blebea
 * Date:        01/12/98 
 * Description: New dbC prototypes. For functions that access the 
 *              DB/DM engines and might break the DMSPATH controll
 *              We do not want "dmsC" to create a library but we
 *              want to be able to do that. Today's idms cannot create
 *              a library
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

Boolean dbCCreateLibrary(CONST char *libName, CONST char *path,         
                          CONST char *workGroup, CONST char *userGroup);

/*============*
 * Prototypes *
 *============*/

#ifdef __cplusplus
  }
#endif

#endif  
