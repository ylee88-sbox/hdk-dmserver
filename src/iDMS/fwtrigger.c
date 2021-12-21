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
# $Id: fwtrigger.c,v 1.1.6.1.10.8 1997/12/15 20:19:39 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
/*
#Revision 1.1  93/01/04  11:12:01  dhejmad
#Initial revision
*/


/* 
  Source Code for the Opus triggers
*/
#include <string.h>
#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
#include <itkDB.h>
#endif
#include "attr.h"
#include "fwimsg.h"

#include <dmsC_triggers.h>

extern int disable_obsolete_messages;

/*---------------------*
 * Function prototypes *
 *---------------------*/

#include <dmsC_reports.h>
#include <dmsC_internal_functions.h>

/***
extern dmVersionId idmsGlobalCheckinVersionShadowId;
extern dmLibId idmsGlobalCheckinShadowLibId;
***/
extern Boolean globalAttrDisableFlag;
extern Boolean globalUnixAttrToFile;
Boolean globalUserTriggerDisableFlag;	/* REM: check fwusertrigger.c */
extern char globalMappingKeyword[];
extern int debugDmsOn;

static char buffer[2048];

/* Triggers we are using at present */

/*
** This procedure makes sure the import and export list 
** for the user will be correctly set - so that 
** this user will not get anybody else's private copy. Or 
** allow anyone to read its own checked out version.
*/


/* 
** The purpose of this checkin trigger is to cause 
** the hierarchical attribute shadowing the checked out 
** object to be written to the newly checked in version
*/

/******************************************************************
Boolean 
idmCheckinTrigger(libId, cellId, viewId, oldVersionId, newVersionId)
dmLibId libId;
dmCellId cellId;
dmViewId viewId;
dmVersionId oldVersionId, newVersionId;
{
    dbPropId propId;
    dbPropType pType;
    dbPropValue pValue;
    dmRec *record;
    char cellName[1025];
    dmGenStatePtr state;
    dmCellViewId cvId;

    * if PI is disabled then simply return success *
    if (globalAttrDisableFlag)
	return(TRUE);

    * check the global variable if it is not set 
    ** then this checkin had nothing to do with
    ** unix files - check if it is a cadence dm
    ** object.
    *
    if (idmsGlobalCheckinVersionShadowId == NULL) {
	if ((propId = dbGetPropByName(oldVersionId, \
			iDMS_SHADOW_PROPNAME, &pType, &pValue)) != NULL) {
	    if (dbCopyProp(propId, newVersionId)) 
		* delete the hierarchical prop on the old version *
		return(dbDeleteProp(oldVersionId, propId));
	    return(FALSE);
 	}
	return(TRUE);
    }
    * copy all the properties from the shadow onto the newVersion *
    if (idmCopyAllProps(idmsGlobalCheckinVersionShadowId,newVersionId,FALSE)){
	* delete the shadow version *
	strcpy(cellName,dmGetCellName(cellId));
	(void) dmDeleteCellView(idmsGlobalCheckinShadowLibId, \
		    cellName,dmGetViewName(viewId));
	* if all the cellviews are deleted then delete the cell itself *
	state = dmStartGenCellCellView(libId, cellId);
	if (!dmGenCellCellView(state, &cvId)) {
	    (void) dmDeleteCell(libId,cellName);
	}
	dmStopGen(state);
	return(TRUE);
    }
    return(FALSE);
}
****************************************************************************/
/* 
** This procedure gets the specified version from the 
** skill interface - this call does not 
** work in C for version 0.0 which the checkout trigger
** uses
*/
/***************************************************************************
dmCellViewId 
idmGetCellViewFromSkill(libName,cellName,viewName,verName)
char *libName,*cellName,*viewName,*verName;
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
      if (strcmp(vid,verName)==0) {
         dmStopGen(pState);
         return(verId);
      }
   }
   dmStopGen(pState);
   return(NULL);
}
****************************************************************************/

/* 
** The purpose of the checkout trigger is to create the hierachical
** shadow property in case the object does have any properties on it 
*/
/***************************************************************************
Boolean 
idmCheckoutTrigger(libId, cellId, viewId, versionId)
dmLibId libId;
dmCellId cellId;
dmViewId viewId;
dmVersionId versionId;
{
    dmCellViewId cellviewId;
    dbPropId propId, newVersionPropId;
    dmLibId lId;
    int status;
    dmVersionId vId;
    dbGenStateId state;
    dmLibId lib;
    DMSINFO *cvInfo;

    * if PI is disabled then simply return success *
    if (globalAttrDisableFlag)
	return(TRUE);

    if (dbObjHasProp(versionId)) {
	* we have two cases - one in which the object has been 
	** checked to a unix dir (user's private area) and
	** the second where it has been checked to a co@user
	** dir - in the second case we follow a scheme of
	** shadowing within the golden area - in the first scheme we
	** need to set all the attributes on the newly created
	** object in a shadow area in the local directory
	*
	status = dmsCInfoCellView(0, dmGetCellName(cellId),\
				  dmGetViewName(viewId),&cvInfo);
	* we know that the checkout has been successfull - so simple 
	** find out where the file has gone to *
	if (status == idmsInfoCvIsInWorkdir) {
	    * the file is in some unix area and we 
	    ** do need to add the lib to the path and
	    ** get the cellviewId for the shadow
	    ** cellview which has the attributes
	    *
	    if ((vId = idmGetUnixShadowVersionId( \
	             dmGetCellName(cellId),dmGetViewName(viewId), \
		      cvInfo->fullPathName,CREATE_IT,&lib)) == NULL)
		return(FALSE);
	    * now to copy all the properties from 
	    ** the old versionid to the shadow version
	    ** checkin to see that they are supposed to
	    ** be inherited.
	    *
	    return(idmCopyAllProps(versionId,vId,TRUE));
	}
	* Here we copy to a co@user directory 
	** here we cannot simply create a hierachical property
	** and use dbCopyProp - infinte loop. So we can use a
	** step approach using the cellview as a place holder
	** temporarily
	*
	else if ((cellviewId = idmGetCellViewFromSkill( \
			 dmGetLibName(libId),  \
			 dmGetCellName(cellId), \
			 dmGetViewName(viewId), \
			 "0.0")) != NULL) {
	    propId = dbCreateHierProp(cellviewId,iDMS_SHADOW_PROPNAME);
	    (void) dbCopyProp(versionId, propId);
	    newVersionPropId = dbCreateHierProp( \
			   versionId,iDMS_SHADOW_PROPNAME);
	    (void) idmCopyAllProps(propId, newVersionPropId,TRUE);
	    (void) dbDeleteProp(cellviewId, propId);
	    return(TRUE);
	}
   	return(FALSE);
    }
    return(TRUE);
}
****************************************************************************/


/* The cancel checkout trigger - this trigger is simply used
** to delete the shadow version if this is unix metadata, else it 
** will delete the shadow version property on the older version in
** case of the co@user directory - behaviour is almost identical to 
** the checkin trigger
*/
/****************************************************************************
Boolean 
idmUncheckoutTrigger(libId, cellId, viewId, versionId)
dmLibId libId;
dmCellId cellId;
dmViewId viewId;
dmVersionId versionId;
{
    dmRec *record;
    char cellName[1025];
    dmGenStatePtr state;
    dmCellViewId cvId;

    * if PI is disabled then simply return success *
    if (globalAttrDisableFlag)
	return(TRUE);
    
    * quick check - if this object has no properties
    ** do nothing
    *
    if (!dbObjHasProp(versionId)) 
	return(TRUE);

    * check the global variable if it is not set 
    ** then this checkin had nothing to do with
    ** unix files - check if it is a cadence dm
    ** object.
    *
    if (idmsGlobalCheckinVersionShadowId == NULL) {
	(void) dbDeletePropByName(versionId,iDMS_SHADOW_PROPNAME);
	return(TRUE);
    }
    * delete the shadow cellview version *
    strcpy(cellName,dmGetCellName(cellId));
    (void) dmDeleteCellView(idmsGlobalCheckinShadowLibId, \
			    cellName,dmGetViewName(viewId));
    * if all the cellviews are deleted then delete the cell itself *
    state = dmStartGenCellCellView(libId, cellId);
    (void) dmGenCellCellView(state, &cvId);
    if (cvId == NULL) {
	(void) dmDeleteCell(libId,cellName);
    }
    dmStopGen(state);
    return(TRUE);
}


Boolean idb_open_lib_trigger(dmLibId dmsLibID);
****************************************************************************/

/*-----------------------------------------------------------------------
 * idmRegisterTriggers
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
 * Description:  reads the global idms file. If error, warning and disables
 *               the attribute generation.
 *               If all ok registers the basic triggers.
 *-----------------------------------------------------------------------*/

Boolean idmRegisterTriggers(int nullgt)
{
    char *mappingKeyword;
    char temp[1024];
    globalAttrDisableFlag = FALSE;
    globalUserTriggerDisableFlag = FALSE;
    globalUnixAttrToFile = FALSE;
    
    /* find out what the keyword for distinguishing 
    ** unix vs non unix metadata is - the default 
    ** mapping is the cadence provided "co@"
    */
    if ((mappingKeyword = dmsAppGetEnv(iDMSMappingKeywordEnvVar)) != NULL) 
	strcpy(globalMappingKeyword,mappingKeyword);
    else
	strcpy(globalMappingKeyword,"co@");

    if (idmReadGlobalFile() != idmsSuccess) {
	voWarn("Could not access or read the iDMS global tables file\n");
	voInfo("Using Default mappings: Attribute generation Disabled\n");
	/* disable attribute PI */
	globalAttrDisableFlag = TRUE;
	/* disable the user triggers */
	globalUserTriggerDisableFlag = TRUE;
    }

    if (debugDmsOn){
        dmsPrintIdmsLog("<<*C-CALL*>> globalUserTriggerDisableFlag set to %d\n", globalUserTriggerDisableFlag);
    }

    /******* REM: later
    if (!dmRegReadinTrigger(idb_open_lib_trigger,IDMS_TRIGGER_PRIORITY,"openlib"))
	voWarn("Could Not Register Open Lib Trigger\n");
	
    if (!dmRegCheckinTrigger(idmCheckinTrigger,IDMS_TRIGGER_PRIORITY,"checkin"))
	voWarn("Could Not Register Checkin Trigger\n");

    if (!dmRegCheckoutTrigger(idmCheckoutTrigger,IDMS_TRIGGER_PRIORITY,"checkout"))
	voWarn("Could Not Register Checkout Trigger\n");
    
    if (!dmRegUncheckoutTrigger(idmUncheckoutTrigger,IDMS_TRIGGER_PRIORITY,"CancelCheckout"))
	voWarn("Could Not Register Cancel Checkout Trigger\n");
    *******/

    /* Added by Sorin to separate the Init of user triggers from general triggs
       and also to disable voWarns */

    if (!disable_obsolete_messages)
       if (!idmInitUserTriggers(nullgt))
	   voWarn("User Triggers Mechanism is disabled\n");

    /* check for the environment variable in case the users want the 
       attribute mechanism to be disabled
    */
    if ((!globalAttrDisableFlag) && (dmsAppGetEnv("DMS_DISABLE_ATTRIBUTES"))) {
	globalAttrDisableFlag = TRUE;
	voInfo("Attribute Mechanism is disabled\n");
    }

	/* check if unix attributes are to be written to a unix file */
	/************************************************************
	REM: later
	if (dmsAppGetEnv("IDMS_WRITE_UNIX_ATTR_TO_FILE")) {
		globalUnixAttrToFile=TRUE;
		voInfo("Unix Attributes will be written to Files\n");
		* load the skill file which does this stuff *
		sprintf(temp,"%s/fwuxattr.il", dmsAppGetEnv("IDMS"));
		if (!voFileIsReadable(temp)) {
			voWarn("File fwuxattr.il not found, attr will be written to Unix Metadata\n");
			globalUnixAttrToFile=FALSE;
		}	
		else {
			sprintf(buffer,"load(\"%s\")",temp);
			ilErrSetString2(buffer,1);
		}	
	}
	************************************************************/
    voFlushWarn();
    return(TRUE);
}

