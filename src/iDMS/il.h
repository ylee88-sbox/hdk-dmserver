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
# $Id: il.h,v 1.1 1997/04/05 02:01:25 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef CDS_HEADER_INFO /*startCDS_INCLUDE_HEADERstart*/
#ifdef il_P
    crash if included 2+ times
#endif /* il_P */
#define il_P
/****************************************************************/
/* Copyright (C) 1983, 1984, 1985, 1986, 1987, 1988, 1989, 1990	*/
/*	Cadence Design Systems Inc.  All Rights Reserved.	*/
/****************************************************************/
#endif /*CDS_HEADER_INFO endCDS_INCLUDE_HEADERend*/

#ifndef VOSTDIO
#include "voStdio.h"
#endif	/* VOSTDIO */

/********************* PORT-SPECIFIC PARAMETERS ********************/

/* All port-specific parameters removed.  With the demise of the
 * old Masscomps, optimization for 24 bit addresses no longer needed.
 */

/********************* CONFIGURATION PARAMETERS ********************/

/* These are configuration parameters that are used throughout IL.  Users
may want to use some of these constants in their own array declarations
to assure that there will be no size mismatches.  The values of these 
parameters may be changed without notice.
*/

#define ilcMaxIdent 256		/* max # of characters in a symbol name	*/
#define ilcMaxLine 8192		/* max length of an input line		*/
#define ilcMaxCommand 1024	/* max length of a unix command line	*/
#define ilcMaxArgs 4000000	/* max # of arguments a fn may accept	*/
#define ilcPrintBufSize 8188	/* size of output buffer for print fns	*/
/*
If a binary function has no upper limit on the number of arguments it can
accept, the upper limit field for the function should be set to maxargs.
Any function which has its upper limit field set to maxargs is assumed
to impose no upper limit on the no. of arguments it can accept.  As a
practical matter, however, we must set a maximum that would allow us to
detect circularity in argument lists within a reasonable amount of time.  
For now, we will set maxargs to 4M.
*/

/********************* CONSTANT DEFINITIONS ********************/

/* Most of the constants use throughout IL are defined in this
 * section.  Constants specific to fields in structures are defined
 * right next to the structure itself for easy reference.
 *
 * The constant and structure definitions in the next section should
 * not be used directly by the user since these internal definitions 
 * may change without notice.  Users should ALWAYS use the many predicates 
 * and access macros defined later on in this file to interface to IL.
 *
 * NOTE: Please don't change the following encoding arbitrarily, some 
 *       part of the code relies on the layout of the numbers.
 *       Specifically, the new evaluator relies on the fact that the
 *       type code for binary functions (ilcBinary) is 0 and the code
 *       for V-code functions (ilcFunObj) is 1. Some places assumes
 *       that ilcSymbol is 2 and ilcList is 3 as well. (see veval.c)
*/

/* #define IL_LINK_COMPATIBLE	1  It's link incompatible now! 6/1/93 <wu> */

#ifndef IL_LINK_COMPATIBLE

#define	ilcBinary	0	/* Binary			*/
#define ilcFunObj	1	/* Function objects        <wu> */
#define	ilcSymbol	2	/* Symbol			*/
#define	ilcList		3	/* List				*/
#define	ilcFixNum	4	/* Fixnum (32 bit)		*/
#define ilcStdObj	5	/* Standard objects        <wu> */
#define ilcEnvObj	6	/* Environment objects     <wu> */
#define	ilcFloNum	7	/* Flonum			*/
#define	ilcString	8	/* String			*/
#define	ilcPort		9	/* I/O Port			*/
#define	ilcArray	10	/* Array			*/
#define	ilcOther	11	/* Other, pointer to Anything	*/
#define ilcEnumObj	13	/* Enumerated objects      <wu> */
#define	ilcIllegal	15	/* Illegal, for catching GC bugs */

#define ilcLambda	1
#define ilcLexpr	ilcLambda /* for compatibility          */
#define ilcNLambda	2
#define ilcLambda2	3	/* passes args as argv or (argc,argv) */
#define ilcMacro	4
#define ilcLambda1	5	/* lambda function with fixed # of args */
#define ilcNLambda1	6	/* nlambda function with fixed # of args */

#else  /* be link-compatible with existing code */

#define	ilcList		0	/* List				*/
#define	ilcFixNum	1	/* Fixnum (32 bit)		*/
#define ilcStdObj	2	/* Standard objects        <wu> */
#define ilcEnvObj	3	/* Environment objects     <wu> */
#define	ilcFloNum	4	/* Flonum			*/
#define	ilcString	5	/* String			*/
#define	ilcSymbol	6	/* Symbol			*/
#define	ilcBinary	7	/* Binary			*/
#define	ilcPort		8	/* I/O Port			*/
#define	ilcArray	9	/* Array			*/
#define	ilcOther	10	/* Other, pointer to Anything	*/
#define ilcEnumObj	12	/* Enumerated objects      <wu> */
#define ilcFunObj	13	/* Function objects        <wu> */
#define	ilcIllegal	15	/* Illegal, for catching GC bugs */

#define ilcLambda	0
#define ilcLexpr	ilcLambda /* for compatibility          */
#define ilcNLambda	1
#define ilcLambda2	2	/* passes args as argv or (argc,argv) */
#define ilcMacro	3
#define ilcLambda1	4	/* lambda function with fixed # of args */
#define ilcNLambda1	5	/* nlambda function with fixed # of args */

#endif


#define ilcInPort 1             /* port direction */
#define ilcOutPort 2
#define ilcEInPort 3

#define ilcPortClosed 0         /* port status */
#define ilcPortOpen 1
#define ilcPortReserved 2

#define ilcNormalPort 0        /* port type */
#define ilcTempPort 1
#define ilcBufferedPort 2

typedef enum {
	ilcIL	= 1,
	ilcILS	= 2,
	ilcSCM	= 3
} iltLangMode;

/********************* TYPE DEFINITIONS ********************/

typedef unsigned char iltCons;
typedef unsigned char iltFunc;
typedef void *iltVoidPtr;

typedef union iltLispval *list;
typedef list	(*iltBinary) ();
typedef void    (*iltVoidp) ();
typedef String 	(*iltCharp) ();
typedef Boolean (*iltBooleanp) ();
typedef int     (*iltIntp) ();
typedef void    (*iltCWHook) (String hookFunc);
typedef int     (*iltExitHook) (int status);

/* alternate names for IL macro used in ilfuncs.c */
/* possible classes */
#define SYSTEM          ilcNoWrite
#define ENCRYPTED       ilcNoRead
#define PROTECTED       ilcNoRead|ilcNoWrite
#define USER_REDEFINABLE 0

#define ilcNoWrite      01
#define ilcNoRead       02
#define ilcTraceVar     04
/* #define ilcNestable     010  (OBSOLETE) never used <wu> */

/* possible disciplines */
#define LAMBDA		ilcLambda
#define NLAMBDA		ilcNLambda
#define MACRO		ilcMacro
#define LEXPR		ilcLexpr
#define LAMBDA1		ilcLambda1
#define NLAMBDA1	ilcNLambda1
#define LAMBDA2		ilcLambda2

/* maximum number of arguments a function may accept */
#define MAXARGS         ilcMaxArgs /* a big number */

struct iltTempSatom {
    iltCons     stype;
    unsigned char evalhook;
    unsigned char protection;
    unsigned char protectvar;
    String      pname;
    list        plist;
    list        value;
    list        func;
    list        dummy;
    long        dummy1;
};

struct iltTempYatom {
    iltCons     stype;
    iltFunc     functype;
    unsigned short lower;
    unsigned long  upper;
    String      template;
    iltBinary   v;
};

/* Pair s and y for initialization of each group */
typedef struct iltInitStruct iltInitStruct;
struct iltInitStruct {
    struct iltTempSatom    s;
    struct iltTempYatom    y;
};

/* Structure for the atom tables for each group */
typedef struct	iltFuncArray	iltFuncArray;
struct	iltFuncArray {
    iltInitStruct   *atoms;     /* s and y atom pairs */
};


/*********************************************************/
/*** Initialization sequence                          ***/

typedef struct iltInitClientsStruct iltInitClientsStruct;
struct iltInitClientsStruct {
        String    name;
        String    (*proc)();
};

extern void ilInitClients(int *argc, String argv[], 
                    iltInitClientsStruct *initProcs, int nProcs);

extern String ilInit(int *argc, String argv[]);

/*********************************************************/
/*** User defined Types                               ***/

#define ilcMaxUTcxtNameValue 10000

typedef struct  iltUTinContext iltUTinContext;
struct  iltUTinContext {
     list    fieldName;
     list    fieldValue;
};

typedef struct	iltUTHeader iltUTHeader;
struct	iltUTHeader {
    iltCons	stype;
    iltCons     utype;
};

typedef struct iltMethodTable iltMethodTable;
struct iltMethodTable {
    iltVoidp    initUT,            /* called after IL makes an empty instance */
		checkUT,           /* called from ilCheck */
                gcMarkUT,          /* called to mark the subfields of the struct */
                gcUNMarkUT,        /* called to unmark the subfields of the struct */
                gcReclaimUT;       /* called before the struct is reclaimed by GC */
    iltBinary   evalUT,            /* called when struct is evaluated */
                putPropUT,         /* called for setting a property */
                setqUT,            /* called before binding struct to a symbol */
                getPropUT,         /* called to get a property */
                getSinglePropUT;   /* called to get a property by name */
    iltCharp    printUT;           /* called to print an instance */
    iltBooleanp equalUT;           /* equality test */
    iltIntp     hashUT;            /* hash fucntion for assoc tables */
    iltBooleanp getGenUT;          /* The generator get next function */
    iltVoidp    stopGenUT;         /* Stop the generator activation */
    iltBinary   startGenUT;        /* start the generator activation */ 
    iltIntp     saveContextUT;     /* called to convert the UT into a list, for contexts */
    iltBooleanp loadContextUT;     /* called to restore the contents of a UT from a list */
    iltBinary   typepUT;           /* called from typep function, must return symbol */
    iltBinary   fastPutPropUT,     /* putprop that causes no consing */
		fastRemPropUT;     /* remprop that causes no consing */
    int         nEntries;          /* no of privately defined functions */
    iltBinary   private[1];        /* user defined functions */
};

extern unsigned int ilRegisterType(String typeName, int typeSize, iltMethodTable *methodTable, iltMethodTable *superClass);
extern list ilTypeFunction();
extern iltMethodTable * ilMakeMethodTable(int privateFuncs);
extern void ilUpdateUTMinCells(int UTID, int mcells, int cellsPerCmd);

#define REGISTER ilRegisterType
#define DoFUNCTION ilTypeFunction
#define MARKUT ilMarkUT
#define UNMARKUT ilUNMarkUT
#define mku(typeId) ilMakeUserType(typeId, NULL)
#define mkun(name)  ilMakeUserType(0, name)       /* make user type by name */
#define mkmt(num) ilMakeMethodTable(num)

#define ilmMakeUT(typeId) ilMakeUserType(typeId, NULL)
#define ilmMakeNUT(name)  ilMakeUserType(0, name)  /* make user type by name */

/********************* SPECIAL ATOMS **************************/

#define ilcNil		( (list) 0 )

/********************* EXTERN DECLARATIONS ********************/


/* Atoms */

extern list

    /* Special Atoms */

    ilcT,
    ilcUnbound,
    ilcError,

    /* type names */

    ilaArray,
    ilaBinary,
    ilaFunObj,
    ilaFixNum,
    ilaFix32,
    ilaFloNum,
    ilaList,
    ilaOther,
    ilaPdv,
    ilaSplice,
    ilaPort,
    ilaString,
    ilaSymbol,

    /* general utility */

    ilaArrayRef,
    ilaAutoLoad,
    ilaBor,
    ilaBitField,
    ilaBitField1,
    ilaCompress,
    ilaDefProp,
    ilaErrport,
    ilaErrSet,
    ilaExitBefore,
    ilaDebugger,
    ilaDisableGC,
    ilaEncrypt,
    ilaEqual,
    ilaErrorHandler,
    ilaEval,
    ilaGCafter,
    ilaGCbefore,
    ilaGreaterp,
    ilaLessp,
    ilaLinereadstring,
    ilaLoad,
    ilaLoadi,
    ilaLoadString,
    ilaPiport,
    ilaPoport,
    ilaRoport,
    ilaWoport,
    ilaProg,
    ilaPtport,
    ilaQuote,
    ilaRange,
    ilaReturn,
    ilaStderr,
    ilaSetq,
    ilaSetqBitField,
    ilaSetqBitField1,
    ilaStdin,
    ilaStdout,
    ilaSkillEvalString,
    ilaTraceLength,
    ilaTraceLevel,
    ilaTopLevelErrorHandler,
    ilaPrintLevel,
    ilaPrintLength,
    ilaPprintResult,
    ila_PrintGC,
    ila_StackTrace,
    ila_gccount;

/* A few routines people really shouldn't know about */

extern list ilfEncrypt(list l);
extern list ilfLoad(list l);
extern list ilfNconc(int argc, list *argv);		/* used by ilmMapCan macro */
extern list ilfTrace(list l);
extern list ilfUntrace(list l);

/* Routines which are defined only with a debug key */
/* (And can't be compiled in from ilfuncs.h) */

extern list ilfDbx(void);
extern list ilfPrintFileDescriptors(void);
/* General purpose routines */

/* INTERNAL USE extern String ilCopyString();*/
extern list ilSprintf(String s, int max, list l);	/* printing formatted output */
extern list ilSprintf2(String s, int max, int argc, list *argv);

/* Functions for printing output */

extern void	ilPrintPort(list port, const char *format, ...);
extern void	ilPrintOutFile(const char *format, ...);
extern void	ilPrintErrorFile(const char *format, ...);

#define ilmInFile	ilGetFile(ilaPiport)
#define ilmOutFile	ilGetFile(ilaPoport)

/********************* EXTERN DECLARATIONS ********************/

/* ilInitContext takes three string arguments:
 * 1 - name of the context, e.g. "skillCore"
 * 2 - the relative path where the context file will be stored, e.g. "tools/dfII/etc/context"
 * 3 - the relative path where the srouce files will be stored, 
 *     e.g. "tools/dfII/pvt/etc/context/skillCore"
 */
extern void ilInitContext(constString cxtName, constString cxtPath, constString srcPath);
extern void ilDetachFromLicenseManager(void);
extern void ilForceLoadContext(constString cxtName);

extern void ilDefineProcedure(String s, list (*value) (/* ??? */), int lower, int upper, String template);

/* Predicate Functions for Values */
extern Boolean ilIsList(list l);
extern Boolean ilIsListCell(list l);
extern Boolean ilIsFix(list l);
extern Boolean ilIsInt(list l);
extern Boolean ilIsFloat(list l);
extern Boolean ilIsDouble(list l);
extern Boolean ilIsNumber(list l);
extern Boolean ilIsString(list l);
extern Boolean ilIsSymbol(list l);
extern Boolean ilIsPort(list l);
extern Boolean ilIsArray(list l);
extern Boolean ilIsFunction(list l);
extern Boolean ilIsAtom(list l);
extern Boolean ilIsNull(list l);
extern Boolean ilIsNotNull(list l);
extern Boolean ilUTypep(list l);
extern list ilIsDefstruct(list defstr, list name);

/* Atom Accessing Functions */
extern int	    ilGetInt(list value);
extern double	    ilGetDouble(list value);
extern double	    ilGetNumber(list value);
extern String	    ilGetString(list value);
extern String       ilGetPName(list sym);
extern list         ilGetValue(list sym);
extern list         ilGetFunction(list sym);
extern iltBinary    ilGetAddress(list func);

/* List Accessing Functions */
extern list ilCar (register list l);
extern list ilCdr (register list l);
extern list ilCaar (register list l);
extern list ilCadr (register list l);
extern list ilCdar (register list l);
extern list ilCddr (register list l);
extern list ilCaaar (register list l);
extern list ilCaadr (register list l);
extern list ilCadar (register list l);
extern list ilCdaar (register list l);
extern list ilCaddr (register list l);
extern list ilCddar (register list l);
extern list ilCdadr (register list l);
extern list ilCdddr (register list l);
extern list ilCaaaar (register list l);
extern list ilCaaadr (register list l);
extern list ilCaadar (register list l);
extern list ilCadaar (register list l);
extern list ilCaaddr (register list l);
extern list ilCaddar (register list l);
extern list ilCadadr (register list l);
extern list ilCadddr (register list l);
extern list ilCdaaar (register list l);
extern list ilCdaadr (register list l);
extern list ilCdadar (register list l);
extern list ilCddaar (register list l);
extern list ilCdaddr (register list l);
extern list ilCdddar (register list l);
extern list ilCddadr (register list l);
extern list ilCddddr (register list l);
extern list ilNthCdr (int x_index, list l_list);
extern list ilNthElem (int x_index, list l_list);
extern list ilNth (int x_index, list l_list);
extern list ilLast (list l);
extern int ilLength (register list l);


/* Garbage Collection Functions */
extern void ilGC(void);
/***********************************************/
/* Under no circumstances should application code
   rely on turning GC on and off. The following
   functions are for internal use only */
extern void ilDisableGC(void);
extern void ilEnableGC(void);
/***********************************************/

/* Atom Generation Functions */
extern list ilMakeArray(list s_name, list arraytype, int size);
extern list ilMakeBinary(iltBinary value, iltFunc ftype, int minArgs, int maxArgs, String template);
extern list ilMakeDouble(double value);
extern list ilMakeInt(long value);
extern list ilMakePort(FILE *file, unsigned char iodirection, int blevel, int plevel, String name, int lineno);
extern list ilMakeTime(time_t tmptime);
extern list ilMakeTempPort(void);
extern list ilMakeSplice();
extern list ilMakeString(String value);
extern list ilMakeChar(unsigned char c);
/* extern list ilNeedNCells(list atype, int num); */

/* INTERNAL USE ONLY extern list ilMakeSymbol();*/
extern list ilMakeUserType(unsigned int typeId, String typeName);

/* List Generation Functions */
extern list ilNcons(list l);
extern list ilCons(register list l1, register list l2);
extern list ilXcons(list l1, list l2);
extern list ilList2(list l1, list l2);
extern list ilList3(list l1, list l2, list l3);
extern list ilList4(list l1, list l2, list l3, list l4);
extern list ilList5(list l1, list l2, list l3, list l4, list l5);
extern list ilTconc(list l_ptr, list g_x);

/* List Handling Functions */
extern list ilAppend(list l1, list l2);
extern list ilAppend1(list l_arg1, list g_arg2);
extern list ilAppendNoCopy(list l_arg1, list g_arg2);
extern list ilCopy(list l);
extern list ilLconc(list l_ptr, list l_x);
extern list ilLdiff(list l_x, list l_y);
extern list ilRemove(list g_x, list l_1);
extern list ilRemoveNoCopy(list g_x, list l);
extern list ilRemq(list g_x, list l_1);
extern list ilReverse(list l);
extern list ilSubst(list g_x, list g_y, list l_s);
extern list ilNconc2(list l1, list l2);
extern list ilStringToFunction(String s, iltLangMode langMode);

/* Property Handling Functions */
extern list ilDefProp();
extern list ilPutProp(list ls_name, list g_val, list g_ind);
extern list ilPutPropString(list ls_name, list value, String valueName);
extern list ilRemProp(list ls_name, list g_ind);
extern list ilGetPropString(list ls_name, String propName);

/* String Handling Functions */
String ilIndex(String string1, String string2);	/* index of a string */
String ilRIndex(String string1, String string2); /* rindex of a string */

/* compare two strings alphanumerically (this is not quite correct) */
int ilAlphaNumCmp(const char *s1, const char *s2, ...);

extern void ilAddProtection(list *ptr, String file, int line);
extern void ilRemoveProtection(list *ptr, String file, int line);

#define ilAddProtect(lptr)  (void) ilAddProtection((list*)&(lptr), __FILE__ , __LINE__)
#define ilRemoveProtect(lptr)  (void) ilRemoveProtection((list*)&(lptr) , __FILE__, __LINE__)


/* Stack Protection Routines */
#define ilProtect(lptr)
#define ilProtect2(lptr, llptr)
#define ilUnprotect(lptr) 	
#define ilUnprotect2(lptr, llptr)


/* Access to Ports */
extern FILE * ilGetOutputFile(void); /* Get file for standard output port */
extern FILE * ilGetFile(list port);	 /* Get file for any port */
extern FILE * ilFopen(String rawfilename, String filemode);
/* extern int    ilFclose(FILE *iop); */

/* Prototypes for the port methods */

typedef int (*port_putc_t)(char c, list port);
typedef int (*port_puts_t)(String s, list port);
typedef int (*port_flush_t)(list port);
typedef int (*port_getc_t)(list port);
typedef int (*port_ungetc_t)(char c, list port);
typedef String (*port_gets_t)(String s, int max, list port);
typedef String (*port_fill_t)(String buf, int bufsize, list port);

/* Generic Output Port Functions */

extern int ilSetBufferedOutputPort(list port, port_putc_t charFn, 
				   port_puts_t stringFn, port_flush_t flushFn);
extern int ilSetBufferedInputPort(list port, port_getc_t getcharfn,
				  port_gets_t getstringfn,
				  port_ungetc_t ungetfn, port_fill_t fillfn);
extern int ilBadPortCall();	/* Placeholder where no method applies */
				/* Dispatching Routines */
extern int ilPutStringPort(char *string, list port);	/* Write a string to a port */
extern int ilPutCharPort(char c, list port);	/* Write a character to a port */
extern int ilFlushPort(list port);	/* Flush a port */

/* Generic Input Port Functions */

extern int ilGetStringPort();	/* Return a string from a port */
extern int ilGetCharPort(list port);	/* Return a character from a port */
extern int ilUngetCharPort(char c, list port);	/* Place a character back in a port */
extern int iliFillPort();	/* Refill the buffer of an input port */

/* THE FOLLOWING PORT FUNCTIONS ARE NOT INTENDED TO BE CALLED DIRECTLY.
   THEY ARE DEFAULT PORT METHODS, WHICH MAY BE INSTALLED IN PORT DEFINITIONS. */

				/* Default Unbuffered Methods */
extern int ilFPutc();		/* Send a character to a file  */
extern int ilFPuts();		/* Send a string to a file */
extern int ilFFlush();		/* Flush a file */
				/* Default Buffered Methods */
extern int ilPPutc();		/* Send a character to a buffered port */
extern int ilPPuts();		/* Send a string to a buffered port */
extern int ilPFlush();		/* Flush a buffered port */

extern int ilFGets();		/* Fill input port buffer */

extern void ilGetPrompt(char *buf, int max);	/* returns prompt strings */
extern void ilSetPrompts(String prompt1, String prompt2);

/* IL Debugging Aids */
extern Boolean ilAddFuncArray(struct iltInitStruct *q); /* Add function defining arrays */
extern int ilMarkUT(list cell);           /* return the no of cells marked */

extern void ilRegCAfterCall(iltBinary p),
            ilRegCBeforeCall(iltBinary p),
            ilResetCCallBack(iltBinary func);
extern void ilRegChangeWorkingDirHook(iltCWHook func);
extern void ilRegExitHook(iltExitHook func);

/* Miscellaneous IL functions C programmers may find useful */

extern list
ilAbs(list arg),
ilAdd1(list arg),
ilAddLambda(String name, iltBinary fName, int lower, int upper, String template),
ilAddNLambda(String name, iltBinary fName, int lower, int upper, String template),
ilAlias(list NewName, list OldName),
ilAlphaLessp(list st_1, list st_2),
ilApply(list func, list args),
ilArray(list s_name, list s_type, int x_dim1),
ilArrayRef(list array, int index),
ilAssoc(list g_arg1, list l_arg2),
ilAssq(list g_arg1, list l_arg2),
ilBitField(list x_val, list x_msb, list x_lsb),
ilBitField1(list x_val, list x_bit),
ilBoundp(register list arg),
ilCTopLevel(void),
ilClose(list p_port),
ilCompress(String InputFile, String OutputFile),
ilCprintf(),
ilCsh(),
ilDef(),
ilDefun(),
ilDrain(list p_port),
ilEcho(Boolean flag),
ilEd(String file_name, Boolean vi),
ilEdi(String file_name, Boolean vi),
ilEdl(String file_name, Boolean vi),
ilEncrypt(String src, String dst, String password, int sdakey),
ilErr(list errorFlag),
ilError(constString id, constString message, list l),
ilErrSet(list g_expr),
ilErrSet2(list g_expr, int services),
ilErrSetString(String str),
ilErrSetString2(String str, int services),
ilEval(list l),
ilEvalString(String s),
ilExit(int status, Boolean doAbort),
ilExp(list n_arg),
ilExpt(list n_base, list n_power),
ilFindSym(String s),
ilFix(list n_arg),
ilFixp(list l),
ilFloat(list n_arg),
ilFloatp(list l),
ilFprintf(list port, list l),
ilFscanf(FILE *f, list l),
ilfSetPrompts(list l),
ilFuncall(list f, list a),
ilGenSym(String prefix),
ilGeqp(),
ilGet(list ls_name, list g_ind),
ilGetILPath(),
ilGet_pname(list s_arg),
ilGetd(list s_name),
ilGetDirFiles(constString dirPath),
ilGetFunType(list fp),
ilGetl(list ls_name, list l_indicators),
ilGetSkillPath(void),
ilGreaterp(),
ilInclude(String filename),
ilInfile(String filename),
ilInstring(String s),
ilIntern(list sAtom),
ilLeftShift(list x_val, list x_amt),
ilLeqp(),
ilLessp(),
ilLineRead(list p_arg),
ilLineReadString(String s),
ilLoad(String filename, Boolean ignoreError),
ilForceLdi(String filename),
ilLoadString(String s),
ilLookupTag(String tagName, String fileName),
ilLsh(list x_val, list x_amt),
ilMakeSym(String s),
ilMap(),
ilMapc(),
ilMapcan(),
ilMapcar(),
ilMaplist(),
ilMember(list g_arg, list l_arg),
ilMemq(list g_arg, list l_arg),
ilMergesort(list l1, list l2, list u_comparefn, Boolean calledBySortcar),
ilMinus(list arg),
ilMinusp(list l),
ilMProcedure(),
ilNProcedure(),
ilNumberp(list l),
ilOutfile(String filename, String mode),
ilPlist(list s_arg),
ilPlusp(list l),
ilPp(list s_name, list port),
ilPprint(list l, list port),
ilPrint(list l, list port),
ilPrintError(list arg, list port),
ilPrintStack(int level, int skip, list port, Boolean evaluated, Boolean locals),
ilPrintf(),
ilPrintLev(list l, int PrintDepth, int PrintLength, list port),
ilPrintln(list g_arg, list port),
ilProcedure(),
ilPutd(list s_name, list u_func),
ilPutDisc(),
ilQuote(list l),
ilRead(list p_arg),
ilRightShift(list x_val, list x_amt),
ilRound(list n_arg),
ilScanf(),
ilSendcommmand(),
ilSet(list sarg, list g_val),
ilSetFnWriteProtect(list s_name),
ilSetGuard(list sarg, list g_val),
ilSetSkillPath(list l),
ilSetPlist(list s_arg, list l_plist, list prop),
ilSetStderrMaker(iltBinary func),
ilSetStdoutMaker(iltBinary func),
ilSetVarWriteProtect(list s_name),
ilSetq(list s_arg, list g_val),
ilSh(),
ilSignp(),
ilSort(list l_data, list u_comparefn, Boolean calledBySortcar),
ilSortcar(list l_data, list u_comparefn),
ilSprintf(String s, int max, list l),
ilSstatus(list NameOfSwitch, list State),
ilStatus(list NameOfSwitch),
ilStore(),
ilSub1(list arg),
ilSymeval(list s_arg),
ilTerpri(list port),
ilTopLevel(Boolean development),
ilTopLevelCmd(String cmd),
ilTopLevelSet(list g_expr),
ilTopLevelSetString(String str),
ilTypep(list l),
ilUNMarkUT(void),
ilIsCallable(list obj),
ilVi(),
ilVii(),
ilVil();

extern String ilGetFilename(list S_arg);

extern void	ilLegalFilename(String filename, String resultname);
extern String	ilPrependInstallPath(String filename);
extern Boolean	ilCheck(void);
/* extern Boolean  ilCheckInitSequence(void); */
extern String   ilGetCurrentTime(void);
extern String   ilGetTypeName(iltCons typeNumber);

/********************* DECLARATIONS FOR DEBUGGING ********************/

/* ilListAssert is a "friendly" assertion routine for use by developers.
It checks to see if its argument is a legal list pointer.  If not, it
gracefully informs the developer something is seriously wrong and prints
as much debugging information as possible before calling ilError.
LPDLISTASSERT, and LPLDLISTASSERT provide two different ways of using the 
checking routine - check only when LPDEBUG is defined, or check only when 
LPLONGDEBUG is defined.  I use these macros rather than calling
ilListAssert directly.
*/

extern void	ilListAssert(list l, String s, String FileName, int LineNumber);
extern void	ilAssert(String ErrorMessage, String FileName, int LineNumber);
extern void	ilFatal(const char *routine, const char *format, ...);
extern void	ilWarning(const char *routine, const char *format, ...);

/* This should NOT be used, but some group is (mis)using it, so we have to
   give its prototype here just to make sure it's linked properly.
*/
extern void	ilNonfatal(const char *routine, const char *format, ...);

/* Error Reporting Functions */
extern list	ilError2(constString id, constString message);

/* Assertions in IL are only activated when debugging is
turned on - i.e. either LPDEBUG or LPLONGDEBUG are defined.
*/

# define LPDASSERT(l)
# define LPDLISTASSERT(l)
# define LPLDASSERT(l)
# define LPLDLISTASSERT(l)
# define LPLDMSG(format,arg)
# define LPLDMSG2(format,arg1,arg2)

/* The following functions make output available to dbx */

extern  void	iliPrintStack(int level, int skip);
extern  void	iliPrintCell(list l);
extern  void	iliPrintList(list l);

/*********** Internal Data Structure Access & Predicates ***********/

extern iltCons ilGetType(register list l);
extern int ilGetX(register list xarg);
extern double ilGetF(register list farg);
extern FILE * ilGetP(register list parg);
extern list *ilGetA(register list aarg);
extern int ilGetArrayUpper(register list arg);
extern String ilGetT(register list arg);
extern RefAny ilGetO(register list arg);
extern String ilGetPname(register list arg); 
extern list ilGetPlist(register list arg); 
extern unsigned int ilGetNameSpace(register list arg);
extern list ilPutPlist(list arg, list val);
extern list ilGetFunc(register list arg);
extern list ilPutFunc(register list s_name, register list u_func);
extern list ilGetSym(register list arg);
extern iltBinary ilGetY(register list arg); 
extern unsigned short ilGetBinaryLower(register list arg);
extern unsigned int ilGetBinaryUpper(register list arg);
extern iltFunc ilGetFuncType(register list arg);
extern String ilGetTemplate(register list arg);
extern iltCons ilGetUType(register list arg); 

/* macros for protection bits of functions and variables */

extern Boolean ilGetNoReadFunc(register list arg);
extern Boolean ilGetNoWriteFunc(register list arg);
extern Boolean ilGetNoWriteVar(register list arg);
extern Boolean ilGetTraceVar(register list arg); 
extern Boolean ilGetResumeFunc(register list arg);
extern Boolean ilGetGuardedFunc(register list arg);
extern Boolean ilGetGuardedVar(register list arg);

extern unsigned int ilSetNoReadFunc(register list arg);
extern unsigned int ilSetNoWriteFunc(register list arg);
extern unsigned int ilSetNoWriteVar(register list arg);
extern unsigned int ilSetTraceVar(register list arg); 
extern unsigned int ilSetResumeFunc(register list arg);
extern unsigned int ilSetGuardedFunc(register list arg);
extern unsigned int ilSetGuardedVar(register list arg); 

extern unsigned int ilClearNoReadFunc(register list arg);
extern unsigned int ilClearNoWriteFunc(register list arg);
extern unsigned int ilClearNoWriteVar(register list arg); 
extern unsigned int ilClearTraceVar(register list arg);
extern unsigned int ilClearResumeFunc(register list arg);
extern unsigned int ilClearGuardedFunc(register list arg); 
extern unsigned int ilClearGuardedVar(register list arg); 

/*------------------------------------------------------------*/
/* LISP Data Structure Construction & Access */

extern list ilRplacaError(list l);
extern list ilRplacdError(list l);

extern list ilRplaca(list lh_arg1, list g_arg2);
extern list ilRplacd(list lh_arg1, list g_arg2);
extern	Boolean	ilStringCompare(list l, String s);
extern	Boolean	ilEqual(list l1, list l2);

/* Type Predicates */

extern Boolean ilCheckType(register list arg, iltCons tp);
extern Boolean ilNull(register list arg);      
extern Boolean ilNonnull(register list arg);  
extern Boolean ilIsBound(register list arg);
extern Boolean ilEq(register list arg1, register list arg2);  
extern Boolean ilNeq(register list arg1, register list arg2);

extern Boolean ilAtom(register list arg);
extern Boolean ilArrayp(register list arg);
extern Boolean ilBcdp(register list arg);
extern Boolean ilDtpr(register list arg);
extern Boolean ilListp(register list arg);
extern Boolean ilNonNilListp(register list arg);
extern Boolean ilPortp(register list arg);
extern Boolean ilInPortp(list arg);
extern Boolean ilOutPortp(list arg);
extern Boolean ilEInPortp(list arg);
extern Boolean ilEMode(list arg);
extern Boolean ilStringp(register list arg);
extern Boolean ilSymbolp(register list arg);
extern Boolean ilSymstrp(register list arg);
extern Boolean ilOtherp(register list arg);

/* Constant string for use in throw/catch toplevel situation: HACK! */

#define ilcCatchString "__C_A_T_C_H__"

/*************************************************************************/
/* The compiler switch LP_SHORT_MACROS allows users who interface to the IL
package to use shorter names without having to remember all the appropriate
package prefixes.  There is a possiblity that the abbreviated names may clash
with names defined in header files of other packages and it is the user's
responsbility to make sure that there are no naming conflicts.
*/

#ifdef LP_SHORT_MACROS			/* abbreviated names */
#define	symbol		iltSymbol
#define binary		iltBinary

#define InFile		ilmInFile
#define OutFile		ilmOutFile
#define MsgOut		ilPrintOutFile
#define MsgError	ilPrintErrorFile
#define XtrType		ilGetType
#define Xtrx		ilGetX
#define Xtrf		ilGetF
#define Xtrb		ilGetB
#define Xtrp		ilGetP
#define Xtra		ilGetA
#define Xtraupper	ilGetArrayUpper
#define Xtrt		ilGetT
#define Xtro		ilGetO
#define Xtrv		ilGetV
#define Xtrc		ilGetC
#define Xtry		ilGetY
#define Xtrylower	ilGetBinaryLower
#define Xtryupper	ilGetBinaryUpper
#define Xtryfunctype	ilGetFuncType
#define Xtrytemplate	ilGetTemplate
#define NIL		ilcNil
#define T		ilcT
#define UNBOUND		ilcUnbound
#define ERROR		ilcError
#define	quote		ilQuote
#define eval		ilEval
#define set		(void) ilSet
#define setq		(void) ilSetq

#define car		ilCar
#define cdr		ilCdr
#define caar		ilCaar
#define cadr		ilCadr
#define cdar		ilCdar
#define cddr		ilCddr
#define caaar		ilCaaar
#define caadr		ilCaadr
#define cadar		ilCadar
#define caddr		ilCaddr
#define cdaar		ilCdaar
#define cdadr		ilCdadr
#define cddar		ilCddar
#define cdddr		ilCdddr
#define cons		ilCons
#define xcons		ilXcons
#define ncons		ilNcons
#define append		ilAppend
#define append1		ilAappend1
#define copy		ilCopy
#define reverse		ilReverse
#define subst		ilSubst
#define	tconc		(void) ilTconc

#define PROTECT		
#define UNPROTECT	
#define UNPROTECT1	
#define PROTECT2
#define UNPROTECT2

#define print		(void) ilPrint
#define terpri		(void) ilTerpri
#define mkx		ilMakeInt
#define mkf		ilMakeDouble
#define mkt		ilMakeString
#define mky		ilMakeBinary
#define mka		ilMakeArray

#define mkchar		ilMakeChar
#define lookup		ilLookup
#define addsym		ilAddSym
#define initsym		ilInitsym
#define foreach		ilmForeach
#define FOREACH		ilmFOREACH  /* old form */
#define MAPCAR		ilmMapCar
#define MAPCAN		ilmMapCan
#define getNoReadFunc	ilGetNoReadFunc
#define getNoWriteFunc	ilGetNoWriteFunc
#define getNoWriteVar	ilGetNoWriteVar
#define ilSetILPath     ilSetSkillPath
#define ilInfo 		voInfo
#define ilWarn		voWarn
#define ilDebug 	voDebug
#define ilProgError 	voProgError
#endif /* LP_SHORT_MACROS */

#define ilmDf(name) CPP_CONCAT(ila,name), CPP_CONCAT(ilf,name)(), CPP_CONCAT(il,name)()

#define LFMsymstrp(l) ( ilSymstrp(l) ? LPT : LPNIL) /* for si/simin.c */
extern list LFCerrset(); /* for genld,libil,simssh */

/*********************************************************************/
/* The following functions can be called from framwork ONLY.         */

extern list ilCatchToplevel();
extern list ilETopLevel(void);
extern Boolean	ilContexts(void);
extern void	ilDoPrintLev(list l, int PrintDepth, int PrintLength, list port, Boolean PrettyPrint, Boolean ForceFunctionPrint);	/* useful? maybe. hard to use & buggy? yes. */
extern void ilRegCursorFunc(iltVoidp func);
/*********************************************************************/
/* The following functions only make sense if the caller creates a   */
/* special executable binary; e.g. oss,cdbatest                      */

extern Boolean	ilInitLisp(int *argc, String *argv);
extern Boolean	ilMakeCapContext(String capName, String cxtPath, String srcPath);
extern void     ilSetILPrompt(void);
extern list     ilSetPromptCallBack(iltBooleanp func);
extern void     ilSetStandAloneSignals(void);
/*********************************************************************/
/* OBSOLETE. The following functions will be made obsolete           */
/* On each set of functions there is a comment and a release number  */
/* for which the functions are likely to become undefined or         */
/* unexterned */


/* users are advised to call ilFindSym & ilMakeSym instead           */
extern	list	ilLookup(String s, Boolean insert), ilAddSym(String s, list v, list f), ilInitsym(String str, list val, list function);

/* Use ilMakeString. voCopyString for raw strings  */
/*extern String ilAllocateString();*/

/*----------------------------------------------------------------*/
/* These functions are for framework use !ONLY! */
extern int ilSetPortInputChar(list port, int value);
extern int ilSetPortOutputChar(list port, int value);
extern int ilGetPortOutputChar(list port);
extern int ilGetPortInputChar(list port);
extern char* ilGetPortBuffer(list port);
extern FILE* ilGetPortV(list port);
extern void ilSetPortBufferSlot(list port, unsigned int idx, char val);
extern int ilFlushPort(list port);

extern Boolean ilIsClass(list arg); 
extern Boolean ilIsGeneric(list arg);
extern Boolean ilVcodeIsData(list arg);
extern Boolean ilIsClosure(list arg);
extern u_long  ilFunGetVSize(list cell);
extern long    *ilFunGetVCode(list cell);
extern u_short ilFunGetNreq(list cell);
extern u_short ilFunGetNopt(list cell);
extern u_short ilFunGetStart(list cell);
extern u_char  ilFunGetFullFtype(list cell);

extern list    ilCreateFunObj(u_char ftype, u_long size, u_short nreq,
                            u_short nopt, u_short start);

extern u_char  ilGetEnvLevel(list env);
extern u_short ilGetEnvSize(list env);
extern list    ilGetEnvParent(list env);
extern list    *ilGetEnvFrame(list env);
extern list    ilCreateEnvObj(u_short size);
extern void    ilUpdateEnvObj(list env, u_char level, 
			      u_short size, list parent, list *frame);

extern list ilGetClosureEnv(list closure);
extern list ilGetClosureCode(list closure);
extern list ilCreateClosure();
extern void ilUpdateClosure(list closure, list env, list code);

/*----------------------------------------------------------------*/
/* Compatibility macros (will be removed later).

   The following macros have two versions.  The ones defined here are to be
   used by all groups other than IL.  For IL, when the file "ilsystem.h" is
   included, it will undefine these macros and redefine them.
*/

#define ilmCar(x)	ilCar(x)
#define ilmCdr(x)	ilCdr(x)
#define ilmNull(x)	ilNull(x)
#define ilmNonnull(x)	ilNonnull(x)
#define ilmNcons(x)	ilNcons(x)
#define ilmFixp(x)	ilFixp(x)
#define ilmFloatp(x)	ilFloatp(x)

/* Macros useful to system developers   */
/*
Foreach applies action to each element in the argument list
    VAR_        : formal variable
    LIST_       : list of elements to be iterated upon
    ACTION_     : action to be applied, should refer to VAR_

    e.g. Foreach(x,l,print(x)); will print each element in l

    Foreach(x,l1,Foreach(y,l2,if (x == y) Match = TRUE));
    will compare each element in l1 to every element in l2
    and set Match to TRUE if any of them are eq
*/

#define ilmForeach(VAR_,LIST_,ACTION_) {\
    register list Pp_pP;\
    for (Pp_pP = LIST_; ilmNonnull(Pp_pP); Pp_pP = ilmCdr(Pp_pP)) {\
        VAR_ = ilmCar(Pp_pP);\
        ACTION_;\
    };\
}

/* Original form ... redeclares first argument for you */
#define ilmFOREACH(VAR_,LIST_,ACTION_) {\
    register list VAR_;\
    ilmForeach(VAR_,LIST_,ACTION_);\
}

/*
MAPCAR is similiar to Foreach except that it saves the result of each
application of ACTION_ in a list.  ACTION_ must be an expression that
returns a list pointer.  The list of results is returned in the variable
RESULT_.

e.g. MAPCAR(ArgList,x,l,temp = ilEval(x));
    evaluates each element in l and returns a list of evaluated
    values in ArgList.  MAPCAR is a highly optimized macro and
    it does not automatically protect the intermediate result from
    evaluating ACTION_ from garbage collection.  In the above example,
    temp is a temporary variable used to protect the intermediate
    result from being GC'ed.

We have to be extremely careful in the implementation of MAPCAR since
RESULT_ and LIST_ can be the same list.
*/

#define ilmMapCar(RESULT_,VAR_,LIST_,ACTION_) {\
    register list VAR_;\
    if (ilmNull(LIST_)) RESULT_ = ilcNil;\
    else {\
    register list LastElement_;\
    list HeadOfList_;\
    VAR_ = ilmCar(LIST_);\
    LastElement_ = HeadOfList_ = ilmNcons(ACTION_);\
    ilmForeach(VAR_,ilmCdr(LIST_), {\
     (void) ilRplacd(LastElement_, ilmNcons(ACTION_));\
        LastElement_ = ilmCdr(LastElement_);\
    });\
    RESULT_ = HeadOfList_;\
    }\
}

/* MAPCAN is just like MAPCAR except that it uses nconc to splice together
the lists returned by ACTION_, recommended only for experts */

#define ilmMapCan(RESULT_,VAR_,LIST_,ACTION_) {\
    list TEMP_;\
    ilmMapCar(TEMP_,VAR_,LIST_,ACTION_);\
    ilmForeach(ELT_,TEMP_, \
    if (!(ilListp(ELT_))) \
        ilError("mapcan","args to nconc must be lists",TEMP_);)\
    RESULT_ = ilfNconc(TEMP_);\
}

