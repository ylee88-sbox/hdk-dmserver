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
# $Id: dmsC_API_new.c,v 1.1.2.29 1998/01/13 23:03:40 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>


#include <dmsC_defines.h>
#include <fwierr.h>
#include <dmsC.h>
#include <dmsC_new.h>
#include <dmsC_functions2.h>
#include <dmsC_internal_functions.h>
#include <table_functions.h>
#include <dmsC_new_vars.h>
#include <idb_functions.h>
#include <fwimsg.h>
#include <dmsC_status.h>
#include <dmsC_reports.h>
#include <dmsC_dm.h>

static char *_LastMessage = 0;
static int _SIZE_LastMessage = 0;

/*-----------------------------------------------------------------------
 * dmsCGetLastMessage 
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars: _LastMessage;
 *                 _SIZE_LastMessage;
 *
 * Description: Sort of a C String class to not reallocate the last message
 *              everytime as old idms did.
 * Just gets the pointer. If pointer is null, allocates first!
 *-----------------------------------------------------------------------*/

/* SHOULD REPORT WHEN MALLOC FAILS !!! */

const char* dmsCGetLastMessage (void)
{
   if (!_LastMessage){
      if ((_LastMessage = (char *)malloc(_SIZE_LastMessage = MAXBUFSIZE)) == 0)
         return 0;
      _LastMessage[0] = '\0';
   }
   return _LastMessage;
}

/*-----------------------------------------------------------------------
 * dmsSetLastMessage
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars: _LastMessage;
 *                 _SIZE_LastMessage;
 *
 * Description: Sort of a C String class to not reallocate the last message
 *              everytime as old idms did.
 * Copies the message into the buffer , reallocates the buffer if necessary
 * and returns it.
 *-----------------------------------------------------------------------*/

/* SHOULD REPORT WHEN MALLOC FAILS !!! */

const char* dmsCSetLastMessage (const char *new_message)
{
   int l=0;
   if (!_LastMessage){
      if ((_LastMessage = (char *)malloc(_SIZE_LastMessage = MAXBUFSIZE)) == 0){
         _SIZE_LastMessage = 0;
         return 0;
      }
      _LastMessage[0] = '\0';
   }
   if (!new_message)
      _LastMessage[0] = '\0'; 
   else{
      l=strlen(_LastMessage);
      if (strlen(new_message) >= (_SIZE_LastMessage - 1)){
         free(_LastMessage);
         _LastMessage = (char *)malloc(_SIZE_LastMessage = 
            l+1);
         if (!_LastMessage){
            _SIZE_LastMessage = 0;
            return 0;
         }
      }
      strcpy(_LastMessage, new_message); 
   }
   return _LastMessage;
}

const char *dmsCGetLastVendorMessage()
{
   return idb_get_cdn_vendor_message();
}

/*-----------------------------------------------------------------------
 * dmsCAppendToLastMessage
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars: _LastMessage;
 *                 _SIZE_LastMessage;
 *
 * Description: Surprisingly, appends a string to the last message
 *-----------------------------------------------------------------------*/

const char* dmsCAppendToLastMessage (const char *to_append)
{
   char *tmp;
   int l=0;
   if (!_LastMessage){
      if ((_LastMessage = (char *)malloc(_SIZE_LastMessage = MAXBUFSIZE)) == 0){
         _SIZE_LastMessage = 0;
         return 0;
      }
      _LastMessage[0] = '\0';
   }
   if (!to_append)
      return _LastMessage;
   if (to_append[0] == '\0')
      return _LastMessage;
   if ((strlen(_LastMessage)+ strlen(to_append)) >= (_SIZE_LastMessage - 1)){
         if (!(tmp = strdup(_LastMessage)))
            return 0; 
         free(_LastMessage);
         l=strlen(_LastMessage);
         _LastMessage = (char *)malloc(_SIZE_LastMessage =
                      l + strlen(to_append) + 1);
         if (!_LastMessage){
            _SIZE_LastMessage = 0;
            return 0;
         }
         strcpy(_LastMessage, tmp);
         free(tmp);
      }
   strcat(_LastMessage, to_append);
   return _LastMessage;
}


static int extend_last_message(int extra_size)
{
   char *tmp;
   int l=0;

   dmsCGetLastMessage(); /* to force allocation if buffer points to null */       
   if (extra_size <= 0)
      return 1;
   if ((strlen(_LastMessage)+extra_size) < _SIZE_LastMessage)
      return 1; /* big enough */

   /* if we are here, we need to reallocate */

   if (!(tmp = strdup(_LastMessage)))
        return 0; 
   l=strlen(_LastMessage);
   free(_LastMessage);
   _LastMessage = (char *)malloc(_SIZE_LastMessage = 
                                 l+ extra_size + 1);
   if (!_LastMessage){
      _SIZE_LastMessage = 0;
      return 0;
   }
   strcpy(_LastMessage, tmp);
   free(tmp);
   return 1;
}


/*-----------------------------------------------------------------------
 * dmsSetLastMessageSize
 *
 * Parameters:
 *    size : new size
 *
 * Modifies:
 *    Static vars: _LastMessage;
 *                 _SIZE_LastMessage;
 *
 * Description: If enough, nothing, otherwise it reallocates
 *-----------------------------------------------------------------------*/


const char *dmsCSetLastMessageSize(int size)
{
   if (size <= _SIZE_LastMessage)
      return _LastMessage;

   if (!_LastMessage)
      free(_LastMessage);

   if ((_LastMessage = (char *)malloc(_SIZE_LastMessage = size)) == 0){
      _SIZE_LastMessage = 0;
      return 0;
   }
   _LastMessage[0] = '\0';
   return _LastMessage;
}


/*-----------------------------------------------------------------------
 * dmsCVarAppendToLastMessage 
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars:  The last message static var
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description: Appends to the last message using directly a sprintf like
 *              format. Extends the buffer if necessary.
 *-----------------------------------------------------------------------*/


const char *dmsCVarAppendToLastMessage(int extra_size, char *format,...)
{
  va_list         ptr;

  if (extra_size < strlen(format)) /* improper call */
    extra_size = strlen(format) + 1024;
  if (!extend_last_message(extra_size))
     return 0;

  va_start(ptr, format);

  vsprintf(_LastMessage + strlen(_LastMessage), format, ptr);

  va_end(ptr);
  return _LastMessage;
}



/*-----------------------------------------------------------------------
 * dmsCPrintWarning
 *
 * Parameters:
 *    variable # of args
 *
 * Modifies:
 *    Static vars: 
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description: prints idms warnings
 *-----------------------------------------------------------------------*/

static char *_warn_prompt = 0;
static int _Lwarn_prompt = 0;


char *dmsCGetWarnPrompt()
{
   if (!_warn_prompt)
      return set_string_in_buff(&_warn_prompt, &_Lwarn_prompt,
                                "*IDMS WARNING* ");
   return _warn_prompt;
}

char *dmsCSetWarnPrompt(char *s)
{
   if (!s)
      return 0;
   return set_string_in_buff(&_warn_prompt, &_Lwarn_prompt, s);   
}


int dmsCGetLastErrorCode()
{
   return dmsLastErrorNumber;
}

int dmsCGetLastStatusCode()
{
   return get_last_status_code();
}


static char *_tmpBuff1 = 0;
static int _LtmpBuff1 = 0;

void dmsCPrintWarning(char *format, ...)
{
  int rc = check_for_buff_space(&_tmpBuff1, &_LtmpBuff1, MAXBUFSIZE);
  va_list         ptr;

  va_start(ptr, format);

  strcpy(_tmpBuff1, dmsCGetWarnPrompt());
  vsprintf(_tmpBuff1 + strlen(_tmpBuff1), format, ptr);

  va_end(ptr);


  fprintf(stderr, _tmpBuff1);
  fflush(stderr);
}


void dmsCPrintError(char *format, ...)
{
  int rc = check_for_buff_space(&_tmpBuff1, &_LtmpBuff1, MAXBUFSIZE);
  va_list         ptr;

  va_start(ptr, format);

  strcpy(_tmpBuff1, "*IDMS ERROR* ");
  vsprintf(_tmpBuff1 + strlen(_tmpBuff1), format, ptr);

  va_end(ptr);


  fprintf(stderr, _tmpBuff1);
  fflush(stderr);
}



/*-----------------------------------------------------------------------
 * dmsCGetToolName 
 *
 * Parameters:
 *    tname - if not null will be used to return the tool name by COPYING
 *            it -- user's responsability to destroy it.
 *    otherwise will use a static buffer which the user SHOULD NOT destroy
 *
 * Modifies:
 *    Static vars: _tnbuff
 *    Global vars:
 *
 * Uses:
 *    Global var: toolName
 *
 *
 * Description: gets the toolName or 0 if problems
 *-----------------------------------------------------------------------*/

static char *_tnbuff = 0;

char *dmsCGetToolName(char *tname)
{
   if (!toolName.name)
      return 0;
   if (!tname){
      if (_tnbuff)
         free(_tnbuff);
      return (_tnbuff = strdup(toolName.name));
   }
   else
      return strcpy(tname, toolName.name);  
}


/*-----------------------------------------------------------------------
 * dmsSetToolName
 *
 * Parameters:
 *   newname - new tool name     
 * Modifies:
 *    Static vars: _tnbuff
 *    Global vars:
 *
 * Uses:
 *    Global var: toolName
 *
 *
 * Description: gets the toolName or 0 if problems
 *-----------------------------------------------------------------------*/

char *dmsCSetToolName(const char *newname)
{
   if (!newname)
      return 0;
   if (!strcmp(toolName.name, newname)) /* already set to that name */
      return toolName.name;
   if (toolName.name)
      free(toolName.name);
   if (!(toolName.name = strdup(newname)))
      return 0;
   dmsSetSearchPath();
   dmsSetToolConfig();
   return toolName.name;
}


/*-----------------------------------------------------------------------
 *  TABLE RELATED FUNCTIONS  
 *
 * Parameters:
 *
 * Modifies:
 *    Static vars: 
 *    Global vars:
 *
 * Uses:
 *    Global var: dmsAppName, dmsDoLogAfterAll
 *
 *
 * Description: 
 *-----------------------------------------------------------------------*/

static char tableRowContents[BUFFER_LENGTH];

/* Procedure to get a table ptr to the begining 
 * of the table */

int dmsCGetTable(char *tableName, idmTable **tablePtr)
{
    idmTable *t = (idmTable *)getGlobalTablePtr();
    
    if ((tableName == NULL) || (strlen(tableName) == 0))
	return(idmsError);

    while ((t != NULL) && (strcmp(tableName,t->name) != 0))
	t = t->nextTable;
    if (t == NULL)
	return(idmsError);
    *tablePtr = t;
    return(idmsSuccess);
}

/* this procedure will copy all the columns
** of the tablePtr into the global structure
** - the rownumber is specified as an argument
*/
int dmsCGetTableEntry(idmTable *tablePtr, int rowNumber, char **tableEntry)
{
	int i;
	idmTableRowEntry *row;
	idmTableColumnEntry *column;
	
	if ((tablePtr == NULL) ||
		(rowNumber <= 0))
			return(idmsBadparameter);
			
			
 	row = tablePtr->table;
	for(i=1;i<rowNumber;++i) {
		if (row == NULL)
			return(idmsNoRow);
		row = row->nextRow; 
	}

	if (row == NULL)
		return(idmsNoRow);
	
	/* copy all the columns into the global structure
	** and point the tableEntry ptr to that 
	*/
	tableRowContents[0] = '\0';
	column = row->columns;
	while (column != NULL) {
		if ((strlen(column->valueList)+1)>= BUFFER_LENGTH) {
			dmsCPrintWarning("Buffer Length exceeded in dmsCGetTableEntry");
			return(idmsError);
		}
		strcat(tableRowContents,column->valueList);
		strcat(tableRowContents," ");
		column = column->nextColumn;
	}
	*tableEntry = tableRowContents;	
	return(idmsSuccess);
}



/*-----------------------------------------------------------------------
 * dmsCRefreshAllLib
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
 * Description: refreshes the libraries if needed by the DM 
 *-----------------------------------------------------------------------*/


int dmsCRefreshAllLib(char *outString)
{
   return idb_refresh_all_libraries(outString);
}


/******
long coralCGetCheckInTime(void *pver_id)
{
   return idb_get_checkin_time(pver_id);
}
******/


int dmsCPathInitDMS(char *appName, char *bootName, char *dmsPath, 
                   int nullgtMode)
{
   int noError,error;
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCPathInitDMS(%s,%s,%s,%d)\n",
                      appName, bootName, dmsPath, nullgtMode);

   dmsPIInitDMS(bootName, dmsPath, nullgtMode, &noError);
   error = dmsGenErrorCode(noError);

   if (debugDmsOn){
      dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCPathInitDMS(%s,%s,%s,%d)\n",
                      error, appName, bootName, dmsPath, nullgtMode);
      dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n", dmsLastErrorNumber,
                      dmsCGetLastMessage());
   }
   return error;
}


void *coralCGetLibIdByLib(char *lName, char *cName, char *vName,
                          char *viewType, int *error)
{
   int noError;
   char FPN[BUFSIZE];
   /*** void *plib_id = idb_allocate_lib_id(); ***/
   void *plib_id = NULL;

   if (debugDmsOn)
       dmsPrintIdmsLog("<<*C-CALL*>> dmsCGetLibIdByLib(%s,%s,%s,%s,%p)\n",
                       lName, cName, vName, viewType, &plib_id);

   if (idmsNotInitialized) {
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, IDMSNOTINITIALIZED);
      return 0;
   }
   strcpy(theLibName,lName);

   dmsReadCellView(1,3, DONT_CREATE, cName, vName, "", viewType, FPN, &plib_id,
                   &noError, DONT_CHANGE_CODES);

   /* close the library if it was opened because some code is dum */
   idb_close_cellview(plib_id, cName, vName, "");
   *error = dmsGenErrorCode(noError);

   if (debugDmsOn){
      dmsPrintIdmsLog("<<*RTN(%d)*>> dmsCGetLibIdByLib(%s,%s,%s,%s,%p)\n",
                      *error, lName, cName, vName, viewType, &plib_id);
      dmsPrintIdmsLog("<<*MSG(%d)*>> %s\n",dmsLastErrorNumber,
                      dmsCGetLastMessage());
   }
   if (idb_valid_lib_id(plib_id))
      return plib_id;
   else{
      /***idb_deallocate_lib_id(plib_id); ***/
      return 0;
   }
}


void *coralCGetLibId(char *cName, char *vName, char *viewType, int *error)
{
   return coralCGetLibIdByLib("*", cName, vName, viewType, error);
}


int coralCRemoveCVFromConfig(void *plib_id, char *confName, char *cName, 
                          char *vName)
{
   char locker[BUFSIZE], *libNamePtr;
   char libName[BUFSIZE];
   char *tverName;

   if (!plib_id)
      return save_status(idmsCvNotFound, 0, 0, cName, vName, "", "", "",
                         DONT_CHANGE_CODES); 

    if(libNamePtr = (char *)idb_get_library_name(plib_id))
      strcpy(libName, libNamePtr);
    else
     return save_status(idmsCvNotFound, 0, 0, cName, vName, "", "", "",
                         DONT_CHANGE_CODES);
 
   if (idb_find_cellview(plib_id, cName, vName)) {
      if (!(idb_find_version_and_locker(plib_id, cName, vName, "",
            locker, 0, GET_LAST_IF_NOT_LOCKED, &tverName))) {
         dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, VERSIONNOTEXIST, cName, vName, libName, "");
         return save_status(idmsCvNotFound, 0, 0, (char *)cName,
                            (char *)vName, "", "", "", DONT_CHANGE_CODES);
      }
   }

   if (!idb_remove_cellview_from_config(plib_id, confName, cName, vName)){
      idb_capture_dm_vendor_warning();
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, CANTRMFROMCONFIG, libName, cName, vName,
                     confName);
      return save_status(idmsCvNotFound, 0, 0, cName, vName, "", "", "",
                         DONT_CHANGE_CODES);
   }

   dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,RMFROMCONFIGOK, libName, cName, vName, confName);
   return save_status(idmsCvInRwlib,
                      (char *)idb_get_library_directory(plib_id, "r"),
                      libName, cName, vName,
                      tverName, locker, 0, DONT_CHANGE_CODES);
}


int coralCAddCVToConfig(void *plib_id, char *confName, char *cName,
                      char *vName, char *verName, int bind)
{
  char *tverName, *libNamePtr;
  char locker[BUFSIZE];
  char libName[BUFSIZE];

  if (!plib_id)
      return save_status(idmsCvNotFound, 0, 0, cName, vName, "", "", "",
                         DONT_CHANGE_CODES);

  if(libNamePtr = (char *)idb_get_library_name(plib_id))
    strcpy(libName, libNamePtr);
  else
    return save_status(idmsCvNotFound, 0, 0, cName, vName, "", "", "",
                         DONT_CHANGE_CODES);

  if (!(idb_find_version_and_locker(plib_id, cName, vName, verName,
						locker, 0, 0, &tverName))) {
     dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, VERSIONNOTEXIST, cName, vName, libName, "");
     return save_status(idmsCvNotFound, 0, 0, (char *)cName,
			(char *)vName, "", "", "", DONT_CHANGE_CODES);
  }

  if (debugDmsOn) {
      fprintf(stderr, "locker:%s\n", locker);
      fflush(stderr);
  }

  if(!idb_add_cellview_to_config(plib_id, confName, cName, vName, verName, bind)) {
    idb_capture_dm_vendor_warning();
    dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, WARNING, CANTADDTOCONFIG, libName, cName, vName,
		   confName);
    return save_status(idmsCvNotFound, 0, 0, cName, vName, "", "", "",
		       DONT_CHANGE_CODES);
   }

  dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, OKTYPE,ADDTOCONFIGOK, libName, cName, vName, confName);
  return save_status(idmsCvInRwlib,
		    (char *)idb_get_library_directory(plib_id, "r"),
		    libName, cName, vName,
		    tverName, locker, 0, DONT_CHANGE_CODES);
}


int dmsCExecutePreTrigger(char *triggName, char *libName, char *cName,
                          char *vName, char *verName, char *FPN)
{
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCExecutePreTrigger(%s, %s, %s, %s, %s, %s\n",
                       triggName, libName, cName, vName, verName, FPN);

   if (!idmExecuteUserTrigger(triggName, 1, libName, cName, vName, 
                              verName, FPN)){
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, PRETRIGGERFAILED);
      dmsCPrintWarning("COULD NOT EXECUTE PRE-TRIGGER\n");
      return idmsPreTriggerFailed;
   }
   return idmsSuccess;
}


int dmsCExecutePostTrigger(char *triggName, char *libName, char *cName,
                           char *vName, char *verName, char *FPN)
{
   if (debugDmsOn)
      dmsPrintIdmsLog("<<*C-CALL*>> dmsCExecutePostTrigger(%s, %s, %s, %s, %s, %s\n",
                       triggName, libName, cName, vName, verName, FPN);

   if (!idmExecuteUserTrigger(triggName, 0, libName, cName, vName, 
                              verName, FPN)){
      dmsFormMessage(__FUNCTION__, __FILE__, __LINE__, ERROR, POSTTRIGGERFAILED);
      dmsCPrintWarning("COULD NOT EXECUTE POST-TRIGGER\n");
      return idmsPostTriggerFailed;
   }
   return idmsSuccess;
}


/**************
int dmsCSetGeneratedAttribute(char *event, char *libName, char *cellName,
                              char *viewName, char *versionName)
{
    if (!idmSetGeneratedAttribute(event, libName, cellName, viewName,
                                           versionName)){
       dmsCPrintWarning("COULD NOT SET GENERATED ATTRIBUTE\n");
       return 0;
    }
    return 1;
}
**************/

int dmsCSetGeneratedAttribute(char *event, char *libName, char *cellName,
                              char *viewName, char *versionName)
{
       dmsCPrintWarning("WILL NOT SET GENERATED ATTRIBUTE AT THIS TIME\n");
       return 0;
    return 1;
}
