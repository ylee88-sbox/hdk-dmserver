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
# $Id: fwitktrack.c,v 1.1.4.1 1997/04/03 20:27:47 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fwierr.h"
#include "fwidms.h"
/*#include "fwitk.h"*/
#include "fwitktrack.h"

void readFile(char *designDataPtr,char *fullPathName);

void printTrack(ITKTRACK *tracker)
{
    ITKTRACK *current=tracker;

    while (current){
	printf("Info=>  cVName:%s\n        lLoc:%s\n        lCR:%s\n",
	       current->cellViewName,current->libLocation,current->libCurrentRev
	      );
        printf("        lData:%s\n        wdLocation:%s\n        wdData:%s\n",
	       current->libData,current->wdLocation,current->wdData);
	printf("        memData:%s\n",current->memData);
	printf("Flags=> stat:%c cCreat:%d unixF:%d co:%d ro:%d wdDatV:%d\n",
	       current->status,current->cellCreated,current->unixFile,
	       current->checkedOut,current->readOnly,current->wdDataValid);
        printf("        inMem:%d memEB:%d memDB:%d error:%d\n\n",current->inMem,
	       current->memEditBit,current->memDirtyBit,current->errorCode);
	current = current->next;
    }
}

ITKTRACK *trackCons(ITKTRACK *node,ITKTRACK *list)
{
    node->next = list;
    return(node);
}

ITKTRACK *findCellView(ITKTRACK *tracker,char *cellName,char *viewName)
{
    char *cellViewName=(char *)malloc((strlen(cellName)+strlen(viewName)+1)*
				      sizeof(char));
    ITKTRACK *current=tracker;

    sprintf(cellViewName,"%s%s",cellName,viewName);
    while (current!=NULL&&strcmp(current->cellViewName,cellViewName)!=0)
	current=current->next;
    return(current);
}

ITKTRACK *buildCell(char *cellName,char *viewName)
{
    DMSINFO *info;
    char *temp=(char *)malloc(40000*sizeof(char));
    ITKTRACK *node=(ITKTRACK *)malloc(sizeof(struct itkTrackStruct));
    
    node->readOnly = FALSE;
    node->inMem = FALSE;
    node->memEditBit = FALSE;
    node->memDirtyBit = FALSE;
    node->errorCode = FALSE;
    node->checkedOut = FALSE;    /* Only a precaution */
    node->libLocation = NULL;
    node->libCurrentRev = NULL;
    node->libData = NULL;
    node->wdData = NULL;
    node->memData = NULL;
    node->cellViewName=(char *)malloc((strlen(cellName)+strlen(viewName)+1)*
				      sizeof(char));
    sprintf(node->cellViewName,"%s%s",cellName,viewName);
    (void) dmsCInfoCellView(1,cellName,viewName,&info);
    if (!strcmp(info->fullPathName,"")){
	node->libLocation = nullifyString(node->libLocation);
	node->libData = nullifyString(node->libData);
	node->libCurrentRev = nullifyString(node->libCurrentRev);
	node->cellCreated = FALSE;
	node->status = '?';
	node->unixFile = FALSE;
	node->checkedOut = FALSE;
    }else{
	node->libLocation=(char *)malloc((strlen(info->fullPathName)+1)*
					 sizeof(char));
	strcpy(node->libLocation,info->fullPathName);
	node->libCurrentRev=(char *)malloc((strlen(info->verName)+1)*
					   sizeof(char));
	strcpy(node->libCurrentRev,info->verName);
	readFile(temp,node->libLocation);
	node->libData=(char *)malloc((strlen(temp)+1)*sizeof(char));
	strcpy(node->libData,temp);
	node->status = '?';
    }
    
    /* build info on cell in work directory */
    (void) dmsCInfoCellView(0,cellName,viewName,&info);
    if (node->libLocation && strcmp(node->libLocation,info->fullPathName)){
	node->wdLocation=(char *)malloc((strlen(info->fullPathName)+1)*
					sizeof(char));
	strcpy(node->wdLocation,info->fullPathName);
    } else {
	node->wdLocation=(char *)malloc((strlen("")+1)*sizeof(char));
	strcpy(node->wdLocation,"");
    }
    if (!strcmp(node->wdLocation,"")){
	node->wdDataValid = FALSE;
	node->wdData = nullifyString(node->wdData);
	node->unixFile = FALSE;
    } else {
	node->wdDataValid = TRUE;
	readFile(temp,node->wdLocation);
	node->wdData = (char *)malloc((strlen(temp)+1)*sizeof(char));
	strcpy(node->wdData,temp);
	node->unixFile = TRUE;
    }
    node->memData = nullifyString(node->memData);
    node->next = NULL;
    free(temp);
    return(node);
}
/*
void
readFile(designDataPtr,fullPathName)
char *designDataPtr;
char *fullPathName;
{
  FILE *fp;

  if (fp=fopen(fullPathName,"r")) {
    fscanf(fp,"%s",designDataPtr);
    fclose(fp);
  } else
    printf("Can not read file: %s\n",fullPathName);
}
*/
void updateTrackNode(ITKTRACK *tracker,char *cellName,char *viewName,int libOnly)
{
    DMSINFO *info;
    FILE *dummyFp=NULL;
    ITKTRACK *current = findCellView(tracker,cellName,viewName);
    char *temp=(char *)malloc(256*sizeof(char));

    if (current==NULL){
	printf("For some reason current is NULL trying to find %s, %s\n",
		cellName,viewName);
	return;
    }
    if (libOnly==0){      /* update work directory information */
	dmsCInfoCellView(libOnly,cellName,viewName,&info);
	current->wdLocation=changeString(current->wdLocation,info->fullPathName);
	dummyFp = fopen(current->wdLocation,"r");
	if (!dummyFp)
	    current->wdData = nullifyString(current->wdData);
	else{
	    fclose(dummyFp);
	    readFile(temp,current->wdLocation);
	    current->wdData=changeString(current->wdData,temp);
	    current->wdDataValid = TRUE;
	}
    }else{
	dmsCInfoCellView(libOnly,cellName,viewName,&info);
	current->libLocation=changeString(current->libLocation,
					  info->fullPathName);
	current->libCurrentRev=changeString(current->libCurrentRev,info->verName);
	if (!dummyFp)
	    current->libData = nullifyString(current->libData);
	else{
	    fclose(dummyFp);
	    readFile(temp,current->libLocation);
	    current->libData=changeString(current->libData,temp);
	}
    }
}

char *changeString(char *dest,char *source)
{
    if (dest!=NULL)
	free(dest);
    dest=(char *)malloc((strlen(source)+1)*sizeof(char));
    strcpy(dest,source);
    return(dest);
}

char *nullifyString(char *name)
{
    if (name!=NULL)
	free(name);
    name=(char *)malloc((strlen("")+1)*sizeof(char));
    strcpy(name,"");
    return(name);
}

/*int verifyCommand(char command,char *cellName,char *viewName,ITKTRACK *tracker)
{
    ITKTRACK *current=findCellView(tracker,cellName,viewName);
    DMSINFO *info;
    int error;

    switch(command){
    case 'c': 
	error = dmsCInfoCellView(1,cellName,viewName,&info);
	switch(current->errorCode) {
	case 83 : 
	
	    
    }
}
*/
