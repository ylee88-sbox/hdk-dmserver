%{ 

/*----------------------------------------------------------
 * File : table.yacc
 * Date : 07/24/97
 * Redesigned by Sorin Blebea because the original was bad 
 *----------------------------------------------------------*/

/* Use default yywrap from -lfl */
int ytablewrap(void){
  int yywrap(void);
  return yywrap();
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fwtable.h>
#include <fwimsg.h>
#include <table_functions.h>
#include <dmsC_defines.h>
#include <dmsC_new.h>

/* defined by lexer */

extern char *ytabletext;
#define yytext ytabletext
#ifdef __cplusplus
extern "C"
#endif
int yylex();

/*------------------*
 * Static variables *
 *------------------*/

static idmTable *idmGlobalTablePtr = 0;
static idmTable *lastTblPtr = 0;
static int tableLineNumber = 1;
static no_errors = 0;
static idmTableColumnEntry *firstCol;
static idmTableColumnEntry *lastCol;

/*-----------------------------*
 * Static functions prototypes *
 *-----------------------------*/

static void add_table(char *name);
static void add_elem_to_crtrow(char *value);
static int lt(idmTableRowEntry *first, idmTableRowEntry *second);
static void add_row_to_table();

%}

/*--------*
 * Tokens *
 *--------*/

%token TABLE
%token IDENTIFIER
%token LPAREN
%token RPAREN
%token NEWLINE
%token DASH

%%

/*-------*
 * Rules *
 *-------*/

s : s tbl {;}
  | {;}
  ;

tbl : TABLE IDENTIFIER { add_table(yytext); } LPAREN tblrows RPAREN

tblrows: tblrows row {add_row_to_table(); } NEWLINE {;}
       | {;}
       ;

row : row value {;}
    | {;}
    ;

value: IDENTIFIER {add_elem_to_crtrow(yytext); }
    | DASH {add_elem_to_crtrow(0); }
    ;

	
%%

/*------------------------*
 * Functions used by yacc *
 *------------------------*/

/*........*
 * STATIC *
 *........*/


static void add_table(char *name)
{
   idmTable *newt = (idmTable *)malloc(sizeof(idmTable));
#if 0
   printf("\nnew table: %s\n", name);
#endif
   firstCol = lastCol = 0;
   if (!name)
     newt->name = strdup("");
   else
     newt->name = strdup(name);
   newt->table = 0;
   newt->nextTable = 0;
   if (!idmGlobalTablePtr)
      idmGlobalTablePtr = newt;
   if (lastTblPtr)
      lastTblPtr->nextTable = newt;
   lastTblPtr = newt;
}


static void add_elem_to_crtrow(char *value)
{
   idmTableColumnEntry *newc = (idmTableColumnEntry *)malloc(
                                   sizeof(idmTableColumnEntry));
   newc->nextColumn = 0;
   if (!value)
     newc->valueList = 0;
   else
     newc->valueList = strdup(value);
#if 0
   if (!value)
     printf("...value = -\n");
   else
     printf("...value = %s\n", value);
#endif
   if (!firstCol)
     firstCol = newc;
   if (lastCol)
     lastCol->nextColumn = newc;
   lastCol = newc;
}


/* Returns 1 ONLY if first less then second */

static int lt(idmTableRowEntry *first, idmTableRowEntry *second) 
{
   int ready = 0;
   int ret_code = 0;
   idmTableColumnEntry *fc, *sc;
   int comp_result;
  
   if (!first) 
     return (second)?1:0;
   if (!second)
     return 0;

   /* here both pointers are ok */

   
   fc = first->columns;
   sc = second->columns; 
   /* finally here, the pointers to colums are non null */

   do{ 
      ready = (!fc) || (!sc);
#if 0
      printf("ready = %d\n", ready);
#endif
      if (!fc)
         return (sc)?1:0;
      if (!sc)
         return 0; 

      /* Here we have valid pointers to both current columns */

      if (!fc->valueList)
        if (sc->valueList)
           return 1;
      if (!sc->valueList)
           return 0;     

      if ((fc->valueList) && (sc->valueList)){
#if 0
         printf("comparing %s with %s\n", fc->valueList, sc->valueList);
#endif
         if ((comp_result = strcmp(fc->valueList, sc->valueList)) != 0){
            if (comp_result == -1)
               return 1;
            else
               return 0;
         }
      }
      if (fc)
         fc = fc->nextColumn;
      if (sc)
         sc = sc->nextColumn;

   }while (!ready);

   /* If we are here, it is not lower */

   return 0;
}


static void add_row_to_table()
{
   idmTableRowEntry *p1, *p2;
   idmTableRowEntry *newr = (idmTableRowEntry *)malloc(
                             sizeof(idmTableRowEntry));
   newr->columns = firstCol;
   newr->nextRow = 0;
#if 0
   printf(" *new row formed\n");
#endif
   if (!lastTblPtr)
      return;
   if (!(p1 = lastTblPtr->table))
      lastTblPtr->table = newr;
   else{
      if (lt(newr, lastTblPtr->table)){ /* to be inserted first */
         newr->nextRow = lastTblPtr->table;
         lastTblPtr->table = newr;
      }
      else{
         p2 = p1->nextRow;
         while (p2){
            if (!lt(newr, p2)){
               p1 = p1->nextRow;
               p2 = p2->nextRow;
            } 
            else
               break; 
         }   
         p1->nextRow = newr;
         newr->nextRow = p2; 
      }    
   }
   firstCol = lastCol = 0;
}


/*............*
 * EXPORTABLE *
 *............*/

int yyerror(char *s)
{
   dmsCPrintWarning("Syntax Error in table file: Line #%d: %s near \"%s\"\n",
   getTableLineNumber(), s, yytext);
   incrTableErrors();
   return 1;
}


void initTableParserVars()
{
   /* Should deallocate first if no null pointer; */
 
   idmGlobalTablePtr = 0;
   lastTblPtr = 0;
   tableLineNumber = 1;
   no_errors = 0;
}


const idmTable* getGlobalTablePtr()
{
   return idmGlobalTablePtr;
}


idmTable* setGlobalTablePtr(const idmTable* value)
{
   return idmGlobalTablePtr = (idmTable *)value;
}


int setTableLineNumber(int num) 
{
   return tableLineNumber = num;
}

int getTableLineNumber()
{
   return tableLineNumber;
}


int incrTableLineNumber()
{
   return ++tableLineNumber;
}


int incrTableErrors()
{
   return ++no_errors;
}


int getNoTableErrors()
{
   return no_errors;
}

