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
# $Id: hi.h,v 1.1 1997/04/07 18:10:52 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef CDS_HEADER_INFO /*startCDS_INCLUDE_HEADERstart*/
#ifdef P_hi
    crash if included 2+ times
#endif /* P_hi */
#define P_hi
/****************************************************************/
/* Copyright (C) 1983, 1984, 1985, 1986, 1987, 1988, 1989, 1990	*/
/*		 1991						*/
/*	Cadence Design Systems Inc.  All Rights Reserved.	*/
/****************************************************************/
#endif /*CDS_HEADER_INFO endCDS_INCLUDE_HEADERend*/

/*
 * This file defines the public interface to hi code.
 */


#ifndef db_P
/*
 * NOTE:  The following are definitions taken from db.h.  They are
 * included here because hi.h has dependencies on db.h.  In order
 * to "remove" these dependencies, they have been included here.
 */

typedef int	dbCoord;
typedef unsigned int    dbDistance;

struct dbPoint {  /* point is a structure instead of a 2-element array */
    dbCoord    x;
    dbCoord    y;
};
typedef struct dbPoint dbPoint;

struct dbBBox {
    dbPoint	lowerLeft;
    dbPoint     upperRight;
};
typedef struct dbBBox dbBBox;

typedef double dbUserCoord;

struct dbUserPoint {
    dbUserCoord x, y;
};

typedef struct dbUserPoint dbUserPoint;

typedef int     dbDisplayId;
typedef int	dbLibId;

typedef struct dbColor dbColor;
typedef struct dbColor *dbColorId;
struct dbColor {
    unsigned long index;
    unsigned short red, green, blue;
    String name;
};

typedef int	dbLayerId;
enum dbLayerFillStyle {		/* object FillStyle */ /* prefix Layer */ /* suffix Fill */
    dbcLayerUnknownFill		= 0, /* alias layerFillUnknown */
    dbcLayerOutlineFill		= 1, /* alias layerOutlineNoFill */
    dbcLayerSolidFill		= 2, /* alias layerSolidFill */
    dbcLayerXFill		= 3, /* alias xFill */ /* alias layerXFill */
    dbcLayerStippleFill		= 4, /* alias layerStippleFill */
    dbcLayerOutlineStippleFill	= 5  /* alias layerOutlineStippleFill */
};
#define	dbcMaxLayerFillStyle	5
typedef enum dbLayerFillStyle	dbLayerFillStyle;

typedef unsigned char dbLayer;
typedef unsigned char dbPurpose;

/* New line style structure -- will replace dbLayerLineStyle */
typedef struct dbLineStyle dbLineStyle;
struct dbLineStyle {
    String        name;      /* line style name */
    unsigned int  index;     /* line style index */
    unsigned int  length;    /* line style pattern length in pixels */
    unsigned int  width;     /* line width in pixels */
    unsigned char *pattern;  /* line style pattern */
};

/* New stipple structure -- will replace dbLayerStipple */
typedef struct dbStipple dbStipple;
struct dbStipple {
    String        name;      /* stipple name */
    unsigned int  index;     /* stipple index */
    unsigned int  width;     /* stipple pattern width in pixels */
    unsigned int  height;    /* stipple pattern height in pixels */
    unsigned char *pattern;  /* stipple pattern */
};

typedef struct dbiGenState dbiGenState;
typedef dbiGenState 	  *dbGenStatePtr;
typedef dbiGenState 	  *dbGenStateId;

typedef Boolean  	(*FuncBooleanPtr)();
typedef Boolean         (*PntFuncBoolean)();
typedef dbPoint         (*PntFuncPoint)();

enum dbPathStyle {
    dbcUnknownPathStyle = 0,
    dbcTruncateExtend   = 1,    /* alias squareFlush */
    dbcExtendExtend     = 2,    /* alias squareOffset */
    dbcRoundRound       = 3,    /* alias octagonEnded */
    dbcVarExtendExtend  = 4
};
#define dbcMaxPathStyle  4
typedef enum dbPathStyle dbPathStyle;


#endif /* db_P */
 
/* analog to pixel index */
typedef unsigned long hiPixel;

/* User mode :
 *    hicExpert, hicTutorial
 */
typedef enum {
                hicTutorial = 1,
                hicExpert = 2
} hiUserMode;
 
extern hiUserMode hivUserMode;

/* Public Definitions for enter functions */
typedef int (*hiEnterFunction)();
/* take args...
 * hiEnterStack *stackPtr;
 * and depending on the functions
 * Widget pWidget;
 * XEvent *event;
 * or
 * dbPoint (for addpoint)
 * or nothing .. for delpoint .
 */

/* the greatest number of digitized points */
#define MAXENTERPOINTS 1024
/* the greatest number of nested enter function levels */

#define MAXNESTEDLEVELS 64

#define SUPERSTACKINITIALSIZE 128 

typedef struct _hiRepEdit hiRepEdit,*hiRepEditPtr ;

typedef struct _hiEnterStack hiEnterStack;

typedef enum {
		hicUnknownShell = 0,
		hicTopLevelShell = 1,
		hicEnterFunShell = 2,
		hicFormShell = 3
} hiShellType;

typedef enum {
		hicUnknownEvent = 0,
		hicMotionEvent = 1,
		hicButtonEvent = 2,
	 	hicKbdEvent = 3,
		hicDoneEvent = 4
} hiEventType;

typedef enum {
		hicUnknownFun = 0,
		hicEnterLineFun = 1,
		hicEnterPointFun = 2,
		hicEnterBoxFun = 3,
		hicEnterPolygonFun = 4,
		hicEnterPathFun = 5,
		hicEnterCircleFun = 6,
		hicEnterEllipseFun = 7,
		hicEnterArcFun = 8,
		hicEnterSegmentFun = 9,
		hicCursorTrackFun = 10,
		hicEnterPoints = 11,
		hicEnterDonutFun = 12,
		hicEnterMultiRepFun = 13,
		hicEnterStringFun = 14,
		hicEnterNumberFun = 15
} hiEnterFunctionType;

typedef enum {
		hicNormalMode = 1,
		hicCursorMode = 2,
		hicHiLightMode = 3,
		hicClearPlaneMode = 4,
		hicSelectionMode = 5,
		hicClearSelectMode = 6
} hiDrawModeType;

/* end enter functions */

enum hiWinWidgetType {
    hicErrorWinWidget 		= 0,		/* error */
    hicNoWinWidget 		= 1,		/* for plot windows */
    hicGraphicsWinWidget	= 2, 
    hicTextWinWidget		= 3, 
    hicCIWinWidget		= 4, 
    hicBrowserWinWidget		= 5, 
    hicEncapWinWidget		= 6,
    hicNullWinWidget		= 7,		/* for pixmaps */
    hicFormWinWidget		= 8		
};
typedef enum hiWinWidgetType hiWinWidgetType;

typedef struct _hiWindow *hiWindowId;

#define hiGetWindowId(win) \
    (hiWindowId)hiGetWindowStruct(win);

typedef struct {
   int      x,y ;
   Boolean  tempPoint;
} efPoint;

#define hicMaxRGB 1000

/*JM*/
/* enterfunctions call adjust point function
 * to filter the raw points, as entered by the
 * user.  These flags are used to tell the enter
 * functions what the adjust procedure did
 */
#define hicAcceptPoint       (1<<0)
#define hicRejectPoint       (1<<1)
#define hicModifyPoint       (1<<2)
#define hicPointListAdjusted (1<<3)
#define hicAdjustFailure     (1<<4)

typedef void        (*PntFuncVoid)(); /* pointer to void function */
typedef double      (*PntFuncDouble)() ; /* accepts dbu, returns float */
typedef dbUserPoint (*PntFuncUPoint)(); /* takes a point gives a point */
typedef dbBBox      (*PntFuncBBox)()   ; /* returns a Bounding box */
typedef hiWindowId  (*hiWindowProc)()  ; /* accepts windowId, returns windowId*/
typedef unsigned int (*hiAdjustProc)() ; /* adjust function */
typedef RefAny	    (*hiCursorProc)()  ; /* args to be determined */
typedef RefAny	    (*hiDragProc)()    ; /* args to be determined */
typedef dbPoint     (*PntFuncDbPoint)(); /* takes a point, gives a point */
typedef String      (*hiBannerProc)()  ; /* returns a String */
typedef String      (*PntFuncString)() ; /* returns a String */

typedef unsigned long hiInputId        ; /* Id for toolkit input source */
typedef unsigned long hiTimerId        ; /* Id for toolkit timer        */

#define hicNoMask      0L   
#define hicReadMask   (1L<<0)
#define hicWriteMask  (1L<<1)
#define hicExceptMask (1L<<2)

typedef unsigned long hiInputMask;

typedef struct {
   PntFuncBoolean rectangle;
   PntFuncBoolean polygon;
   PntFuncBoolean pixelArray;
   PntFuncBoolean line;
   PntFuncBoolean segment;
   PntFuncBoolean arc;
   PntFuncBoolean circle;
   PntFuncBoolean donut;
   PntFuncBoolean ellipse;
   PntFuncPoint   rasterChar;
   PntFuncPoint   rasterLabel;
   PntFuncBoolean flush;
   PntFuncBoolean clear;
   PntFuncBoolean init;
   PntFuncBoolean nextPage;
   PntFuncBoolean setPen;
   PntFuncBoolean clearBox;
   PntFuncBoolean copyBox;
   PntFuncBoolean setClip;
   PntFuncBoolean startPage;
   PntFuncPoint   strokeAnyChar;
   PntFuncPoint   strokeAnyLabel;
   PntFuncPoint   strokeChar;
   PntFuncPoint   strokeLabel;
   PntFuncBoolean marker;
} hiWindowFuncTable;

typedef struct {
	hiWindowProc     equivWinProc  ;
	hiCursorProc     cursorProc    ;
	PntFuncBoolean   freeProc      ;
	PntFuncBoolean   displayProc   ;
	PntFuncDbPoint   transformProc ;
	hiAdjustProc     adjustProc    ;
	PntFuncUPoint    dbuToUPProc   ;
	PntFuncBoolean   zoomPanProc   ;
	PntFuncDbPoint   uPointToDbuProc;

	PntFuncDbPoint   dbuToScrnProc ;
	PntFuncBoolean   winPutQProc   ;
	PntFuncBoolean	 winGetQProc   ;
	PntFuncBoolean   winSetInfoProc;
	PntFuncBoolean	 winGetInfoProc;
	PntFuncBoolean	 closeProc;
	PntFuncVoid	 resizeProc;
} hiWindowInterfaceValues     ;


typedef struct {
	RefAny		formWidget   ; /* The form to map         */
	char 	      **promptStrings; /* array of prompt strings */
	int		numPrompts   ; /* number of prompt strings */
	dbPoint		relativePoint; /* x,y of start position */
	dbPathStyle	pathStyle    ; /* style of path */
	dbDistance	pathWidth    ; /* width of a path */
	int		pathBeginExt ; /* beginning extension for path*/
	int		pathEndExt   ; /* end extension for path */
	Boolean		alwaysDrag   ; /* should we always drag (even if no first point) */
	Boolean	 	drawOrUndraw ; /* are we drawing or undrawing ?
				        * Drawing   is TRUE 
					* Undrawing is FALSE
					*/	
	Boolean		firstWindow  ; /* is it the first window in 
					 * a window list ? 
					 */
	hiEnterFunction initProc     ; /* initialization procedure */
	hiEnterFunction addPointProc ; /* add point procedure */
	hiEnterFunction	delPointProc ; /* del point procedure */
	hiEnterFunction dragProc     ; /* drag procedure */
	hiEnterFunction doneProc     ; /* "done" procedure */
	hiEnterFunction pushProc     ;
	hiEnterFunction popProc      ;
	hiEnterFunction cleanProc    ; /* called before every action */
	voidStar 	clientData   ; /* client data */
	char *ilInitProc,*ilDoneProc,*ilAddPointProc,
	     *ilDelPointProc,*ilFormProc ; /* string valued names of 
					    * the il functions that
					    * should be called
					    */
	int	 	wantPoints   ; /* number of Points the user wants */
	int		numPoints    ; /* number of points .. 
				         * preloaded & returned 
					 */
	Boolean		dontDraw     ; /* when set TRUE, the enterfunction
					 * will not do graphics .. but
					 * will call the drags
					 */
	Boolean         alwaysMap    ; /* always map the form */
	Boolean		dontCheckColinear; /* dont check for colinear
					     * points.
					     */
	Boolean		acceptString ; /* can we accept a string instead ? */
	Boolean		acceptNumber ; /* can we accept a number instead ? */
	Boolean		dontFinishOnStringOrNum ; 
			    	       /* if you get a string
					* or number .. don't finish the
					* enterfunction .. just call the
					* done proc and reset ..
					*/
	String		stringResult ; /* result of enter string */
	double		numberResult ; /* result of enter number */
	hiEnterFunctionType efunType ; /* type of enterfunction  */
	dbPoint		pointArray[MAXENTERPOINTS]; /* point buffer */
	Boolean		freePrompts  ; /* a flag to denote whether we should
					* free the prompts or not .. this is
					* private to HI
					*/
	Boolean		noInfix	     ; /* if infix is on .. don't take
					* the infix point for this
					* enterfunction
					*/
	String		cmdName	     ; /* command name for enterfunction */
	PntFuncVoid	mousePromptProc ; /* mouse prompt callback */
	unsigned int	selectOptions;    /* option flags used for selection */
 } hiEFunStruct;

typedef   	dbDisplayId	 (*PntFuncDisplayId)() ;
typedef  	dbLibId		 (*PntFuncLibId)() ;
typedef		dbColor		*(*PntFuncColor)() ;
typedef		dbLayerId	 (*PntFuncLayerId)();
typedef 	dbLayerFillStyle (*PntFuncFillStyle)() ;	
typedef		dbLayer		 (*PntFuncLayer)() ;
typedef		dbPurpose	 (*PntFuncPurpose)() ;
typedef		dbLineStyle	*(*PntFuncLineStyle)() ;
typedef		dbStipple	*(*PntFuncStipple)() ;
typedef		dbGenStateId     (*PntFuncGenState)() ;
typedef		dbPoint         *(*PntFuncPoints)() ;

#define HIDBFUNCS
/*
 * this table of function pointers exists so that hi can
 * call db functions if they are defined .. the graphics
 * editor calls a function that then registers these.
 */
typedef struct _hiDbFuncTable {
  /* dm lock and unlock functions */
  PntFuncInt   		lockFile ;
  PntFuncBoolean 	unlockFile ;
  /* dbtech trigger routines */
  PntFuncBoolean	regColorModTrigger ;
  PntFuncBoolean	regLayerModTrigger ;
  PntFuncBoolean        regLineStyleModTrigger ;
  PntFuncBoolean	regOpenTechTrigger ;
  PntFuncBoolean	regStippleModTrigger ;
  /* dbtech attribute routines */
  PntFuncDisplayId      getDisplayId ;
  PntFuncLibId		getLayerLibId;
  PntFuncColor		getColor ;
  PntFuncColor		getColorByName;
  PntFuncLayerId        getLayer ;
  PntFuncBoolean	getLayerBlink ;
  PntFuncInt	 	getLayerFillColor ;
  PntFuncFillStyle      getLayerFillStyle;	
  PntFuncInt	        getLayerLineStyle ;
  PntFuncLayer          getLayerNumById ;
  PntFuncInt		getLayerOutlineColor ;
  PntFuncPurpose        getLayerPurpose;
  PntFuncInt		getLayerStipple ;
  PntFuncLineStyle      getLineStyle;
  PntFuncStipple 	getStipple;
  PntFuncBoolean	techContainsLP ;
  PntFuncVoid		stopGen ;
  PntFuncBoolean	genLayerId ;
  PntFuncInt		getLayerBrightColor ;
  PntFuncString		getLayerName ;
  PntFuncInt		getLayerPriority ;
  PntFuncBoolean	getLayerSelectable ;
  PntFuncBoolean	getLayerVisible ;
  PntFuncLineStyle      getLineStyleByName;
  PntFuncStipple        getStippleByName ;
  PntFuncString	        libPurposeToString ;
  PntFuncBoolean	makeColor ;
  PntFuncBoolean	setLayerBrightColor ;
  PntFuncBoolean	setLayerFillColor ;
  PntFuncBoolean	setLayerLineStyle ;
  PntFuncBoolean	setLayerOutlineColor ;
  PntFuncBoolean	setLayerStipple ;
  PntFuncGenState       startGenLayerId;
  PntFuncBoolean	listToLP ;
  PntFuncVoid		fixBBox ;
  PntFuncBoolean	computeLabelBBox ;
  PntFuncLayer		getLayerNum;
  PntFuncPurpose	stringToLibPurpose;
  PntFuncPoints         pathRecToPolyRec;
  PntFuncBoolean	setCurrentFont ;
  PntFuncInt		getFontHeight ;
  PntFuncInt		getStringWidth ;
  PntFuncInt		getStrokes ;
  PntFuncInt		getCharWidth ;
  PntFuncVoid		getArcAngle ;
  PntFuncInt		calculateArc ;
  PntFuncString	        getDisplayName ;
} hiDbFuncTable;

extern hiDbFuncTable hivDbFuncTable;

extern Boolean hiSetDbFuncTable() ;

/*
 * hiDeskId
 */
typedef int hiDeskId;
#define hicNullDeskId	NULL
extern void		hiFreeState();

#define hiStopGen(pState) {	\
    hiFreeState(pState); 	\
    pState = NULL;		\
}
typedef struct hiGenState hiGenState;
typedef hiGenState	*hiGenStatePtr;

/* type of the selection triggers for the showFile. */
typedef enum hiSelectAction {
    hicSelectText,
    hicDeselectText,
    hicSetCursorPos,
} hiSelectAction;
/* type of the selection token for the showFile. */
typedef enum hiSelectToken {
    hicSelectDrag,
    hicSelectWord,
    hicSelectLine,
    hicSelectAll,
} hiSelectToken;
/* positioning enums for the showfile */
typedef enum hiTextPosition {
    hicCurrentPosition,
    hicEndOfText,
    hicStartOfText,
} hiTextPosition;

/*
 * Window public externals
 */
extern Boolean 		hiChangeBannerLabel();
extern hiGenStatePtr	hiStartGenDeskToWindow();
extern Boolean		hiGenDeskToWindow();
extern unsigned int	hiGetWindowNum();
extern hiWindowId 	hiNumToWindowId();
extern Boolean 		hiSetCurrentWindow();
#ifdef il_P
extern list 		hiGetWindowAtom();
#endif /* il_P */
extern hiWindowId 	hiGetCurrentWindow();
extern hiWindowId	hiGetWindowCursorIsIn();
extern hiWinWidgetType 	hiGetWidgetType();
extern Boolean		hiSwitchWindowType();
extern String 		hiGetAppType();
extern Boolean		hiSetAppType();
extern RefAny 		hiGetWindowInfo();
extern Boolean 		hiSetWindowInfo();
extern RefAny 		hiGetProbeInfo();
extern Boolean 		hiSetProbeInfo();
extern Boolean		hiGetWindowInterface();
extern Boolean		hiSetWindowInterface();
extern Boolean		hiCopyWindowInterface();
extern double		hiGetDBToScreen();
extern Boolean 		hiSetDBToScreen();
extern Boolean 		hiUnmapWindow();
extern Boolean 		hiCloseWindow();
extern Boolean 		hiDisplayWindow();
extern Boolean 		hiGetWindowRefresh();
extern Boolean 		hiGetWindowScreenBBox();
extern Boolean 		hiGetRelWindowScreenBBox();
extern Boolean 		hiGetWindowFullBBox();
extern Boolean 		hiSetWindowFullBBox();
extern Boolean 		hiGetWindowDBBBox();
extern Boolean 		hiSetWindowDBBBox();
extern Boolean		hiCreateStatusBanner();
extern Boolean		hiDeleteStatusBanner();
extern hiWindowId 	hiCreateWindow();
extern Boolean 		hiValidateWidget();
extern unsigned int	hiValidateWindowId();
extern Boolean		hiSetWindowDrawMode();
extern hiDrawModeType   hiGetWindowDrawMode();
extern Boolean		hiSetWindowName();
extern Boolean		hiRegWindowFuncTable();
extern String  		hiGetWindowDisplayName();
extern Boolean		hiSetWindowDisplayName();
extern Boolean		hiAppendWindowPropList();
extern Boolean 		hiMergeWindowPropList();
extern hiWindowId	hiWTypeToWindow();
#ifdef il_P
extern list 		hiGetWindowPropList();
#endif /* il_P */
extern dbBBox		hiGetWindowInDBUnits();
extern Boolean		hiIsWindowVisible();
extern hiWindowId	hiGetCIWindowId();

/*
 * Event Management public externals
 */
extern Boolean		hiShowBindKeysByAppType();
extern Boolean		hiShowBindKeysByWindow();
extern Boolean		hiGetCommandPoint();
extern Boolean		hiGetPoint();
extern Boolean		hiGetDbuPoint();
extern Boolean		hiGetScreenPoint();
extern Boolean		hiRegWindowWithFilePtr();
/*
 * CIW public externals
 */
extern unsigned int	hivScreenWidth;
extern unsigned int	hivScreenHeight;

extern void		hiRegAppName();
extern Boolean		hiAppendInputCmd();
extern void		hiGetAttention();
extern void		hiProcessExposures();
extern Boolean 		hiAreMoreExposuresComing();
extern Boolean 		hiMorePointsAreComing();
extern Boolean		hiGetFontList();
extern String		hiGetEncapSkillCmd();
extern Boolean		hiGetUserAbort();
#ifdef il_P
extern list		hiInputCmd();
#endif /* il_P */
extern Boolean		hiIsInReplay();
extern Boolean		hiReplayFile();
extern Boolean		hiStartLog();
extern Boolean		hiEndLog();
extern Boolean		hiRepeat();
extern String		hiGetBindKey();
extern Boolean		hiSetBindKey();
extern Boolean		hiSetEncapHistory();
extern Boolean		hiSetEncapSkillCmd();
extern Boolean		hiSetFont();
extern Boolean		hiSetFilter();
extern Boolean		hiSetWindowPrompt();
extern void		hiSync();
extern void		hiSynchronize();  /* for compatability */
extern void		hiMainLoop();
/*
 * view file public externals
 */
extern Boolean 		hiDisableTailViewfile();
extern Boolean          hiTextDisplayStringScrollflag();
#define hiTextDisplayString(windowId, text, erase) \
        hiTextDisplayStringScrollflag(windowId, text, erase, FALSE)
extern Boolean 		hiEnableTailViewfile();
extern int 		hiGetTextClass();
extern int		hiGenTextIndex();
#ifdef il_P
extern list		hiGetTextSelection();
#endif /* il_P */
extern int		hiGetCurrentIndex();
extern Boolean		hiReadInViewfile();
extern Boolean		hiSaveViewfile();
extern Boolean		hiScrollWindowToIndex();
extern Boolean		hiScrollWindowToCurrentIndex();
extern Boolean		hiScrollWindowTop();
extern Boolean		hiScrollWindowBottom();
extern Boolean		hiScrollWindowUp();
extern Boolean		hiScrollWindowDown();
extern Boolean		hiScrollWindowLeft();
extern Boolean		hiScrollWindowRight();
extern Boolean		hiSetCurrentIndex();
extern Boolean 		hiSetTextClass();
extern Boolean		hiSetTextSelection();
extern Boolean		hiSetTextSelectAll();
extern Boolean		hiSelectTextByLoc();
extern Boolean		hiSelectTextFromTrigger();
extern int		hiStartGenTextIndex();
extern Boolean		hiUnselectText();
extern Boolean		hiUnselectTextAllScrollflag();
#define hiUnselectTextAll(windowId, text, class) \
	hiUnselectTextAllScrollflag(windowId, text, class, hicCurrentPosition)
extern Boolean 		hiUnselectTextClass();
extern Boolean		hiUnselectTextByLoc();
extern Boolean		hiUnselectTextFromTrigger();
extern Boolean 		hiSetTextHighlightColor();
extern Boolean		hiUpdateTextSelectionColors();
extern Boolean		hiRefreshTextWindow();
#ifdef il_P
extern list		hiGetTextWordDelimiter();
#endif /* il_P */
extern Boolean		hiAddTextWordDelimiter();
extern Boolean		hiRemoveTextWordDelimiter();
extern Boolean		hiReplaceTextWordDelimiter();
#ifdef il_P
extern list		hiGetTextSelByLoc();
#endif /* il_P */
extern Boolean		hiGetTextDispLoc();
extern char		hiGetTextCharAtLoc();
extern int		hiGetTextSourceLength();
extern int		hiGetTextIndexLoc();
extern Boolean		hiGetTextLineColumn();

extern Boolean 		hiSetFormValue();
extern Boolean 		hiGetFormValue();
/*
 * hi colors public externals
 */
extern void		hiHLSToRGB();
extern Boolean		hiGetLayerColor();
extern Boolean		hiGetHiColor();
extern Boolean		hiPresetLayerColor();
extern Boolean		hiSetHiColor();
extern Boolean		hiSetHiColorOwner();
extern Boolean		hiSetLayerColor();
extern hiPixel		hiMatchColor();
extern hiPixel		hiMatchExactColor();
extern hiPixel		hiGetColorByOwner();
extern hiPixel 		hiGetFreeLayerColor();
extern Boolean		hiNameToRGB();
extern void		hiRGBToHLS();
extern void		hiSetColorFlash();
extern String		hiGetCurrentDisplayName();
/*
 * display list public externals
 */
extern int		dlAddBox();
extern int		dlAddPolygon();
extern int		dlAddLine();
extern int		dlAddPath();
extern int		dlAddArc();
extern int		dlAddArc();
extern int		dlAddDonut();
extern int		dlAddEventObject();
extern int		dlAddStrokeText();
extern Boolean		dlAlwaysDraw();
extern Boolean		dlBindEvent();
extern void     	dlButtonRefresh();
extern void		dlClearDlist();
extern unsigned long	dlDlistToIcon();
extern Boolean		dlEnableItem();
extern Boolean		dlFitDlistOnDraw();
extern Boolean		dlPopTransform() ;
extern Boolean		dlPushTransform() ;
extern Boolean 		dlRefreshDlist();
extern void     	dlRefreshWidget();
extern Boolean		dlScaleIndependently();
extern Boolean		dlSetClearOnDraw();
extern Boolean		dlSetDlistPosition();
extern Boolean		dlSetDlistScale();
extern Boolean		dlSetGadgetDimensions();
extern int		dlSetPenColor();
extern void		dlSetPenFillStyle();
extern void		dlSetPenFilledSolid();
extern void		dlSetPenLineStyle();
extern void		dlSetPenOutlineStippleFilled();
extern void		dlSetPenStipple();
extern void		dlSetPenStippleFilled();
extern void		dlSetPenTable();
extern void		dlSetPenToLayerAttributes();
extern int		dlTransformX();
extern int		dlTransformY();
extern int		dlTransformScalar();
extern double 		dlTransformScale();
extern void		dlWidgetDestroyed();

/* Timer and input functions */

/* timers */

extern hiTimerId hiRegTimer();
extern void      hiUnregTimer();

/* enterfunctions */
extern Boolean hiAddPoint();
extern Boolean hiDeletePoint();

extern Boolean hiUndrawEnterFunction();
extern Boolean hiDrawEnterFunction();

extern Boolean hiClearAllEnterFunctions();
extern Boolean hiCancelEnterFunction();
extern Boolean hiFinishEnterFunction();
extern Boolean hiApplyEnterFunction();

extern Boolean hiChangeEnterFun();
extern Boolean hiModifyEnterFun();
extern Boolean hiModifyPointList();

extern void    hiInitEFunStruct() ;
extern Boolean hiEnterLine();
extern Boolean hiEnterPoint();
extern Boolean hiEnterBox();
extern Boolean hiEnterPolygon();
extern Boolean hiEnterPath();
extern Boolean hiEnterCircle();
extern Boolean hiEnterEllipse();
extern Boolean hiEnterArc();
extern Boolean hiEnterDonut();
extern Boolean hiEnterSegment();

extern Boolean hiEnterMultiRep();
extern Boolean hiEnterPoints();
extern Boolean hiEnterString();
extern Boolean hiEnterNumber();

extern void	hiEnterArcDraw();
extern void	hiEnterArcUnDraw();

extern Boolean hiEnterScreenBox();
extern void	hiSetEfunPrompt();
extern hiEFunStruct *hiGetWindowEFunStruct() ;

extern String	hiGetCommandName() ;

/* undo/redo support */
extern Boolean hiRedo() ;
extern Boolean hiUndo() ;
extern Boolean hiRequestCheckPoint() ;
extern Boolean hiSetUndoLimit() ;

#define HIREGUNDO
#ifdef HIREGUNDO
/* Callback registries */
extern Boolean hiRegDbEnableUndoProc() ;
extern Boolean hiRegDbQueryCkPointProc() ;
extern Boolean hiRegDbSetUndoProc() ;
extern Boolean hiRegDbUndoProc() ;
extern Boolean hiRegDbRedoProc() ;
extern Boolean hiRegDbVerifyUndoProc() ;
extern Boolean hiRegDbDisableUndoProc();
#endif


extern  Boolean hiRegWindowEnvTrigger();
extern  Boolean hiRegTextSelTrigger();
extern  Boolean hiRegCurWindowTrigger();

extern  Boolean hiUnregWindowEnvTrigger();
extern  Boolean hiUnregTextSelTrigger();
extern  Boolean hiUnregCurWindowTrigger();

extern  void    hiDefaultCursorMotion();


/* update procs */
extern void	hiRegUpdateProc() ;
extern void	hiUnregUpdateProc() ;
extern void	hiUpdate();

extern Boolean	hiCheck();

/* these masks are used to pass the state of
 * the modifier keys to an application
 */
#define hicShiftMask               (1<<0)
#define hicLockMask                (1<<1)
#define hicControlMask             (1<<2)
#define hicMod1Mask                (1<<3)
#define hicMod2Mask                (1<<4)
#define hicMod3Mask                (1<<5)
#define hicMod4Mask                (1<<6)
#define hicMod5Mask                (1<<7)

/*
 * Event Management externals
 */
extern void hiSetMultiClickTime();
extern int hiGetMultiClickTime();

extern void hiSetDrawThruDelta();
extern int hiGetDrawThruDelta();

/*
 * post command for later execution
 */
extern void hiEnqueueCmd();

/* 
 * predicate function to tell if we're in
 * reverse video mode or not.
 */
extern Boolean hiIsReverseVideo();

extern void hiSetMousePrompts( /* windowId, left, middle, right */ ) ;

/* new function to update the status banner */
extern void hiRefreshStatusBanner();

/*
 * Call this function before iliInitClients in main.c if you
 * don't want the CIW to be mapped:  hiPreventCIW()
 */
extern void hiPreventCIW();

/*
 * Redefine the following functions to the new HI functions dealing
 * with user types.  These were defined in IL.
 */
#define ilGetWindowStruct hiGetWindowStruct
#define ilGetWStruct hiGetWStruct
#define ilGetWType hiGetWType

extern int hiGetWindowStruct();
extern int hiGetWStruct();
extern int hiGetWType();
extern Boolean hiIsWType();

