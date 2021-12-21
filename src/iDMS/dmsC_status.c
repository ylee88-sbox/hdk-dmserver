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
# $Id: dmsC_status.c,v 1.1.2.19 1998/01/12 18:47:32 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

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
//        Date: 08/20/97
//
//        Authors: Sorin Blebea
//
//        File: dmsC_status.c -- status reporting functions, and internal vars
//              This file should actually be a C++ class 
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <dmsC_defines.h>
#include <dmsC_reports.h>
#include <dmsC_functions2.h>
#include <fwierr.h>
#include <fwimsg.h>
#include <idb_functions.h>
#include <os_functions.h>
#include <dmsC_status.h>

/*-----------------------*
 * Global variables used *
 *-----------------------*/

extern char *userName; /* used by save_status() -- yacky! */

extern int debugDmsOn;

/*------------------*
 * Static variables *
 *------------------*/

/*---------------------------*
 * Internally used functions * 
 *---------------------------*/

/*-----------------------*
 * Functions definitions *
 *-----------------------*/


static char *_bempty = "";
static DMS_NEW_INFO *_dms_status = 0;

static int _lenlastLibName = 0;
static int _lenlastLibPath = 0;
static int _lenlastVerName = 0;
static int _lenlastFullPathName = 0;

static int _lenlastSearchPath = 0;
static int _lenlastPathConfName = 0;
static int _lenlastToolConfName = 0;

int init_dms_structs()
{
   if ((_dms_status = (DMS_NEW_INFO *)malloc(sizeof(DMS_NEW_INFO))) == 0)
      return 0;
   _dms_status->location     = 0;
   _dms_status->libPath      = 0;
   _dms_status->libName      = 0;
   _dms_status->cellName     = 0;
   _dms_status->viewName     = 0; 
   _dms_status->verName      = 0;
   _dms_status->coUser       = 0;
   _dms_status->fullPathName = 0;

   _dms_status->searchPath   = 0;
   _dms_status->pathConfName = 0;
   _dms_status->toolConfName = 0;
   return 1;
}

int deallocate_dms_structs()
{
   /* location is an int -- don't deallocate */
   FREE_NZ(_dms_status->libPath);
   FREE_NZ(_dms_status->libName);
   FREE_NZ(_dms_status->viewName);
   FREE_NZ(_dms_status->verName);
   FREE_NZ(_dms_status->coUser); 
   FREE_NZ(_dms_status->fullPathName);
   FREE_NZ(_dms_status->searchPath);
   FREE_NZ(_dms_status->pathConfName);
   FREE_NZ(_dms_status->toolConfName); 
   FREE_NZ(_dms_status);
   return 1;
}

/* TEMPORARY -- CHANGE TO NEW !!!! */

extern DMSINFO dmsLastCVInfo;

int get_last_status_code()
{
   return dmsLastCVInfo.location;
}

int save_status(int code, char *lP, char *lN, char *cN, char *vN, char *ver,
                char *user, char *fpn, int change_codes)
{
   dmsLastCVInfo.location = code;

      strcpy(dmsLastCVInfo.libPath, lP?lP:"");
      strcpy(dmsLastCVInfo.libName, lN?lN:"");
      strcpy(dmsLastCVInfo.cellName, cN?cN:"");
      strcpy(dmsLastCVInfo.viewName, vN?vN:"");
      strcpy(dmsLastCVInfo.verName, ver?ver:"");
      dmsSetLastVerName(ver?ver:"");
      strcpy(dmsLastCVInfo.coUser, user?user:"");
      strcpy(dmsLastCVInfo.fullPathName, fpn?fpn:"");

      if (debugDmsOn){
         dmsPrintIdmsLog("*IDMS DEBUG* save status: %s,%s,%s,%s,%s,%s,%s\n", dmsLastCVInfo.libPath, dmsLastCVInfo.libName, dmsLastCVInfo.cellName, dmsLastCVInfo.viewName, dmsLastCVInfo.verName, dmsLastCVInfo.coUser, dmsLastCVInfo.fullPathName);
      }

   if (change_codes == DONT_CHANGE_CODES)
      return code;

   else { /* emulate the precambrian times */
      switch (code){ 
         case idmsCvNotFound:
            return idmsInfoCvNotFoundOnRwpaths;
         case idmsCvInWorkdir:
            return idmsInfoCvIsInWorkdir;
         case idmsCvInUnixdir:
            return idmsInfoCvIsInReadonlyUnixdir;
         case idmsCvInWorklib:
            return idmsInfoCvCanBeCheckedOut;
         case idmsCvInRwlib:
            if (!strcmp(user, NOBODY))
               return idmsInfoCvCanBeCheckedOut;
            else
               return strcmp(user, userName)?idmsInfoCvCheckedOutByAnotherUser:
                                             idmsInfoCvCheckedOutByMe;
            break;
         case idmsCvInRolib:
               return idmsInfoCvIsInReadonlyLibrary;
         default:
               return idmsInfoUnknownEvent;
      }     
      return idmsInfoUnknownEvent; /* aint happenin */
   }
}

/*-----------------------------------------------------------------------
 *
 * FOLLOWING FUNCTIONS SET/GET INDIVIDUAL FIELDS OF THE DMS_NEW_INFO STRUCT
 *
 *  dmsGetStatus
 *  dms(Get/Set)LastLibName
 *  dms(Get/Set)LastLibPath
 *  dms(Get/Set)LastFullPathName
 *
 *  dms(Get/Set/AppendTo)LastSearchPath
 *  dms(Get/Set)LastPathConfName
 *  dms(Get/Set)LastToolConfName
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description:
 *-----------------------------------------------------------------------*/

DMSINFO *dmsGetStatus(int version_and_locker)
{
void *libObj;
char *latestVer=NULL;

/* get the latest version status */
if(version_and_locker&&dmsLastCVInfo.libName&&strcmp(dmsLastCVInfo.libName,"")) {
  libObj = idb_find_lib_id(dmsLastCVInfo.libName, 0, "r");
  if(!strcmp(dmsLastCVInfo.verName, DUMMY) && 
	!strcmp(dmsLastCVInfo.coUser, DUMMY)) { /* get the latest version & locker status */
    idb_find_version_and_locker(libObj, dmsLastCVInfo.cellName,
				  dmsLastCVInfo.viewName, "", 
				  dmsLastCVInfo.coUser, NULL, 1,
				  &latestVer);
    if(latestVer)
      strcpy(dmsLastCVInfo.verName, latestVer);
  }
  if(!strcmp(dmsLastCVInfo.verName, DUMMY)) { /* get the latest version only */
    idb_find_version_and_locker(libObj, dmsLastCVInfo.cellName,
				  dmsLastCVInfo.viewName, "", 
				  NULL, NULL, 1,
				  &latestVer);
    if(latestVer)
      strcpy(dmsLastCVInfo.verName, latestVer);
  }
  if(!strcmp(dmsLastCVInfo.coUser, DUMMY)) { /* get the latest version only */
    idb_find_version_and_locker(libObj, dmsLastCVInfo.cellName,
				  dmsLastCVInfo.viewName, "", 
				  dmsLastCVInfo.coUser, NULL, 1,
				  &latestVer);
  }
}

return &dmsLastCVInfo;
}


/* Following used in conjunction with _dms_status->libName */

char *dmsGetLastLibName()
{
   if (!_dms_status->libName)
      return _bempty;
   return _dms_status->libName;
}


char *dmsSetLastLibName(char *s)
{
   return set_string_in_buff(&_dms_status->libName, &_lenlastLibName, s);
}


char *dmsGetLastLibPath()
{
   if (!_dms_status->libPath)
      return _bempty;
   return _dms_status->libPath;
}


char *dmsSetLastLibPath(char *s)
{
   return set_string_in_buff(&_dms_status->libPath, &_lenlastLibPath, s);
}

char *dmsGetLastVerName()
{
   if (!_dms_status->verName)
      return _bempty;
   return _dms_status->verName;
}


char *dmsSetLastVerName(char *s)
{
   return set_string_in_buff(&_dms_status->verName, &_lenlastVerName, s);
}


char *dmsGetLastFullPathName()
{
   if (!_dms_status->fullPathName)
     return _bempty;
   return _dms_status->fullPathName;
}

char *dmsSetLastFullPathName(char *s)
{
   return set_string_in_buff(&_dms_status->fullPathName,
                             &_lenlastFullPathName, s);
}


char *dmsGetLastSearchPath()
{
   if (!_dms_status->searchPath)
      return _bempty;
   return _dms_status->searchPath;
}


char *dmsSetLastSearchPath(char *s)
{
   return set_string_in_buff(&_dms_status->searchPath, &_lenlastSearchPath, s);
}


char *dmsAppendToLastSearchPath(char *s)
{
   return append_string_to_buff(&_dms_status->searchPath,
                                   &_lenlastSearchPath, s);
}


char *dmsGetLastPathConfName()
{
   if (!_dms_status->pathConfName)
      return _bempty;
   return _dms_status->pathConfName;
}


char *dmsSetLastPathConfName(char *s)
{
   return set_string_in_buff(&_dms_status->pathConfName,
                             &_lenlastPathConfName, s);
}


char *dmsGetLastToolConfName()
{
   if (!_dms_status->toolConfName)
      return _bempty;
   return _dms_status->toolConfName;
}


char *dmsSetLastToolConfName(char *s)
{
   return set_string_in_buff(&_dms_status->toolConfName,
                             &_lenlastToolConfName, s);
}

