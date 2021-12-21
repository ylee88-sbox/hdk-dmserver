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
# $Id: iitkDA.h,v 1.1 1997/04/05 00:29:47 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef CDS_HEADER_INFO /*startCDS_INCLUDE_HEADERstart*/
#ifdef itkDA_P
    crash if included 2+ times
#endif /* itkDA_P */
#define itkDA_P
/****************************************************************/
/* Copyright (C) 1983, 1984, 1985, 1986, 1987, 1988, 1989, 1990	*/
/*	Cadence Design Systems Inc.  All Rights Reserved.	*/
/****************************************************************/
#endif /*CDS_HEADER_INFO endCDS_INCLUDE_HEADERend*/


/*****************************************************************************
 * itkDA.h --
 *	Public structures, functions, etc., that define the interface for
 *	ITK90.
 *
 * Copyright, 1990, 1991, CADENCE Design Systems, Inc.
 * No part of this file may be reproduced, stored in a retrieval system,
 * or transmitted in any form or by any means --- electronic, mechanical,
 * photocopying, recording, or otherwise --- without prior written permission
 * of CADENCE Design Systems, Inc.
 *
 *****************************************************************************
 */

/******************   # d e f i n e    S t a t e m e n t s   *****************/

/*
2-12-96 andrewt change da*() to ida*()

*/
#define idaCreatePaletteByAttributeNumber       idaCreatePaletteByAttributeNumb
#define idaGetAnyPortInstPortInstImpByName      idaGetAnyPortInstPortInstImpByN
#define idaGetFigGroupPaletteByDisplayName      idaGetFigGroupPalatteByDispName
#define idaGetPortInstBundleAnyPortInstByIndex  idaGetPortInstBundleAnyPortInst
#define idaGetPortInstBundleMemberNameList      idaGetPortInstBundleMemberNameL
#define idaInitAnyPortInstPortInstImpList       idaInitAnyPortInstPortInstImpLi
#define idaInitPortInstBundleAnyPortInstList    idaInitPortInstBundleAnyPortIns
#define idaInitPortInstBundlePortInstList       idaInitPortInstBundlePortInstLi
#define idaSearchAnyInstDescriberViewByName     idaSearchAnyInstDescriberViewBy
#define idaSetPaletteOutlineColorByNumber       idaSetPaletteOutlineColorByNumb


#define daListState voidStar

/***************   I n c l u d e d    D e p e n d e n c i e s   **************/
#include <vo.h>
#include <il.h>

/**************    T y p e d e f ' e d    V a l u e s    *********************/
typedef String  daNlpExpr;

		     /* The Object Id Class-Hierarchy */
typedef long		daId;

typedef daId		daCellId;
typedef daId		daDisplayId;
typedef daId		daElementId;
typedef daId		daFigGroupId;
typedef daId		daFigGroupRefId;
typedef daId		daGroupId;
typedef daId		daLibId;
typedef daId		daPaletteId;
typedef daId		daPortImpId;
typedef daId		daPortInstImpId;
typedef daId		daPropId;
typedef daId		daTechId;
typedef daId		daViewId;
typedef daId		daViewTypeId;

typedef daId		daFigId;
typedef daFigId		daAnnotateId;
typedef daFigId		daCurveId;
typedef daFigId		daDonutId;
typedef daFigId		daDotId;
typedef daFigId		daEllipseId;
typedef daFigId		daPathId;
typedef daFigId		daPolygonId;
typedef daFigId		daRectangleId;

typedef daId		daAnyInstId;
typedef daAnyInstId	daInstBundleId;
typedef daAnyInstId	daInstId;
typedef daAnyInstId	daRipperId;

typedef daId		daAnyNetId;
typedef daAnyNetId	daNetBundleId;
typedef daAnyNetId	daNetId;

typedef daId		daAnyPortId;
typedef daAnyPortId	daPortBundleId;
typedef daAnyPortId	daPortId;

typedef daId		daAnyPortInstId;
typedef daAnyPortInstId	daPortInstBundleId;
typedef daAnyPortInstId	daPortInstId;

typedef enum
{
  DAnotObjectType,
  DAnullObject,
  DAannotateObject,
  DAcellObject,
  DAcurveObject,
  DAdisplayObject,
  DAdonutObject,
  DAdotObject,
  DAelementObject,
  DAellipseObject,
  DAfigGroupObject,
  DAfigGroupRefObject,
  DAgroupObject,
  DAlibObject,
  DAnetBundleObject,
  DAnetObject,
  DApaletteObject,
  DApathObject,
  DApolygonObject,
  DAportBundleObject,
  DAportImpObject,
  DAportInstImpObject,
  DAportInstBundleObject,
  DAportInstObject,
  DAportObject,
  DApropObject,
  DArectangleObject,
  DAripperObject,
  DAtechObject,
  DAinstBundleObject,
  DAinstObject,
  DAviewObject,
  DAviewTypeObject,
} daObjectType;

typedef enum
{
  DAnotViewPurpose,
  DAinterface,
  DAcontents,
} daViewPurpose;

typedef enum
{
  DAnotPropValueType = 0,
  DAstring = 1,
  DAinteger = 2,
  DAfloat = 3,
  DAboolean = 4,
  DAtime = 5,				/* Cadence-specific */
  DAfileName = 6,			/* Cadence-specific */
  DAilExpr = 7,				/* Cadence-specific */
  DAnlpExpr= 8,				/* Cadence-specific */
  DAilList = 10,			/* Cadence-specific */
} daPropValueType;

typedef enum
{
  DAnotOrient = 0,
  DAr0 = 1,
  DAr90 = 2,
  DAr180 = 3,
  DAr270 = 4,
  DAmy = 5,
  DAmyR90 = 6,
  DAmx = 7,
  DAmxR90 = 8,
} daOrient;

typedef enum
{
  DAnotPortDirection = 0,
  DAundefinedPortDirection = 0,
  DAinput = 1,
  DAoutput = 2,
  DAinputOutput = 3,
} daPortDirection;


typedef enum
{
  DAnotJustify,
  DAupperLeft,
  DAcenterLeft,
  DAlowerLeft,
  DAupperCenter,
  DAcenterCenter,
  DAlowerCenter,
  DAupperRight,
  DAcenterRight,
  DAlowerRight,
} daJustify;

typedef enum
{
  DAnotFont,
  DAeuroFont,
  DAgothicFont,
  DAmathFont,
  DAromanFont,
  DAscriptFont,
  DAstickFont,
  DAfixedFont,
  DAswedishFont,
} daFontStyle;

typedef enum
{
  DAnotAnnotateType,
  DAnormalAnnotate,
  DAnlpAnnotate,			/* Cadence-specific */
  DAilAnnotate,				/* Cadence-specific */
} daAnnotateType;

typedef enum
{
  DAnotPathStyle,
  DAtruncateExtend,
  DAextendExtend,
  DAroundRound,
  DAextendVarExtend,
} daPathStyle;


/* 
 * Moved daIdSys and daObType here from itkDApvt.h as a workaround
 * for 4.2.2 solaris integration
 */
typedef enum {			/* daIdSys */
     dacNotIdType = 0,
     dacDaIdType = 1,
     dacDbIdType = 2,
     dacDmIdType = 3
} daIdSys;


typedef enum {			/* daObType */
     dacDeletedType,
     dacPortType,
     dacNetType,
     dacInstType,
     dacPortInstType,
     dacFigGroupRefType,
     dacTechType,
     dacPaletteType
}               daObType;



	       /* System-defined FigGroup purpose constants */

#define DAunknownPurpose       0    /* Invalid purpose */                                           
#define	DAminUserPurpose       1                                              
#define	DAmaxUserPurpose       128                                            
#define	DAminPurpose	       234                                            
#define	DAmaxPurpose	       255                                            
#define DAwarningPurpose       234  /* Warning graphics */
#define DAtool1Purpose         235  /* Tool-specific graphics (invisible by default) */
#define DAtool0Purpose         236  /* Tool-specific graphics (invisible by default) */
#define DAlabelPurpose         237  /* Text graphics */                                          
#define DAflightPurpose        238  /* Cadence-specific, use for implicit connectivity */
#define DAerrorPurpose         239  /* Error graphics */                                          
#define DAannotatePurpose      240  /* Generic graphics */
#define DAdrawing1Purpose      241  /* Generic graphics */
#define DAdrawing2Purpose      242  /* Generic graphics */
#define DAdrawing3Purpose      243  /* Generic graphics */
#define DAdrawing4Purpose      244  /* Generic graphics */
#define DAdrawing5Purpose      245  /* Generic graphics */
#define DAdrawing6Purpose      246  /* Generic graphics */
#define DAdrawing7Purpose      247  /* Generic graphics */
#define DAdrawing8Purpose      248  /* Generic graphics */
#define DAdrawing9Purpose      249  /* Generic graphics */
#define DAboundaryPurpose      250  /* Containing area  */                                           
#define DApinPurpose           251  /* Port Implementation graphics */ 
#define DAdrawingPurpose       252
#define DAnetPurpose           253  /* Net and NetBundle graphics */                                 
#define DAcellPurpose          254  /* Cell-specific graphics (rarely used anymore within Cadence) */
#define DAallPurpose           255  /* Applies to all FigGroups with the same Name */

typedef unsigned int daDistance;

typedef unsigned char daDirection;

#define      DAnoDirection       0x00
#define      DAleftDirection     0x04
#define      DArightDirection    0x08
#define      DAbottomDirection   0x02
#define      DAtopDirection      0x01
#define      DAanyDirection      0x0f

typedef struct daColor
{
  int             number;
  unsigned short  red, green, blue;
  String          name;
} daColor;

typedef struct daFillPattern
{
  String          name;
  unsigned int    number, width, height;
  unsigned char*  pattern;
} daFillPattern;

typedef struct daLineStyle
{
  String          name;
  unsigned int    number, length, width;
  unsigned char* pattern;
} daLineStyle;

typedef struct daPoint
{
  int             x, y;
} daPoint;

typedef struct daPointArray
{
  int             length;
  daPoint*        point;
} daPointArray;

typedef struct daBBox
{
  daPoint         lowerLeft;
  daPoint         upperRight;
} daBBox;


/***************    E n u m e r a t e d    T y p e s    **********************/

/**********************   E x p o r t e d    I t e m s   *********************/

/* Declarations for the (external) ITK functions.  Note the use
 * of the EXTERN, ARGS, and NO_ARGUMENTS "macros" allow the ITK
 * functions to work with K&R (Classic) C, ANSI C, or C++.
 */
#ifndef ARGS
# if defined(__cplusplus) || defined(c_plusplus)

  /* C++ 2.0...
   */
#  define EXTERN	extern "C"
#  define ARGS(x)	x
#  define NO_ARGUMENTS

# else

  /* ANSI C or K&R (Classic) C...
   */

#  define EXTERN	extern
#  if (!defined(apollo) && !MD_NCR) && ((MD_SUNOS_5_X && __STDC__) || (!MD_SUNOS_5_X && defined(__STDC__)))
#   define ARGS(x)	x
#   define NO_ARGUMENTS	void
#  else
#   define ARGS(x)	()
#   define NO_ARGUMENTS
#  endif
# endif
#endif



EXTERN String		idaGetObjectTypeName		ARGS((daObjectType type));
EXTERN String		idaGetViewPurposeName		ARGS((daViewPurpose type));
EXTERN String		idaGetPropValueTypeName		ARGS((daPropValueType type));
EXTERN String		idaGetOrientName			ARGS((daOrient type));
EXTERN String		idaGetPortDirectionName		ARGS((daPortDirection type));
EXTERN String		idaGetJustifyName		ARGS((daJustify type));
EXTERN String		idaGetFontStyleName		ARGS((daFontStyle type));
EXTERN String		idaGetAnnotateTypeName		ARGS((daAnnotateType type));
EXTERN String		idaGetPathStyleName		ARGS((daPathStyle type));
EXTERN String		idaGetIdSysName			ARGS((daIdSys type));
EXTERN String		idaGetObTypeName			ARGS((daObType type));
EXTERN String		idaGetDirectionName		ARGS((daDirection type));
EXTERN String		idaGetAnnotateText		ARGS((daAnnotateId annotateId));
EXTERN daJustify	idaGetAnnotateJustify		ARGS((daAnnotateId annotateId));
EXTERN daOrient		idaGetAnnotateOrient		ARGS((daAnnotateId annotateId));
EXTERN daFontStyle	idaGetAnnotateFontStyle		ARGS((daAnnotateId annotateId));
EXTERN int		idaGetAnnotateFontSize		ARGS((daAnnotateId annotateId));
EXTERN Boolean		idaGetAnnotateDrafting		ARGS((daAnnotateId annotateId));
EXTERN Boolean		idaGetAnnotateOverbar		ARGS((daAnnotateId annotateId));
EXTERN daAnnotateType	idaGetAnnotateTextType		ARGS((daAnnotateId annotateId));
EXTERN daOrient		idaGetAnyInstOrient		ARGS((daAnyInstId anyInstId));
EXTERN double		idaGetAnyInstMagnification	ARGS((daAnyInstId anyInstId));
EXTERN daViewId		idaSearchAnyInstDescriberViewByName ARGS((daAnyInstId anyInstId, String viewTypeList));
EXTERN daViewId		idaGetAnyInstDescriberViewByName	ARGS((daAnyInstId anyInstId, String viewTypeList));
EXTERN daViewId		idaGetAnyInstOwnerView		ARGS((daAnyInstId anyInstId));
EXTERN daViewId		idaGetRipperOwnerView		ARGS((daRipperId ripperId));
EXTERN daInstBundleId	idaGetAnyInstInstBundle		ARGS((daAnyInstId anyInstId));
EXTERN daViewId		idaGetAnyNetView			ARGS((daAnyNetId anyNetId));
EXTERN daAnyNetId	idaGetAnyNetAnyNet		ARGS((daAnyNetId anyNetId));
EXTERN daPortDirection	idaGetAnyPortDirection		ARGS((daAnyPortId portId));
EXTERN daViewId		idaGetAnyPortView		ARGS((daAnyPortId portId));
EXTERN daPortBundleId	idaGetAnyPortPortBundle		ARGS((daAnyPortId portId));
EXTERN daAnyNetId	idaGetAnyPortAnyNet		ARGS((daAnyPortId anyPortId));
EXTERN daPortDirection	idaGetAnyPortInstDirection	ARGS((daAnyPortInstId anyPortInstId));
EXTERN daAnyInstId	idaGetAnyPortInstAnyInst		ARGS((daAnyPortInstId anyPortInstId));
EXTERN daAnyPortId	idaGetAnyPortInstAnyPort		ARGS((daAnyPortInstId anyPortInstId));
EXTERN daAnyNetId	idaGetAnyPortInstAnyNet		ARGS((daAnyPortInstId anyPortInstId));
EXTERN daLibId		idaGetCellLib			ARGS((daCellId cellId));
EXTERN daLibId		idaGetViewTypeLib		ARGS((daViewTypeId viewTypeId));
EXTERN int		idaGetDonutInnerRadius		ARGS((daDonutId donutId));
EXTERN int		idaGetDonutOuterRadius		ARGS((daDonutId donutId));
EXTERN daId		idaGetElementObject		ARGS((daElementId elementId));
EXTERN daGroupId	idaGetElementGroup		ARGS((daElementId elementId));
EXTERN int		idaGetFigGroupNumber		ARGS((daFigGroupId figGroupId));
EXTERN String		idaGetFigGroupPurposeName	ARGS((daFigGroupId figGroupId));
EXTERN int		idaGetFigGroupPurposeNumber	ARGS((daFigGroupId figGroupId));
EXTERN int		idaGetFigGroupPriority		ARGS((daFigGroupId figGroupId));
EXTERN Boolean		idaGetFigGroupBlink		ARGS((daFigGroupId figGroupId));
EXTERN Boolean		idaGetFigGroupSelect		ARGS((daFigGroupId figGroupId));
EXTERN Boolean		idaGetFigGroupDrag		ARGS((daFigGroupId figGroupId));
EXTERN daTechId		idaGetFigGroupTech		ARGS((daFigGroupId figGroupId));
EXTERN int		idaGetFigGroupRefNumber		ARGS((daFigGroupRefId figGroupRefId));
EXTERN String		idaGetFigGroupRefPurposeName	ARGS((daFigGroupRefId figGroupRefId));
EXTERN int		idaGetFigGroupRefPurposeNumber	ARGS((daFigGroupRefId figGroupRefId));
EXTERN daViewId		idaGetFigGroupRefView		ARGS((daFigGroupRefId figGroupRefId));
EXTERN daFigGroupId	idaGetFigGroupRefFigGroup	ARGS((daFigGroupRefId figGroupRefId));
EXTERN daBBox*	 	idaGetFigBBox			ARGS((daFigId figId));
EXTERN daId		idaGetFigObject			ARGS((daFigId figId));
EXTERN daFigGroupRefId	idaGetFigFigGroupRef		ARGS((daFigId figId));
EXTERN Boolean		idaGetGroupUniqueRef		ARGS((daGroupId groupId));
EXTERN Boolean		idaGetGroupOrdered		ARGS((daGroupId groupId));
EXTERN Boolean		idaGetGroupUniqueName		ARGS((daGroupId groupId));
EXTERN daId		idaGetGroupObject		ARGS((daGroupId groupId));
EXTERN int		idaGetInstBundleNumMembers	ARGS((daInstBundleId instBundleId));
EXTERN String		idaGetPILibPath			ARGS((void));
EXTERN daTechId		idaGetLibTech			ARGS((daLibId libId));
EXTERN String		idaGetPILibSearchPath		ARGS((void));
EXTERN String		idaGetLibReferenceLibs		ARGS((daLibId libId));
EXTERN int		idaGetNetBundleNumMembers	ARGS((daNetBundleId netBundleId));
EXTERN String		idaGetNetBundleMemberNameList	ARGS((daNetBundleId netBundleId));
EXTERN daObjectType	idaGetObjectType			ARGS((daId anyId));
EXTERN daColor*	 	idaGetPaletteFillColor		ARGS((daPaletteId paletteId));
EXTERN daColor*	 	idaGetPaletteOutlineColor	ARGS((daPaletteId paletteId));
EXTERN daColor*	 	idaGetPaletteBoldColor		ARGS((daPaletteId paletteId));
EXTERN daFillPattern*	idaGetPaletteFillPattern		ARGS((daPaletteId paletteId));
EXTERN daLineStyle*	idaGetPaletteLineStyle		ARGS((daPaletteId paletteId));
EXTERN daDisplayId	idaGetPaletteDisplay		ARGS((daPaletteId paletteId));
EXTERN int		idaGetPathWidth			ARGS((daPathId pathId));
EXTERN int		idaGetPathBeginExt		ARGS((daPathId pathId));
EXTERN int		idaGetPathEndExt			ARGS((daPathId pathId));
EXTERN daPathStyle	idaGetPathStyle			ARGS((daPathId pathId));
EXTERN int		idaGetPortBundleNumMembers	ARGS((daPortBundleId portBundleId));
EXTERN String		idaGetPortBundleMemberNameList	ARGS((daPortBundleId portBundleId));
EXTERN daDirection	idaGetPortImpAccessDirection	ARGS((daPortImpId portImpId));
EXTERN daAnyPortId	idaGetPortImpAnyPort		ARGS((daPortImpId portImpId));
EXTERN daAnyPortInstId	idaGetPortInstImpAnyPortInst	ARGS((daPortInstImpId portInstImpId));
EXTERN daPortImpId	idaGetPortInstImpPortImp		ARGS((daPortInstImpId portInstImpId));
EXTERN int		idaGetPortInstBundleNumMembers	ARGS((daPortInstBundleId portInstBundleId));
EXTERN String		idaGetPortInstBundleMemberNameList ARGS((daPortInstBundleId portInstBundleId));
EXTERN long		idaGetPropTimeValue		ARGS((daPropId propId));
EXTERN daPropValueType	idaGetPropValueType		ARGS((daPropId propId));
EXTERN float		idaGetPropFloatValue		ARGS((daPropId propId));
EXTERN int		idaGetPropIntegerValue		ARGS((daPropId propId));
EXTERN String		idaGetPropStringValue		ARGS((daPropId propId));
EXTERN Boolean		idaGetPropBooleanValue		ARGS((daPropId propId));
EXTERN String		idaGetPropFileNameValue		ARGS((daPropId propId));
EXTERN list		idaGetPropILListValue		ARGS((daPropId propId));
EXTERN String		idaGetPropILExprValue		ARGS((daPropId propId));
EXTERN daNlpExpr	idaGetPropNlpExprValue		ARGS((daPropId propId));
EXTERN daAnyNetId	idaGetRipperAnyNet1		ARGS((daRipperId ripperId));
EXTERN daAnyNetId	idaGetRipperAnyNet2		ARGS((daRipperId ripperId));
EXTERN daViewPurpose	idaGetViewPurpose		ARGS((daViewId viewId));
EXTERN daCellId		idaGetViewCell			ARGS((daViewId viewId));
EXTERN daViewTypeId	idaGetViewViewType		ARGS((daViewId viewId));
EXTERN String		idaGetViewTypeUse		ARGS((daViewTypeId viewTypeId));
EXTERN daPaletteId	idaGetFigGroupPaletteByDisplayId	ARGS((daFigGroupId subj, daDisplayId displayId));
EXTERN daPaletteId	idaGetFigGroupPaletteByDisplayName ARGS((daFigGroupId subj, String displayName));
EXTERN daAnyInstId	idaGetInstBundleAnyInstByIndex	ARGS((daInstBundleId instBundleId, int index));
EXTERN daAnyNetId	idaGetNetBundleAnyNetByIndex	ARGS((daNetBundleId netBundleId, int index));
EXTERN daAnyPortId	idaGetPortBundleAnyPortByIndex	ARGS((daPortBundleId portBundleId, int index));
EXTERN daAnyPortInstId	idaGetPortInstBundleAnyPortInstByIndex ARGS((daPortInstBundleId portInstBundleId,
								    int index));
EXTERN daAnyPortInstId	idaGetAnyInstAnyPortInstByName	ARGS((daAnyInstId anyInstId, String name));
EXTERN daPortInstImpId	idaGetAnyPortInstPortInstImpByName ARGS((daAnyPortInstId anyPortInstId, String refName));
EXTERN daPortImpId	idaGetAnyPortPortImpByName	ARGS((daAnyPortId anyPortId, String name));
EXTERN daColor*	 	idaGetDisplayColorByName		ARGS((daDisplayId displayId, String name));
EXTERN daFillPattern*	idaGetDisplayFillPatternByName	ARGS((daDisplayId displayId, String name));
EXTERN daLineStyle*	idaGetDisplayLineStyleByName	ARGS((daDisplayId displayId, String name));
EXTERN daCellId		idaGetLibCellByName		ARGS((daLibId libId, String cellName));
EXTERN daViewTypeId	idaGetLibViewTypeByName		ARGS((daLibId libId, String viewTypeName));
EXTERN daPropId		idaGetObjectPropByName		ARGS((daId anyId, String name));
EXTERN daGroupId	idaGetObjectGroupByName		ARGS((daId anyId, String name));
EXTERN daDisplayId	idaGetTechDisplayByName		ARGS((daTechId techId, String name));
EXTERN daAnyInstId	idaGetViewContainedAnyInstByName	ARGS((daViewId viewId, String name));
EXTERN daAnyNetId	idaGetViewAnyNetByName		ARGS((daViewId viewId, String name));
EXTERN daAnyPortId	idaGetViewAnyPortByName		ARGS((daViewId viewId, String name));
EXTERN daViewId		idaGetViewTypeViewByName		ARGS((daViewTypeId viewTypeId, String cellName));
EXTERN daViewId		idaGetCellViewByName		ARGS((daCellId cellId, String viewTypeName));
EXTERN daFigGroupRefId	idaGetViewFigGroupRefByName	ARGS((daViewId viewId, String layerName,
							      String purposeName));
EXTERN daFigGroupId	idaGetTechFigGroupByName		ARGS((daTechId techId, String layerName,
							      String purposeName));
EXTERN daColor*	 	idaGetDisplayColorByNumber	ARGS((daDisplayId displayId, int number));
EXTERN daFillPattern*	idaGetDisplayFillPatternByNumber	ARGS((daDisplayId displayId, int number));
EXTERN daLineStyle*	idaGetDisplayLineStyleByNumber	ARGS((daDisplayId displayId, int number));
EXTERN daTechId		idaGetDisplayTech		ARGS((daDisplayId displayId));
EXTERN daFigGroupId	idaGetTechFigGroupByNumber	ARGS((daTechId techId, int num, int purposeNum));
EXTERN daPointArray*	idaGetFigPointArray		ARGS((daFigId FigId));
EXTERN String		idaGetObjectName			ARGS((daId anyId));
EXTERN daListState	idaInitAnyInstPortInstList	ARGS((daAnyInstId anyInstId));
EXTERN daListState	idaInitAnyInstFigList		ARGS((daAnyInstId anyInstId));
EXTERN daListState	idaInitAnyNetPortInstList	ARGS((daAnyNetId anyNetId));
EXTERN daListState	idaInitAnyNetAnyPortInstList	ARGS((daAnyNetId anyNetId));
EXTERN daListState	idaInitAnyNetPortList		ARGS((daAnyNetId anyNetId));
EXTERN daListState	idaInitAnyNetAnyPortList		ARGS((daAnyNetId anyNetId));
EXTERN daListState	idaInitAnyNetFigList		ARGS((daAnyNetId anyNetId));
EXTERN daListState	idaInitAnyNetRipperList		ARGS((daAnyNetId anyNetId));
EXTERN daListState	idaInitAnyPortPortImpList	ARGS((daAnyPortId anyPortId));
EXTERN daListState	idaInitAnyPortInstPortInstImpList ARGS((daAnyPortInstId anyPortInstId));
EXTERN daListState	idaInitCellViewList		ARGS((daCellId cellId));
EXTERN daListState	idaInitFigGroupPaletteList	ARGS((daFigGroupId figGroupId));
EXTERN daListState	idaInitFigGroupRefFigList	ARGS((daFigGroupRefId figGroupRefId));
EXTERN daListState	idaInitGroupElementList		ARGS((daGroupId groupId));
EXTERN daListState	idaInitAnyInstAnyPortInstList	ARGS((daInstId anyInstId));
EXTERN daListState	idaInitInstBundleAnyInstList	ARGS((daInstBundleId instBundleId));
EXTERN daListState	idaInitInstBundleInstList	ARGS((daInstBundleId instBundleId));
EXTERN daListState	idaInitLibCellList		ARGS((daLibId libId));
EXTERN daListState	idaInitLibViewTypeList		ARGS((daLibId libId));
EXTERN daListState	idaInitNetChildNetList		ARGS((daNetId netId));
EXTERN daListState	idaInitNetConnectedNetList	ARGS((daNetId netId));
EXTERN daListState	idaInitNetRippedNetList		ARGS((daNetId netId, long mem, daRipperId ripper));
EXTERN daListState	idaInitNetBundleNetList		ARGS((daNetBundleId netBundleId));
EXTERN daListState	idaInitNetBundleAnyNetList	ARGS((daNetBundleId netBundleId));
EXTERN daListState	idaInitObjectPropList		ARGS((daId anyId));
EXTERN daListState	idaInitObjectGroupList		ARGS((daId anyId));
EXTERN daListState	idaInitObjectElementList		ARGS((daId anyId));
EXTERN daListState	idaInitPortBundleAnyPortList	ARGS((daPortBundleId portBundleId));
EXTERN daListState	idaInitPortBundlePortList	ARGS((daPortBundleId portBundleId));
EXTERN daListState	idaInitPortImpFigList		ARGS((daPortImpId portImpId));
EXTERN daListState	idaInitPortInstBundleAnyPortInstList ARGS((daPortInstBundleId portInstBundleId));
EXTERN daListState	idaInitPortInstBundlePortInstList ARGS((daPortInstBundleId portInstBundleId));
EXTERN daListState	idaInitTechFigGroupList		ARGS((daTechId techId));
EXTERN daListState	idaInitTechDisplayList		ARGS((daTechId techId));
EXTERN daListState	idaInitViewContainedInstList	ARGS((daViewId viewId));
EXTERN daListState	idaInitViewContainedAnyInstList	ARGS((daViewId viewId));
EXTERN daListState	idaInitViewActiveInstList	ARGS((daViewId viewId));
EXTERN daListState	idaInitViewActiveAnyInstList	ARGS((daViewId viewId));
EXTERN daListState	idaInitViewFigGroupRefList	ARGS((daViewId viewId));
EXTERN daListState	idaInitViewFigList		ARGS((daViewId viewId));
EXTERN daListState	idaInitViewNetList		ARGS((daViewId viewId));
EXTERN daListState	idaInitViewAnyNetList		ARGS((daViewId viewId));
EXTERN daListState	idaInitViewPortList		ARGS((daViewId viewId));
EXTERN daListState	idaInitViewAnyPortList		ARGS((daViewId viewId));
EXTERN daListState	idaInitViewRipperList		ARGS((daViewId viewId));
EXTERN daListState	idaInitViewTypeViewList		ARGS((daViewTypeId viewTypeId));
EXTERN daAnyInstId	idaNextAnyInst			ARGS((daListState state));
EXTERN daAnyNetId	idaNextAnyNet			ARGS((daListState state));
EXTERN daAnyPortId	idaNextAnyPort			ARGS((daListState state));
EXTERN daAnyPortInstId	idaNextAnyPortInst		ARGS((daListState state));
EXTERN daCellId		idaNextCell			ARGS((daListState state));
EXTERN daDisplayId	idaNextDisplay			ARGS((daListState listState));
EXTERN daElementId	idaNextElement			ARGS((daListState state));
EXTERN daFigGroupId	idaNextFigGroup			ARGS((daListState state));
EXTERN daFigGroupRefId	idaNextFigGroupRef		ARGS((daListState state));
EXTERN daFigId		idaNextFig			ARGS((daListState state));
EXTERN daGroupId	idaNextGroup			ARGS((daListState state));
EXTERN daInstId		idaNextInst			ARGS((daListState state));
EXTERN daNetId		idaNextNet			ARGS((daListState state));
EXTERN daPaletteId	idaNextPalette			ARGS((daListState state));
EXTERN daPortId		idaNextPort			ARGS((daListState state));
EXTERN daPortImpId	idaNextPortImp			ARGS((daListState state));
EXTERN daPortInstImpId	idaNextPortInstImp		ARGS((daListState state));
EXTERN daPortInstId	idaNextPortInst			ARGS((daListState state));
EXTERN daPropId		idaNextProp			ARGS((daListState state));
EXTERN daRipperId	idaNextRipper			ARGS((daListState state));
EXTERN daViewId		idaNextView			ARGS((daListState state));
EXTERN daViewTypeId	idaNextViewType			ARGS((daListState state));
EXTERN void		idaQuitList			ARGS((daListState state));
EXTERN void		idaAttachAnyNetAnyPort		ARGS((daAnyNetId anyNetId, daAnyPortId anyPortId));
EXTERN void		idaAttachAnyNetAnyPortInst	ARGS((daAnyNetId anyNetId, daAnyPortInstId anyPortInstId));
EXTERN void		idaCloseLib			ARGS((daLibId libId));
EXTERN daPropId		idaCreateBooleanProp		ARGS((daId anyId, String propName, Boolean propValue));
EXTERN daCellId		idaCreateCell			ARGS((daLibId libId, String name));
EXTERN daDisplayId	idaCreateDisplay			ARGS((daTechId techId, String displayName,
							      int maxColors, int maxFills, int maxLineStyles));
EXTERN daDotId		idaCreateDot			ARGS((daViewId viewId, daFigGroupId figGroupId,
							      daPointArray* pointArray));
EXTERN daElementId	idaCreateElement			ARGS((daGroupId groupId, daId objectId));
EXTERN daFigGroupId	idaCreateFigGroup		ARGS((daTechId techId, String layerName, int layer,
							      String purposeName, int purpose));
EXTERN daPropId		idaCreateFileNameProp		ARGS((daId anyId, String propName, String propValue));
EXTERN daPropId		idaCreateFloatProp();
/* andrewt 2-13-96 
EXTERN daPropId		idaCreateFloatProp		ARGS((daId anyId, String propName, float propValue));
*/
EXTERN daGroupId	idaCreateGroup			ARGS((daId anyId, String name, Boolean uniqueRef,
							      int ordered, Boolean uniqueName));
EXTERN daPropId		idaCreateILExprProp		ARGS((daId anyId, String propName, String propValue));
EXTERN daPropId		idaCreateILListProp		ARGS((daId anyId, String propName,
							      union iltLispval *propValue));
EXTERN daInstId		idaCreateInst			ARGS((daViewId viewId, daViewId describerId,
							      String instName, daPointArray *origin,
							      daOrient orientation, double magnification));
EXTERN daInstBundleId	idaCreateInstBundle();
/* ### andrewt 2-12-96 
EXTERN daInstBundleId	idaCreateInstBundle		ARGS((daViewId viewId, daViewId describerId,
							      String bundleName, daPointArray *origin,
							      daOrient orient, double magnification, int numMem));
*/
EXTERN daPropId		idaCreateIntegerProp		ARGS((daId anyId, String propName, int propValue));
EXTERN daLibId		idaCreateLib			ARGS((String libName, String path, String technology));
EXTERN daNetId		idaCreateNet			ARGS((daViewId viewId, String netName, daNetId parentNet));
EXTERN daNetBundleId	idaCreateNetBundle		ARGS((daViewId viewId, String bundleName,
							      String memberNameList));
EXTERN daPropId		idaCreateNlpExprProp		ARGS((daId anyId, String propName, String propValue));
EXTERN daPortId		idaCreatePort			ARGS((daViewId viewId, String portName,
							      daPortDirection direction));
EXTERN daPortBundleId	idaCreatePortBundle		ARGS((daViewId viewId, String bundleName,
							      String memberNameList, daPortDirection direction));
EXTERN daPortImpId	idaCreatePortImp			ARGS((daPortId portId, String name, daFigId figureId,
							      daDirection accessDirection));
EXTERN daPortInstImpId	idaCreatePortInstImp		ARGS((daPortInstId portInstId, daPortImpId portImpId));
EXTERN daPortInstId	idaCreatePortInst		ARGS((daAnyInstId anyInstId, daPortId portId,
							      String portName));
EXTERN daPortInstBundleId idaCreatePortInstBundle	ARGS((daAnyInstId anyInstId, daPortBundleId portBundleId));
EXTERN daRipperId	idaCreateRipper			ARGS((daViewId viewId, String ripperName,
							      daPointArray *origin, daOrient orientation,
							      double magnification, daAnyNetId anyNetId1,
							      daAnyNetId anyNetId2, String map1, String map2));
EXTERN daPropId		idaCreateStringProp		ARGS((daId anyId, String propName, String propValue));
EXTERN daPropId		idaCreateTimeProp		ARGS((daId anyId, String propName, long propValue));
EXTERN daViewId		idaCreateView			ARGS((daCellId cellId, daViewTypeId viewTypeId,
							      daViewPurpose viewPurpose));

EXTERN String    	idaGetViewMode 			ARGS((daViewId viewId));

EXTERN Boolean   	idaReOpenView 			ARGS((daViewId viewId, String pcNewMode));

EXTERN daViewTypeId	idaCreateViewType		ARGS((daLibId libId, String viewTypeName, String use));
EXTERN daPaletteId	idaCreatePaletteByAttributeName	ARGS((daFigGroupId figGroupId, daDisplayId displayId,
							      String boldColor, String fillColor,
							      String outlineColor, String fillPattern,
							      String lineStyle));
EXTERN daPaletteId	idaCreatePaletteByAttributeNumber ARGS((daFigGroupId figGroupId, daDisplayId displayId,
							       int boldColor, int fillColor, int outlineColor,
							       int fillPattern, int lineStyle));
EXTERN void		idaDeleteObject			ARGS((daId anyId));
EXTERN void		idaDetachAnyNetAnyPort		ARGS((daAnyNetId anyNetId, daAnyPortId anyPortId));
EXTERN void		idaDetachAnyNetAnyPortInst	ARGS((daAnyNetId anyNetId, daAnyPortInstId anyPortInstId));
EXTERN void		idaInitPI			ARGS((String programName));
EXTERN void		idaInitPIIL			ARGS((void));
EXTERN Boolean		idaIsObjectIdValid		ARGS((daId anyId));
EXTERN Boolean		idaIsObjectIdNull		ARGS((daId anyId));
EXTERN void		idaMoveGroupElement		ARGS((daElementId elementId, daElementId afterElementId));
EXTERN daLibId		idaOpenLib			ARGS((String libName, String path, String configName,
							      String mode));
EXTERN String      	idaGetLibMode 			ARGS((daLibId libId));

EXTERN void		idaPurgeObject			ARGS((daId anyId));
EXTERN void		idaSaveObject			ARGS((daId anyId));
EXTERN daViewId		idaCheckInView			ARGS((daViewId viewToBeCheckedInId));
EXTERN void		idaTerminatePI			ARGS((void));
EXTERN daAnnotateId	idaCreateAnnotate();
/* ### andrewt 2-12-96 
EXTERN daAnnotateId	idaCreateAnnotate		ARGS((daViewId viewId, daFigGroupId figGroupId,
							      daPointArray *origin, String text,
							      daJustify justify, daOrient orient,
							      daFontStyle font, int size, Boolean drafting,
							      Boolean overbar, daAnnotateType type));
*/
EXTERN daCurveId	idaCreateCurve			ARGS((daViewId viewId, daFigGroupId figGroupId,
							      daPointArray *pointArray));
EXTERN daDonutId	idaCreateDonut			ARGS((daViewId viewId, daFigGroupId figGroupId,
							      daPointArray *origin, daDistance innerRadius,
							      daDistance outerRadius));
EXTERN daEllipseId	idaCreateEllipse			ARGS((daViewId viewId, daFigGroupId figGroupId,
							      daPointArray *pointArray));
EXTERN daPathId		idaCreatePath			ARGS((daViewId viewId, daFigGroupId figGroupId,
							      daPointArray *pointArray, unsigned int width,
							      daPathStyle style));
EXTERN daPolygonId	idaCreatePolygon			ARGS((daViewId viewId, daFigGroupId figGroupId,
							      daPointArray *pointArray));
EXTERN daRectangleId	idaCreateRectangle		ARGS((daViewId viewId, daFigGroupId figGroupId,
							      daPointArray *pointArray));
EXTERN void		idaSetAnnotateJustify		ARGS((daAnnotateId annotateId, daJustify justify));
EXTERN void		idaSetAnnotateOrient		ARGS((daAnnotateId annotateId, daOrient orient));
EXTERN void		idaSetAnnotateFontStyle		ARGS((daAnnotateId annotateId, daFontStyle fontStyle));
EXTERN void		idaSetAnnotateFontSize		ARGS((daAnnotateId annotateId, int fontSize));
EXTERN void		idaSetAnnotateDrafting		ARGS((daAnnotateId annotateId, Boolean drafting));
EXTERN void		idaSetAnnotateOverbar		ARGS((daAnnotateId annotateId, Boolean overbar));
EXTERN void		idaSetAnnotateTextType		ARGS((daAnnotateId annotateId, daAnnotateType type));
EXTERN void		idaSetAnyInstOrient		ARGS((daAnyInstId anyInstId, daOrient orient));
EXTERN void		idaSetAnyInstMagnification	ARGS((daAnyInstId anyInstId, double scale));
EXTERN void		idaSetAnyPortDirection		ARGS((daAnyPortId portId, daPortDirection direction));
EXTERN void		idaSetDonutInnerRadius		ARGS((daDonutId donutId, int radius));
EXTERN void		idaSetDonutOuterRadius		ARGS((daDonutId donutId, int radius));
EXTERN void		idaSetFigGroupNameNumber		ARGS((daFigGroupId figGroupId, String name, int number));
EXTERN void		idaSetFigGroupPurposeNameNumber	ARGS((daFigGroupId figGroupId, String name, int number));
EXTERN void		idaSetFigGroupPriority		ARGS((daFigGroupId figGroupId, int priority));
EXTERN void		idaSetFigGroupBlink		ARGS((daFigGroupId figGroupId, Boolean blink));
EXTERN void		idaSetFigGroupSelect		ARGS((daFigGroupId figGroupId, Boolean selectable));
EXTERN void		idaSetFigGroupDrag		ARGS((daFigGroupId figGroupId, Boolean drag));
EXTERN void		idaAttachFigObject		ARGS((daFigId figureId, daId anyId));
EXTERN void		idaDetachFigObject		ARGS((daFigId figureId));
EXTERN void		idaSetAnnotateText		ARGS((daFigId annotateId, String text));
EXTERN void		idaSetGroupUniqueRef		ARGS((daGroupId groupId, Boolean uniqueRef));
EXTERN void		idaSetGroupOrdered		ARGS((daGroupId groupId, Boolean ordered));
EXTERN void		idaSetGroupUniqueName		ARGS((daGroupId groupId, Boolean uniqueName));
EXTERN void		idaSetLibReferenceLibs		ARGS((daLibId libId, String referenceLibs));
EXTERN void		idaSetPathWidth			ARGS((daPathId pathId, int width));
EXTERN void		idaSetPathBeginExt		ARGS((daPathId pathId, int ext));
EXTERN void		idaSetPathEndExt			ARGS((daPathId pathId, int ext));
EXTERN void		idaSetPathStyle			ARGS((daPathId pathId, int pathStyle));
EXTERN void		idaSetPortImpAccessDirection	ARGS((daPortImpId portImpId, daDirection accessDirection));
EXTERN void		idaSetPropFloatValue();
/* ### andrewt 2-13-96
EXTERN void		idaSetPropFloatValue		ARGS((daPropId propId, float floatValue));
*/
EXTERN void		idaSetPropIntegerValue		ARGS((daPropId propId, int intValue));
EXTERN void		idaSetPropStringValue		ARGS((daPropId propId, String stringValue));
EXTERN void		idaSetPropBooleanValue		ARGS((daPropId propId, Boolean truthValue));
EXTERN void		idaSetPropTimeValue		ARGS((daPropId propId, long timeValue));
EXTERN void		idaSetPropFileNameValue		ARGS((daPropId propId, String fileName));
EXTERN void		idaSetPropILListValue		ARGS((daPropId propId, String ILList));
EXTERN void		idaSetPropILExprValue		ARGS((daPropId propId, String ILExpr));
EXTERN void		idaSetPropNlpExprValue		ARGS((daPropId propId, String NlpValue));
EXTERN void 		idaSetPILibSearchPath		ARGS((String searchPath));
EXTERN void		idaSetViewPurpose		ARGS((daViewId viewId, daViewPurpose purpose));
EXTERN void		idaSetViewTypeUse		ARGS((daViewTypeId viewTypeId, String use));
EXTERN void		idaSetDisplayColorByName		ARGS((daDisplayId displayId, daColor *color));
EXTERN void		idaSetDisplayFillPatternByName	ARGS((daDisplayId displayId, daFillPattern *fill));
EXTERN void		idaSetDisplayLineStyleByName	ARGS((daDisplayId displayId, daLineStyle *lineStyle));
EXTERN void		idaSetPaletteFillColorByName	ARGS((daPaletteId paletteId, String name));
EXTERN void		idaSetPaletteOutlineColorByName	ARGS((daPaletteId paletteId, String name));
EXTERN void		idaSetPaletteBoldColorByName	ARGS((daPaletteId paletteId, String name));
EXTERN void		idaSetPaletteFillPatternByName	ARGS((daPaletteId paletteId, String name));
EXTERN void		idaSetPaletteLineStyleByName	ARGS((daPaletteId paletteId, String name));
EXTERN void		idaSetDisplayColorByNumber	ARGS((daDisplayId displayId, daColor *color));
EXTERN void		idaSetDisplayFillPatternByNumber	ARGS((daDisplayId displayId, daFillPattern *fill));
EXTERN void		idaSetDisplayLineStyleByNumber	ARGS((daDisplayId displayId, daLineStyle *style));
EXTERN void		idaSetPaletteFillColorByNumber	ARGS((daPaletteId paletteId, int number));
EXTERN void		idaSetPaletteOutlineColorByNumber ARGS((daPaletteId paletteId, int number));
EXTERN void		idaSetPaletteBoldColorByNumber	ARGS((daPaletteId paletteId, int number));
EXTERN void		idaSetPaletteFillPatternByNumber	ARGS((daPaletteId paletteId, int number));
EXTERN void		idaSetPaletteLineStyleByNumber	ARGS((daPaletteId paletteId, int number));
EXTERN void		idaSetObjectName			ARGS((daId anyId, String name));
EXTERN void		idaSetFigPointArray		ARGS((daFigId FigId, daPointArray pointArray));
EXTERN Boolean          idaGetFigGroupVisible            ARGS((daFigGroupId figGroupId));
EXTERN void             idaSetFigGroupVisible            ARGS((daFigGroupId figGroupId, Boolean
 visible));
/* ### andrewt 2-12-96  
EXTERN daPortInstBundleIid idaGetAnyPortInstPortInstBundle ARGS((daAnyPortInstId anyPortInstId));
*/
