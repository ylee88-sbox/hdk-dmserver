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
# $Id: fwmem.c,v 1.1.14.2 1998/05/08 21:46:11 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
/*
#Revision 1.7  1993/06/22  04:28:07  dhejmad
#*** empty log message ***
#
#Revision 1.6  1993/05/20  17:34:05  dhejmad
#*** empty log message ***
#
#Revision 1.5  1993/05/18  17:56:23  cchiu
#char -> char**, because char did not work.
#
#Revision 1.4  1993/05/18  00:28:00  cchiu
#Change int -> char for portability.
#
#Revision 1.3  1993/05/17  23:54:40  dhejmad
#*** empty log message ***
#
#Revision 1.2  1993/05/14  21:36:28  dhejmad
#*** empty log message ***
#
#Revision 1.1  1993/05/14  17:58:22  dhejmad
#Initial revision
#
*/

/*
** Source code for memory stuff - 
** today it only frees pointers
** Also ONLY pointers allocated 
** with a voMalloc or any other 
** vo call.
*/

#define voUnistd_h
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#if !(defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
#include "itkDB.h" 
#endif

void
dmsVoFreePtrs(int num, ...)
{
  va_list argptr;
  char **ptr;

  va_start(argptr,num);
  for ( ; num; num--) {
    ptr=(char **) va_arg(argptr,char**);
    if (*ptr) {
      free(*ptr);
      *ptr=NULL;
    }
  }
  va_end(argptr);
}

