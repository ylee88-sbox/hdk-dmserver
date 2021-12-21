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
# $Id: fwattr.c,v 1.1.2.3.4.1.4.1.6.8 1997/11/17 23:39:01 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
/* 
#Revision 1.39  1994/03/02  22:58:18  fw
#puts dbGetPropByName in idmDeleteAttr in slave conditional
#Revision 1.2  93/01/13  14:07:06  dhejmad
#synched with 2.1
*/


/* 
**   This sourcefile contains the attribute PI source code.
*/

/**********************************************************
** Changes for 4.4.x:
**
** Prop Implementation:
** Props are being stored in the vault. We cannot store
** them in the cadence cell/view/prop.xx file since, the cv
** needs to be chkedout. So we store the non-shadow props 
** on the vault url:
** "[.../lib/cell/view/cell.view|.../lib/cell/view.sync.cds];ver"
**
** Shadow Props:
** Shadow props are NOT being stored in .iDMS.
** For cheked out CVs, props are being stored in the
** locally in: ".../lib/cell/view/cell.view|.../lib/cell/view.sync.cds".
**
** Prop Info:
** Prop Storage: All Sync user defined meta-data can be
** in the form of: "propName" "propVal". They are actually
** being stored as: "propName" "propType propValue". List 
** prop values being set from Skill are first converted into 
** strings. Hier props can be set as:"gp/p/c/gc" "pTyp pVal"
** If setting hier props, the calling function must ensure
** and interpret the syntax correctly while setting and 
** retrieving props respectively.
**
** viewId prop: The concept of viewId is no longer available in
** 4.4.x. So,  viewId props are stored on the vault url:
** ".../lib/prop.xx". They would be stored as:
** "vName:pName" "pType pValue"
**********************************************************/


/* #define voRandom_h */
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
#include <itkDB.h>
#endif
#include <fwidms.h>
/** #include <il.h> **/
#include <attr.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <fwtable.h>
#include <fwimsg.h>
#include <idb_functions.h>
#include <dmsC_defines.h>

int ListAttrCode;
char ListAttrLibName[1024];
char ListAttrVersionName[1024];
char ListAttrLibPathName[1024];

/* global variable to handle the problem in checkin 
** for unix files so that the shadow attribute 
** cellview version's id can be cached 
** for use by the trigger
*/

/* global space allocated to keep the attribute value
** and type. This is to prevent user from needing to 
** allocate and de-allocate space
*/

/*** FIX - for the netlister writting HUGE properties - making the
** attr value as a heap allocation
**/
char *globalAttrValue,globalAttrType[1024],globalAttrName[1024];
int lengthGlobalAttrValue=0;
char globalJunkBuffer[8191];
Boolean globalAttrDisableFlag;
Boolean globalUnixAttrToFile;
extern Boolean idmsNotInitialized;
extern int debugDmsOn;

/* global variables to be used by idmIsVersionCheckedOut for
** finding out from skill if version is checked out
*/
char lNameforSkill[1024];
char cNameforSkill[1024];
char vNameforSkill[1024];

/* the global variable to hold the keyword for 
** mapping functions if any 
*/
char globalMappingKeyword[1024];
char skillCommand[2048];
long checkLibNameVar; /* HACK TO SEE IF THERE IS CORRUPTION */

/********** dss stuff ***************************************/
static char **_dssResults;
char _dssTmpAttrValue[8191*5];

/*********************
PRIVATE INTERFACE BEGINS HERE
*******************/

/* forward declaration */
Boolean idmSetAttr(char *, char *, char *, char , Boolean );
void dmsVoFreePtrs(int num, ...);

/* This procedure returns the versionId of the 
** 0.0 object from skill - for some reason the 
** C interface in DM does not return 0.0 
** versionId
*/
/*************************************************
dmVersionId
idmGetVersion0FromSkill(libName, cellName, viewName)
char *libName, *cellName, *viewName;
{
   dmVersionId verId;
   dmLibId libId;
   dmGenStatePtr pState;
   int pi,si;
   char vid[1024];

   libId=dmFindLib(libName);
   if (libId==NULL) {
      return(NULL);
   }
   pState=dmStartGenVersion(libId,cellName,viewName);
   while (dmGenVersion(pState,&verId)) {
      dmGetVersionIndex(verId,&pi,&si);
      sprintf(vid,"%d.%d",pi,si);
      if (strcmp(vid,"0.0")==0) {
         dmStopGen(pState);
         return(verId);
      }
   }
   dmStopGen(pState);
   return(NULL);
}
****************************************************/

/* simple utility procedure which looks at a cellview versions
** record to decide if the version is checked out or not 
** and returns a TRUE if it is
*/
/****************************************************
Boolean
idmIsVersionCheckedOut(versionId)
dmVersionId versionId;
{
** THIS STUFF IS SUSPECTED TO BE BUGGY 
 dmRec *record;
 
 if ((record = dmGetRec(versionId)) != NULL) {
     if (record->version.checkOutCount > 0) 
	 return(TRUE);
 }
 return(FALSE);
**

    char path[1024], locker[1024];
    char version[1024];
    char tmppath[1024];
    int p,s;

	if (strlen(lNameforSkill) == 0)
		return(FALSE);

    if (!dmGetVersionIndex(versionId, &p, &s))
		return(FALSE);

    sprintf(version,"%d.%d",p,s);
    idb_find_version_and_locker(idb_find_lib_id(lNameforSkill, tmppath, "r"),
                                cNameforSkill,
                                vNameforSkill, version, locker, path, 
                                GET_LAST_IF_NOT_LOCKED, 0);
    if (!strcmp(locker,"NONE") || !strcmp(locker, ""))
		return(FALSE);
    return(TRUE);
}
****************************************************/

/* This procedure will tell the caller whether the
** file is in a co@user area (or equivalent) 
** or not - it could very likely be made much nicer
** in the future - this is an EMERGENCY
*/
/****************************************************
Boolean
idmIsFileInUnixArea(path)
char *path;
{
* co@ is the original mapping func, check_out is the
** reverse mapping func - is set at run-time by a flag
*
    if (strstr(path,globalMappingKeyword) != NULL)  
		return(FALSE);
    return(TRUE);
}
****************************************************/

/* this procedure will read the global file which is
** stored in the fwtable.c file. And then nullify all 
** the attributes which are listed in that table 
** as nullified by the setting of this attribute
*/
/***************************************************
Boolean
idmNullifyRelatedAttributes(objId,attrName)
dbId objId;
char *attrName;
{
    char nullifyList[1024];
	char temp[1024];
    char *nullList = nullifyList;
    char *ptr;
    Boolean retVal = TRUE;
    dbPropType propType = dbcStringType;

    if (idmGetColumn(IDMS_ATTRIBUTE_TABLENAME,attrName,
		     IDMS_ATTRIBUTE_NULLIFY_COLUMN, &nullList)
		== idmsBadparameter) 
		return(FALSE);
    if (nullifyList == NULL) 
		return(TRUE);
    
    * copy the list to another place and then get each attribute
    ** and nullify it
    *
    strcpy(temp,nullifyList);
    ptr = strtok(temp,iDMS_ListDelimiter);
    while (ptr != NULL) {
	* we cannot simply replace the attribute since it maybe 
	** hierachical - also we set the nullify flag to FALSE to
	** prevent cascading
	*
		retVal = retVal && idmSetAttr(objId,ptr,IDMS_ATTRIBUTE_NULLIFY_VALUE,
			      propType,FALSE);
		ptr = strtok(NULL,iDMS_ListDelimiter);
    }
    return(retVal);
}
*******************************************************/


/* this procedure simple traverses down a specified hierarchy
** returning the object Id of the parent attribute of the leaf
** attributeName - as a side-effect it modifies attrName to 
** return the leaf attribute.
** Depending on the mode it either creates the hierarchy tree
** or does not create it, returning NULL.
*/
/******************************************************
dbPropId
idmGetHierAttrId(objectId, attrName, mode)
dbId objectId;
char *attrName;
int  mode;
{
    String token;
    char temp[NAMELENGTH];
    dbId parentId, propId;
    dbPropType pType;
    dbPropValue pVal;
    char tempAttrName[NAMELENGTH];

    if (strcpy(tempAttrName,attrName) == NULL)
		return(NULL);
    token = strtok(tempAttrName,iDMS_HIERARCHY_SEPERATOR_STRING);
    propId = objectId;
    while (token != NULL) {
		parentId = propId;
		if ((propId = dbGetPropByName(parentId, token, &pType, &pVal)) == NULL) {
	    	if (mode) {
				if ((propId = dbCreateHierProp(parentId, token)) == NULL)
		    		return(NULL);
	    	}
	    	else { * if mode is not CREATE_IT *
				return(NULL);
	    	}
		}
		* else just move the token into temp and get another token *
		strcpy(temp, token);
		token = strtok(NULL, iDMS_HIERARCHY_SEPERATOR_STRING);
    }
    * now parentId contains the id pf the parent and temp contains the
    ** child attribute
    *
    if (strcpy(attrName, temp) == NULL)
		return(NULL);
    return(parentId);
}
****************************************************************/

/* this procedure will set the attribute for the simple cases 
** and also the complex case of the hierarchical attribute 
** where the name is for eg (parentAttr/child/grandchild).
** In this we just seperate the two cases using the strchr 
** c lib call
**
** Added feature to read global table and then nullify 
** attributes which are so defined in the global table
** - the nullify parameter is set to FALSE, if the calling
** program does not want the global table to be checked for
** whether this attributes' setting should nullify other 
** attributes.
** 
** For 4.4.x: hierarchical attributes functionality is not
** provided. At this point I so not know why is nullify feature
** is being used. -sh 3/3/00
*/

/*
  Sorin - adding this function to be able to compile optimized - the thing does not find tolower
*/

inline char ToLower (char c)
{
  return c | ('a' - 'A');
}

Boolean
idmSetAttr(char *dssObj, char *attrName, char *attrValue, char attrType, 
								Boolean nullify)
{
    HRESULT hresult;

    /* get the value into the appropriate format depending on type */
    switch(attrType) {
	case(iDMS_BOOLEAN): {
	    if ((attrValue != NULL) && (ToLower(attrValue[0]) == 't'))
		sprintf(_dssTmpAttrValue, "%c TRUE", attrType);
	    else
		sprintf(_dssTmpAttrValue, "%c FALSE", attrType);
	    break;
	}
	case(iDMS_FLOAT): {
		if(attrValue != NULL)
			sprintf(_dssTmpAttrValue, "%c %s", attrType, attrValue);
		else
			sprintf(_dssTmpAttrValue, "%c iDMSNULLFLOATVALUE", attrType);
	    break;
	}
	case(iDMS_INTEGER): {
		if(attrValue != NULL)
			sprintf(_dssTmpAttrValue, "%c %s", attrType, attrValue);
		else
			sprintf(_dssTmpAttrValue, "%c iDMSNULLINTVALUE", attrType);
	    break;
	}
	case(iDMS_STRING): {
	    if (attrValue != NULL)
			sprintf(_dssTmpAttrValue, "%c %s", attrType, attrValue);
	    else
			sprintf(_dssTmpAttrValue, "%c %s", attrType, iDMSNULLSTRINGVAL);
	    break;
	}
	case(iDMS_TIME): {
	    if (attrValue != NULL)
			sprintf(_dssTmpAttrValue, "%c %s", attrType, attrValue);
	    else
			sprintf(_dssTmpAttrValue, "%c", attrType);

	    break;
	}
#ifdef SLAVEATTR
	case(iDMS_LIST): {
	    if (attrValue != NULL)
			sprintf(_dssTmpAttrValue, "%c %s", attrType, attrValue);
	    else
			sprintf(_dssTmpAttrValue, "%c", attrType);

		break;
	}
#endif
    default:
		return(FALSE);
    }

    /** REM: hierarchical attribues and the nullify stuff have been whacked !!
								-sh 3/3/00 
    ***/
    hresult = sapiUrlSetprop(NULL, dssObj, attrName, _dssTmpAttrValue, NULL, 
							&_dssResults);
    if(SUCCEEDED(hresult))
      return(TRUE);
    else
      return(FALSE);
}

/* to set several attributes on one object */
/*************************************************************************
Boolean
idmSetAllAttr(objectId,attrNameList,attrValueList,attrTypeList,nullify,size)
dbId objectId;
char *attrNameList;
char *attrValueList;
char *attrTypeList;
Boolean nullify;
int  size;
*************************************************************************/




/* this is a simple procedure - just get the value of the 
** attribute and depending on the type package it into a 
** string. However first check if this is an hierarchical 
** attribute name, and if it is decode the name to get 
** the required leaf
** For 4.4.x: Hierarhical attributes will be implemented later.
*/
Boolean
idmGetAttr(char *dssObj, char *attrName, char **attrValue, char *attrType, int *lenAttrVal)
{
  HRESULT hresult;
  int i=0, len=0;
  char *tmpCharPtr;

    /* if this object is a version is it checked out ?? */

    hresult = sapiUrlGetprop(NULL, dssObj, attrName, NULL, &_dssResults);
    if (FAILED(hresult))
	return(FALSE);

    /* set defaults */
    strcpy(_dssTmpAttrValue, "");
    strcpy(attrType, "s");

    /* get attrtype and attr value from _dssResults */
    while(_dssResults && _dssResults[i] && strcmp(_dssResults[i], "")) {
      if(i == 0) {
	tmpCharPtr = idb_strip_nth_token(_dssResults[i], " ", 1);
	if(tmpCharPtr && strcmp(tmpCharPtr, "")) {
	  strcpy(attrType, tmpCharPtr);
	  tmpCharPtr = strchr(_dssResults[i], attrType[0]);
	  strcpy(_dssTmpAttrValue, tmpCharPtr+2);
	}
      }
      else
	sprintf(_dssTmpAttrValue, "%s %s", _dssTmpAttrValue, _dssResults[i]);
      ++i;
    }

    if ((len=strlen(_dssTmpAttrValue)) >= *lenAttrVal) {
	    if(*attrValue) free(*attrValue);
	    *attrValue = (char *) malloc(len+1);
	    *lenAttrVal = len;
    }

    switch (attrType[0]) {
	case(iDMS_BOOLEAN): {
	    strcpy(attrType,"boolean");
	    strcpy(*attrValue,_dssTmpAttrValue);
	    break;
	}
	case(iDMS_FLOAT): {
	    strcpy(attrType,"float");
	    strcpy(*attrValue,_dssTmpAttrValue);
	    break;
	}
	case(iDMS_INTEGER): {
	    strcpy(attrType,"integer");
	    strcpy(*attrValue,_dssTmpAttrValue);
	    break;
	}
	case(iDMS_STRING): {
	    strcpy(attrType,"string");
	    strcpy(*attrValue,_dssTmpAttrValue);
	    break;
	}
	case(iDMS_TIME): {
	    strcpy(attrType,"time");
	    sprintf(*attrValue,"%s",_dssTmpAttrValue);
	    break;
	}
#ifdef SLAVEATTR
	case(iDMS_LIST): {
	    strcpy(attrType,"list");
	    strcpy(*attrValue,_dssTmpAttrValue);
	    break;
	}
#endif
	default:
	    return(FALSE);
    }
    return(TRUE);
}

/* This procedure uses the above procedure to get all
** the attributes associated with the object given
*/
Boolean
idmGetAllAttr(char *dssObj, char *attrName, char *attrValue, char *attrType)
{
  printf("idmGetAllAttr will be implemented later\n");
}

/* 
** If this object is not a checked out version then
** simply delete the atttribute, else depending on 
** whether it is checked out or not delete it in
** the correct place
** Since there is no sapiUrlDeleteProp, the propValue is being set to "" 
**								-sh 3/7/00
*/
Boolean
idmDeleteAttr(char *dssObj, char *attrName)
{    
    if(SUCCEEDED(sapiUrlSetprop(NULL, dssObj, attrName, "", NULL, 
						&_dssResults)))
      return(TRUE);
    else
      return(FALSE);

}


/* the private interface for the unix meta-data begins here
** the procedures below will track files in unix directories
** by means of shadow libs in those directories 
*/

/* 
** Critical procedure: This procedure has the 
** responsibility to do the following:
** - find lib corresponding to path (create it if mode is create)
** - open this lib (put into search path)
** - get versionId corresponding to unix file (cell.view)
*/

/**************************************************
dmCellViewId
idmGetUnixShadowVersionId(cellName, viewName, path, mode, shadowLibId)
char *cellName, *viewName, *path;
int mode;
dmLibId *shadowLibId; 


Boolean
idmSetUnixAttr(cellName,viewName,path,attrName,attrValue,propType)
char *cellName, *viewName, *path, *attrName, *attrValue;
dbPropType propType;
{
    dmCellViewId versionId;
    Boolean retVal;
    dmLibId lib;

    if ((versionId = idmGetUnixShadowVersionId(cellName,
			 viewName, path, CREATE_IT, &lib)) == NULL)
	return(FALSE);
    
    * now that we have the dmVersionId corresponding to the
    ** shadow object which holds the unix files meta-data
    ** we have reduced this to the earlier cadence case and
    ** can simply call the earlier procedure
    *
    retVal=idmSetAttr(versionId, attrName, attrValue, propType, TRUE);
    return(retVal);
}

Boolean
idmSetAllUnixAttr(cellName,viewName,path,attrName,attrValue,attrType,size)
char *cellName, *viewName, *path; 
char *attrName, *attrValue, *attrType;
{
	dmCellViewId versionId;
	Boolean retVal;
	dmLibId lib;
	char attrNameList[1024];
	char attrTypeList[1024];
	char attrValList[1024];
	int i=0;
	list skillret;

	if (globalUnixAttrToFile) {
		strcpy(attrNameList, "'(");
		strcpy(attrTypeList,"'(");
		strcpy(attrValList,"'(");
		while (strlen(attrName)>0) {
			sprintf(attrNameList,"%s \"%s\"",attrNameList,attrName);
			sprintf(attrValList,"%s \"%s\"",attrValList,attrValue);
			sprintf(attrTypeList,"%s \"%s\"",attrTypeList,attrType);
			attrName += size;
			attrValue += size;
			attrType += size;
		}
		strcat(attrNameList,")");
		strcat(attrValList,")");
		strcat(attrTypeList,")");

		sprintf(skillCommand,"idmSetAllAttrToFile(\"%s\" \"%s\" \"%s\" %s %s %s)",
			path,cellName,viewName,attrNameList,attrValList,attrTypeList);

		skillret=ilcNil;
		skillret = ilErrSetString2(skillCommand,1);
		while (!ilIsNull(skillret) && (!ilIsSymbol(skillret))) {
				skillret = ilCar(skillret);
		}
		return(skillret==ilcT ? TRUE : FALSE);
	}

	if ((versionId = idmGetUnixShadowVersionId(cellName,viewName,path,CREATE_IT,&lib)) 
		== NULL)
		return(FALSE);

	retVal=idmSetAllAttr(versionId,attrName,attrValue,attrType,TRUE,size);
	return(retVal);
}


Boolean
idmGetUnixAttr(cellName,viewName,path,attrName,attrValue,attrType,lenVal)
char *cellName, *viewName, *path, *attrName, **attrValue;
char *attrType;
int *lenVal;
{
    dmCellViewId versionId;
    Boolean retVal;
    dmLibId lib;

    if ((versionId = idmGetUnixShadowVersionId(cellName,
			 viewName, path, NO_CREATE, &lib)) == NULL)
	return(FALSE);
    retVal = idmGetAttr(versionId, attrName, attrValue, attrType,lenVal);
    return(retVal);
}


Boolean
idmGetAllUnixAttr(cellName,viewName,path,attrName,attrValue,attrType)
char *cellName, *viewName, *path, *attrName, *attrValue;
char *attrType;
{
    dmCellViewId versionId;
    Boolean retVal;
    dmLibId lib;

    if ((versionId = idmGetUnixShadowVersionId(cellName,
			 viewName, path, NO_CREATE, &lib)) == NULL)
	return(FALSE);
    retVal = idmGetAllAttr(versionId, attrName, attrValue, attrType);
    return(retVal);
}



Boolean
idmDeleteUnixAttr(cellName,viewName,path,attrName)
char *cellName, *viewName, *path, *attrName;
{ 
    dmCellViewId versionId;
    Boolean retVal;
    dmLibId lib;

    if ((versionId = idmGetUnixShadowVersionId(cellName,
			 viewName, path, NO_CREATE, &lib)) == NULL)
  	return(FALSE);
    retVal = idmDeleteAttr(versionId,attrName);
    return(retVal);
}
**************************************************/

/* This is a hack procedure which sets the global 
** variables which will later be used by the 
** checkin trigger 
*/
/**************************************************
void idmSetCheckinGlobalVar(char *cellName, char *viewName)
{
    dmLibId libId;
    int status;
    DMSINFO *cvInfo;

    * if PI is disabled then simply return *
    if (globalAttrDisableFlag)
	return;

    idmsGlobalCheckinVersionShadowId = NULL;
    idmsGlobalCheckinShadowLibId = NULL;
    status = dmsCInfoCellView(0, cellName,viewName,&cvInfo);
    if (status == idmsInfoCvIsInWorkdir) {
	idmsGlobalCheckinVersionShadowId =
	    idmGetUnixShadowVersionId(cellName,viewName,
	      cvInfo->fullPathName,NO_CREATE, &idmsGlobalCheckinShadowLibId);
    }
    else if (status == idmsInfoCvCheckedOutByMe)  {
	* check to see if it is in unix/co@ area *
	if (idmIsFileInUnixArea(cvInfo->fullPathName))
	    idmsGlobalCheckinVersionShadowId =
		idmGetUnixShadowVersionId(cellName,viewName,
                cvInfo->fullPathName,NO_CREATE, &idmsGlobalCheckinShadowLibId);
    }
}
**************************************************/

/* procedure to copy all properties on one version
** to another dmVersion - when the versions are in
** two different libraries
** This procedure checks againt the global file table
** to ensure that only those properties mentiones as
** to be inherited w/wo value are copied
*/
/**************************************************
Boolean
idmCopyAllProps(sourceId, destId, checkout)
dbId sourceId, destId;
Boolean checkout;
{
    dbGenStateId state;
    dbPropId propId, temp;
    dbPropType propType;
    dbPropValue propValue;
    String propName;
    char valHolder[1024];
    char nullArray[20];
    char *val = valHolder;
    Boolean returnVal = FALSE;

    state = dbStartGenProp(sourceId);
    while (dbGenProp(state, &propId)) {
	dbGetProp(propId, &propName, &propType, &propValue);
	if (propType == dbcHierPropType) {
	    * for a hierarchical property this is a recursive
	    ** call
	    *
	    temp = dbReplaceHierProp(destId, propName);
	    if (temp) {
		if( !idmCopyAllProps(propId,temp,checkout)) {
		    * delete the hierarchical attribute 
		     * since there are no sub attributes
		     *
		    dbDeletePropByName(destId, propName);
		} 
	    }
	}
	else {
	    * non-hierarchical is simple - simply create it
	     **  on destination if it is on the inheritance list
	     *
	    if (checkout) {
		* look at the attribute control table *
		if (idmGetColumn(IDMS_ATTRIBUTE_TABLENAME, propName,
		     IDMS_ATTRIBUTE_INHERIT_COLUMN,&val) == idmsSuccess) {
		    if (strcmp(val,IDMS_ATTRIBUTE_INHERIT_VALUE) == 0) {
			dbReplaceProp(destId, propName, propType, propValue);
			returnVal = TRUE;
		    }
		    else if (strcmp(val,IDMS_ATTRIBUTE_INHERIT_NULL) == 0) {
			propType = dbcStringType;
			strcpy(nullArray,IDMS_ATTRIBUTE_NULLIFY_VALUE);
			propValue.aString = nullArray;
			dbReplaceProp(destId, propName, propType, propValue);
			returnVal = TRUE;
		    }
		}
	    }
	    else {
		* no need to look - simply replace *
		dbReplaceProp(destId, propName, propType, propValue);  
		returnVal = TRUE;
	    }
	}
    }
    dbStopGen(state);
    return(returnVal);
}
**************************************************/

/*
** procedure which returns the number of elements in a 
** colon delimited list, and also seperated each element
** into an array
*/
int
idmFillArgs(char *inputList, char *outArray[], int optional)
{
    int i = 0;
    char *t, *temp;

    if ((inputList == NULL) || (strlen(inputList) == 0)) {
		outArray[i] = NULL;
		return((optional == 1) ? 1 : 0);
    }

	if (strlen(inputList) > 8190)
		return(-1);
	strcpy(globalJunkBuffer,inputList);
    t = strtok(globalJunkBuffer,iDMS_ListDelimiter);
    while (t != NULL) {
		if (strcmp(t,iDMS_EmptyListSymbol) == 0)
	    	temp = NULL;
		else {
	    	temp = (char *) malloc(strlen(t)+1);
	    	strcpy(temp,t);
		}
		outArray[i++] = temp;
		t = strtok(NULL,iDMS_ListDelimiter);
    }
    return(i);
}

/***************************
 *********PUBLIC INTERFACE BEGINS HERE *****************
********************************/

/**********
** This procedure is called by applications which want to set 
** attributes on a particular object. This procedure does NO
** sanity checks - it assumes that whatever it is asked to do
** is perfectly ok. Later on it is possible that this may have to 
** be moved into the private interface/ more error checking has to be
** added into it
****************/

int dmsCSetAttribute(char *libName, char *cellName, char *viewName,
                     char *versionName, char *attrName, char *attrType,
                     char *attrValue)
/*library name - optional*/
/*cellName - compulsary*/
/*viewName - compulsary*/
/* versionName - optional */
/*attribute Name - compulsary*/
/*attribute Type - compulsary */
/*attribute Value - optional */
{
    ddId libId;
    ddId cvId;
    ddId cellId;
    ddId viewId;
    ddId intelCVId;
    int status;
    Boolean UnixMD = FALSE;
    DMSINFO *cvInfo;
    char *ddObjPath, dssObjVaultPath[1024], ver[256];
    HRESULT hresult;

    /* if not init the return error */
    if (idmsNotInitialized)
	return(idmsNotInit);
    /* if PI is disabled then simply return success */
    if (globalAttrDisableFlag)
	return(idmsSuccess);

    lNameforSkill[0] = '\0';
    cNameforSkill[0] = '\0';
    vNameforSkill[0] = '\0';

    /* check every argument to see if they are "" */
    if (libName!=NULL) {
    if (strlen(libName) == 0)
	libName = NULL;
    else 
	strcpy(lNameforSkill,libName);
    }

    if (cellName !=NULL)
       if (strlen(cellName) == 0)
          cellName = NULL;

    if (viewName !=NULL)
       if (strlen(viewName) == 0)
          viewName = NULL;

    if(versionName !=NULL)
       if (strlen(versionName) == 0)
          versionName = NULL;

#ifndef OH_MACH_LINUX
    libId = cellId = viewId = cvId = intelCVId = NULL;
#else
    libId=NULL;
    cellId=NULL;
    viewId=NULL;
    cvId=NULL;
    intelCVId=NULL;
#endif

    /* check for compulsary parameters */
    if (((libName == NULL) && (cellName == NULL) && (viewName == NULL)) ||
	(attrName == NULL) ||
	(attrType == NULL) 
	)
	return(idmsBadparameter);

    if ((libName == NULL) || (strlen(libName) == 0)) {
	/* it is possible that the cell and view is 
	** not present in the cadence DMS at all - in such a
	** case we will set a switch to go the unix meta-data
	** approach
	*/
	status = dmsCInfoCellView(0, cellName,viewName,&cvInfo);
	strcpy(lNameforSkill,cvInfo->libName);
	switch (status) {
	    case(idmsInfoCvIsInWorkdir):{
		libId = ddGetObj(cvInfo->libName,NULL,NULL,NULL,NULL,"r");
		UnixMD = TRUE;
		break;
	    }
	    case(idmsInfoCvIsInReadonlyUnixdir):
	    case(idmsInfoCvCheckedOutByMe):{
		libId = ddGetObj(cvInfo->libName,NULL,NULL,NULL,NULL,"r");
		UnixMD = TRUE;
		break;
	    }
	    case(idmsInfoCvIsInReadonlyLibrary):
	    case(idmsInfoCvCheckedOutByAnotherUser):{
		/* error - should not be allowed to set attributes */
		       return(idmsError);
	    }
	    case(idmsInfoCvCanBeCheckedOut):{
		/* it is in the golden area */
		libId = ddGetObj(cvInfo->libName,NULL,NULL,NULL,NULL, "r");
		break;
	    }
	    case(idmsInfoCvNotFoundOnRwpaths):
	    case(idmsInfoCvNotFoundInAnyLibrary):
	    case(idmsInfoUnknownEvent):
	    default: {
		/* the idms cannot find the cellview */
		return(idmsBadparameter);
	    }
	}
    }
    else
	libId = ddGetObj(libName, NULL, NULL, NULL,NULL, "r");

#ifdef SLAVEATTR
    if ((libName == NULL) || (strlen(libName) == 0)) {
	if (strlen(lNameforSkill)<1024)
	    strcpy(ListAttrLibName,lNameforSkill);
    }
    else {
	if (strlen(libName)<1024)
	    strcpy(ListAttrLibName,libName);
    }
#endif

    if ((libId == NULL) && (!UnixMD))
	return(idmsBadparameter);

    /* now check the attrType */
    switch (ToLower(attrType[0])) {
	case(iDMS_BOOLEAN): break;
	case(iDMS_STRING):  break;
	case(iDMS_FLOAT):   break;
	case(iDMS_INTEGER): break;
	case(iDMS_TIME):    break;
	case(iDMS_LIST):    break;
	/** setting hier props are not available in dss **/
    default:
	return(idmsBadparameter);
    }

    /* now if the attribute is being set on the lib or 
    ** cell or view just do it 
    */
    if ((cellName == NULL) && (viewName == NULL)) {
	/* attribute is on a library */
	if(!(ddObjPath = ddGetObjReadPath(libId)))
	  return(idmsBadparameter);
	hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
	if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
          strcpy(dssObjVaultPath, _dssResults[0]);
	  if(idmSetAttr(dssObjVaultPath, attrName, attrValue, ToLower(attrType[0]), FALSE))
	    return(idmsSuccess);
	}
	return(idmsFailure);
    }

    /*************************************************************
    REM: 4.4.x does not support the concept of viewId any more.
    Soln.: put it on lib/prop.xx in the form of viewName:propName
    *************************************************************/
    if (cellName == NULL) { 
       /* attr defn on a view object */
       strcpy(vNameforSkill,viewName);
       if ((viewId = ddGetObj(NULL, NULL, NULL, "prop.xx", libId, "r")) == NULL)
	    return(idmsBadparameter);
       if(!(ddObjPath = ddGetObjReadPath(viewId)))
	  return(idmsBadparameter);
       hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
	if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
          strcpy(dssObjVaultPath, _dssResults[0]);
	  sprintf(attrName, "%s:%s", viewName, attrName);
	  if (idmSetAttr(dssObjVaultPath,attrName,attrValue, ToLower(attrType[0]), FALSE))
	    return(idmsSuccess);
	}
	return(idmsFailure);
   }

    if (viewName == NULL) {
	/* attr defn on a cell object */
	strcpy(cNameforSkill,cellName);
	if ((cellId = ddGetObj(NULL, cellName, NULL, NULL, libId, "r")) == NULL)
	    return(idmsBadparameter);
	if(!(ddObjPath = ddGetObjReadPath(cellId)))
	  return(idmsBadparameter);
	hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
	if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
          strcpy(dssObjVaultPath, _dssResults[0]);
	  if (idmSetAttr(dssObjVaultPath,attrName,attrValue, ToLower(attrType[0]), FALSE))
	    return(idmsSuccess);
	}
	return(idmsFailure);
    }
   
    /* the attribute is to go on a cellview version or on a
     unix meta-data object */
    strcpy(cNameforSkill,cellName);
    strcpy(vNameforSkill,viewName);

    if(!(ddObjPath = idb_get_collection_obj(libId, cellName, viewName, 1)))
      return(idmsBadparameter);

    /* For 4.4.x: set the unix-meta data on the ddPath:
    ** .../lib/cell/view/cell.view|.../lib/cell/view.sync.cds].
    ** DO NOT set it on dmsworkdir/cell.view. If intel cellviews are ckdout 
    ** from the 4.4.x lib mgr they will not be in  workdir/cell.view.
    ** Also, a chkin post trigger can be used to put the meta-data 
    ** into the new version while cheking in cadence or intel views.
    ** This trig would execed if the  chkin operation is from the lib mgr or idms.
    **
    ** For cancelco, a pre cancelco trigger can wipe out the meta-data on ddPath. 
    */
    if (UnixMD) {
      if (idmSetAttr(ddObjPath, attrName, attrValue, ToLower(attrType[0]),TRUE))
	return(idmsSuccess);
      else
	return(idmsFailure);
    }

    /* else this is a DMS object  in golden area */
    if (versionName) /* get version */
      strcpy(ver, versionName);
    else
      strcpy(ver, "");

    hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
    if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
      strcpy(dssObjVaultPath, _dssResults[0]);
      sprintf(dssObjVaultPath, "%s%s", dssObjVaultPath, ver);
      if (idmSetAttr(dssObjVaultPath, attrName, attrValue, ToLower(attrType[0]),TRUE))
	return(idmsSuccess);
    }
    return(idmsFailure);
}

int dmsCGetAttribute(char *libName, char *cellName, char *viewName,
		       char *versionName, char *attrName, 
		       char **attrType, char **attrValue)
  /*library name - optional*/
  /*cellName - compulsary*/
  /*viewName - compulsary*/
  /* versionName - optional */
  /*attribute Name - compulsary*/
  /* returned by this procedure */
  /* returned by this procedure */
  { 
      ddId libId;
      ddId cvId;
      ddId cellId;
      ddId viewId;
      ddId intelCVId;
      int status;
      char *ddObjPath, dssObjVaultPath[1024], intelViewName[256], ver[256];
      HRESULT hresult;
      Boolean UnixMD = FALSE;
      DMSINFO *cvInfo;

	  /* check if globalAttrValus has been initialized */
	  if (lengthGlobalAttrValue == 0) {
		  globalAttrValue = (char *) malloc(8191+1);
		  lengthGlobalAttrValue = 8191;
	  }
      /* nullify global array and set the value of the return param 
	  ** Note: We do not set the value of *attrValue = globalAttrValue
	  ** because for long values the pointer will be moved by idmGetAttr
	  */
      globalAttrType[0] = '\0';
      globalAttrValue[0] = '\0';
      *attrType = globalAttrType;
      *attrValue = globalAttrValue; /* set it anyway [JP] */

      lNameforSkill[0] = (cNameforSkill[0] = (vNameforSkill[0] = '\0'));
      /* if not init the return error */
      if (idmsNotInitialized)
	  return(idmsNotInit);

      /* if PI is disabled then simply return success */
      if (globalAttrDisableFlag)
	  return(idmsSuccess);

      /* check every argument to see if they are "" */
      if (!libName || strlen(libName) == 0)
	  libName = NULL;
      else 
	  strcpy(lNameforSkill,libName);

      if (!cellName || strlen(cellName) == 0)
	  cellName = NULL;

      if (!viewName || strlen(viewName) == 0)
	  viewName = NULL;

      if (!versionName || strlen(versionName) == 0)
	  versionName = NULL;

      libId = cellId = viewId = cvId = intelCVId = NULL;
      
      /* check for compulsary parameters */
      if (((libName == NULL) && (cellName == NULL) && (viewName == NULL)) ||
	  (attrName == NULL) 
	  )
	  return(idmsBadparameter);

      if ((libName == NULL) || (strlen(libName) == 0)) {
	  /* it is possible that the cell and view is 
	  ** not present in the cadence DMS at all - in such a
	  ** case we will set a switch to go the unix meta-data
	  ** approach
	  */
	  status = dmsCInfoCellView(0, cellName,viewName,&cvInfo);
	  strcpy(lNameforSkill,cvInfo->libName);
	  switch (status) {
	      case(idmsInfoCvIsInWorkdir):
	      case(idmsInfoCvIsInReadonlyUnixdir):{
		  libId = ddGetObj(cvInfo->libName,NULL,NULL,NULL,NULL,"r");
		  UnixMD = TRUE;
		  break;
	      }
	      case(idmsInfoCvCheckedOutByMe):
	      case(idmsInfoCvCheckedOutByAnotherUser):{
		  libId = ddGetObj(cvInfo->libName,NULL,NULL,NULL,NULL,"r");
		  UnixMD = TRUE;
		  break;
	      }
	      case(idmsInfoCvIsInReadonlyLibrary):
	      case(idmsInfoCvCanBeCheckedOut):{
		  /* it is in the golden area */
		  libId = ddGetObj(cvInfo->libName,NULL,NULL,NULL,NULL,"r");
		  break;
	      }
	      case(idmsInfoCvNotFoundOnRwpaths):
	      case(idmsInfoCvNotFoundInAnyLibrary):
	      case(idmsInfoUnknownEvent):
	      default: {
		  /* the idms cannot find the cellview */
		  return(idmsBadparameter);
	      }
	  }
      }
      else
	  libId = ddGetObj(libName, NULL, NULL, NULL, NULL, "r");

  #ifdef SLAVEATTR
      if ((libName == NULL) || (strlen(libName) == 0)) {
	  if (strlen(lNameforSkill)<1024)
	      strcpy(ListAttrLibName,lNameforSkill);
      }
      else {
	  if (strlen(libName)<1024)
	      strcpy(ListAttrLibName,libName);
      }
  #endif

      if ((libId == NULL) && (!UnixMD))
	  return(idmsBadparameter);
    
      /* now if the attribute is being set on the lib or 
      ** cell or view just get it 
      */
      if ((cellName == NULL) && (viewName == NULL)) {
	  /* attribute is on a library */
	  if(!(ddObjPath = ddGetObjReadPath(libId)))
	    return(idmsBadparameter);
	  hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
	  if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
	    strcpy(dssObjVaultPath, _dssResults[0]);
	    if (idmGetAttr(dssObjVaultPath,attrName,&globalAttrValue,globalAttrType,
			   &lengthGlobalAttrValue)) {
		    *attrValue = globalAttrValue;	
		return(idmsSuccess);
	    }
	  }
	  return(idmsFailure);
      }

      /*************************************************************
      REM: 4.4.x does not support the concept of viewId any more.
      Soln.: get it from lib/prop.xx in the form of viewName:propName
      *************************************************************/
      if (cellName == NULL) { 
	 /* attr on a view object */
	  strcpy(vNameforSkill, viewName);
	 if ((viewId = ddGetObj(NULL, NULL, NULL, "prop.xx", libId, "r")) == NULL)
	    return(idmsBadparameter);
	  if(!(ddObjPath = ddGetObjReadPath(viewId)))
	    return(idmsBadparameter);
	  hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
	  if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
	    strcpy(dssObjVaultPath, _dssResults[0]);
	    sprintf(attrName, "%s:%s", viewName, attrName);
	    if (idmGetAttr(dssObjVaultPath,attrName,&globalAttrValue,globalAttrType,
			 &lengthGlobalAttrValue)) {
		  *attrValue = globalAttrValue;
	      return(idmsSuccess);
	    }
	  }
	  return(idmsFailure);
      }

      if (viewName == NULL) {
	  /* attr on a cell object */
	  strcpy(cNameforSkill,cellName);
	  if ((cellId = ddGetObj(NULL, cellName, NULL, NULL, libId, "r")) == NULL)
	      return(idmsBadparameter);
	  if(!(ddObjPath = ddGetObjReadPath(cellId)))
	    return(idmsBadparameter);
	  hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
	  if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
	    strcpy(dssObjVaultPath, _dssResults[0]);
	    if (idmGetAttr(dssObjVaultPath,attrName,&globalAttrValue,globalAttrType,
			   &lengthGlobalAttrValue)) {
		    *attrValue = globalAttrValue;
		return(idmsSuccess);
	    }
	  }
	  return(idmsFailure);
      }

      /* the attribute is to come from a cellview version or a
       unix meta-data object */
      strcpy(cNameforSkill,cellName);
      strcpy(vNameforSkill,viewName);

      if(!(ddObjPath = idb_get_collection_obj(libId, cellName, viewName, 1)))
	return(idmsBadparameter);

      if (UnixMD) {
        if (idmGetAttr(ddObjPath,attrName,&globalAttrValue,
				globalAttrType, &lengthGlobalAttrValue)) {
		  *attrValue = globalAttrValue;
		  return(idmsSuccess);
	}
	else
          return(idmsFailure);
      }

      /* the attribute is to come from a cellview version */
      if (versionName) /* get version */
	strcpy(ver, versionName);
      else	
        strcpy(ver, "");

      hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
      if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
	strcpy(dssObjVaultPath, _dssResults[0]);
        sprintf(dssObjVaultPath, "%s%s", dssObjVaultPath, ver);
        if (idmGetAttr(dssObjVaultPath,attrName,&globalAttrValue,globalAttrType,
			 &lengthGlobalAttrValue)) {
		  *attrValue = globalAttrValue;
		  return(idmsSuccess);
	}
      }
      return(idmsFailure);
  }


  /** Since there is no sapiUrlDeleteProp, the propValue is being set to "" 
   **								-sh 3/7/00
   **/
  int dmsCDeleteAttribute(char *libName, char *cellName, char *viewName,
		      char *versionName, char *attrName)
  /*library name - optional*/
  /*cellName - compulsary*/
  /*viewName - compulsary*/
  /*versionName - optional */
  /*attribute Name - compulsary*/
  { 
      ddId libId;
      ddId cvId;
      ddId cellId;
      ddId viewId;
      ddId intelCVId;
      int status;
      char Path[PATHLENGTH];
      char *ddObjPath, dssObjVaultPath[1024], intelViewName[256], ver[256];
      HRESULT hresult;
      Boolean UnixMD = FALSE;
      DMSINFO *cvInfo;

      lNameforSkill[0] = (cNameforSkill[0] = (vNameforSkill[0] = '\0'));
      /* if not init the return error */
      if (idmsNotInitialized)
	  return(idmsNotInit);
      
      /* if PI is disabled then simply return success */
      if (globalAttrDisableFlag)
	  return(idmsSuccess);

      /* check every argument to see if they are "" */
      if (strlen(libName) == 0)
	  libName = NULL;
      else 
	  strcpy(lNameforSkill, libName);

      if (strlen(cellName) == 0)
	  cellName = NULL;

      if (strlen(viewName) == 0)
	  viewName = NULL;

      if (strlen(versionName) == 0)
	  versionName = NULL;

      libId = cellId = viewId = cvId = intelCVId = NULL;

      /* check for compulsary parameters */
      if (((libName == NULL) && (cellName == NULL) && (viewName == NULL)) ||
	  (attrName == NULL) 
	  )
	  return(idmsBadparameter);

      if ((libName == NULL) || (strlen(libName) == 0)) {
	  /* it is possible that the cell and view is 
	  ** not present in the cadence DMS at all - in such a
	  ** case we will set a switch to go the unix meta-data
	  ** approach
	  */
	  status = dmsCInfoCellView(0, cellName,viewName,&cvInfo);
	  strcpy(lNameforSkill,cvInfo->libName);
	  switch (status) {
	      case(idmsInfoCvIsInReadonlyUnixdir):
	      case(idmsInfoCvIsInWorkdir):{
		  libId = ddGetObj(cvInfo->libName, NULL, NULL, NULL, NULL, "w");
		  UnixMD = TRUE;
		  break;
	      }
	      case(idmsInfoCvCheckedOutByMe):{
		  libId = ddGetObj(cvInfo->libName, NULL, NULL, NULL, NULL, "w");
		  UnixMD = TRUE;
	      }
	      case(idmsInfoCvIsInReadonlyLibrary):
	      case(idmsInfoCvCheckedOutByAnotherUser):{
		  /* error - should not be allowed to delete attributes */
			 return(idmsError);
	      }
	      case(idmsInfoCvCanBeCheckedOut):{
		  /* it is in the golden area */
		  libId = ddGetObj(cvInfo->libName, NULL, NULL, NULL, NULL, "w");
		  break;
	      }
	      case(idmsInfoCvNotFoundOnRwpaths):
	      case(idmsInfoCvNotFoundInAnyLibrary):
	      case(idmsInfoUnknownEvent):
	      default: {
		  /* the idms cannot find the cellview */
		  return(idmsBadparameter);
	      }
	  }
      }
      else
	  libId = ddGetObj(libName, NULL, NULL, NULL, NULL, "w");

  #ifdef SLAVEATTR
      if ((libName == NULL) || (strlen(libName) == 0)) {
	  if (strlen(lNameforSkill)<1024)
	      strcpy(ListAttrLibName,lNameforSkill);
      }
      else {
	  if (strlen(libName)<1024)
	      strcpy(ListAttrLibName,libName);
      }
  #endif

      if ((libId == NULL) && (!UnixMD))
	  return(idmsBadparameter);
      /* now if the attribute is being set on the lib or 
      ** cell or view just delete it 
      */
      if ((cellName == NULL) && (viewName == NULL)) {
	  /* attribute is on a library */
	  if(!(ddObjPath = ddGetObjReadPath(libId)))
	    return(idmsBadparameter);
	  hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
	  if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
	    strcpy(dssObjVaultPath, _dssResults[0]);
	    if (idmDeleteAttr(dssObjVaultPath, attrName))
	      return(idmsSuccess);
	  }
	  return(idmsFailure);
      }
      if (cellName == NULL) { 
	 /* attr defn on a view object */
          strcpy(vNameforSkill,viewName);
	  if ((viewId = ddGetObj(NULL, NULL, NULL, "prop.xx", libId, "r")) == NULL)
	    return(idmsBadparameter);
	  if(!(ddObjPath = ddGetObjReadPath(viewId)))
	    return(idmsBadparameter);
	  hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
	  if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
	    strcpy(dssObjVaultPath, _dssResults[0]);
	    sprintf(attrName, "%s:%s", viewName, attrName);
	    if (idmDeleteAttr(dssObjVaultPath, attrName))
	      return(idmsSuccess);
	  }
	  return(idmsFailure);
     }
      if (viewName == NULL) {
	  /* attr defn on a cell object */
	  strcpy(cNameforSkill, cellName);
	  if((cellId = ddGetObj(NULL, cellName, NULL, NULL, libId, "r")) == NULL)
	   return(idmsBadparameter);
	  if(!(ddObjPath = ddGetObjReadPath(cellId)))
	    return(idmsBadparameter);
	  hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
	  if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
	    strcpy(dssObjVaultPath, _dssResults[0]);
	    if (idmDeleteAttr(dssObjVaultPath, attrName))
	      return(idmsSuccess);
	  }
	  return(idmsFailure);
      }
      /* the attribute is to be deleted from a cellview version or a
       unix meta-data object */
      strcpy(vNameforSkill, viewName);
      strcpy(cNameforSkill, cellName);

      if ((cvId = ddGetObj(NULL, cellName, viewName, NULL, libId, "r")) == NULL)
	return(idmsBadparameter);

      sprintf(intelViewName, "%s.%s", cellName, viewName);
      if(intelCVId = ddGetObj(NULL, cellName, viewName, intelViewName, libId, "r"))
	cvId = intelCVId;

      if(!(ddObjPath = ddGetObjReadPath(cvId)))
	return(idmsBadparameter);

      if (UnixMD) {
	if (idmDeleteAttr(dssObjVaultPath, attrName)) 
	  return(idmsSuccess);
	else
          return(idmsFailure);
      }
      /* the attribute is on  a cellview version */
      if (versionName){
	strcpy(ver, versionName);
      } else	{	// get latest 
       if(cvInfo==NULL){
         dmsCInfoCellView(0, cellName,viewName,&cvInfo);
        }
        strcpy(ver, cvInfo->verName);
      }
      /** REM: resolve cadence cv versions **/
      hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
      if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
	  strcpy(dssObjVaultPath, _dssResults[0]);
          sprintf(dssObjVaultPath, "%s%s", dssObjVaultPath, ver);
	  if (idmDeleteAttr(dssObjVaultPath, attrName)) 
	  return(idmsSuccess);
      }
      return(idmsFailure);
  }

  /*
  ** The following  procedure is slightly complicated,
  ** it unpackage the list and each list can have only 
  ** two possible values - either a single element (for
  ** optional lists this could be NULL), or n elements
  ** where n is the same for all lists of n
  ** This will give us 128 possible combinations - so
  ** we first find the vector passed and then depending
  ** on make a call to the above procedure either once or
  ** n times
  */
  int dmsCSetAttributeList(char *libNameList, char *cellNameList, 
			   char *viewNameList, char *versionNameList,
			   char *attrNameList, char *attrTypeList,
			   char *attrValueList)
  /*library name list - optional*/
  /*cellName list - compulsary*/
  /*viewName list - compulsary*/
  /* versionName list - optional */
  /*attribute Name list- compulsary*/
  /*attribute Type list - compulsary */
  /*attribute Value list - optional */
  {
      int numLibs,numCells,numViews,numVersions,numAttrs;
      int numTypes,numVals;
      int i;

      char *libs[iDMS_MaxElementsInList], *L;
      char *cells[iDMS_MaxElementsInList], *C;
      char *views[iDMS_MaxElementsInList], *V;
      char *versions[iDMS_MaxElementsInList], *Ve;
      char *aNames[iDMS_MaxElementsInList], *aN;
      char *aTypes[iDMS_MaxElementsInList], *aT;
      char *aVals[iDMS_MaxElementsInList], *aV;

      int n = 0;
      int retVal = idmsSuccess;

      /* if not init the return error */
      if (idmsNotInitialized)
	  return(idmsNotInit);
      /* if PI is disabled then simply return success */
      if (globalAttrDisableFlag)
	  return(idmsSuccess);

      /* parse all the arguments and fill in the arrays */
      numLibs = idmFillArgs(libNameList,libs,1);
      numCells = idmFillArgs(cellNameList,cells,0);
      numViews = idmFillArgs(viewNameList,views,0);
      numVersions = idmFillArgs(versionNameList,versions,1);
      numAttrs = idmFillArgs(attrNameList,aNames,0);
      numTypes = idmFillArgs(attrTypeList,aTypes,0);
      numVals = idmFillArgs(attrValueList,aVals,1);

      /* now check to see that each arg is either 1 or n, where
      ** n is decided by the first non 1 arg 
      */
       
      if (numLibs != 1) n = numLibs;
      else if (numCells != 1) n = numCells;
      else if (numViews != 1) n = numViews;
      else if (numVersions != 1) n = numVersions;
      else if (numAttrs != 1) n = numAttrs;
      else if (numTypes != 1) n = numTypes;
      else if  (numVals != 1) n = numVals;

       /* if n = 0 => this is a single element call */
      if (n == 0) {
		  dmsVoFreePtrs(7,&libs[0],&cells[0],&views[0],&versions[0],
			  &aNames[0], &aTypes[0], &aVals[0]);
		  return(dmsCSetAttribute(libNameList,cellNameList,
				  viewNameList,versionNameList,
				  attrNameList,attrTypeList,
				  attrValueList));
	  }
      /* check and make sure that every list has either n
      ** or 1 element
      */
      if (
		  ((numLibs != 1) && (numLibs != n)) ||
		  ((numCells != 1) && (numCells != n)) ||
		  ((numViews != 1) && (numViews != n)) ||
		  ((numVersions != 1) && (numVersions != n)) ||
		  ((numAttrs != 1) && (numAttrs != n)) ||
		  ((numTypes != 1) && (numTypes != n)) ||
		  ((numVals != 1) && (numVals != n))
	 )
		  retVal = idmsBadparameter;
	  else {
	  /* now simply call the above simple set n times -
	  ** this is not the best/fastest - but it is the simplest !
	  */
      
	  for (i=1;i<=n;++i){
			  L = ((numLibs == 1) ? libs[0] : libs[i-1]);
			  C = ((numCells == 1) ? cells[0] : cells[i-1]);
			  V = ((numViews == 1) ? views[0] : views[i-1]);
			  Ve = ((numVersions == 1) ? versions[0] : versions[i-1]);
			  aN = ((numAttrs == 1) ? aNames[0] : aNames[i-1]);
			  aT = ((numTypes == 1) ? aTypes[0] : aTypes[i-1]);
			  aV = ((numVals == 1) ? aVals[0] : aVals[i-1]);

			  if (dmsCSetAttribute(L,C,V,Ve,aN,aT,aV) != idmsSuccess)
			  retVal = idmsFailure;
	  }
	  }
	  /* free all memory being used */
	  for(i=0;i<numLibs;free(libs[i++]));
	  for(i=0;i<numCells;free(cells[i++]));
	  for(i=0;i<numViews;free(views[i++]));
	  for(i=0;i<numVersions;free(versions[i++]));
	  for(i=0;i<numAttrs;free(aNames[i++]));
	  for(i=0;i<numTypes;free(aTypes[i++]));
	  for(i=0;i<numVals;free(aVals[i++]));

      return(retVal);
  }

/*
** This procedure returns all the attributes assocaited with
** a single object - here the attrName, attrType and attrVal
** are equal sized arrays using the iDMS_ListDelimiter as a
** delimiter
*/

int dmsCGetAllAttributes(char *libName, char *cellName, char *viewName,
                         char *versionName, char **attrNameList,
                         char ** attrTypeList, char **attrValueList)
/*library name - optional*/
/*cellName - compulsary*/
/*viewName - compulsary*/
/*versionName - optional */
/*returned by this procedure*/
/* returned by this procedure */
/* returned by this procedure */
{ 
    ddId libId;
    ddId versionId;
    ddId cellId;
    ddId viewId;
    int status;
    char *ddObjPath, dssObjVaultPath[1024], tmpViewName[256];
    HRESULT hresult;
    Boolean UnixMD = FALSE;
    DMSINFO *cvInfo;

	/* check if globalAttrValus has been initialized */
	if (lengthGlobalAttrValue == 0) {
		globalAttrValue = (char *) malloc(8191+1);
		lengthGlobalAttrValue = 8191;
	}
    /* nullify global array and set the value of the return param */
    globalAttrType[0] = '\0';
    globalAttrValue[0] = '\0';
    globalAttrName[0] = '\0';
    *attrNameList = globalAttrName;
    *attrTypeList = globalAttrType;
    *attrValueList = globalAttrValue;

    /* if not init the return error */
    if (idmsNotInitialized)
	return(idmsNotInit);

    /* if PI is disabled then simply return success */
    if (globalAttrDisableFlag) 
	return(idmsSuccess);

	/* check every argument to see if they are "" */
	if (strlen(libName) == 0)
		libName = NULL;

	if (strlen(cellName) == 0)
		cellName = NULL;

	if (strlen(viewName) == 0)
		viewName = NULL;

	if (strlen(versionName) == 0)
		versionName = NULL;

    libId = versionId = cellId = viewId = NULL;

    /* check for compulsary parameters */
    if (((libName == NULL) && (cellName == NULL) && (viewName == NULL)))
	return(idmsBadparameter);

    if ((libName == NULL) || (strlen(libName) == 0)) {
	/* it is possible that the cell and view is 
	** not present in the cadence DMS at all - in such a
	** case we will set a switch to go the unix meta-data
	** approach
	*/
	status = dmsCInfoCellView(0, cellName,viewName,&cvInfo);
	switch (status) {
	    case(idmsInfoCvIsInWorkdir):
	    case(idmsInfoCvIsInReadonlyUnixdir):{
		libId = ddGetObj(libName,NULL,NULL,NULL,NULL,"r");
		UnixMD = TRUE;
		break;
	    }
	    case(idmsInfoCvCheckedOutByMe):
	    case(idmsInfoCvCheckedOutByAnotherUser):{
		/* check to see if it is in unix/co@ area */
		libId = ddGetObj(libName,NULL,NULL,NULL,NULL,"r");
		UnixMD = TRUE;
		break;
	    }
	    case(idmsInfoCvIsInReadonlyLibrary):
	    case(idmsInfoCvCanBeCheckedOut):{
		/* it is in the golden area */
		libId = ddGetObj(libName,NULL,NULL,NULL,NULL,"r");
		break;
	    }
	    case(idmsInfoCvNotFoundOnRwpaths):
	    case(idmsInfoCvNotFoundInAnyLibrary):
	    case(idmsInfoUnknownEvent):
	    default: {
		/* the idms cannot find the cellview */
		return(idmsBadparameter);
	    }
	}
    }
    else
	libId = ddGetObj(libName,NULL,NULL,NULL,NULL,"r");

    if ((libId == NULL) && (!UnixMD))
	return(idmsBadparameter);
  
    /* now if the attribute is being set on the lib or 
    ** cell or view just get it 
    */
    if ((cellName == NULL) && (viewName == NULL)) {
	/* attribute is on a library */
	if(!(ddObjPath = ddGetObjReadPath(libId)))
	  return(idmsBadparameter);
	hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
	if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
	  strcpy(dssObjVaultPath, _dssResults[0]);
	  if (idmGetAllAttr(dssObjVaultPath,globalAttrName,globalAttrValue,globalAttrType))
	    return(idmsSuccess);
	}
	return(idmsFailure);
    }

    if (cellName == NULL) { 
       /* attr on a view object */
	sprintf(tmpViewName, ".%s", viewName);
	strcpy(vNameforSkill, tmpViewName);
	if ((viewId = ddGetObj(NULL, NULL, NULL, tmpViewName, libId, "r")) == NULL)
	  return(idmsBadparameter);
	if(!(ddObjPath = ddGetObjReadPath(viewId)))
	  return(idmsBadparameter);
	hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
	if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
	  strcpy(dssObjVaultPath, _dssResults[0]);
	  if (idmGetAllAttr(dssObjVaultPath,globalAttrName,globalAttrValue,globalAttrType))
	    return(idmsSuccess);
	}
	return(idmsFailure);
   }
    if (viewName == NULL) {
	/* attr on a cell object */
	if ((cellId = ddGetObj(NULL,cellName, NULL, NULL, libId, "r")) == NULL)
	    return(idmsBadparameter);
	if(!(ddObjPath = ddGetObjReadPath(cellId)))
	  return(idmsBadparameter);
	hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
	if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
	  strcpy(dssObjVaultPath, _dssResults[0]);
	  if (idmGetAllAttr(dssObjVaultPath,globalAttrName,globalAttrValue,globalAttrType))
	    return(idmsSuccess);
	}
	return(idmsFailure);
    }

    /* the attribute is to come from a cellview version or a
     unix meta-data object */
    
    /** REM: not treating UNIXMD any differently  **/
    /* the attribute is to come from a cellview version */
    /** REM: do versions later **/
    if (!versionName || (strlen(versionName) <= 1))
	versionName = NULL;
    if ((versionId = ddGetObj(NULL, cellName, viewName, NULL, libId, "r")) == NULL)
	return(idmsBadparameter);
    if(!(ddObjPath = ddGetObjReadPath(versionId)))
	return(idmsBadparameter);
    hresult = sapiUrlVault(NULL, ddObjPath, NULL, &_dssResults);
    if(SUCCEEDED(hresult) && strcmp(_dssResults[0], "")) {
	strcpy(dssObjVaultPath, _dssResults[0]);
        if (idmGetAllAttr(dssObjVaultPath,globalAttrName,globalAttrValue, globalAttrType))
		return(idmsSuccess);
    }
    return(idmsFailure);
}

/******************
* Put in for netlister test
******************/
/*********************************************************************
Is not being called from any iDMS modules, so commented out. -sh 3/7/00

int dmsCSetAllAttributes(char *libName, char *cellName, char *viewName, 
                         char *versionName, char *attrNameList,
                         char *attrTypeList, char *attrValueList, int arraySize)

*********************************************************************/
