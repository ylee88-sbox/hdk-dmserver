/*
#Revision 1.1  92/04/06  09:37:04  cchiu
#Initial revision
*/

/*---------------------------------------------------------------------------*
 *
 * File: fwidms.yacc -- or how not to write a parser
 *
 *  Anyway we'll do some clean up
 *
 * Before, this file was including the lexer and included in the fwidms.c
 * which was included in fwidmb.c
 *
 * This means C file includes C file which include C file which includes C file
 * and so on about six (6) TIMES !!!
 *
 * For some time we had to keep the files unchanged to compare sources with 
 * Zeus. Finally let's move it from level -2 to -1
 *---------------------------------------------------------------------------*/

%{

#define yy_create_buffer iDMSyy_create_buffer
#define yy_delete_buffer iDMSyy_delete_buffer
#define yy_scan_buffer iDMSyy_scan_buffer
#define yy_scan_string iDMSyy_scan_string
#define yy_scan_bytes iDMSyy_scan_bytes
#define yy_flex_debug iDMSyy_flex_debug
#define yy_init_buffer iDMSyy_init_buffer
#define yy_flush_buffer iDMSyy_flush_buffer
#define yy_load_buffer_state iDMSyy_load_buffer_state
#define yy_switch_to_buffer iDMSyy_switch_to_buffer
#define yyin iDMSyyin
#define yyleng iDMSyyleng
#define yylex iDMSyylex
#define yyout iDMSyyout
#define yyrestart iDMSyyrestart
#define yytext iDMSyytext
/*#define yywrap iDMSyywrap*/
/* Use default yywrap from -lfl */
int iDMSyywrap(void){
  int yywrap(void);
  return yywrap();
}

/* disable conflicts between Cadence and stdlib */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

/* PROTOTYPES -- required by RS6000.4.1.4 */

struct elem *newElement();
#include <dmsC_functions2.h>
#include <idb_functions.h>
int yyinput();

#ifdef HPPA
#ifdef __cplusplus
   int yyparse(); /* required by C++ */
   extern "C" int yylex();
#endif
#endif

/* from Cadence */


/*-----------------------------------*
 * External vars used by the parser  *
 *-----------------------------------*/

#define IDMS_MAIN
#include <dmsC_new_vars.h>

#ifdef OH_MACH_LINUX
extern unsigned char* yytext;
#else
extern unsigned char yytext[];
#endif
extern FILE *yyin;

/*---------------------------------*
 * Static vars used by the parser  *
 *---------------------------------*/

static char tName[BUFSIZE],vName[BUFSIZE];
static char tpStr[BUFSIZE],varName[BUFSIZE],parseFile[BUFSIZE],yk_buffer[BUFSIZE];
static struct elem *ptr, *tptr, *aptr, *lastPtr, *nextPtr;
static int i,j;

/* New grammar will need like 5 */

#define YYMAXDEPTH 64000 /* as of 5/21/2008 - 10240 was exceeded in a 151k+ dms.pth file -- Sorin */ 
                         /* and then on 2/8/2012 extending to 64000 */
#define CLEANSTACK	strcpy(dmsStack,"");
#define NEWINCLUDE	{\
			strcpy(include[incCount],dmsStack);\
			CLEANSTACK\
			sprintf(yk_buffer,"%*s$include %s\n",includeDepth*3,"",include[incCount]);\
			strcat(resultBuf,yk_buffer);\
			incCount++;\
			}
#define NEWDIRLIB(op)	{\
			ptr->dirLib=(char *)malloc(strlen(dmsStack)+2);\
			strcpy(ptr->dirLib,dmsStack);\
                        ptr->absPath = 0;\
			if (!ptr->opt) ptr->opt = op;\
			CLEANSTACK\
			}
#define NEWVAR		{\
			var[varCount].name=(char *)malloc(strlen((char *)yytext)+2);\
			strcpy(var[varCount].name, (char *)yytext);\
			var[varCount].next=newElement();\
			CLEANSTACK\
			}
#define NEWINFO(a,b)	{\
			a[b].name=(char *)malloc(strlen(tpStr)+2);\
			strcpy(a[b].name,tpStr);\
			a[b].next=newElement();\
			dmsSeparateByDot(tpStr,tName,vName);\
			b++;\
			CLEANSTACK\
			}
#define DEFAULT(a,b,c,d) {\
			strcpy(tpStr,"*.*");\
			NEWINFO(a,b);\
			strcpy(dmsStack,c);\
			NEWDIRLIB(d);\
			}
#define FREEOLDDEF(a,b) {\
                        if (b>=2) for (j=b-2; j>=0; j--) {\
                          if (!strcmp(a[j].name,a[b-1].name)){\
                            lastPtr = a[j].next;\
                            a[j].next = a[b-1].next;\
			    free(a[b-1].name);\
                            while (lastPtr) {\
                              nextPtr = lastPtr->next;\
                              free(lastPtr->dirLib);\
                              free(lastPtr);\
                              lastPtr = nextPtr;\
                            }\
			    b--;\
                            break;\
                          }\
                        }\
			}                       
#define GETAPPEND(a,b)  {\
			if (b>=2) for (i=b-2; i>=0; i--) {\
			  if (!strcmp(a[i].name,a[b-1].name)){\
			    aptr=a[i].next;\
			    while (aptr) {\
			      if (aptr->dirLib) {\
			        ptr->opt=10+aptr->opt;\
			        ptr->dirLib=(char *)malloc(strlen(aptr->dirLib)+2);\
			        strcpy(ptr->dirLib,aptr->dirLib);\
			      }\
			      aptr=aptr->next;\
			      if (aptr) {\
			        tptr=ptr;\
		        	tptr->next=newElement();\
			      }\
			    }\
			    break;\
			  }\
		 	}\
			}

#define TURNDMSON	dmsIsOn=1;
#define TURNDMSOFF	dmsIsOn=0;

#define TOOLCHECKON	toolCheck=1;
#define TOOLCHECKOFF	toolCheck=0;

#define PRINTYYTEXT 	sprintf(yk_buffer,"%*s%s ",includeDepth*3,"", (char *)yytext);\
			strcat(resultBuf,yk_buffer);

%}
%token COMMENT1 DOLLAR RW RO EQUAL LB1 RB1 LB2 RB2 DOT SPACE VER COLUMN CR SLASH
%token PERCENT RWPATHS LIBPATH STRING WD INCLUDE WORKDIR WORKLIB APPEND TCONFIG
%token UNKNOWN
%start grammer
%%
grammer	: legals
	| legals grammer
	;

legals	: enter
	| legal enter {CLEANSTACK}
	;

enter	: CR
	| space CR
	;

legal	: define  
	| include 
	| libpath {FREEOLDDEF(libpath,libpathCount)}
	| worklib {FREEOLDDEF(worklib,worklibCount)}
	| rwpaths {FREEOLDDEF(rwpaths,rwpathsCount)}
	| workdir {FREEOLDDEF(workdir,workdirCount)}
	| tconfig {FREEOLDDEF(tconfig,tconfigCount)}
	| COMMENT1
	;

define	: STRING {strcpy(tpStr,toolName.name); NEWVAR}
	  equal
	  value {varCount++;}
	;

equal	: EQUAL
	| SPACE EQUAL
	| EQUAL SPACE
	| SPACE EQUAL SPACE
	;

include : INCLUDE {CLEANSTACK}
	  space
	  string {NEWINCLUDE
		  dmsYaccParser(include[incCount-1]);}
      	;

libpath	: LIBPATH {appendType=1;}
	  space {TURNDMSON}
	  PERCENT
	  name {strcpy(tpStr,dmsStack); NEWINFO(libpath,libpathCount)}
	  space {TURNDMSOFF TOOLCHECKOFF}
	  value {TOOLCHECKON}
	;

rwpaths	: RWPATHS {appendType=2;}
	  space {TURNDMSON}
	  PERCENT
	  name {strcpy(tpStr,dmsStack); NEWINFO(rwpaths,rwpathsCount)}
	  space {TURNDMSOFF TOOLCHECKOFF}
	  value {TOOLCHECKON}
	;

worklib	: WORKLIB
	  space {TURNDMSON}
	  PERCENT
	  name {strcpy(tpStr,dmsStack); NEWINFO(worklib,worklibCount)}
	  space {TURNDMSOFF TOOLCHECKOFF}
	  string {NEWDIRLIB(3) TOOLCHECKON}
	;

workdir	: WORKDIR
	  space {TURNDMSON}
	  PERCENT
	  name {strcpy(tpStr,dmsStack); NEWINFO(workdir,workdirCount)}
	  space {TURNDMSOFF TOOLCHECKOFF}
	  string {NEWDIRLIB(7) TOOLCHECKON}
	;

tconfig	: TCONFIG
	  space {TURNDMSON}
	  PERCENT
	  name {strcpy(tpStr,dmsStack); NEWINFO(tconfig,tconfigCount)}
	  space {TURNDMSOFF TOOLCHECKOFF}
	  string {NEWDIRLIB(5) TOOLCHECKON}
	;

value	: combine
	| combine column {tptr=ptr; tptr->next=newElement();} value
	;

combine	: dms {TURNDMSOFF}
	| string {NEWDIRLIB(7)}
	| WD {NEWDIRLIB(4)}
	| APPEND {
			switch(appendType) {
			case 1:	GETAPPEND(libpath,libpathCount);
				break;
			case 2:	GETAPPEND(rwpaths,rwpathsCount);
				break;
			default:strcat(resultBuf,"*ILLEGAL APPENDING*\n");
			}
		}
	;

dms	: RW {TURNDMSON} LB1 name {NEWDIRLIB(1)} RB1
	| RO {TURNDMSON} LB1 name {NEWDIRLIB(2)} RB1
	| RW {TURNDMSON} LB2 name {NEWDIRLIB(1)} RB2
	| RO {TURNDMSON} LB2 name {NEWDIRLIB(2)} RB2
	;

name	:
	| string
	| string column {strcat(dmsStack," ");} name
	;

column	: COLUMN
	;

space	: SPACE {CLEANSTACK}
	| space SPACE {CLEANSTACK}
	;

string	: element
	| element string
	;

element	: STRING {strcat(dmsStack, (char *)yytext);}
	| SLASH  {strcat(dmsStack, (char *)yytext);}
	| DOT	 {strcat(dmsStack, (char *)yytext);}
	| content
	;

content	: DOLLAR LB1 variable RB1
	| DOLLAR LB2 variable RB2
	| DOLLAR variable
	;

variable: STRING {
		strcpy(varName, (char *)yytext);
		for (i=varCount-1; i>=0; i--) {
			if (!strcmp(var[i].name,varName)) {
				aptr=var[i].next;
				if (dmsIsOn) {
					while (aptr) {
						strcat(dmsStack,aptr->dirLib);
						aptr=aptr->next;
						if (aptr) strcat(dmsStack," ");
					}
				} else {
					ptr->opt=10+aptr->opt;
					strcat(dmsStack,aptr->dirLib);
					aptr=aptr->next;
					while (aptr) {
						NEWDIRLIB(aptr->opt)
						tptr=ptr;
						tptr->next=newElement();
						ptr->opt=10+aptr->opt;
						strcpy(dmsStack,aptr->dirLib);
						aptr=aptr->next;
					}
				}
				break;
			}
		}
		if (i<0) {
			if (getenv(varName)) {
				strcat(dmsStack,getenv(varName));
			} else {
				if (toolCheck||
				   (dmsMatchExpression(tName,toolName.name)||
				   (!strcmp(toolName.name,"*")))) {
				   sprintf(yk_buffer,"%*sLINE: %d UNDEFINED VARIABLE: \"%s\"\n",includeDepth*3,"",dmsLineNumber+1,varName);
				   strcat(resultBuf,yk_buffer);
				   errorCount++;
				}
			} 
		}
	}
	;

%%

int yyerror(char *s)
{
  sprintf(yk_buffer,"%*s*%s* in FILE: %s LINE: %d near CHAR: \"%s\"\n",includeDepth*3,"",s,parseFile,dmsLineNumber+1, (char *)yytext);
  strcat(resultBuf,yk_buffer);
  yyinput(); /* remove the next character yk_buffered, for next smoothly parse */
  errorCount++;
  return(errorCount);
}


struct elem *newElement()
{
  ptr = (OPERATION *) malloc(sizeof(OPERATION));
  ptr->opt = 0;
  ptr->next = NULL;
  ptr->dirLib = NULL;
  ptr->absPath = NULL;
  return(ptr);
}


int dmsYaccParser(char *dmsName)
{
  FILE *lastFile;
  int lastChar,lastLineNumber;
  char lastFileName[BUFSIZE];

  /* initial values for parser to work */
  dmsIsOn=0; toolCheck=1;

  if (!includeDepth) { /* set defaluts to make * always be the last entry */
    DEFAULT(libpath,libpathCount,".",7);
    DEFAULT(worklib,worklibCount,"idmsWorklib",3);
    DEFAULT(workdir,workdirCount,".",7);
    DEFAULT(tconfig,tconfigCount,"",5);
    DEFAULT(rwpaths,rwpathsCount,".",7);
  }

  if (!strcmp(dmsName,ALLPOLICYUSEDEFAULT)) return(0);

  strcpy(lastFileName,parseFile);
  if(!idb_simplify_file_name(dmsName,NULL))
   strcpy(parseFile,dmsName);
  else
   strcpy(parseFile,idb_simplify_file_name(dmsName,NULL));

  includeDepth++;
  if (includeDepth>MAXINCLUDEDEPTH) {
    sprintf(yk_buffer,"%*s*REACH MAXIMUM INCLUDING DEPTH %d*\n",includeDepth*3,"",MAXINCLUDEDEPTH);
    strcat(resultBuf,yk_buffer);
    includeDepth--;
    errorCount++;
    return 1;
  }
  if (includeDepth>maxDepth) maxDepth=includeDepth;
  lastFile = yyin;
  lastChar = yychar;
  lastLineNumber = dmsLineNumber;
  if ((yyin=fopen(parseFile,"r"))==NULL) {
    sprintf(yk_buffer,"%*s*CAN NOT OPEN DMS PATH FILE: %s*\n",includeDepth*3,"",parseFile);
    strcat(resultBuf,yk_buffer);
    yyin = lastFile;
    includeDepth--;
    errorCount++;
    return 1;
  }
  sprintf(yk_buffer,"%*s<<<START PARSING %s>>>\n",(includeDepth-1)*3,"",parseFile);
  strcat(resultBuf,yk_buffer);
  yyparse();
  sprintf(yk_buffer,"%*s<<<END OF PARSING %s>>>\n",(includeDepth-1)*3,"",parseFile);
  strcat(resultBuf,yk_buffer);
  fclose(yyin);
  yyin = lastFile;
  yychar = lastChar;
  dmsLineNumber = lastLineNumber;
  includeDepth--;
  strcpy(parseFile,lastFileName);
	
  return(errorCount);
}


