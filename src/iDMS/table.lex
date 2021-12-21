
%{ 

/*----------------------------------------------------------
 * File : table.lex
 * Date : 07/24/97
 * Redesigned by Sorin Blebea because the original was bad 
 *----------------------------------------------------------*/


#include <string.h>

#ifndef _TABLE_TAB_H
#define _TABLE_TAB_H
#include <table.tab.h>
#endif

#include <dmsC_new.h>
#include <table_functions.h>
#ifdef OH_MACH_LINUX 
#ifndef __cplusplus
static int input YY_PROTO(( void ));
static int yyinput YY_PROTO(( void )) {
  input();
};
#endif
#endif

%}

%{
/*============================================================================*
 * Rules Section                                       
 *============================================================================*/
%}

%%

[#][^\n]*[\n]                 {};
^[ \t]*[\n]                   {};
[ \t]+                        {};
[(][ \t]*[\n]                 {incrTableLineNumber(); return LPAREN; }
[)][ \t]*[\n]                 {incrTableLineNumber(); return RPAREN; }
[\n]                          {incrTableLineNumber(); return NEWLINE; }
[-]                           {return DASH; } 
[A-Za-z0-9][\.\/\:A-Za-z0-9_-]* {if (!strcasecmp((char *)yytext, "table")) return TABLE;
                               else return IDENTIFIER; }
%%

/*============================================================================*
 * Functions used by lex 
 *============================================================================*/

