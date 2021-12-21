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
# $Id: iitkDApvt.h,v 1.1 1997/04/05 00:29:49 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef CDS_HEADER_INFO /*startCDS_INCLUDE_HEADERstart*/
#ifdef itkDApvt_P
    crash if included 2+ times
#endif /* itkDApvt_P */
#define itkDApvt_P
/****************************************************************/
/* Copyright (C) 1983, 1984, 1985, 1986, 1987, 1988, 1989, 1990	*/
/*	Cadence Design Systems Inc.  All Rights Reserved.	*/
/****************************************************************/
#endif /*CDS_HEADER_INFO endCDS_INCLUDE_HEADERend*/
#ifndef  daGlobals_C_ITK

/*
andrewt 2-12-96  change ida() to ida*()
*/

extern dbType  dbGetType();
extern void    sincos();
extern dbId    dbGetPropObject();
extern dmLibId dmGetLib();

extern  String          dacErrMsgFmt;
extern  String          dacGlobalStgFmt;
extern  String          dacGlobalNoLuckFmt;
extern  String          dacGlobalNumFmt;
extern  String          dacHierPropValueName;
extern  String          dacGlobalPortNetName;

#endif

/*
 * workarounds to avoid lint problems from cdba interfacing problems which
 * won't be fixed in 4.0
 */
#define   idacCreatePaletteByAttributeNumber        idacCreatePaletteByAttributeNum
#define   idacGetDisplayFillPatternByNumber	   idacGetDisplayFillPatternByNumb
#define   idacGetFigGroupPalatteByDisplayName	   idacGetFigGroupPalatteByDisplay
#define   idacGetPortInstBundleAnyPortInstByIndex   idacGetPortInstBundleAnyPortIns
#define   idacGetPortInstBundleMemberNameList	   idacGetPortInstBundleMemberName
#define   idacGetTechFigGroupByNumberPurposeNumber  idacGetTechFigGroupByNumberPurp
#define   idacGetViewContainedAnyInstByName	   idacGetViewContainedAnyInstByNa
#define   idacInitPortInstBundleAnyPortInstList	   idacInitPortInstBundleAnyPortIn
#define   idacInitPortInstBundlePortInstList	   idacInitPortInstBundlePortInstL
#define   idacPortInstBundleAnyPortInstList         idacPortInstBundleAnyPortInstLi

void      ilSetStandAloneSignals();
void      dbiFreeExpName();
Boolean   dbiEvalName();

/* TODO: remove these when they get promoted */
extern void     sincos();
extern int      dbGetPropObject();
extern Boolean  dbiEvalPatchExpr(); /* dbPrivate.h */

#ifndef dbPrivate_P
typedef dbPoint *dbPointArray;
#endif

typedef enum {			/* daCallerType */
     dacNotListType,
     dacAnyInstAnyPortInstList,
     dacAnyInstFigList,
     dacAnyInstPortInstList,
     dacAnyNetAnyPortInstList,
     dacAnyNetAnyPortList,
     dacAnyNetFigList,
     dacAnyNetPortInstList,
     dacAnyNetPortList,
     dacAnyNetRipperList,
     dacAnyPortInstPortInstImpList,
     dacAnyPortPortImpList,
     dacCellViewList,
     dacFigChildList,
     dacFigGroupPaletteList,
     dacFigGroupPalletteList,
     dacFigGroupRefFigList,
     dacGroupElementList,
     dacInstBundleAnyInstList,
     dacInstBundleInstList,
     dacLibCellList,
     dacLibViewTypeList,
     dacNetAnyPortList,
     dacNetBundleAnyNetList,
     dacNetBundleAnyPortList,
     dacNetBundleFigList,
     dacNetBundleNetList,
     dacNetBundlePortInstList,
     dacNetBundlePortList,
     dacNetBundleRipperList,
     dacNetChildNetList,
     dacNetConnectedNetList,
     dacNetFigList,
     dacNetPortInstList,
     dacNetPortList,
     dacNetRippedNetList,
     dacNetRipperList,
     dacObjectElementList,
     dacObjectGroupList,
     dacObjectPropList,
     dacPortBundleAnyPortList,
     dacPortBundlePortImpList,
     dacPortBundlePortList,
     dacPortImpFigList,
     dacPortInstBundleAnyPortInstList,
     dacPortInstBundlePortInstList,
     dacPortPortImpList,
     dacTechDisplayList,
     dacTechFigGroupList,
     dacViewActiveAnyInstList,
     dacViewActiveInstList,
     dacViewAnyNetList,
     dacViewAnyPortList,
     dacViewContainedAnyInstList,
     dacViewContainedInstList,
     dacViewFigGroupRefList,
     dacViewFigList,
     dacViewNetList,
     dacViewPortList,
     dacViewRipperList,
     dacViewTypeViewList
}               daCallerType;

typedef enum {			/* daStateType */
     dacNotState = 0,
     dacDmState = 1,
     dacDbState = 2,
     dacDbExtendState = 3
} daStateType;

typedef enum {                  /* daSubjType */
     dacDoesntMatter,
     dacScalarMember,
     dacScalar,
     dacBundle,
} daSubjType;

/* used to format a layer-purpose pair end to end so they fit into a long */
typedef struct daLPP {
     dbLayer         layer;
     dbPurpose       purpose;
}               daLPP;

/* used by dasGetNextRippedNet to keep track of signals ripped to a given signal */
typedef struct daThisRipperEntry{
        int*  src;
        int*  dst;
        Boolean dstIsScalar;
}	daThisRipperEntry;

/* used by daiGetRippedMemNet to return a net/member pair */
typedef struct daRipTarget {
        dbNetId   net;
        long      mem;
}	daRipTarget;

/*
 * union to map a long,  and an lpp into the same space, for use by
 * daListState, and by daIdElement. The use of these fields never coincides
 * since index is only used by    connectivity objects, while lpp is never
 * used by connectivity objects.
 */
typedef union daStateExtra {
     long            id;
     int             index;
     daLPP           lpp;
     daThisRipperEntry* rip;
}               daStateExtra;

typedef struct daListStateBlock {    /* FOR THIS DEF: parent means the object passed to daInit...*/
     char            tag;            /* ascii tag to visually mark a list state for debug */
     daSubjType      subjType;       /* Was the subj of the reln. a bundle, scalar-mem or scalar */
     daCallerType    callerType;     /* enum saying which routine allocated the list state */ 
     struct daListStateBlock *next;  /* pointer to the previous list state in a stack of 'em */
     union {
	  dbGenStateId    db;
	  dmGenStatePtr   dm;
     }               current;        /* Cdba (da or dm) generator if any */
     dbId            lastId;         /* Last id returned from generator; parent if no generator */
     int             mem;            /* Member number of parent if any */
     int             upto;           /* Times we've been in daNext... if simulating objects */
     int             lim;            /* Size of parent if it's a bundle */
     HashStruct     *names;          /* Pointer to hash structure when netlisting */
     daStateExtra    etc;	     /* returned item's lpp, or subj when there is a */
}               daListStateBlock;    /*  generator, or index of subj if bundle member or */
                                     /*    or src/dst ptrs if rippedNet list */ 
typedef struct daPropStatus {
     String          propName;
     dbPropType      propValueType;
     dbPropValue     propValue;
}               daPropStatus;

typedef struct daIdElement {
     struct daIdElement *next;
#if MD_NCR
     /* daId is defined as a typedef for "long", this causes compiler error
      * on NCR port. -- whagi */
     long int        daId;
#else
     long            daId;
#endif
     long            cdbaId;
     daStateExtra    etc;
     daObType        obType;
}               daIdElement;

typedef struct daAliasDef {
     String          name;
     struct daAliasDef *next;
     dbId            object;
}               daAliasDef;

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

#define damMapPoint(b,a)       { union { dbPoint *db; daPoint *da;} t; t.da = a; b = *t.db;}

#define damMapDaToDbBBox(b,a)  { union { dbBBox *db;  daBBox  *da;} t; t.da = a; b = *t.db;}

#define damMapDbToDaBBox(a,b)  { union { dbBBox *db;  daBBox  *da;} t; t.db = b; b = *t.da;}

#define damMapDaPtArrDb(len,point,in) \
  {daPointArray q;union{dbPoint *b; daPoint *a;}u;q= *in;len=q.length;u.a=q.point;point=u.b;}

#define damLookupId( A )     idaiLookupId( (long) A )

#define damIssueDaId( A, B ) idaiIssueId( (long) A, B )

#define damIsDaId( A )       ( A & 1 )

#define damPopState( pState ) \
    {daListStateBlock* temp = pState->next; idaQuitList(pState); pState = temp;}

#define damPushState( pState ) \
    {daListStateBlock* temp = (daListStateBlock*)voMalloc(sizeof(daListStateBlock));\
    temp->next=pState; temp->tag='#'; temp->upto=0; temp->lim=0; temp->lastId=0; pState=temp;}

#define damIsStateNull(a) \
 {if(a == NULL){voWarn(dacNextErrMsg, vovFuncName); RETURN(0);}}

#define  damSetSubjType(pSub) \
 {if(pSub)                pState->subjType = dacScalarMember; \
  else if(pState->lim >1) pState->subjType = dacBundle;\
  else                    pState->subjType = dacScalar; }

#define damMapDaIdToDbIdAndIndex(daSubj, subj, mem) \
{if ((subj & 1) && NULL !=(daSubj = idaiLookupId((daId)subj))){\
   subj = daSubj->cdbaId;  mem  = daSubj->etc.index;}}

#define damCheckObjId(sub, obType, daSub, idSys) \
{  if ((sub & 1) && (NULL != (daSub = idaiLookupId((daId) (sub))))) idSys = dacDaIdType;     \
	  else if (dbIsId((dbId) sub)) idSys = dacDbIdType;     \
	  else if (dmIsId((dmId) sub)) idSys = dacDmIdType;     \
	  else {voWarn("%s was passed an invalid %s Id (%d).\n",vovFuncName,obType,sub);RETURN(0);}}

#define damCheckObjIdNRV(sub, obType, daSub, idSys) \
{  if ((sub & 1) && (NULL != (daSub = idaiLookupId((daId) (sub))))) idSys = dacDaIdType;     \
	  else if (dbIsId((dbId) sub)) idSys = dacDbIdType;     \
	  else if (dmIsId((dmId) sub)) idSys = dacDmIdType;     \
	  else {voWarn("%s was passed an invalid %s Id (%d).\n",vovFuncName,obType,sub); RETURN_;}}

#define damCheckFigId(sub)\
{    switch ((int)dbGetType((int) sub)) {\
     case dbcLabelType: case dbcArcType: case dbcDonutType: case dbcDotType: case dbcEllipseType: \
     case dbcPathType:	case dbcLineType: case dbcPolygonType: case dbcRectType: case dbcInstType: \
     break;  default:  {voWarn("%s was passed an invalid Fig Id (%d).\n",\
     vovFuncName,(sub)); RETURN(0);}}}

#define damCheckFigIdNRV(sub)\
{    switch ((int)dbGetType((int) sub)) { \
     case dbcLabelType: case dbcArcType: case dbcDonutType: case dbcDotType: case dbcEllipseType: \
     case dbcPathType:	case dbcLineType: case dbcPolygonType: case dbcRectType: case dbcInstType: \
     break; default: {voWarn("%s was passed an invalid Fig Id (%d).\n",vovFuncName,sub);RETURN_;}}}

#define damCheckAnyId(sub, typeName, dbType, daType, daSub) \
{ if ((sub & 1) && (NULL != (daSub = idaiLookupId((daId)sub))) && daSub->obType == daType); \
  else if (dbGetType((int)sub) == dbType);\
  else {voWarn("%s was passed an invalid %s Id (%d).\n",vovFuncName,typeName,sub);RETURN(0); }}

#define damCheckAnyIdNRV(sub, typeName, dbType, daType, daSub) \
{ if ((sub & 1) && (NULL != (daSub = idaiLookupId((daId) sub))) && daSub->obType == daType); \
       else if (dbGetType((int) sub) == dbType);     \
       else{voWarn("%s was passed an invalid %s Id (%d).\n",vovFuncName,typeName,sub);RETURN_;}}

#define damCheckAnyIdREnum(sub, typeName, dbType, daType, daSub, rType) \
{    if ((sub & 1) && (NULL != (daSub = idaiLookupId((daId) sub))) && daSub->obType == daType); \
	  else if (dbGetType((int) sub) == dbType);     \
	  else {voWarn("%s was passed an invalid %s Id (%d).\n", vovFuncName, typeName,(sub));\
          RETURN((rType)0);}}

#define damCheckDbIdREnum(sub, typeName, dbType, rType) \
{    if (dbGetType((int) sub) != (dbType)){	  \
	 voWarn("%s was passed an invalid %s Id (%d).\n", vovFuncName, typeName, (sub));\
         RETURN((rType)0);}}

#define damCheckDbId(sub, typeName, dbType) \
{    if (dbGetType((int) sub) != (dbType)){	  \
	 voWarn("%s was passed an invalid %s Id (%d).\n", vovFuncName, typeName, (sub));\
          RETURN(0);}}

#define damCheckDbIdNRV(sub, typeName, dbType) \
{    if (dbGetType((int) sub) != (dbType)){	  \
	 voWarn("%s was passed an invalid %s Id (%d).\n", vovFuncName, typeName,(sub)); RETURN_;}}

#define damCheckDmId(sub, typeName, dmType) \
{    if (dmGetType((dmId) sub) != (dmType))	  \
	 {voWarn("%s was passed an invalid %s Id (%d)\n", vovFuncName, typeName,(sub));RETURN(0);}}

#define damCheckDmIdNRV(sub, typeName, dmType) \
{    if (dmGetType((dmId) sub) != (dmType))	  \
	 {voWarn("%s was passed an invalid %s Id (%d)\n", vovFuncName, typeName,(sub));RETURN_;}}

#define damCheckDaId(sub, typeName, daType, daSub) \
{    if (!(daSub = idaiLookupId((daId) (sub))) || daSub->obType != daType)  \
	{ voWarn("%s was passed an invalid %s Id (%d).\n", vovFuncName, typeName,sub); RETURN(0);}}

#define damCheckDaIdNRV(sub, typeName, daType, daSub) \
{    if (!(daSub = idaiLookupId((daId) (sub))) || daSub->obType != daType)  \
	{ voWarn("%s was passed an invalid %s Id (%d).\n", vovFuncName, typeName,(sub)); RETURN_;}}

#define damCheckPALenEq(len, shape, num, retType) \
{    if (len != num){\
        voWarn("In %s - Need %d points to create a %s, found: %d.", vovFuncName, num, shape, len);\
        RETURN((retType) 0);}}

#define damGetNameSize(name, width) \
{dbExpName  *ptNameStruct; (void) dbiEvalName(name, &ptNameStruct, &width);\
          (void) dbiFreeExpName( ptNameStruct);}

/* macro to return a cdba id as a daId value  */
#define damReturnPhonyId(id, daType, outType ) \
   { long    lint1 =  (id);                    \
     outType lint2 =  idaiGetId( lint1, 0L, (daType), True);return( lint2 ); }

/* macro to make an LTF return the next cdba id of a series as an itkId value */
#define damReturnNextPhonyId(id, itkType, outType) \
   {long    lint1 = (id);                          \
   outType  lint2 = idaiGetId( lint1, state->count++, (itkType), (state->size < 2)); return(lint2);}

/* macro to get the type and value when an hierarchical prop is found */
#define damCheckHierProp( subj, pT, pV) \
 {if(pT == dbcHierPropType) (void) dbGetPropByName(subj, dacHierPropValueName, &pT, &pV);}

#define damCheckHierPropGetId( subj, pT, pV) \
 {if(pT == dbcHierPropType) subj = dbGetPropByName(subj, dacHierPropValueName, &pT, &pV);}

String idaiGetCallerTypeName();
String idaiGetStateTypeName();
String idaiGetNameProp();
String idaiGetRipperPatchCord();
void   idaiCreateIdTable();
void   idaiDestroyIdTable();
daIdElement *idaiLookupId();
daId   idaiGetId();
dbPropId idaiIsRipper();
String idaiGetObjectType();
void   idbiDeleteDaElements();
void   idaiAdoptStepChildren();
void   idaiDeleteDaObject();
void   idaiDeleteDmObject();
void   idaiDeleteDbObject();
Boolean idaiSetLibLevelObjectAppendMode();
Boolean idaiSetViewLevelObjectAppendMode();
Boolean idaiCheckPropOwner();
void idaiMakeAliasTable();
daAliasDef *idaiMakeAliasTableEntry();
void idaiMakeIdList();
void idaiFreeIdList();
Boolean idaiCheckIdListForDup();
Boolean idaiSetNameProp();
