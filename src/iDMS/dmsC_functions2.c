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
# $Id: dmsC_functions2.c,v 1.1.2.11 1998/05/09 00:12:54 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

/*
//----------------------------------------------------------------------------
//            (C) Copyright Intel Corporation, 1994
//
//        Licensed material --  Program property of Intel
//        Corporation All Rights Reserved
//
//        This program is the   property of Intel  and is
//        furnished  pursuant  to  a    written   license
//        agreement.  It may not  be used, reproduced, or
//        disclosed  to others  except in accordance with
//        the terms and conditions of that agreement.
//
//        Intel Design Technology Copyright reserved 1994
//        Date: 08/06/97
//
//        Authors: Sorin Blebea
//
//        File: dmsC_functions2 -- functions used internally by idms
//                  
//        Whenever possible they were implemented in ANSI C (translated from
//        an ancient K&R very bad written C).
//        All the functions in this file should pass a code review.
//        Some of them are weird because of the huge amount of bad 
//        legacy code inherited.
//----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include <dmsC_defines.h>
#include <fwierr.h>
#include <fwimsg.h>
#include <idb_functions.h>

#include <dmsC_internal_functions.h>
#include <dmsC_functions2.h>
#include <table_functions.h>
#include <os_functions.h>

/*-----------------------*
 * Global variables used *
 *-----------------------*/

#include <dmsC_new_vars.h>

/*------------------*
 * Static variables *
 *------------------*/

/*---------------------------*
 * Internally used functions * 
 *---------------------------*/

int check_for_buff_space(char **Buff, int* lb, int space)
{
   char *new_buff = 0;
   if ((Buff == 0) || (lb == 0)) /* this should never happen! */
      return 0;

   /* protection if *Buff is null, reset *lb */

   if (!*Buff)
      *lb = 0;
   if (*lb >= space)
      return 1; /* enough space -- nothing to do */
  
   new_buff = realloc((void *)*Buff, space);
   if (new_buff == 0)
      return 0; /* cannot allocate */
   *lb = space;
   *Buff = new_buff;  
   return 1;
}


static char *_bempty = "";

char *set_string_in_buff(char **Buff, int* lb, char *content)
{
   if (!content)
      return _bempty; 
   if (!check_for_buff_space(Buff, lb, strlen(content)+1))
      return 0;
   return strcpy(*Buff, content);      
}


char *append_string_to_buff(char **Buff, int *lb, char *content)
{
   char *new_buff = 0;
   int buff_len = 0;
   int new_size = 0;

   /* If any parameter is NULL, get out ! */

   if ((Buff == 0) || (lb == 0))
      return 0;
   if ((content == 0) || (content[0] == '\0'))
      return *Buff;

   /* To correct if someone forgot to clear length when resetting the buffer */

   if (*Buff == 0){
      *lb = 0;
   }
   else{
      buff_len = strlen(*Buff);
   }
   new_size = buff_len + strlen(content) + 1;
   if (*lb < new_size){
      new_buff = realloc((void *)*Buff, new_size);
      if (new_buff == 0) /* not enough space */
         return 0;
      if (*Buff == 0)
         new_buff[0] = '\0';
      *Buff = new_buff;
      *lb = new_size;
   }
   return strcat(*Buff, content);
}


/*-----------------------*
 * Functions definitions *
 *-----------------------*/


/*-----------------------------------------------------------------------
 * dmsPutBackSlashBeforeQuote
 *
 * No static/global vars affected.
 *
 * Description:
 *   Whenever it finds a quote in the input string, adds a backslash in
 *   front of it. If second parameter is non zero, will allocate new
 *   string and return it. Otherwise, make sure you have enough room in the
 *   input string which will be overwritten !!!
 *
 *-----------------------------------------------------------------------*/


char *dmsPutBackSlashBeforeQuote(char *inString, int allocate_new)
{
   char *out_str = (char *)malloc(2*strlen(inString)+1);
   char *ip = inString;
   char *op = out_str;
   if (!inString)
      return 0;
   while (*ip){
     if (*ip == '"')
        *op++ = '\\';
     *op++ = *ip++;
   }
   *op = '\0';
   if (allocate_new)
      return out_str;
   else{
      strcpy(inString, out_str);
      free(out_str);
      return inString;
   }
} 


static char *_fnbuff = 0;
static int _lfnbuff = 0;
static char *_fempty = "";

char *dmsFormFileName(char *dir, char *cName, char *vName, int call_get_ext)
{
   int lname;
   char dirc = osGetDirChar();
   int ldir;

   if (!_fnbuff)
      if (!(_fnbuff = (char *)malloc(_lfnbuff = BUFSIZE)))
      {
         _lfnbuff = 0;
         return 0;
      }
   if (!dir)
      dir = _fempty;
   if (!cName)
      cName = _fempty;

   if (call_get_ext)
      vName = idmGetExtension(vName, 0);
   else
      if (!vName)
         vName = _fempty;

   if ((lname = (ldir = strlen(dir)) + 1 +1 + strlen(cName) + 1 + strlen(vName)
+ 1) >
       _lfnbuff)
      if (!(_fnbuff = (char *)malloc(_lfnbuff = lname +BUFSIZE)))
      {
         _lfnbuff = 0;
         return 0;
      }
   if (!*dir){
         _fnbuff[0] = dirc;
         _fnbuff[1] = '\0';
   }
   else{
      strcpy(_fnbuff, dir);
      if (*(_fnbuff + ldir-1) != dirc){
         _fnbuff[ldir] = dirc;
         _fnbuff[++ldir] = '\0';
      }
   }
   strcpy((char *)(_fnbuff+ldir), cName);
   if (*vName){
      strcat(_fnbuff, ".");
      strcat(_fnbuff, vName);
   }
   return _fnbuff;
}



/*-----------------------------------------------------------------------
 *  matchToolName
 *
 * Parameters:
 *    tvNameList: tool.<view>....
 *    tName     : tool to match
 *
 * Modifies:
 *    Static vars:
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description:  returns 1 if tool name matches
 *-----------------------------------------------------------------------*/

/*does not check null pointers */

int matchToolName(const char *tvNameList, const char *tName)
{
  char *dotpos = strchr(tvNameList, '.');
  if (!dotpos) /* no dots, compare all */
     return strcmp(tvNameList, tName)?0:1;

  if (tvNameList[0] == '.')
     return (tName[0] == '\0');

  return (strncmp(tvNameList, tName, (size_t)(dotpos - tvNameList)))?0:1;
}


/*-----------------------------------------------------------------------
 *  matchvName
 *
 * Parameters:
 *    tvNameList: tool.<view> vn1 vn2 .... vnn
 *    vName
 *
 * Modifies:
 *    Static vars:
 *    Global vars:
 *
 * Uses:
 *    Global var:
 *
 *
 * Description:  returns 1 if format ok and Vname matches <view> or
 *               any of vn1 to vnn
 *               Also returns 1 if no views attached (weird ha?)
 *-----------------------------------------------------------------------*/

char *_mbuff = 0;
int _lmbuff = 0;


int matchvName(const char *tvNameList, const char *vName)
{
  /* this function is written in this weird way to preserve the functionality
     of the bad original */

  char *p = strchr(tvNameList, '.');

  if (!p)
     return 1;
  p++;

  /* positioned here with p on the first of <view> vn1 .... vnn; */

  /* changed to strtok because of itk mystery failure */

  if (!check_for_buff_space(&_mbuff, &_lmbuff, strlen(p)+1))
     return 0;
  strcpy(_mbuff, p);
  p = strtok(_mbuff, " ");
  while (p){
     if (!strcmp(p, vName))
        return 1;
     p = strtok(0, " ");
  }
  return 0;
}


int matchToolvName(const char *tvNameList, const char *tName, const char *vName)
{
  return (matchToolName(tvNameList, tName))?matchvName(tvNameList, vName):0;
}



int match_for_tool_n_version(const INFO* policy, int i, const char *tname,
                             const char *vName)

{
   return ( (!strcmp(tname, "*") || matchToolName(policy[i].name, tname)
             || matchToolName(policy[i].name, "*")
            ) &&
            (!strcmp(vName,"*") || matchvName(policy[i].name, vName)
             || matchvName(policy[i].name,"*")
            )
          );
}


/*-----------------------------------------------------------------------
 * findPolicyEntry
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
 * Description: replaces the ugly macro FINDPOLICYNAME, looks for exact
 *              match then any ver, any tool, any.
 *-----------------------------------------------------------------------*/

/* rewritten for speed */

int findPolicyEntry(const INFO* policy, int policyCount,
                    const char *tName, const char *vName)
{
   int i;
   const char *pn;
   int WildExt = -1;
   int WildTool = -1;
   int WildAll = -1;

   for (i = policyCount -1; i >= 0; i--){
      if (matchToolName((pn = policy[i].name), tName)){ /* possible EX or WE */
         if (matchvName(pn, vName)) /* EX */
            return i; /* found exact match, get out ! */
         if (matchvName(pn, "*")){  /* WE */
            if (WildExt < 0)
                WildExt = i;
            continue;
         }
      }
      if (matchToolName(pn, "*")){ /* possible WT or WA */
         if (matchvName(pn, vName)){/* WT */
            if (WildTool < 0)
               WildTool = i;
            continue;
         }
         if (matchvName(pn, "*")){  /* WA */
            if (WildAll < 0)
               WildAll = i;
            continue;
         }
      }
   }

   /* If we did not return by this point there, wasn't an exact match */
   if (WildExt > 0)
     return WildExt;
   if (WildTool > 0)
     return WildTool;
   if (WildAll > 0)
     return WildAll;

   return 0; /* to preserve all behavior*/
}


/*-----------------------------------------------------------------------
 * dmsSeparateByWhatever
 *
 * No static/global vars affected.
 *
 * Description:
 *    Looks for the first occurence of the character c. If found it will
 *    copy the parts of is buffer into o1 and o2 respectivelly.
 *    It will handle is == o1 OR is == o2 but not o1 == o2 or any overlap
 *    between o1,o2 and is (this is because of its use of strcpy).
 *
 *-----------------------------------------------------------------------*/


void dmsSeparateByWhatever(char c, char *is, char *o1, char *o2)
{
   char *p, *po;
   char *dotpos = strchr(is, c);
   if (!dotpos){ /* No bananas, o2 will be empty and o1 same content as is */
      if (o1 != is)
         strcpy(o1, is);
      *o2 = '\0';
   }
   else{
 
      /* if input == first out buffer, just set the string terminator */

      if (o1 == is)
         *dotpos = '\0';
      else{
         strncpy(o1, is, (size_t)(dotpos-is));
         *(o1+(size_t)(dotpos-is)) = '\0';
      }

      /* if input == second buffer, do not use strcpy because of overlapping */

      if (o2 != is)
         strcpy(o2, dotpos+1);
      else { /* don't use strcpy */
         p  = dotpos+1;
         po = o2;
         while (*p)
            *po++ = *p++;
         *po = '\0';
      }
   }
}


void dmsSeparateByDot(char *inString, char *output1, char *output2)
{
   dmsSeparateByWhatever('.', inString, output1, output2);
}


void dmsSeparateByColon(char *inString, char *output1, char *output2)
{
   dmsSeparateByWhatever(':', inString, output1, output2);
}


void dmsSeparateBySpace(char *inString, char *output1, char *output2)
{
   /* remove leading blanks */

   char *is = inString;
   while ((*is == ' ') && (*is != '\0'))
      is++;
   dmsSeparateByWhatever(' ', is, output1, output2);
}


/*-----------------------------------------------------------------------
 * dmsMatchExpression 
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
 * Description: Matches a reg exp using ancient functions
 *              Should be obsolete. For now just did it faster.
 *-----------------------------------------------------------------------*/

/* prototypes */

char *re_comp(char *);
int   re_exec(char *);

static char _regbuff[BUFSIZE];

int dmsMatchExpression(char *expressionList, char *pattern)
{
   char *p;
   char c;

   if (!expressionList)
      return 0; /* no bananas today */
  
   while (*expressionList)
      if (*expressionList == ' ')
         expressionList++;
      else
         break;
   if (!*expressionList) /* empty list */
      return 0;
  
   /* If we are here, we are positioned on the first non blank char */

   p = _regbuff;
   *p++ = '\\';
   *p++ = '<'; /* mark beginning of word ? */
   while (c = *expressionList++)
      switch(c){
         case '*':
            *p++ = '.';
            *p++ = '*';
            break;
         case '?':
            *p++ = '.';
            break;
         case '.':
            *p++ = '\\';
            *p++ = '.';
            break;
         case ' ':
            /* ignore spaces -- preserve the past behavior !!!! */
            break;
         default:
            *p++ = c;
            break;
      }
   *p++ = '\\';
   *p++ = '>'; /* mark end of word ? */
   *p = '\0';

   re_comp(_regbuff);
   if (re_exec(pattern)) /* bananas */
      return 1; 
   return 0;
}


int  is_wild_exp(const char *exp) 
{
   char c; 
   if (!exp)
     return 0; 
   while (c = *exp++){
     if (c == '*')
       return 1;
     if (c == '?') /* just to preserve ol' behavior */
       return 1;
   } 
   return 0;
}
