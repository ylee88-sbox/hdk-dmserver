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
# $Id: damCheck.h,v 1.1 1997/04/07 17:15:46 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
extern  String          dacHierPropValueName;

#define damIsDaId( A )       ( A & 1 )

#define damCheckVMode(id)\
      {if(!idaiSetViewLevelObjectAppendMode((dbId)id)){\
         voWarn(dacErrMsgFmt, vovFuncName, "couldn\'t open owner to append", id);\
         RETURN(0);}}

#define damCheckName(name)\
      {if(!name || 0 == strlen(name)){\
         voWarn(dacGlobalNoLuckFmt, vovFuncName, "NULL or empty name argument" );\
         RETURN(0);}}

#define damCheckVModeNRV(id)\
      {if(!idaiSetViewLevelObjectAppendMode((dbId) id)){\
         voWarn(dacErrMsgFmt, vovFuncName, "couldn\'t open owner to append", id);\
         RETURN_;}}

#define damiCheckVModeNRV(id)\
      {if(!idaiSetViewLevelObjectAppendMode((dbId) id)){\
         voWarn(dacErrMsgFmt, vovFuncName, "couldn\'t open owner to append", id);\
         return;}}

#define damCheckLMode(id)\
      {if(!idaiSetLibLevelObjectAppendMode((dbId)id)){\
         voWarn(dacErrMsgFmt, vovFuncName, "couldn\'t open owning lib to append", id);\
         RETURN(0);}}

#define damCheckLModeNRV(id)\
      {if(!idaiSetLibLevelObjectAppendMode((dbId) id)){\
         voWarn(dacErrMsgFmt, vovFuncName, "couldn\'t open owning lib to append", id);\
         RETURN_;}}

#define damiCheckLModeNRV(id)\
      {if(!idaiSetViewLevelObjectAppendMode((dbId) id)){\
         voWarn(dacErrMsgFmt, vovFuncName, "couldn\'t open owning lib to append", id);\
         return;}}

#define damCheckDbId(sub, typeName, dbType) \
{    if (dbGetType((int) sub) != (dbType)){	  \
	 voWarn("%s was passed an invalid %s Id (%d).\n", vovFuncName, typeName, (sub));\
          RETURN(0);}}

#define damCheckHierProp( subj, pT, pV) \
 {if(pT == dbcHierPropType) (void) dbGetPropByName(subj, dacHierPropValueName, &pT, &pV);}

#define damCheckDbIdNRV(sub, typeName, dbType) \
{    if (dbGetType((int) sub) != (dbType)){	  \
	 voWarn("%s was passed an invalid %s Id (%d).\n", vovFuncName, typeName,(sub)); RETURN_;}}

#define damCheckFigId(sub)\
{    switch ((int)dbGetType((int) sub)) {\
     case dbcLabelType: case dbcArcType: case dbcDonutType: case dbcDotType: case dbcEllipseType: \
     case dbcPathType:	case dbcLineType: case dbcPolygonType: case dbcRectType: case dbcInstType: \
     break;  default:  {voWarn("%s was passed an invalid Fig Id (%d).\n",\
     vovFuncName,(sub)); RETURN(0);}}}

