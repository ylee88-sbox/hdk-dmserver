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
# $Id: fwitktrack.h,v 1.1.4.1 1997/04/03 20:27:48 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
typedef struct itkTrackStruct {
    int status,cellCreated,unixFile,checkedOut,readOnly,wdDataValid,inMem,
        memEditBit,memDirtyBit,errorCode;
    char *cellViewName,*libLocation,*libCurrentRev,*libData,*wdLocation,*wdData,
         *memData;
    struct itkTrackStruct *next;
} ITKTRACK;

typedef struct itkCommandStruct {
    int command;
    char *cellViewName,*data;
} ITKCOMMAND;

#define TRUE 1
#define FALSE 0
#define NOERROR -3000

void printTrack(ITKTRACK *tracker);
ITKTRACK *trackCons(ITKTRACK *node,ITKTRACK *list);
ITKTRACK *findCellView(ITKTRACK *tracker,char *cellName,char *viewName);
ITKTRACK *buildCell(char *cellName,char *viewName);
void updateTrackNode(ITKTRACK *tracker,char *cellName,char *viewName,int libOnly);
char *changeString(char *dest,char *source);
char *nullifyString(char *name);


