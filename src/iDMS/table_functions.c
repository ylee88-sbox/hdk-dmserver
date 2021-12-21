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
# $Id: table_functions.c,v 1.1.2.7 1997/12/12 17:37:10 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

/*
 * Clean by Sorin on 07/23/97 
 * 
/* Source code to manage the global file table
**  Author:  Daaman Hejmadi
*/

#include <stdio.h>
#include <string.h>

#include <fwtable.h>
#include <fwimsg.h>
#include <table_functions.h>
#include <dmsC_internal_functions.h>
#include <dmsC_defines.h>
#include <dmsC_new.h>

/*------------------------------------------* 
 * Static variables for the data structures *
 *------------------------------------------*/

static char tableViewNameList[BUFFER_LENGTH];
static char tableExtNameList[BUFFER_LENGTH];
static char tableRegistryList[BUFFER_LENGTH];

/*------------------*
/* Parser variables *
 *------------------*/

extern int ytableparse();
extern char ytabletext[];
extern FILE *ytablein;

/*-----------*
 * Functions *
 *-----------*/

/*-----------------------------------------------------------------------
 * idmReadGlobalFile 
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
 * Description: Reads the file pointed by $IDMS_GLOBALFILE if any.
 * - Note that syntax errors in the file will lead
 * to killing the process - this is due to the
 * sensitive nature of this table - any mistake could
 * corrupt the database 
 *-----------------------------------------------------------------------*/

int idmReadGlobalFile()
{
   FILE *fp;
   char *globalFileName;

    initTableParserVars();
   if((globalFileName = dmsAppGetEnv("IDMS_GLOBALFILE")) == NULL)
      return idmsError;

   if ((fp = fopen(globalFileName, "r")) == 0)
      return idmsError;
    
   ytablein = fp;
   /* set the global variables for the parser */
   if ( ytableparse() != idmsSuccess) 
	return idmsError;

    fclose(fp);

    /* now that the parsing is complete check each predefined tables
    ** for correctness */
    return(idmCheckDefinedTables());
}

/* This procedure will return the appropriate 
** column corresponding to the given table and
** the key and column number.
** All it does is walk thru the in-memory table 
** looking for the table, then walks through the 
** rows looking for the key, finally walk through
** the columns till it finds the 'column'nth column
** NOTE: This is obviously not useful for tables
**   that do not have unique first columns (multi-
**   Valued keys cannot be found).
*/

int idmGetColumn(char *table, char *key, int column, char **val)
{
    idmTable *t;
    idmTableRowEntry *r;
    idmTableColumnEntry *c;
    int i;

    *val[0] = '\0';
    if ((table == NULL) || (key == NULL) || (column == 0))
       return(idmsBadparameter);

    t = (idmTable *)getGlobalTablePtr();
    while ((t != NULL) && (strcmp(table,t->name) != 0))
       t = t->nextTable;

    if (t == NULL)
      return(idmsError);

    r = t->table;
    while ((r != NULL) && (strcmp(key,r->columns->valueList) != 0)) 
		r = r->nextRow;

    if (r == NULL)
       return(idmsError);

    c = r->columns->nextColumn;

    for (i=2; i<column; ++i) 
       if (c != NULL) 
          c = c->nextColumn;

    if (c == NULL)
       return(idmsError);

    if(c->valueList == NULL) {
      return(idmsError);
    } else {
      strcpy(*val, c->valueList);
    }
    return(idmsSuccess);
}

/* these two procedures will read/wite the table named by 
** IDMS_VIEWTYPE_VIEWNAME_TABLE, and return the view
** name list associated with it. The calling application
** will have to parse the string based on the colon.
*/

int idmGetViewName(char *toolName, char *viewType, char **viewNameList)
{
    idmTable *tablePtr;
    int row = 1;
    char *contents, *token;
    Boolean done = FALSE;

    tableViewNameList[0] = '\0';
    *viewNameList = tableViewNameList;

    if (dmsCGetTable(IDMS_VIEWTYPE_VIEWNAME_TABLE,
		     &tablePtr) != idmsSuccess) {
	/* table does not exist viewName == viewType */
	if (strlen(viewType) >= BUFFER_LENGTH) {
		dmsCPrintWarning("viewType exceeds buffer length %d",BUFFER_LENGTH);
		return(idmsError);
	}
	strcpy(tableViewNameList,viewType);
	return(idmsSuccess);
    }
    
    while (!done) {
	if (dmsCGetTableEntry(tablePtr,row,&contents) != idmsSuccess) {
	    /* entry not found in table - viewName = viewType */
		if (strlen(viewType) >= BUFFER_LENGTH) {
			dmsCPrintWarning("viewType exceeds buffer length %d",BUFFER_LENGTH);
			return(idmsError);
		}
	    strcpy(tableViewNameList,viewType);
	    return(idmsSuccess);
	}
	token = strtok(contents," ");
	if (strcmp(toolName, token) == 0) {
	    /* check for viewType */
	    token = strtok(NULL," ");
	    if (strcmp(token, viewType) == 0)
		done = TRUE;
	}
	row++;
    }
    strcpy(tableViewNameList,strtok(NULL," "));
    return(idmsSuccess);
}



int idmSetViewName(char *toolName, char *viewType, char *viewNameList)
{
/* look to see if this combination 
** already exists - if it does then modify else
** put it in
*/

    idmTable *tablePtr;
    idmTableRowEntry *r;
    char *c;
    Boolean done = FALSE;

    if ((toolName == NULL) || (strlen(toolName) == 0) ||
	(viewType == NULL) || (strlen(viewType) == 0) ||
	(viewNameList == NULL) || (strlen(viewNameList) == 0))
	return(idmsBadparameter);

    if (dmsCGetTable(IDMS_VIEWTYPE_VIEWNAME_TABLE,
		     &tablePtr) != idmsSuccess)
	return(idmsError);
    r = tablePtr->table;
    /* go till you find the right spot */
    while ((r!=NULL) && (!done)) {
	if ((strcmp(r->columns->valueList,toolName) == 0) &&
	    (strcmp(r->columns->nextColumn->valueList,viewType) == 0))
	    done = TRUE;
	else
	    r = r->nextRow;  
    }

    if (done) {
	/* replace the entry here */
	c = (char *) malloc(strlen(viewNameList)+1);
	strcpy(c,viewNameList);
	free(r->columns->nextColumn->nextColumn->valueList);
 	r->columns->nextColumn->nextColumn->valueList = c;
	return(idmsSuccess);
    }
    return(idmsError);
}


/*-----------------------------------------------------------------------
 * idmGetExtension 
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
 * Description: this procedure will read the table named by 
 *              IDMS_VIEWNAME_EXT_TABLE, and return the extension
 *              associated with the viewName.
 *   
 *              If ext_buff is 0; returns a static buffer pointer
 *              If viewName not found in table returns viewName
 *-----------------------------------------------------------------------*/

static char *tableExtensionName = 0;

char *idmGetExtension(char *viewName, char *ext_buff)
{
   if (!tableExtensionName)
     if (!(tableExtensionName = (char *)malloc(BUFFER_LENGTH)))
        return 0;

   if (idmGetColumn(IDMS_VIEWNAME_EXT_TABLE, viewName, 2, &tableExtensionName)
      != idmsSuccess){
      if (strlen(viewName) >= BUFFER_LENGTH) {
         dmsCPrintWarning("ViewName %s exceeds buffer length %d\n", 
                           viewName, BUFFER_LENGTH);
         return 0;
      }
      if (!ext_buff) 
         return strcpy(tableExtensionName, viewName);
      else
         return strcpy(ext_buff, viewName);
   }
   else{ /* view found in table */ 
      if (!ext_buff)
        return tableExtensionName;
      else
        return strcpy(ext_buff, tableExtensionName);
   }
}


/* procedure which will walk through the table and
** print it out
*/
void idmPrintGlobalTable()
{
    idmTable *t;
    idmTableRowEntry *r;
    idmTableColumnEntry *c;

    t = (idmTable *)getGlobalTablePtr();
    while (t != NULL) {
	printf("---------Table Name = %s-----------\n",t->name);
	r = t->table;
	while (r != NULL) {
	    c = r->columns;
	    while (c != NULL) {
		if (c->valueList) 
		    printf("%s -> ",c->valueList);
		else 
		    printf("NULL -> ");
		c = c->nextColumn;
	    }
	    printf("\n");
	    r = r->nextRow;
	}
	printf("---------End of Table %s -----------\n\n\n",t->name);
	t = t->nextTable;
    }
}


/* procedure to check if every row in the specified
** table has the specified number of columns
*/
int idmCheckColumn(char *name, int columns)
{
    idmTable *tablePtr;
    idmTableRowEntry *row;
    idmTableColumnEntry *col;
    int i;
    char temp[2048];
    Boolean error = FALSE;

    /* table not defined - all is well */
    if (dmsCGetTable(name,&tablePtr) != idmsSuccess)
	return(idmsSuccess);

    /* go through the table */
    row = tablePtr->table;
    while (row != NULL) {
	col = row->columns;
	i = 0;
	while (col != NULL) {
	    ++i;
	    col = col->nextColumn;
	}
	if (i != columns) {
	    /* print an error */
	    error = TRUE;
	    col = row->columns;
	    temp[0] = '\0';
	    while (col != NULL) {
		if ((strlen(temp)+strlen(col->valueList))>=BUFFER_LENGTH) {
			dmsCPrintWarning("Buffer Length %d exceeded in idmCheckColumn\n",
				BUFFER_LENGTH);
			return(idmsError);
		}
		sprintf(temp,"%s %s", temp, col->valueList);
		col = col->nextColumn;
	    }
	    dmsCPrintWarning("Table %s should have %d rows::%s\n",
		   name,columns,temp);
	}
	row = row->nextRow;
    }
    return((error ? idmsError : idmsSuccess));
}


/* procedure which will check all the entries of the predefined 
** idms tables and warn the users about errors/inconsistencies
** if any.
** If any of the tables has an error the procedure returns an
** error, also it always checks ALL the tables.
*/
int idmCheckDefinedTables()
{
    Boolean error = FALSE;

    /* check the attribute control table */
    if (idmCheckColumn(IDMS_ATTRIBUTE_TABLENAME,
		       IDMS_ATTRIBUTE_TABLE_COLUMNS) != idmsSuccess)
	error = TRUE;
    /* check the gen attribute table */
    if (idmCheckColumn(IDMS_GENERATED_ATTR_TABLE,
		       IDMS_GENERATED_ATTR_TABLE_COLUMNS) != idmsSuccess)
	error = TRUE;
    /* check the user trigger table */
    if (idmCheckColumn(IDMS_USERTRIGGER_TABLE, 
		       IDMS_USERTRIGGER_TABLE_COLUMNS) != idmsSuccess)
	error = TRUE;
    /* check the viewtype - namelist table */
    if (idmCheckColumn(IDMS_VIEWTYPE_VIEWNAME_TABLE,
		       IDMS_VIEWTYPE_VIEWNAME_TABLE_COLUMNS) != idmsSuccess)
	error = TRUE;
    /* check the viewname - ext table */
    if (idmCheckColumn(IDMS_VIEWNAME_EXT_TABLE,
		       IDMS_VIEWNAME_EXT_TABLE_COLUMNS) != idmsSuccess)
	error = TRUE;
   
    /* check the dfII registry table */
    if (idmCheckColumn(IDMS_REGISTRY_TABLE,
		       IDMS_REGISTRY_TABLE_COLUMNS) != idmsSuccess)
        error = TRUE;

    /* check the iDMSIntelExtTable table */
    if (idmCheckColumn(IDMS_EXTTYPE_EXTNAME_TABLE,
 		       IDMS_EXTTYPE_EXTNAME_TABLE_COLUMNS) != idmsSuccess)
        error = TRUE;

    return((error ? idmsError : idmsSuccess));
}

/* procedure to get the viewName from the Write
** table IDMS_WRITE_VIEWNAME_TABLE. It is
** exactly like the read viewName procedure
** except the column should be a 1 element
** list (NOT a switch list).
*/
int idmGetWriteViewName(char *toolName, char *viewType, char **viewName)
{
    idmTable *tablePtr;
    int row = 1;
    char *contents, *token;
    Boolean done = FALSE;

    if (dmsCGetTable(IDMS_WRITE_VIEWNAME_TABLE,
		     &tablePtr) != idmsSuccess)
	return(idmsError);

    tableViewNameList[0] = '\0';
    *viewName = tableViewNameList;
    while (!done) {
	if (dmsCGetTableEntry(tablePtr,row,&contents) != idmsSuccess) {
	    /* entry not found viewType == viewName */
	    strcpy(tableViewNameList,viewType);
	    return(idmsSuccess);
	}
	token = strtok(contents," ");
	if (strcmp(toolName, token) == 0) {
	    /* check for viewType */
	    token = strtok(NULL," ");
	    if (strcmp(token, viewType) == 0)
		done = TRUE;
	}
	row++;
    }

    strcpy(tableViewNameList,strtok(NULL," "));
    return(idmsSuccess);
}


/* procedure to get the extNameList from the 
** table IDMS_EXTTYPE_EXTNAME_TABLE. 
*/
int idmGetExtName(char *toolName, char *extType, char **extNameList)
{
    idmTable *tablePtr;
    int row = 1;
    char *contents, *token;
    Boolean done = FALSE;

    tableExtNameList[0] = '\0';
    *extNameList = tableExtNameList;

    if (dmsCGetTable(IDMS_EXTTYPE_EXTNAME_TABLE,
                     &tablePtr) != idmsSuccess) {
        /* table does not exist extName == extType */
        if (strlen(extType) >= BUFFER_LENGTH) {
                dmsCPrintWarning("viewType exceeds buffer length %d",BUFFER_LENGTH);
                return(idmsError);
        }
        strcpy(tableExtNameList,extType);
        return(idmsSuccess);
    }

    while (!done) {
        if (dmsCGetTableEntry(tablePtr,row,&contents) != idmsSuccess) {
            /* entry not found in table - extName = extType */
                if (strlen(extType) >= BUFFER_LENGTH) {
                        dmsCPrintWarning("viewType exceeds buffer length %d",BUFFER_LENGTH);
                        return(idmsError);
                }
            strcpy(tableExtNameList,extType);
            return(idmsSuccess);
        }
        token = strtok(contents," ");
        if (strcmp(toolName, token) == 0) {
            /* check for extType */
            token = strtok(NULL," ");
            if (strcmp(token, extType) == 0)
                done = TRUE;
        }
        row++;
    }
    strcpy(tableExtNameList,strtok(NULL," "));
    return(idmsSuccess);
}


/*-----------------------------------------------------------------------
 * idmGetPattern
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
 * Description: this procedure will read the table named by
 *              IDMS_REGISTRY_TABLE, and return the pattern 
 *              associated with the viewName (dfII viewtype).
 *
 *              If ext_buff is 0; returns a static buffer pointer
 *              If viewName not found in table returns viewName.dat
 *-----------------------------------------------------------------------*/

static char *tablePatternName=0;

char *idmGetPattern(char *viewName, char *pattern_buff)
{
   if (!tablePatternName)
     if (!(tablePatternName = (char *)malloc(BUFFER_LENGTH)))
        return 0;

   if (idmGetColumn(IDMS_REGISTRY_TABLE, viewName, 2, &tablePatternName)
      != idmsSuccess){
      if (strlen(viewName)+4 >= BUFFER_LENGTH) {
         dmsCPrintWarning("ViewName %s exceeds buffer length %d.dat\n",
                           viewName, BUFFER_LENGTH);
         return 0;
      }
      if (!pattern_buff) {
         strcpy(tablePatternName, viewName);
  	 return strcat(tablePatternName, ".dat");
      } else {
         strcpy(pattern_buff, viewName);
	 return strcat(pattern_buff, ".dat");
      }
   }
   else{ /* view found in table */
      if (!pattern_buff)
        return tablePatternName;
      else
        return strcpy(pattern_buff, tablePatternName);
   }
}


