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
# $Id: fwitk.h,v 1.1 1996/04/03 22:20:08 sblebea Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#define FWITKON  1
#define FWITKOFF 0

typedef struct design {
  char cellName[1024];
  char viewName[1024];
  int  editBit;
  int  dirtyBit;
  char designDataPtr[1024];
  struct design *next;
} DESIGN;

void getUserInput();
void getUserInputForByLib();
void getUserInputForView();
void readFile();
void writeFile();
DESIGN *initDataStructure();
void freeDesignData();
int confirmWarning();
DESIGN *findDesignInMemory();
