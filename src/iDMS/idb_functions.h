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
# $Id: idb_functions.h,v 1.1.2.52 1998/01/13 18:49:47 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef __IDB_FUNCTIONS_H
#define __IDB_FUNCTIONS_H

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
//        Date: 04/23/97
//
//        Authors: Sorin Blebea
//
//        File: idb_functions.h  -- prototypes for the "idb_" functions
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
/* Typedefs */

typedef void (*Reply_Fun_Type)(char *, void *);

#ifdef __cplusplus
extern "C" {
#endif

#include <scapi.h>

/*---------------------*
 * Init/Exit functions *
 ----------------------*/

int idb_check_env_settings(char **message);
int idb_init(void *init_data);
char *idb_get_startup(const char *cdslib);
void idb_terminate();

/*------------------------*
 *idb error codes related *
 -------------------------*/

int idb_clear_errors();
int idb_OR_error_code(int errc);
int idb_set_error_code(int errc);
int idb_check_error_code(int errc);
int idb_check_success();

/*-----------------*
 * Cadence Message related *
 ------------------*/

const char *idb_get_cdn_vendor_message();
const char *idb_set_cdn_vendor_message(const char *new_message);
const char *idb_capture_cdn_vendor_warning();
const char *idb_get_cdn_vendor_string();
const char *idb_set_dm_vendor_string(const char *str);

/* String evaluation */

/* void *idb_evaluate_string(const char *str, int report_errors); */

/* Basic CM functions */

/*---------*
 * Library *
 *---------*/

int         idb_set_library_path(const char *libpath);  
char       *idb_append_to_lib_list(char *pattern, char *libNameList,
                                   int reset_string); 
void       *idb_open_library(const char *name, const char *absolute_path,
                             const char *mode);
void       *idb_create_library(const char *libName, const char *path,
                          const char *workGroup, const char *userGroup);

void       *idb_find_lib_id(const char *name, int set_static_lib_id,
                            const char *mode);

void       *idb_set_lib_id(void *plib_id);
void       *idb_get_lib_id();
void       *idb_get_empty_lib_id();
int        idb_valid_lib_id(void *plib_id);
void       **idb_set_content_at_id(void **plib_id, void *plib_id_content);
const char *idb_get_library_name(void *plib_id);
char       *idb_get_last_library_name();
const char *idb_get_library_directory(void *plib_id, char *mode);
int         idb_refresh_all_libraries();


/*-------------------------------------*
 * Hamburger helpers - dmserver take 2 *
 *-------------------------------------*/

/* breadcrumbs when creating new cells in DB_ROOT */

const char *idb_set_last_directory_cell (const char *dn);
const char *idb_get_last_directory_cell ();

const char *idb_set_last_directory1_cell (const char *dn);
const char *idb_get_last_directory1_cell ();

const char *idb_set_last_cell_name (const char *cn);
const char *idb_get_last_cell_name ();

const char *idb_set_last_workfile (const char *wf);
const char *idb_get_last_workfile ();

/*--------*
 * Config *
 *--------*/
/*********************************************************
int        idb_convert_to_vendor_config_codes(int idbCODE);

void       *idb_set_config_id(void *pc_id);
void       *idb_get_config_id();

void       *idb_create_config(void *plib_id, char *conf_name);
void       *idb_open_config(void *plib_id, char *conf_name);
void       idb_close_config(void *plib_id);
void       *idb_find_config(void *plib_id, char *conf_name);

int        idb_replace_cellview_in_config(void *plib_id, char *conf_name,
              char *cName, char *vName, char *verName, int idbBIND);
**********************************************************/
int	   idb_delete_config(void *plib_id, char *conf_name);
int 	   idb_insert_config(void *plib_id, char *confName, char *tocfgName);
int        idb_create_config_instance(void *plib_id, char *conf_name,
              char *instName);
int        idb_add_cellview_to_config(void *plib_id, char *conf_name,         
              char *cName, char *vName, char *verName, int idbBIND);
int        idb_remove_cellview_from_config(void *plib_id, char *conf_name,
              char *cName, char *vName);
int 	    idb_get_cellView_tags( void *plib_id, char *cName, char *vName, 
						char *verName, char *storeTags);

/*----------*
 * CellView *
 *----------*/

void       *idb_create_cellview(void *plib_id, char *cName, char *vName, 
                                char *viewType, char *FPN);
void       *idb_set_cellview_id(void *pcv_id);
void       *idb_get_cellview_id();
void       *idb_find_cellview(void *plib_id, char *cName, char *vName);

void       *idb_check_cell_in_lib(char *libName, char *cellName);
int idb_gen_cellview_list(char *libName,char *cellName,char **cellviewList);

HRESULT idb_delete_cell(void *plib_id, char *cName, char *vName, char *ver,
               		char *FPN, char *locker, char **new_ver, int CDBA);

HRESULT idb_delete_cellview(void *plib_id, char *cName, char *vName, char *ver,
               		char *FPN, char *locker, char **new_ver, int CDBA); 

HRESULT idb_delete_version(void *plib_id, char *cName, char *vName, char *ver,
               		char *FPN, char *locker, char **new_ver, int CDBA);
 
/***********************************************************************
int         idb_is_cellview_in_config(void *plib_id, void *pconf_id,
                                      const char *cName, const char *vName);
void       *idb_open_CDBA_cellview(void *plib_id, char *cName, char *vName,
                            char *ver, char *mode, char **wPath, char **rPath);
void       *idb_open_cellview(void *plib_id, char *cName, char *vName,
                               char *ver, char *mode, int cdba, char **rPath);
***********************************************************************/
int        idb_close_cellview(void *plib_id, char *cName, char *vName, 
                               char *ver);
/** int        idb_close_cellview_by_id(void *plib_id, void *pver_id); **/

/*----------*
 * Versions *
 *----------*/
/*******
char      *idb_get_default_version();
char      *idb_get_first_default_version();
void      *idb_find_last_version(void *plib_id, char *cName, char *vName, 
                                int *is_locked, char **pverstr);
void      *idb_find_last_locked_version(void *plib_id, char *cName, char *vName,
                                    char *locker, char *path, char *user,
                                    int get_last_if_not_locked, char **pverstr);
void      *idb_find_version(void *plib_id, char *cName, char *vName, char *ver,
                           int *is_locked, char **pverstr);
char      *idb_find_version_path(void *plib_id, char *cName, char *vName,
                                 char *ver);
char      *idb_find_version_path_by_id(void *plib_id, void *pver_id);
char      *idb_find_locker_by_version_id(void *plib_id, void *pver_id,
                                 char *copath);
*******/

char      *idb_find_version_and_locker(void *plib_id, char *cName, char *vName,
                                    char *ver, char *locker, char *path, 
                                    int get_last_if_not_locked, char **pverstr);

int    idb_url_exists(void *plib_id, char *cName, char *vName, char *ver);
int	idb_get_all_versions(void *plibid, char *cName, char *vName, char *storeVers);

/*--------------------*
 * Collection Objects *
 *--------------------*/
char *idb_get_collection_obj(void *plibid, char *cName, char *vName, int special);
int idb_is_collection_obj(void *plibid, char *cName, char *vName);

/*---------------*
 * DM Operations *
 *---------------*/

/*** long      idb_get_checkin_time(void *pver_id); *****/
HRESULT	idb_checkout(void *plib_id, char *cName, char *vName, 
		char *ver, char *FPN, char *locker, char **new_ver, int CDBA);
HRESULT	idb_checkin(void *plib_id, char *cName, char *vName, 
		char *FPN, char *locker, char **new_ver, int CDBA, 
		char *logMsg, char *newCell);
HRESULT idb_cancel_checkout(void *plib_id, char *cName, char *vName,
                             char *FPN, int CDBA);
HRESULT	idb_install_cellview(void *plib_id, char *cName, char *vName, 
               	char *FPN, int CDBA, char *logMsg, int *file_was_deleted);

/*---------------*
 * DM MESSAGES *
 *---------------*/
 void idb_capture_dm_vendor_warning(void);

/*---------*
 * General *
 ----------*/
 char *idb_strip_last_token(const char *path, const char *seperator);
 char *idb_strip_nth_token(const char *path, const char *seperator, int n);
 char *idb_map_viewType(char *viewName);

/*------------*
 * File Based *
 *------------*/

 /* Put here to take advantage of the vo Functions from Cadence since the
    first idb package is implemented around Cadence */

char *idb_simplify_file_name(const char *f, const char *d);
char *idb_get_dataReg_file(char *viewType, char *viewName);


/*-----------------------*
 * Disable Synchronicity *
 *-----------------------*/
  
#ifdef DISABLE_SYNCHRONICITY
HRESULT sapiInitialize (int argc, char** argv);
void sapiShutdown ();
HRESULT sapiMakeStrArray (char*** strArray, ...);
  

HRESULT sapiTag (void* extra,
                 char** fileList,
                 char* tag,
                 char** inputOptions,
                 char*** results);
  
HRESULT sapiUrlTags (void* extra,
                     char* webObject,
                     char** inputOptions,
                     char*** results);
  
HRESULT sapiExecuteCmd (char* tclCmd, char** result);

HRESULT sapiAddStrings (char** strArray, ...);

HRESULT sapiUrlVault (void* extra,
                      char* webObject,
                      char** inputOptions,
                      char*** results);

HRESULT sapiSetvault (void* extra,
                      char* vaultDirURL,
                      char* localDir,
                      char** inputOptions,
                      char*** results);

HRESULT sapiPopulate (void* extra,
                      char** inputOptions,
                      char*** results);

HRESULT sapiRmfolder (void* extra,
                      char** folderList,
                      char** inputOptions,
                      char*** results);

HRESULT sapiRmvault (void* extra,
                     char** vaultList,
                     char** inputOptions,
                     char*** results);

HRESULT sapiRmfile (void* extra,
                    char** fileList,
                    char** inputOptions,
                    char*** results);

HRESULT sapiRmversion (void* extra,
                       char** versionList,
                       char** inputOptions,
                       char*** results);

HRESULT sapiUrlVersionid (void* extra,
                          char* webObject,
                          char** inputOptions,
                          char*** results);

HRESULT sapiUrlFetchedstate (void* extra,
                             char* webObject,
                             char** inputOptions,
                             char*** results);

HRESULT sapiUrlGetprop (void* extra,
                        char* webObject,
                        char* propName,
                        char** inputOptions,
                        char*** results);

HRESULT sapiUrlEnsurepathname (void* extra,
                               char** versionList,
                               char** inputOptions,
                               char*** results);

HRESULT sapiUrlExists (void* extra,
                       char* webObject,
                       char** inputOptions,
                       char*** results);

HRESULT sapiUrlVersions (void* extra,
                         char* webObject,
                         char** inputOptions,
                         char*** results);

HRESULT sapiCheckout (void* extra,
                      char** webObjectList,
                      char** inputOptions,
                      char*** results);

HRESULT sapiCheckin (void* extra,
                     char** webObjectList,
                     char** inputOptions,
                     char*** results);

HRESULT sapiCancel (void* extra,
                    char** webObjectList,
                    char** inputOptions,
                    char*** results);

HRESULT sapiUrlSetprop (void* extra,
                        char* webObject,
                        char* propName,
                        char* propValue,
                        char** inputOptions,
                        char*** results);



extern HRESULT sapiRetire (void* extra,
                    char** webObjectList,
                    char** inputOptions,
                    char*** results);


extern HRESULT sapiFreeStrArray (char** strArray); 

#endif
  
/*------------*
 * Linux Port *
 *------------*/

  /* These variables are defined to keep track of the libraries and libpaths
     when cadence is not available on Linux platform.
     char** libName=0;
     char** libPath=0;
     char** libType=0;
     int libMax=0;
     int libNum=0;
  */

#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
#define ddId void*
typedef enum {
     ddUndefinedType,
     ddLibType,       /* directories */
     ddCellType,
     ddViewType,
     ddLibFileType,   /* files       */
     ddCellFileType,
     ddViewFileType
} ddType;

     
  /* these are used to keep track of libraries and their paths/types */  
void idb_clear_libs(int max);
void idb_add_libs(const char *name, const char *path, const char *type);
char *idb_get_libpath(const char *name);
char *idb_get_libtype(const char *name);
 
 
  /* redefinition of some of the cadence functions */ 
void *ddGetObj(char *libName, char *cellName, char *viewName,
               char *fileName, void *contextId, char *mode);
int  ddGetObjType(void *id);
char *ddGetObjWritePath(void *id); 
char *ddGetObjReadPath(void *id);
void *ddCreateLib(char *libName, char *libpath);
char *dbGetLibName(void *id);  
char *ddGetObjName(void *id); /* Given obj_id, return its name */
int  ddIsId(void *id);

  
  /* keep track of warnings and append if necessary */  
void voWarn(const char *msg);
char *voGetWarn(void);
void voFlushWarn(void);
void voInfo(const char *msg);
int  ddUpdateLibList(void);
int  ddDeleteObj(void *objId);
char *nmpMapName(const char *fromSpaceName,
                 const char *toSpaceName,
                 const char *identifier,
                 void* allocFunc, void *allocData);

char *getListbyName(const char *fullpath, const char *library, const char *name, int isCellName, int getLocker);
char *checkName(const char *library, const char *name, int isCellName, int getLocker);
  
  /* form list of cells or views in a library */
char *idb_gen_obj_rel(char*library, char*name, int isCellName, int getLocker);
int idb_get_locker(const char*library, const char*cellname, const char*viewname, char *fpath, int getLocker);

int idb_url_retired(void *plib_id, char *cName, char *vName);
  
#endif
  
#ifdef __cplusplus
}
#endif

#endif



