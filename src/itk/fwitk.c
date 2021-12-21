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
# $Id: fwitk.c,v 1.1.2.2.2.1.10.12 1997/12/12 22:18:24 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
/*

#Revision 1.4  93/02/01  10:11:32  cchiu
#Add \n to write file and read file so that the file on disc will be complete.
#Otherwise the reverse diff of opus will fail.
#
# COMMENTS DELETED
#
#Revision 1.2  92/09/22  18:04:31  cchiu
#Initial version. For demostration of iDMS integration.
#*/

#ifdef __cplusplus
extern "C" {
#endif
#ifndef DISABLE_SYNCHRONICITY
#include "scapi.h"
#endif
#ifdef __cplusplus
}
#endif
/** #include "itkDB.h" **/
#define voRandom_h
#include <voTimeCorrection.h>
/** #include <fwTypes.h> **/
#include <sys/time.h>
#include <sys/timeb.h>
#include <stdio.h>
#include "fwidms.h"
#include "fwierr.h"
/** #include "iitkDA.h" **/
#include "fwitk.h"
#include "fwitktrack.h"
#include <idb_functions.h>
#include <string.h>

struct timeval tp;
struct timezone tzp;
long sec_in, usec_in, sec_out, usec_out;
int iFlag,noPrintFlag=FWITKOFF,yesFlag;

#ifdef HPPA11
//void * __nwa (unsigned int len)
//{
// long llen=len;
// return ::operator new (llen);
//}

//__ConstructMarkedArray(void *,unsigned int,unsigned int,void (*)(),void (*)())


#endif

/* Modified by Sorin not to call system() any more */

void dumpFileContentsToLog(char *cellName, char *viewName)
{ 
   FILE *fo;
   static char ibuf[1024];
  
   char fileName[256];
   DMSINFO *info;
   int infoNum;

   infoNum = dmsCInfoCellView(0,cellName,viewName,&info);
   if (info&&!strcmp(info->fullPathName,"")) 
      return;
   strcpy(fileName,info->fullPathName);
   printf("***Contents of %s:",fileName);
   fflush(stdout);
   if (!(fo = fopen(fileName, "r")))
      printf("<CANNOT OPEN FILE>\n");
   else{
      while (!feof(fo)){
         if (!fgets(ibuf, 1024, fo))
            break;
         *(ibuf+strlen(ibuf)-1) = '\0';
         printf("%s\n", ibuf);
      }
   }  
   printf("***End of Contents\n");
   fflush(stdout);
}


void timeBegin()
{
  gettimeofday(&tp, &tzp);
  sec_in = tp.tv_sec;
  usec_in = tp.tv_usec;
}

void timeEnd()
{
  gettimeofday(&tp, &tzp);
  sec_out = tp.tv_sec;
  usec_out = tp.tv_usec;
}

float timeElapsed()
{
    return((sec_out - sec_in) + ((usec_out - usec_in) / 1000000.0));
}

/*****************************************************************************
  Get user's inputs.
  In real environment, tools might get these information from internal data
  structure, by user's ciw window input, mouse clicking or form filling.
******************************************************************************/
void getUserInput(char *option, char *cellName, char *viewName)
{
    if (option[0]=='1') {
	if (iFlag==FWITKON)
	    printf("cell name: ");
	scanf("%s",cellName);
    }
    if (option[1]=='1') {
	if (iFlag==FWITKON)
	    printf("view name: ");
	scanf("%s",viewName);
    }
    if (option[0]=='2') {
	if (iFlag==FWITKON)
	    printf("tool name: ");
	scanf("%s",cellName);
    }
    if (option[1]=='2') {
	if (iFlag==FWITKON)
	    printf("policy name: ");
	scanf("%s",viewName);
    }
    printf("\n");
}

void getAttributeInfo(int option,char *attrName,char *attrType,char *attrVal)
{
    
    if (iFlag==FWITKON)
	printf("attribute name: ");
    scanf("%s",attrName);
    
    if (option==1) {
	if (iFlag==FWITKON)
	    printf("attribute type: ");
	scanf("%s",attrType);
	if (iFlag==FWITKON)
	    printf("attribute value: ");
	scanf("%s",attrVal);
	printf("\n");
    }
}

void getUserInputForByLib(char *option,char *cellName,char *viewName, char *libName)
{
    if (option[0]=='2') {
	if (iFlag==FWITKON)
	    printf("cell name: ");
	scanf("%s",cellName);
    }
    if (option[1]=='1') {
	if (iFlag==FWITKON)
	    printf("view name: ");
	scanf("%s",viewName);
    }
    if (option[0]=='2') {
        if (iFlag==FWITKON)
            printf("library name: ");
        scanf("%s",libName);
    }
    
    printf("\n");
}

void getUserInputForView(char *viewName)
{
    if (iFlag==FWITKON)
    printf("view name: ");
    scanf("%s",viewName);
    printf("\n");
}

void getUserInputForToolName(char *toolName)
{
    if (iFlag==FWITKON)
    printf("tool name: ");
    scanf("%s",toolName);
    printf("\n");
}
/*****************************************************************************
  Open a file and read the data into the memory.
******************************************************************************/
void readFile(char *designDataPtr,char *fullPathName)
{
    FILE *fp;

    if ((fp=fopen(fullPathName,"r"))) {
	fscanf(fp,"%s",designDataPtr);
	fclose(fp);
    } else printf("Can not read file: %s\n",fullPathName);
}
  

/*****************************************************************************
  Open a file and write the data into the file.
******************************************************************************/
void writeFile(char *designDataPtr,char *fullPathName)
{
    FILE *fp;

    if ((fp=fopen(fullPathName,"w"))) {
	fprintf(fp,"%s\n",designDataPtr);
	fclose(fp);
    } else printf("Can not write file: %s\n",fullPathName);
}

/*****************************************************************************
  Initialize the data structure for storing data in memory
******************************************************************************/
DESIGN *initDataStructure(DESIGN **firstDesign,char *cellName,char *viewName)
{
    DESIGN *ptr=(DESIGN *) malloc(sizeof(DESIGN));
    
    strcpy(ptr->cellName,cellName);
    strcpy(ptr->viewName,viewName);
    strcpy(ptr->designDataPtr,"");
    ptr->editBit=0;
    ptr->dirtyBit=0;
    ptr->next=*firstDesign;
    *firstDesign=ptr;
    return(*firstDesign);
}

/*****************************************************************************
  Free memory used by design data
******************************************************************************/
void
freeDesignData(DESIGN **firstDesign,DESIGN **currentDesign)
{
    DESIGN *ptr;
    
    ptr=*firstDesign;
    if (ptr==*currentDesign) {
	*firstDesign=(*currentDesign)->next;
	free(*currentDesign);
	*currentDesign=*firstDesign;
	return;
    }
    while (ptr) {
	if (ptr->next==*currentDesign) {
	    ptr->next=ptr->next->next;
	    free(*currentDesign);
	    *currentDesign=ptr;
	    return;
	}
	ptr=ptr->next;
    }
}
  
/*****************************************************************************
  Print out warning message and return TRUE if the user decides to continue
******************************************************************************/
int confirmWarning(char *message)
{
    char confirm[256];

    if (!yesFlag){
	printf("WARNING: %s\n",message);
	printf("Do you want to continue (y/n)?");
	scanf("%s",confirm);
	if (confirm[0]=='y')
	    return(1);
	else return(0);
    } else return(1);
}

DESIGN *findDesignInMemory(DESIGN **firstDesign,char *cellName,char *viewName)
{
    DESIGN *ptr;
    
    ptr=*firstDesign;
    while (ptr) {
	if (!strcmp(ptr->cellName,cellName)&&!strcmp(ptr->viewName,viewName))
	    return(ptr);
	ptr=ptr->next;
    }
    return(NULL);
}

/*****************************************************************************
  This program is working as a simple word processor which has all iDMS I/O
  commands. The edit bit and dirty bit concepts are also implemented in this
  program. Integrators are encouraged to run and study this program for
  learning iDMS methodology and integration.

  THIS PROGRAM HAS ENOUGH MATERIAL TO TEACH A CLASS OF HOW NOT TO WRITE CODE
******************************************************************************/
int main(int argc,char *argv[])
{
    struct design *firstDesign=NULL,*currentDesign=NULL;
    char command[256],cellName[256],viewName[256],*vName,*fullPathName,
         *attrType, attrName[256],aType[256],aVal[256],*attrVal,
	 libName[256], itkCommand[100], logInfo[256],
         toolName[256], policyName[256], verName[32],*cdslibName;
    /* removed from declaration because variables unused: shell[256], sCellName[256], sViewName[256],dCellName[256],dViewName[256], relName[256], */
    DMSINFO *info;
    int infoNum;
/*static char libraryList[62048]; */
    /***
    dmCellViewId cvID;
    dmGenStatePtr gsPtr;
    dmRec *cellViewRec;
    ***/
    ITKTRACK *tracker=NULL,*current=NULL;
    /* char *designDataPtr=NULL;  Not Used */
    /* int  editBit=FWITKOFF,dirtyBit=FWITKOFF; Not Used */
    int  error,ct=1;
    FILE *timeFp = fopen("itk.timing.output","w");
    char *attrNameList, *attrTypeList, *attrValueList, *viewList, *extList, *dmsBufPtr;
    void *plib_id;

/* Variables for controlling the "Information:" information */

    char *lP;
    char *lN;
    char *cN;
    char *vN;
    char *vS;
    char *uS;
    char *fP;

    dmsCSetWarnPrompt("*WARNING* ");
    idb_set_dm_vendor_string("*CADENCE* "); /* to make quick test work */

    /* make a pre-init call to induce error message */
    if ((error=dmsCInfoCellView(0,"blah","dude",&info)))
        printf("ERROR #%d: %s\n",error,dmsCReportMessage());
    else printf("OK: %s\n",dmsCReportMessage());
    
    /* initialize iDMS procedural interface before any dms calls */
    if ((error=dmsCPIInitDMS("itk",argv[0],TRUE)))
	printf("ERROR: %s\n",dmsCReportMessage());
    else printf("OK: %s\n",dmsCReportMessage());

    if (argc!=1){
	iFlag = FWITKOFF;
	yesFlag = FWITKOFF;
	while (ct<argc){
	    if (!strcmp(argv[ct],"-i"))
		iFlag = FWITKON;
	    else if (!strcmp(argv[ct],"-y"))
		yesFlag = FWITKON;
	    else if (!strcmp(argv[ct],"-iy")||!strcmp(argv[ct],"-yi")){
		yesFlag = FWITKON;
		iFlag = FWITKON;
            }
	    ct++;
	}
    }
	
    while (1) {
	/* display current data status */
	if (iFlag==FWITKON) {
	    if (firstDesign) {
		currentDesign=firstDesign;
		while (currentDesign) {
		    printf("\n");
		    printf("cellname: %s\n",currentDesign->cellName);
		    printf("viewname: %s\n",currentDesign->viewName);
		    printf("    data: [%s]\n",currentDesign->designDataPtr);
		    printf(" editBit: %d\n",currentDesign->editBit);
		    printf("dirtyBit: %d\n",currentDesign->dirtyBit);
		    currentDesign=currentDesign->next;
		}
	    } else printf("\nno data in memory\n");
	    printf("\n");

	    /* prompt for command */

	    printf(" Create   : [c]reateCellView; createUnix[f]ile\n");
	    printf(" Readonly : [r]eadonlyCVType; [R]eadonlyCVTypeByLib\n");
	    printf("          : [h]ReadonlyCV; [H]ReadonlyCVByLib\n");
	    printf("          : [j]Read/EditPolicy; [J]Read/EditPolicyByLib\n");
	    printf(" Input    : check[o]utCV; check[O]utCVByLib; [l]oadCV\n");
	    printf("          : [e]ditCellView; [E]ditCellViewByLib\n");
     	    printf(" Output   : [s]aveCellView; save[a]ndcheckin \n");
	    printf("          : check[i]nCellView; check[I]nCellViewByLib \n");
	    printf("          : ins[t]allCellView; ins[T]allCellViewByLib \n");
	    printf("          : [u]pdateCellView; [U]pdateCellViewByLib \n");
	    printf(" DMS.PATH : reloa[d]DmsPath; sho[w]DmsPath \n");
	    printf("          : GetWor[k]dir; GetWorkli[b] \n");
	    printf(" Info     : [#]InfoCellView; [$]InfoCellViewLibOnly \n");
	    printf(" Attribute: [x]SetAttr; [X]SetAttrList; [z]DeleteAttr \n");
	    printf("          : [y]GetAttr; [Y]GetAllAttr \n");
	    printf(" Misc     : [v]ersionNumber; [p]urgeCellView \n");
	    printf("          : ca[n]celcheckout; ca[N]celcheckoutByLib \n");
	    printf("          : [q]uitTEST; [m]getLibId; [M]getLibIdByLib \n");
	    printf("          : [G]etMultipleViews; [S]etMultipleViews \n");
	    printf("          : Get[L]astCellViewInfo; [P]rintIdmsLog \n");
	    printf("          : GetPolicy[V]iewList; GetSe[A]rchPath \n");
	    printf("          : [K]SetToolName; [D]GetToolName \n");
	    printf(" Data     : chan[g]e (tool commands for data editing) \n");
	    printf(" NEW      : [1]Delete Cell View; [2]Delete Cell; \n");
	    printf("          : [3]Delete Cell View Version \n");
            printf("          : [5]Get cds.lib File Name \n");
	    printf("          : [6]dmsCReadonlyCellViewTypeByLib \n");
	    printf("          : [0]getMultipleExts \n");
	    printf("Command: ");
	}
	scanf("%s",command);
	if (iFlag==FWITKON)
	    printf("\n");

	/* quit */
	if (command[0]=='q')
	    break;

	switch(command[0]){
	case ';' : /* Comment */
	    strcpy(itkCommand,command);
	    strcpy(cellName,"");
	    strcpy(viewName,"");
	    break;
	case '|' : /* Comment don't print */
	    noPrintFlag = FWITKON;
	    break;
/*****************************************************************************
  create: Tools make the create call to create a new cellview. After this call,
  tools initialize a editable data structure for users to create design data in
  memory. Users need to apply a save command to save the creation.
******************************************************************************/
	case 'c': /* create cellview */
	    
	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = NOERROR;
	    strcpy(itkCommand,"CreateCellView");
	    /* make create call and get the path to the created cellview */
	    timeBegin();
	    if ((error=dmsCCreateCellView(cellName,viewName,&fullPathName))) {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = error;
		break; /* stop create */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		current->cellCreated = TRUE;
		current->status = 'c';
		printf("OK: %s\n",dmsCReportMessage());
		printf("create %s\n",fullPathName);
		updateTrackNode(tracker,cellName,viewName,1);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*****************************************************************************
  create: Tools make the create call to create a new cellview. After this call,
  tools initialize a editable data structure for users to create design data in
  memory. Users need to apply a save command to save the creation.
******************************************************************************/
	case 'f': /* create unix file*/

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"CreateUnixFile");
	    /* make create call and get the path to the created cellview */
	    timeBegin();
	    if ((error=dmsCCreateUnixFile(cellName,viewName,&fullPathName))) {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop create */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		current->unixFile = TRUE;
		current->status = 'f';
		printf("OK: %s\n",dmsCReportMessage());
		printf("create %s\n",fullPathName);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*****************************************************************************
  readonly: Tools make the readonly call to get the path to the requested
  cellview. After this call, tools initialize a data structure to read the
  design data into memory. Tools turn off editBit to make it readonly. Users
  can not edit or save readonly cellviews.
******************************************************************************/
	case 'r': /* readonly */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"ReadOnly");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow readonly of a cellview in memory again */      
	    if (currentDesign) {
		current->inMem = TRUE;
		current->memEditBit = currentDesign->editBit;
		if (current->memEditBit==FWITKOFF)
		current->readOnly = TRUE;
		current->memDirtyBit = currentDesign->dirtyBit;
		printf("ERROR: data in memory already\n");
		break;
	    }

	    /* make readonly call and get the path to the requested cellview */
	    timeBegin();
	    if ((error=dmsCReadonlyCellViewType(cellName,viewName,&vName,
	    &fullPathName))) {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop readonly */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		/* initialize data structure for storing data in memory */
		currentDesign=initDataStructure(&firstDesign,cellName,viewName);

		/* read data into memory */
		readFile(currentDesign->designDataPtr,fullPathName);
		
		/* readonly: not editable and clean */
		currentDesign->editBit=FWITKOFF;
		currentDesign->dirtyBit=FWITKOFF;
		
		current->inMem = TRUE;
		current->status = 'r';
		current->memEditBit = currentDesign->editBit;
		current->memDirtyBit = FALSE;
		current->readOnly = TRUE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("read from %s\n",fullPathName);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (begin) ------ */
/*---------------------------------------------*/
/******************************************************************************
  readonlyCVTypeByLib: This procedure will search only the rwpaths entry which 
		       matches the given Library name for the process.
				-----------
  readonlyCVType: Tools make the readonly call to get the path to the requested
  cellview. After this call, tools initialize a data structure to read the
  design data into memory. Tools turn off editBit to make it readonly. Users
  can not edit or save readonly cellviews.
******************************************************************************/
	case 'R': /* readonlyCVtypeByLib */

            /* get user's input: CellName, ViewName and LibName */
            getUserInputForByLib("21",cellName,viewName,libName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"ReadOnlyCVtypeByLib");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow readonly of a cellview in memory again */      
	    if (currentDesign) {
		current->inMem = TRUE;
		current->memEditBit = currentDesign->editBit;
		if (current->memEditBit==FWITKOFF)
		current->readOnly = TRUE;
		current->memDirtyBit = currentDesign->dirtyBit;
		printf("ERROR: data in memory already\n");
		break;
	    }

	    /* make readonly call and get the path to the requested cellview */
	    timeBegin();
	    if ((error=dmsCReadonlyCellViewTypeByLib(libName,cellName,viewName,
						"",&vName,&fullPathName))) {
		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					  cellName,viewName,timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop readonly */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					cellName,viewName,timeElapsed());
		/* initialize data structure for storing data in memory */
		currentDesign=initDataStructure(&firstDesign,cellName,viewName);

		/* read data into memory */
		readFile(currentDesign->designDataPtr,fullPathName);
		
		/* readonly: not editable and clean */
		currentDesign->editBit=FWITKOFF;
		currentDesign->dirtyBit=FWITKOFF;
		
		current->inMem = TRUE;
		current->status = 'r';
		current->memEditBit = currentDesign->editBit;
		current->memDirtyBit = FALSE;
		current->readOnly = TRUE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("read from %s\n",fullPathName);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (end)   ------ */
/*---------------------------------------------*/
	case '#': /* info */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    strcpy(itkCommand,"Info");
	    timeBegin();
	    printf("Search rwpaths info#[%d]\n",
		   dmsCInfoCellView(0,cellName,viewName,&info));
	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
		timeElapsed());
	    printf("OK: %s\n",dmsCReportMessage());
	    break;
	case '$': /* info skip unix */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    strcpy(itkCommand,"InfoSkipUnix");
	    timeBegin();
	    printf("Search rwpaths skip library info#[%d]\n",
		   dmsCInfoCellView(1,cellName,viewName,&info));
    	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
		timeElapsed());
	    printf("OK: %s\n",dmsCReportMessage());
	    break;
/*****************************************************************************
  checkout: Tools make the checkout call to checkout the requested cellview in
  library and get a path to the checkout copy. Users need to load the cellview
  into memory for editing.
******************************************************************************/
	case 'o': /* checkout */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"CheckOut");
	    /* make checkout call and get the path to the checkout copy */
	    timeBegin();
	    if ((error=dmsCCheckoutCellView(cellName,viewName,&fullPathName))) {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());

		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop checkout */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		current->status = 'o';
		current->cellCreated = TRUE;
		current->checkedOut = TRUE;
		current->unixFile = TRUE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("checkout to %s\n",fullPathName);
		updateTrackNode(tracker,cellName,viewName,0);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (begin) ------ */
/*---------------------------------------------*/
/****************************************************************************
  CheckoutByLib: This checkout proceduree will only search the rwpaths entry
                 which matches the given Library name for checkout process.
****************************************************************************/
	case 'O': /* checkout By Lib */

            /* get user's input: CellName, ViewName and LibName */
            getUserInputForByLib("21",cellName,viewName,libName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"CheckOutByLib");
	    /* make checkout call and get the path to the checkout copy */
	    timeBegin();
	    if ((error=dmsCCheckoutCellViewByLib(libName,cellName,viewName,"",
							      &fullPathName))) {
		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					cellName,viewName, timeElapsed());

		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop checkout */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					cellName,viewName, timeElapsed());
		current->status = 'o';
		current->cellCreated = TRUE;
		current->checkedOut = TRUE;
		current->unixFile = TRUE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("checkout to %s\n",fullPathName);
		updateTrackNode(tracker,cellName,viewName,0);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (end)   ------ */
/*---------------------------------------------*/
/*****************************************************************************
  load: Tools make the load call to get the path to the checkout copy which was
  previously checked out. After this call, tools initialize a data structure to
  read the design data into memory. Tools turn on editBit to make it editable.
  Users need to apply a save command to save the changes. 
******************************************************************************/
	case 'l': /* load */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"Load");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow load of a cellview in memory again */      
	    if (currentDesign) {
		current->inMem = TRUE;
		current->memEditBit = currentDesign->editBit;
		if (current->memEditBit==FWITKOFF)
		current->readOnly = TRUE;
		current->memDirtyBit = currentDesign->dirtyBit;
		printf("ERROR: data in memory already\n");
		current->errorCode = TRUE;
		break;
	    }

	    /* make load call and get the path to the checkout copy */
	    timeBegin();
	    if ((error=dmsCLoadCellView(cellName,viewName,&fullPathName))) {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop load */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		
		/* initialize data structure for storing data in memory */
		currentDesign=initDataStructure(&firstDesign,cellName,viewName);
		
		/* read data into memory */
		readFile(currentDesign->designDataPtr,fullPathName);
		
		/* readonly: not editable and clean */
		currentDesign->editBit=FWITKON;
		currentDesign->dirtyBit=FWITKOFF;

		current->status = 'l';
		current->memEditBit = currentDesign->editBit;
		current->memDirtyBit = currentDesign->dirtyBit;
		current->inMem = TRUE;
		current->readOnly = FALSE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("load from to %s\n",fullPathName);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*****************************************************************************
  edit: Tools make the edit call to checkout the cellview and get a path to the
  checkout copy. After this call, tools initialize a data structure to
  read the design data into memory. Tools turn on editBit to make it editable.
  Users need to apply a save command to save the changes. 
******************************************************************************/
	case 'e': /* edit */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"Edit");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow edit of a cellview in memory again */      
	    if (currentDesign&&currentDesign->editBit) {
		    current->inMem = TRUE;
		    current->memEditBit = currentDesign->editBit;
		    current->memDirtyBit = currentDesign->dirtyBit;
		    printf("ERROR: editable data in memory already\n");
		    current->errorCode = TRUE;
		    break;
	    }

	    /* make edit call to checkout and get path to the checkout copy */
	    timeBegin();
	    if ((error=dmsCEditCellView(cellName,viewName,&fullPathName))) {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop edit */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		
		/* free readonly copy in memory if it is there */
		if (currentDesign){
		    freeDesignData(&firstDesign,&currentDesign);
		    current->readOnly = FALSE;
		}
		
		/* initialize data structure for storing data in memory */
		currentDesign=initDataStructure(&firstDesign,cellName,viewName);
		
		/* read data into memory */
		readFile(currentDesign->designDataPtr,fullPathName);
		
		/* readonly: not editable and clean */
		currentDesign->editBit=FWITKON;
		currentDesign->dirtyBit=FWITKOFF;
		
		current->status = 'o';
		current->cellCreated = TRUE;
		current->memEditBit = currentDesign->editBit;
		current->memDirtyBit = currentDesign->dirtyBit;
		current->inMem = TRUE;
		current->readOnly = FALSE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("edit from %s\n",fullPathName);
		updateTrackNode(tracker,cellName,viewName,0);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (begin) ------ */
/*---------------------------------------------*/
/*****************************************************************************
  EditByLib: This editByLib procedure will search only the rwpaths entry 
	     which matches the given Library name for edit process. 
				----------
  edit: Tools make the edit call to checkout the cellview and get a path to the
  checkout copy. After this call, tools initialize a data structure to
  read the design data into memory. Tools turn on editBit to make it editable.
  Users need to apply a save command to save the changes. 
******************************************************************************/
	case 'E': /* editByLib */

	    /* get user's input: CellName, ViewName and LibName */

	    getUserInputForByLib("21",cellName,viewName,libName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"EditByLib");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow edit of a cellview in memory again */      
	    if (currentDesign&&currentDesign->editBit) {
		    current->inMem = TRUE;
		    current->memEditBit = currentDesign->editBit;
		    current->memDirtyBit = currentDesign->dirtyBit;
		    printf("ERROR: editable data in memory already\n");
		    current->errorCode = TRUE;
		    break;
	    }

	    /* make edit call to checkout and get path to the checkout copy */
	    timeBegin();
	    if ((error=dmsCEditCellViewByLib(libName,cellName,viewName,"",
							  &fullPathName))) {
		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					  cellName,viewName,timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop edit */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					  cellName,viewName,timeElapsed());
		
		/* free readonly copy in memory if it is there */
		if (currentDesign){
		    freeDesignData(&firstDesign,&currentDesign);
		    current->readOnly = FALSE;
		}
		
		/* initialize data structure for storing data in memory */
		currentDesign=initDataStructure(&firstDesign,cellName,viewName);
		
		/* read data into memory */
		readFile(currentDesign->designDataPtr,fullPathName);
		
		/* readonly: not editable and clean */
		currentDesign->editBit=FWITKON;
		currentDesign->dirtyBit=FWITKOFF;
		
		current->status = 'o';
		current->cellCreated = TRUE;
		current->memEditBit = currentDesign->editBit;
		current->memDirtyBit = currentDesign->dirtyBit;
		current->inMem = TRUE;
		current->readOnly = FALSE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("edit from %s\n",fullPathName);
		updateTrackNode(tracker,cellName,viewName,0);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (end)   ------ */
/*---------------------------------------------*/
/*****************************************************************************
  save: Tools make the save call to get the path to write out memory copy.
  Changes in memory will be flushed out to the checkout copy. After the save,
  tools clear the dirtyBit to indicate the checkout copy is up-to-date.
******************************************************************************/
	case 's': /* save */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"Save");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* need data available in memory to save */
	    if (currentDesign) {
		
		/* only editable can be saved */
		if (currentDesign->editBit) {
		    
		    /* only dirty need to be saved */
		    if (currentDesign->dirtyBit) {
			
			/* make save call and get a path to write out data */
			timeBegin();
			if ((error=dmsCSaveCellView(cellName,viewName,
			&fullPathName))) {
			    timeEnd();
			    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,
				    viewName,timeElapsed());
			    printf("ERROR# %d: %s\n",error,dmsCReportMessage());
			    current->errorCode = TRUE;
			    break; /* stop save */
			} else {
			    timeEnd();
			    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,
				    viewName,timeElapsed());
			    /* write data to file */
			    writeFile(currentDesign->designDataPtr,fullPathName);
			    
			    /* after write data become clean */
			    currentDesign->dirtyBit=FWITKOFF;
			    current->memDirtyBit = FWITKOFF;
			    current->status = 's';
			    current->unixFile = TRUE;
			    printf("OK: %s\n",dmsCReportMessage());
			    printf("save to %s\n",fullPathName);
			    updateTrackNode(tracker,cellName,viewName,0);
			}
			
		    } else printf("OK: no need to save\n");
		    
		} else {
		    printf("ERROR: can not save read only data\n");
		    current->errorCode = TRUE;
		}
	    } else {
		printf("ERROR: no data in memory to save\n");
		current->errorCode = TRUE;
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*****************************************************************************
  checkin: Tools make the checkin call to move the checkout copy into library
  and become a new version. The checkin does not write the most up-to-date
  memory copy to the checkout copy. So, memory copy need to be purged or saved
  explicitly by users to checkin. The memory copy will become readonly.
******************************************************************************/
	case 'i': /* checkin */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"CheckIn");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow checkin of a dirty cellview */
	    if (currentDesign&&currentDesign->dirtyBit) {
	     printf("ERROR: purge or save the dirty memory copy then checkin\n");
	            current->errorCode = TRUE;
		    break;
	    }
	    
	    /* make checkin call and get a path to the checkin copy */
	    timeBegin();
	    if ((error=dmsCCheckinCellView(cellName,viewName,"",0,"",
	    &fullPathName))) {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop checkin */
	    } else {
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		current->status = 'i';
		current->cellCreated = TRUE;
		current->checkedOut = FALSE;
		current->unixFile = FALSE;
		current->memDirtyBit = FALSE;
		current->wdLocation = nullifyString(current->wdLocation);
		current->wdData = nullifyString(current->wdData);
		current->wdDataValid = FALSE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("checkin from %s\n",fullPathName);
		updateTrackNode(tracker,cellName,viewName,1);
		
		/* after checkin, memory copy become readonly */
		if (currentDesign) {
		    currentDesign->editBit=FWITKOFF;
		    current->memEditBit = FALSE;
		    current->readOnly = TRUE;
		}
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (begin) ------ */
/*---------------------------------------------*/
/*****************************************************************************
  CheckinByLib: This checkin procedure will search only the rwpaths entry 
	        which matches the given Library name for checkin process. 
******************************************************************************/
	case 'I': /* checkinByLib */

	    /* get user's input: CellName, ViewName and LibName */

	    getUserInputForByLib("21",cellName,viewName,libName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"CheckInByLib");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow checkin of a dirty cellview */

	    if (currentDesign&&currentDesign->dirtyBit) {
	    printf("ERROR: purge or save the dirty memory copy then checkin\n");
	            current->errorCode = TRUE;
		    break;
	    }
	    
	    /* make checkin call and get a path to the checkin copy */
	    timeBegin();
	    if ((error=dmsCCheckinCellViewByLib(libName,cellName,viewName,"",
			                             "",0,"",&fullPathName))) {
		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName, 
					cellName, viewName,timeElapsed());
		
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop checkin */
	    } else {
 		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					cellName, viewName, timeElapsed());
		current->status = 'i';
		current->cellCreated = TRUE;
		current->checkedOut = FALSE;
		current->unixFile = FALSE;
		current->memDirtyBit = FALSE;
		current->wdLocation = nullifyString(current->wdLocation);
		current->wdData = nullifyString(current->wdData);
		current->wdDataValid = FALSE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("checkin from %s\n",fullPathName);
		updateTrackNode(tracker,cellName,viewName,1);
		
		/* after checkin, memory copy become readonly */
		if (currentDesign) {
		    currentDesign->editBit=FWITKOFF;
		    current->memEditBit = FALSE;
		    current->readOnly = TRUE;
		}
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (end)   ------ */
/*---------------------------------------------*/
/*****************************************************************************
  install: Tools make the install call to install the 0.1 version to a newly 
  created cellview. The install does not write the most up-to-date
  memory copy to the workdir copy. So, memory copy need to be purged or saved
  explicitly by users to install. The memory copy will become readonly.
******************************************************************************/
	case 't': /* install */
	    
	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"Install");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow checkin of a dirty cellview */
	    if (currentDesign&&currentDesign->dirtyBit) {
	     printf("ERROR: purge or save the dirty memory copy then checkin\n");
	            current->errorCode = TRUE;
		    break;
	    }

	    /* make checkin call and get a path to the checkin copy */
	    timeBegin();
	    if ((error=dmsCInstallCellView(cellName,viewName,"child","initial version",
	    &fullPathName))) {
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop install */
	    } else {
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		current->status = 't';
		current->cellCreated = TRUE;
		current->checkedOut = FALSE;
		current->unixFile = FALSE;
		current->memDirtyBit = FALSE;
		current->wdLocation = nullifyString(current->wdLocation);
		current->wdData = nullifyString(current->wdData);
		current->wdDataValid = FALSE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("install from %s\n",fullPathName);
		updateTrackNode(tracker,cellName,viewName,1);
		
		/* after install, memory copy become readonly */
		if (currentDesign) {
		    currentDesign->editBit=FWITKOFF;
		    current->memEditBit = FALSE;
		    current->readOnly = TRUE;
		}
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (begin) ------ */
/*---------------------------------------------*/
/*****************************************************************************
  InstallByLib: This InstallByLib procedure will search only the rwpaths entry 
	        which matches the given Library name for InstallByLib process. 
				-----------
  install: Tools make the install call to install the 0.1 version to a newly 
  created cellview. The install does not write the most up-to-date
  memory copy to the workdir copy. So, memory copy need to be purged or saved
  explicitly by users to install. The memory copy will become readonly.
******************************************************************************/
	case 'T': /* installByLib */
	    
            /* get user's input: CellName, ViewName and LibName */
            getUserInputForByLib("21",cellName,viewName,libName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"InstallByLib");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow checkin of a dirty cellview */
	    if (currentDesign&&currentDesign->dirtyBit) {
	       printf("ERROR: purge or save the dirty memory copy then \
								checkin\n");
	            current->errorCode = TRUE;
		    break;
	    }

	    /* make checkin call and get a path to the checkin copy */
	    timeBegin();
	    if ((error=dmsCInstallCellViewByLib(libName,cellName,viewName,
					    "child","log",&fullPathName))) {
 		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					  cellName,viewName,timeElapsed());
		
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop install */
	    } else {
 		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					  cellName,viewName,timeElapsed());
		current->status = 't';
		current->cellCreated = TRUE;
		current->checkedOut = FALSE;
		current->unixFile = FALSE;
		current->memDirtyBit = FALSE;
		current->wdLocation = nullifyString(current->wdLocation);
		current->wdData = nullifyString(current->wdData);
		current->wdDataValid = FALSE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("install from %s\n",fullPathName);
		updateTrackNode(tracker,cellName,viewName,1);
		
		/* after install, memory copy become readonly */
		if (currentDesign) {
		    currentDesign->editBit=FWITKOFF;
		    current->memEditBit = FALSE;
		    current->readOnly = TRUE;
		}
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (end)   ------ */
/*---------------------------------------------*/
/*****************************************************************************
  update: Tools make the update call to move the directoyr copy into library
  and become a new version without a previous checkout.
******************************************************************************/
	case 'u': /* update */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"Update");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow checkin of a dirty cellview */
	    if (currentDesign&&currentDesign->dirtyBit) {
	      printf("ERROR: purge or save the dirty memory copy then update\n");
	            current->errorCode = TRUE;
		    break;
	    }
	    
	    /* make update call and get a path to the update copy */
	    timeBegin();
	    if ((error=dmsCUpdateCellView(cellName,viewName,"",0,"",
	    &fullPathName))) {
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop update */
	    } else {
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		current->status = 'u';
		current->cellCreated = TRUE;
		current->checkedOut = FALSE;
		current->unixFile = FALSE;
		current->wdDataValid = FALSE;
		current->wdLocation = nullifyString(current->wdLocation);
		current->wdData = nullifyString(current->wdData);
		printf("OK: %s\n",dmsCReportMessage());
		printf("update from %s\n",fullPathName);

		/* after update, memory copy become readonly */
		if (currentDesign) {
		    currentDesign->editBit=FWITKOFF;
		    current->memEditBit = FALSE;
		    current->readOnly = TRUE;
		}
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (begin) ------ */
/*---------------------------------------------*/
/*****************************************************************************
  UpdateByLib: This UpdateByLib procedure will search only the rwpaths entry 
	       which matches the given Library name for UpdateByLib process. 
				------------
  update: Tools make the update call to move the directory copy into library
          and become a new version without a previous checkout.
******************************************************************************/
	case 'U': /* updateByLib */

            /* get user's input: CellName, ViewName and LibName */
            getUserInputForByLib("21",cellName,viewName,libName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"UpdateByLib");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow checkin of a dirty cellview */
	    if (currentDesign&&currentDesign->dirtyBit) {
	       printf("ERROR: purge or save the dirty memory copy then \
								update\n");
	            current->errorCode = TRUE;
		    break;
	    }
	    
	    /* make update call and get a path to the update copy */
	    timeBegin();
	    if ((error=dmsCUpdateCellViewByLib(libName,cellName,viewName,"","",
							 0,"",&fullPathName))) {
 		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					  cellName,viewName,timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop update */
	    } else {
 		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
                                          cellName,viewName,timeElapsed());
		current->status = 'u';
		current->cellCreated = TRUE;
		current->checkedOut = FALSE;
		current->unixFile = FALSE;
		current->wdDataValid = FALSE;
		current->wdLocation = nullifyString(current->wdLocation);
		current->wdData = nullifyString(current->wdData);
		printf("OK: %s\n",dmsCReportMessage());
		printf("update from %s\n",fullPathName);

		/* after update, memory copy become readonly */
		if (currentDesign) {
		    currentDesign->editBit=FWITKOFF;
		    current->memEditBit = FALSE;
		    current->readOnly = TRUE;
		}
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (end)   ------ */
/*---------------------------------------------*/
/*****************************************************************************
  save&checkin: Tools make the save&checkin call to save the memory copy and
  move the checkout copy into library and become a new version. This will
  grauantee the new version is the same as the memory copy which become
  readonly.
******************************************************************************/
	case 'a': /* save and checkin */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"SaveAndCheckIn");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* need data available in memory to save */
	    if (currentDesign) {
		
		/* only editable can be saved */
		if (currentDesign->editBit) {
		    
		    /* only dirty need to be saved */
		    if (currentDesign->dirtyBit) {

			/* make save call and get a path to write out data */
			timeBegin();
			if ((error=dmsCSaveCellView(cellName,viewName,
			&fullPathName))) {
			    timeEnd();
			    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,
				    viewName,timeElapsed());
			    printf("ERROR# %d: %s\n",error,dmsCReportMessage());
			    current->errorCode = TRUE;
			    break; /* stop checkin */
			} else {
			    timeEnd();
			    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,
				    viewName,timeElapsed());
			    
			    /* write data to file */
			    writeFile(currentDesign->designDataPtr,fullPathName);
			    
			    /* after write file become clean */
			    currentDesign->dirtyBit=FWITKOFF;
			    current->memDirtyBit = FWITKOFF;
			    
			    printf("OK: %s\n",dmsCReportMessage());
			    printf("save to %s\n",fullPathName);
			    updateTrackNode(tracker,cellName,viewName,0);
			}
		    } else
		    printf("OK: no need to save\n");

		    /* make checkin call and get a path to the checkin copy */
		    timeBegin();
		    if ((error=dmsCCheckinCellView(cellName,viewName,"",0,"",
		    &fullPathName))){
	 		timeEnd();
			fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
				timeElapsed());
			printf("ERROR# %d: %s\n",error,dmsCReportMessage());
			current->errorCode = TRUE;
		    }
		    else {
			timeEnd();
			fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
				timeElapsed());
			
			/* after checkin, memory copy become readonly */
			currentDesign->editBit=FWITKOFF;
			current->status = 'i';
			current->cellCreated = TRUE;
			current->checkedOut = FALSE;
			current->unixFile = FALSE;
			current->memEditBit = FALSE;
			current->memDirtyBit = FALSE;
			current->readOnly = TRUE;
			current->wdLocation = nullifyString(current->wdLocation);
			current->wdData = nullifyString(current->wdData);
			current->wdDataValid = FALSE;
			printf("OK: %s\n",dmsCReportMessage());
			printf("checkin from %s\n",fullPathName);
			updateTrackNode(tracker,cellName,viewName,1);
		    }
		    
		} else{
		    printf("ERROR: can not save read only data\n");
		    current->errorCode = TRUE;
		}
	    } else {
		printf("ERROR: no data in memory to save\n");
		current->errorCode = TRUE;
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*****************************************************************************
  purge: Tools make the purge call to release the link lock registed on cdsd.
  This allows other users to delete the version from the library.
****************** ************************************************************/
	case 'p': /* purge */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"Purge");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* need data available in memory to be purged */
	    if (currentDesign) {
		
		/* warn user a dirty copy is in memory */      
		if (currentDesign->dirtyBit)
		    if (!confirmWarning("will lose changes since last save"))
			break;
		
		/* make purge call and release link lock */
		timeBegin();
		if ((error=dmsCPurgeCellView(cellName,viewName,&fullPathName))) {
		    timeEnd();
		    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			    timeElapsed());
		    printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		    current->errorCode = TRUE;
		    break; /* stop purge */
		}
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		/* purge memory copy */
		/* ok to purge when purge call failed (cellview not exist) */
		freeDesignData(&firstDesign,&currentDesign);
		current->inMem = FALSE;
		current->status = 'p';
		printf("OK: %s\n",dmsCReportMessage());
		printf("purge %s in memory\n",fullPathName);
		
	    } else {
		printf("ERROR: no data in memory to purge\n");
		current->errorCode = TRUE;
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*****************************************************************************
  cancel checkout: Tools make the call to cancel previous checkout. Memory copy
  need to to purged to avoid a modified copy left in memory.
******************************************************************************/
	case 'n': /* cancel checkout */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"CancelCheckOut");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow cancel checkout of a dirty cellview */      
	    if (currentDesign&&currentDesign->dirtyBit) {
     printf("ERROR: purge or save the dirty memory copy then cancel checkout\n");
                    current->errorCode = TRUE;
		    break;
	    }
	    
	    /* warn user the consequence */      
	    if (!confirmWarning("will lose all changes since checkout"))
		break;
	    
	    /* make cancel checkout call and get the path to the deleted copy */
	    timeBegin();
	    if ((error=dmsCCancelCheckoutCellView(cellName,viewName,
							&fullPathName))) {
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop cancel checkout */
	    } else {

 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		
		currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
		if(currentDesign) /* purge memory copy */
		  freeDesignData(&firstDesign,&currentDesign);

		current->readOnly = FALSE;
		current->inMem = FALSE;
		current->status = 'n';
		current->checkedOut = FALSE;
		current->unixFile = FALSE;
		current->wdLocation = nullifyString(current->wdLocation);
		current->wdData = nullifyString(current->wdData);
		current->wdDataValid = FALSE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("cancel checkout %s\n",fullPathName);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (begin) ------ */
/*---------------------------------------------*/
/*****************************************************************************
  CancelCheckoutByLib: This cancel checkout proceduree will only search the 
		       rwpaths entry which matches the given Library name for 
		       cancel checkout process.
*****************************************************************************/
	case 'N': /* cancel checkout By lib*/

	    /* get user's input: CellName, ViewName and LibName */
	    getUserInputForByLib("21",cellName,viewName,libName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } 
	    else current->errorCode = FALSE;
	    strcpy(itkCommand,"CancelCheckOutByLib");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow cancel checkout of a dirty cellview */      
	    if (currentDesign&&currentDesign->dirtyBit) {
	       printf("ERROR: purge or save the dirty memory copy then cancel \
								 checkout\n");
	       current->errorCode = TRUE;
	       break;
	    }
	    
	    /* warn user the consequence */      
	    if (!confirmWarning("will lose all changes since checkout"))
		break;
	    
	    /* make cancel checkout call and get the path to the deleted copy */
	    timeBegin();
	    if ((error=dmsCCancelCheckoutCellViewByLib(libName,cellName,
						viewName,"",&fullPathName))) {
 	        timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,cellName,
					    libName,viewName,timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop cancel checkout */
	    } 
            else {
 		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					  cellName,viewName,timeElapsed());
		
		currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
		if(currentDesign) /* purge memory copy */
		  freeDesignData(&firstDesign,&currentDesign);
		
		current->readOnly = FALSE;
		current->inMem = FALSE;
		current->status = 'n';
		current->checkedOut = FALSE;
		current->unixFile = FALSE;
		current->wdLocation = nullifyString(current->wdLocation);
		current->wdData = nullifyString(current->wdData);
		current->wdDataValid = FALSE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("cancel checkout %s\n",fullPathName);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (end)   ------ */
/*---------------------------------------------*/
/*****************************************************************************
  reload dms.pth: Tools make the call to reload the dms.pth file. 
******************************************************************************/
	case 'd': /* reload dms.pth */
	    
	    strcpy(itkCommand,"Reload");
	    strcpy(cellName,"");
	    strcpy(viewName,"");
	    timeBegin();
	    if ((error=dmsCParsePath())) {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		break; /* stop reload */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("OK: %s\n",dmsCReportMessage());
	    }
	    break;
/*****************************************************************************
  print dms.pth: Tools make the call to display the idms setup in memory.
******************************************************************************/
	case 'w': /* print dms.pth */

	    /* get user's input */
	    getUserInput("01",cellName,viewName);
	    strcpy(itkCommand,"Show");
	    strcpy(cellName,"");
	    timeBegin();
	    printf("OK: %s\n",dmsCPrintPath(viewName));
	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
		    timeElapsed());
	    break;
/*****************************************************************************
  get workdir: Tools make the call to get the workdir of a view
******************************************************************************/
	case 'k': /* get workdir */

	    /* get user's input */
	    getUserInput("01",cellName,viewName);
	    strcpy(cellName,"");
	    strcpy(itkCommand,"GetWorkDir");
	    timeBegin();
	    dmsCGetWorkingDir(viewName,&fullPathName);
	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
		    timeElapsed());
	    printf("OK: %s\n",fullPathName);
	    break;
/*****************************************************************************
  get worklib: Tools make the call to get the worklib of a view
******************************************************************************/
	case 'b': /* get worklib */

	    /* get user's input */
	    getUserInput("01",cellName,viewName);
	    strcpy(cellName,"");
	    strcpy(itkCommand,"GetWorkLib");
	    timeBegin();
	    dmsCGetWorkingLib(viewName,&fullPathName);
	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
		    timeElapsed());
	    printf("OK: %s\n",fullPathName);
	    break;
/*-----------Shesha (begin) -------*/
/*****************************************************************************
  get library ID: Tools make the call to get the library ID of a view
******************************************************************************/
	case 'm': /* get LibId */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    strcpy(itkCommand,"GetLibId");
	    timeBegin();
	    if (!(plib_id = coralCGetLibId(cellName,viewName,"", &error))){
	       timeEnd();
	       fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,
					      viewName, timeElapsed());
	       printf("ERROR# %d: %s\n",error,dmsCReportMessage());
               break; 
	    }
	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
		    timeElapsed());
	    printf("OK: The LibID is %p\n",plib_id);
	    break;
/*****************************************************************************
  get library ID By Lib: Tools make the call to get the library ID of a view
			 in a Library.
******************************************************************************/
	case 'M': /* get LibId By Lib*/

            /* get user's input: CellName, ViewName and LibName */
            getUserInputForByLib("21",cellName,viewName,libName);
	    strcpy(itkCommand,"GetLibIdByLib");
	    timeBegin();
	    if (!(plib_id = coralCGetLibIdByLib(libName,cellName,viewName,"", &error))){
	       timeEnd();
	       fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					cellName,viewName, timeElapsed());
	       printf("ERROR# %d: %s\n",error,dmsCReportMessage());
               break; 
	    }
	    timeEnd();
	    fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
	                             cellName,viewName, timeElapsed());
	    printf("OK: The LibID is %p\n",plib_id);
	    break;
/*-----------Shesha (end) --------*/
/*****************************************************************************
  This portion is for itk only. This simulates tools' data editing commands.
******************************************************************************/
	case 'g': /* change data */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"ChangeData");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* need data available in memory to change */
	    if (currentDesign) {
		
		/* only editable can be changed */
		if (currentDesign->editBit) {

		    if (iFlag==FWITKON)
		    printf("change data from: [%s] to: ",
			   currentDesign->designDataPtr);
		    scanf("%s",currentDesign->designDataPtr);
		    current->memData=changeString(current->memData,
						  currentDesign->designDataPtr);
		    current->memDirtyBit = TRUE;

		    /* data become dirty after change */
		    currentDesign->dirtyBit=FWITKON;
		    
		} else {
		    printf("ERROR: data is read only\n");
		    current->errorCode = TRUE;
		}
		
	    } else {
		printf("ERROR: no data in memory to change\n");
		current->errorCode = TRUE;
	    }
	    break;

	case 'x': /* set attribute */
	    getUserInput("11",cellName,viewName);
	    getAttributeInfo(1,attrName,aType,aVal);
	    /* Added the following for proper Time attr Check .. Shesha'93 */
	    if (!strcmp(aType,"time")||!strcmp(aType,"Time")) 
	       strcpy(aVal,"Wed Sep 22 17:17:17 1993");
	    strcpy(itkCommand,"SetAttribute");
	    timeBegin();
	     if ((error=dmsCSetAttribute(NULL,cellName,viewName,NULL,attrName,
	    aType,aVal))){ 
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
                break;
	    } else {
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("OK: %s\n",dmsCReportMessage());
		printf("attribute %s of type %s set to %s for %s:%s\n",
		       attrName,aType,aVal,cellName,viewName);
	    }
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (begin) ------ */
/*---------------------------------------------*/
	case 'X': /* set all attributes */
            /* get user's input: CellNameList, ViewNameList, LibNameList */
            getUserInputForByLib("21",cellName,viewName,libName);
	    /* get user's input: attrNameList, attrTypeList, attrValueList */
	    getAttributeInfo(1,attrName,aType,aVal);
	    strcpy(itkCommand,"SetAttributeList");
	    timeBegin();
	    if ((error=dmsCSetAttributeList(libName,cellName,viewName,NULL,
						attrName,aType,aVal))){
 		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					  cellName,viewName,timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
                break;
	    } else {
 		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					  cellName,viewName,timeElapsed());
		printf("OK: %s\n",dmsCReportMessage());
		printf("attribute %s of type %s set to %s for %s-%s\n",
		       attrName,aType,aVal,cellName,viewName);
	    }
	    break;
/*---------------------------------------------*/
	case 'G': /* get multiple views */
	    strcpy(itkCommand,"GetMultipleViews");
	    timeBegin();
	    if ((error=dmsCGetMultipleViews(&viewList))){
 		timeEnd();
		fprintf(timeFp,"%s output: %s took %f\n",itkCommand,
						viewList,timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
                break;
	    } else {
 		timeEnd();
		fprintf(timeFp,"%s output: %s took %f\n",itkCommand,
						viewList,timeElapsed());
		printf("OK: %s\n",dmsCReportMessage());
		printf("The Multiple views are: %s\n", viewList);
	    }
	    break;
/*-------------------------------------------------------------*/
	case 'S': /* set multiple views */
            /* get user's input: ViewNameList*/
            getUserInputForView(viewName);
	    strcpy(itkCommand,"SetMultipleViews");
	    timeBegin();
	    if ((error=dmsCSetMultipleViews(viewName))){
 		timeEnd();
		fprintf(timeFp,"%s output: %s took %f\n",itkCommand,
						viewName,timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
                break;
	    } else {
 		timeEnd();
		fprintf(timeFp,"%s output: %s took %f\n",itkCommand,
						viewName,timeElapsed());
		printf("OK: %s\n",dmsCReportMessage());
	    }
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (end)   ------ */
/*---------------------------------------------*/
	case 'y': /* get attribute */
	    getUserInput("11",cellName,viewName);
	    getAttributeInfo(0,attrName,attrType,attrVal);
	    strcpy(itkCommand,"GetAttribute");
	    timeBegin();
	    if ((error=dmsCGetAttribute(NULL,cellName,viewName,NULL,attrName,
	    &attrType,&attrVal))){
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		break;
	    } else {
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("OK: %s\n",dmsCReportMessage());
		printf("attribute %s of type %s for %s:%s found to be %s\n",
		       attrName,attrType,cellName,viewName,attrVal);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (begin) ------ */
/*---------------------------------------------*/
	case 'Y': /* get all attributes */

            /* get user's input: CellName, ViewName and LibName */
            getUserInputForByLib("21",cellName,viewName,libName);

	    strcpy(itkCommand,"GetAllAttribute");
	    timeBegin();

	    if ((error=dmsCGetAllAttributes(libName,cellName,viewName,NULL,
	           &attrNameList, &attrTypeList, &attrValueList))){
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,
						 viewName,timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		break;
	    } else {
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,
						 viewName,timeElapsed());
		printf("OK: %s\n",dmsCReportMessage());
		printf("AttrNameList: %s\nAttrTypeList: %s\nAttrValueList: %s \
			\n",attrNameList,attrTypeList,attrValueList);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
	case 'L': /* get last cell view info */

	    strcpy(itkCommand,"GetLastCellViewInfo");
	    timeBegin();
            infoNum=dmsCGetLastCellViewInfo(&info);
 	    timeEnd();
	    fprintf(timeFp,"%s: DMSINFO contents: %d:%s:%s:%s:%s:%s took %f\n",
		    itkCommand,info->location,info->libPath,info->libName,
		    info->cellName,info->viewName,info->coUser,timeElapsed());
	    printf("* %s: DMSINFO contents:%d:%s:%s:%s:%s:%s\n",itkCommand,
		   info->location,info->libPath,info->libName, info->cellName,
		   info->viewName,info->coUser);
	    break;
/*---------------------------------------------*/
	case 'P': /* print idms log info */

	    strcpy(itkCommand,"PrintIdmsLog");
	    strcpy(logInfo,"* THIS IS A QUICK TEST LOG MESSAGE * PrintIdmsLog");
	    timeBegin();
            infoNum=dmsCPrintIdmsLog(logInfo);
 	    timeEnd();
	    fprintf(timeFp,"%s: IDMS log contents: %s took %f\n",itkCommand,
		    				     logInfo,timeElapsed());
	    printf("* %s: IDMS log contents: %s\n",itkCommand, logInfo);
	    break;
/*---------------------------------------------*/
	case 'V': /* get policy view list */
	    getUserInput("22",toolName,policyName);
	    strcpy(itkCommand,"GetPolicyViewList");
	    timeBegin();
            dmsBufPtr=dmsCGetPolicyViewList(toolName,policyName);
 	    timeEnd();
	    fprintf(timeFp,"%s: Policy info: %s took %f\n",itkCommand,
		    				     dmsBufPtr,timeElapsed());
	    printf("* %s: Policy View info: %s\n",itkCommand, dmsBufPtr);
	    break;
/*---------------------------------------------*/
	case 'A': /* get search path */

	    strcpy(itkCommand,"GetSearchPath");
	    timeBegin();
            dmsBufPtr=dmsCGetSearchPath();
 	    timeEnd();
	    fprintf(timeFp,"%s: %s took %f\n",itkCommand,
		    				     dmsBufPtr,timeElapsed());
	    printf("* %s: %s\n",itkCommand, dmsBufPtr);
	    break;
/*---------------------------------------------*/
	case 'K': /* set tool name */

	    getUserInputForToolName(toolName);
	    strcpy(itkCommand,"SetToolName");
	    timeBegin();
            infoNum=idmsSuccess;
            dmsCSetToolName(toolName);
 	    timeEnd();
	    fprintf(timeFp,"%s: %d took %f\n",itkCommand,
		    				     infoNum,timeElapsed());
	    printf("* %s: %d\n",itkCommand, infoNum);
	    break;
/*---------------------------------------------*/
	case 'D': /* get tool name */

	    strcpy(itkCommand,"GetToolName");
	    timeBegin();
            infoNum=idmsSuccess;
            dmsCGetToolName(toolName);
 	    timeEnd();
	    fprintf(timeFp,"%s: %s took %f\n",itkCommand,
		    				     toolName,timeElapsed());
	    printf("* %s: %s\n",itkCommand, toolName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (end)   ------ */
/*---------------------------------------------*/
	

	case 'z': /* delete attribute */
	    getUserInput("11",cellName,viewName);
	    getAttributeInfo(0,attrName,attrType,attrVal);
	    strcpy(itkCommand,"DeleteAttribute");
	    timeBegin();
	    if ((error=dmsCDeleteAttribute(NULL,cellName,viewName,NULL,attrName))){
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		break;
	    } else {
 		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("OK: %s\n",dmsCReportMessage());
		printf("attribute %s for %s:%s deleted\n",attrName,cellName,
		       viewName);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;

	case 'v': /* get version number */
	    strcpy(itkCommand,"GetVersionNumber");
	    timeBegin();
	    printf("OK: Version number is %s\n",dmsCVersionNumber());
	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
		    timeElapsed());
	    break;

/******************************************************************************
  readonlyCV: Tools make the readonly call to get the path to the requested
  cellview. After this call, tools initialize a data structure to read the
  design data into memory. Tools turn off editBit to make it readonly. Users
  can not edit or save readonly cellviews. Search is based on viewName entry
  only.
******************************************************************************/
	case 'h': /* readonlyCellview */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"ReadOnly");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow readonly of a cellview in memory again */      
	    if (currentDesign) {
		current->inMem = TRUE;
		current->memEditBit = currentDesign->editBit;
		if (current->memEditBit==FWITKOFF)
		current->readOnly = TRUE;
		current->memDirtyBit = currentDesign->dirtyBit;
		printf("ERROR: data in memory already\n");
		break;
	    }

	    /* make readonly call and get the path to the requested cellview */
	    timeBegin();
	    if ((error=dmsCReadonlyCellView(cellName,viewName,&fullPathName))) {
	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
		    timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop readonly */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());

		/* initialize data structure for storing data in memory */
		currentDesign=initDataStructure(&firstDesign,cellName,viewName);

		/* read data into memory */
		readFile(currentDesign->designDataPtr,fullPathName);
		
		/* readonly: not editable and clean */
		currentDesign->editBit=FWITKOFF;
		currentDesign->dirtyBit=FWITKOFF;
		
		current->inMem = TRUE;
		current->status = 'r';
		current->memEditBit = currentDesign->editBit;
		current->memDirtyBit = FALSE;
		current->readOnly = TRUE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("read from %s\n",fullPathName);
	    }

	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (begin) ------ */
/*---------------------------------------------*/
/*****************************************************************************
  readonlyCVByLib: This procedure will search only the rwpaths entry which 
		   matches the given Library name for the process.
				-----------
  readonlyCV: Tools make the readonly call to get the path to the requested
  cellview. After this call, tools initialize a data structure to read the
  design data into memory. Tools turn off editBit to make it readonly. Users
  can not edit or save readonly cellviews. Search is based on viewName entry
  only.
*****************************************************************************/
	case 'H': /* readonlyCellViewByLib */

            /* get user's input: CellName, ViewName and LibName */
            getUserInputForByLib("21",cellName,viewName,libName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"ReadOnlyCellViewByLib");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow readonly of a cellview in memory again */      
	    if (currentDesign) {
		current->inMem = TRUE;
		current->memEditBit = currentDesign->editBit;
		if (current->memEditBit==FWITKOFF)
		current->readOnly = TRUE;
		current->memDirtyBit = currentDesign->dirtyBit;
		printf("ERROR: data in memory already\n");
		break;
	    }

	    /* make readonly call and get the path to the requested cellview */
	    timeBegin();
	    if ((error=dmsCReadonlyCellViewByLib(libName,cellName,viewName,"",
							      &fullPathName))) {
	    timeEnd();
	    fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					cellName,viewName,timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop readonly */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					cellName,viewName,timeElapsed());

		/* initialize data structure for storing data in memory */
		currentDesign=initDataStructure(&firstDesign,cellName,viewName);

		/* read data into memory */
		readFile(currentDesign->designDataPtr,fullPathName);
		
		/* readonly: not editable and clean */
		currentDesign->editBit=FWITKOFF;
		currentDesign->dirtyBit=FWITKOFF;
		
		current->inMem = TRUE;
		current->status = 'r';
		current->memEditBit = currentDesign->editBit;
		current->memDirtyBit = FALSE;
		current->readOnly = TRUE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("read from %s\n",fullPathName);
	    }

	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (end)   ------ */
/*---------------------------------------------*/
	    
/*****************************************************************************
  Read_or_Edit_by_policy: This procedure behaves as 'editCV' if the cellview 
			  is found in a RW library and as 'readOnlyCV' if the
			  cellview is found in a RO library on rwpaths.
******************************************************************************/
	case 'j': /* read or edit by policy */

	    /* get user's input */
	    getUserInput("11",cellName,viewName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"ReadOrEditByPolicy");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow edit of a cellview in memory again */      
	    if (currentDesign&&currentDesign->editBit) {
		    current->inMem = TRUE;
		    current->memEditBit = currentDesign->editBit;
		    current->memDirtyBit = currentDesign->dirtyBit;
		    printf("ERROR: editable data in memory already\n");
		    current->errorCode = TRUE;
		    break;
	    }

	    /* make edit call to checkout and get path to the checkout copy */
	    timeBegin();
	    if ((error=dmsCReadOrEditByPolicy(cellName,viewName,&fullPathName))) {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop edit */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,
			timeElapsed());
		
		/* free readonly copy in memory if it is there */
		if (currentDesign){
		    freeDesignData(&firstDesign,&currentDesign);
		    current->readOnly = FALSE;
		}
		
		/* initialize data structure for storing data in memory */
		currentDesign=initDataStructure(&firstDesign,cellName,viewName);
		
		/* read data into memory */
		readFile(currentDesign->designDataPtr,fullPathName);
		
		/* readonly: not editable and clean */
		currentDesign->editBit=FWITKON;
		currentDesign->dirtyBit=FWITKOFF;
		
		current->status = 'o';
		current->cellCreated = TRUE;
		current->memEditBit = currentDesign->editBit;
		current->memDirtyBit = currentDesign->dirtyBit;
		current->inMem = TRUE;
		current->readOnly = FALSE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("edit from %s\n",fullPathName);
		updateTrackNode(tracker,cellName,viewName,0);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------*/
/*------ New addition by Shesha (begin) ------ */
/*---------------------------------------------*/
/*****************************************************************************
  Read_or_Edit_by_policy_BYLIB: This procedure will check only the rwpaths 
				entry which matches the given Library name for 
				Read_or_Edit_by_policy process. 
				-----------
  Read_or_Edit_by_policy: This procedure behaves as 'editCV' if the cellview 
			  is found in a RW library and as 'readOnlyCV' if the
			  cellview is found in a RO library on rwpaths.
******************************************************************************/
	case 'J': /* read or edit by policy by lib*/

            /* get user's input: CellName, ViewName and LibName */
            getUserInputForByLib("21",cellName,viewName,libName);
	    current=findCellView(tracker,cellName,viewName);
	    if (!current){
		tracker=trackCons(buildCell(cellName,viewName),tracker);
		current = tracker;
	    } else current->errorCode = FALSE;
	    strcpy(itkCommand,"ReadOrEditByPolicyByLib");
	    currentDesign=findDesignInMemory(&firstDesign,cellName,viewName);
	    
	    /* not allow edit of a cellview in memory again */      
	    if (currentDesign&&currentDesign->editBit) {
		    current->inMem = TRUE;
		    current->memEditBit = currentDesign->editBit;
		    current->memDirtyBit = currentDesign->dirtyBit;
		    printf("ERROR: editable data in memory already\n");
		    current->errorCode = TRUE;
		    break;
	    }

	    /* make edit call to checkout and get path to the checkout copy */
	    timeBegin();
	    if ((error=dmsCReadOrEditByPolicyByLib(libName,cellName,viewName,"",
							      &fullPathName))) {
		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					  cellName,viewName,timeElapsed());
		printf("ERROR# %d: %s\n",error,dmsCReportMessage());
		current->errorCode = TRUE;
		break; /* stop edit */
	    } else {
		timeEnd();
		fprintf(timeFp,"%s %s %s %s took %f\n",itkCommand,libName,
					  cellName,viewName,timeElapsed());
		
		/* free readonly copy in memory if it is there */
		if (currentDesign){
		    freeDesignData(&firstDesign,&currentDesign);
		    current->readOnly = FALSE;
		}
		
		/* initialize data structure for storing data in memory */
		currentDesign=initDataStructure(&firstDesign,cellName,viewName);
		
		/* read data into memory */
		readFile(currentDesign->designDataPtr,fullPathName);
		
		/* readonly: not editable and clean */
		currentDesign->editBit=FWITKON;
		currentDesign->dirtyBit=FWITKOFF;
		
		current->status = 'o';
		current->cellCreated = TRUE;
		current->memEditBit = currentDesign->editBit;
		current->memDirtyBit = currentDesign->dirtyBit;
		current->inMem = TRUE;
		current->readOnly = FALSE;
		printf("OK: %s\n",dmsCReportMessage());
		printf("edit from %s\n",fullPathName);
		updateTrackNode(tracker,cellName,viewName,0);
	    }
	    dumpFileContentsToLog(cellName,viewName);
	    break;
/*---------------------------------------------------------------*/
	    /*------ New addition .. Delete ..   ------ */
/*---------------------------------------------------------------*/
	case '1': /* delete cell view */
	  getUserInput("11",cellName,viewName);
	  strcpy(itkCommand,"Delete CellView"); /* Version");*/
	  timeBegin();	  
	  /* make delete_cellview call */
	  if ((error=dmsCDelete(1,cellName,viewName,"",&fullPathName))) {
	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,timeElapsed());
	    printf("ERROR# %d: %s\n",error,dmsCReportMessage());
            
	    break; /* stop delete if can't find it */
	  } else {
	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,timeElapsed());
	    printf("delete: %s\n",fullPathName);
	  }
	  break;
	  
	case '2': 
	  /* getUserInput("11",cellName,viewName); */
	  printf("cell name: ");
	  scanf("%s",cellName);  
	  strcpy(itkCommand,"Delete Cell"); 
	  timeBegin();
	  
	  if ((error=dmsCDelete(2,cellName,"","",&fullPathName))) {
	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,timeElapsed());
	    printf("ERROR# %d: %s\n",error,dmsCReportMessage());
            
	    break; 
	  } else {
	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,timeElapsed());
	    printf("delete: %s\n",fullPathName);
	  }
	  break;

	case '3': /* delete version */
	  getUserInput("11",cellName,viewName);
	  printf("view version: ");
	  scanf("%s",verName);
	  strcpy(itkCommand,"Delete ViewVersion"); /* Version");*/
	  timeBegin();	  
	  /* make delete_version call */
	  if ((error=dmsCDelete(3,cellName,viewName,verName,&fullPathName))) {
	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,timeElapsed());
	    printf("ERROR# %d: %s\n",error,dmsCReportMessage());
            
	    break; /* stop delete if can't find it */
	  } else {
	    timeEnd();
	    fprintf(timeFp,"%s %s %s took %f\n",itkCommand,cellName,viewName,timeElapsed());
	    printf("delete: %s\n",fullPathName);
	  }
	  break;

        case '5':
          getUserInput("11",cellName,viewName);    
          strcpy(itkCommand, "Get cds.lib FileName");
          timeBegin();
          if ((error=dmsCGetCdslibFileName(&cdslibName))) {
            timeEnd();
            fprintf(timeFp,"%s took %f\n",itkCommand,timeElapsed());
            printf("ERROR# %d: %s\n",error,dmsCReportMessage());
            break; 
          } else {
            timeEnd();            
            fprintf(timeFp,"%s took %f\n",itkCommand,timeElapsed());
            printf("cds.lib FileName: %s\n",cdslibName);
          }
          
          break;

        case '6':
          getUserInput("11",cellName,viewName);
          printf("view version: ");
          scanf("%s",verName);
          if(!strcmp(verName, "0")) verName[0] = '\0';
	  strcpy(itkCommand, "dmsCReadonlyCellViewTypeByLib");
          timeBegin();
          if ((error=dmsCReadonlyCellViewTypeByLib("*",cellName,viewName,
                                                verName,&vName,&fullPathName))) {
            timeEnd();
            fprintf(timeFp,"%s took %f\n",itkCommand,timeElapsed());
            printf("ERROR# %d: %s\n",error,dmsCReportMessage());
            break;
          } else {
            timeEnd();
            fprintf(timeFp,"%s took %f\n",itkCommand,timeElapsed());
            printf("get fullPathName: %s\n",fullPathName);
          }

          break;
#if (defined(OH_MACH_LINUX) || defined(NEW_LIB_STRUCTURE))
        case '7':
	  getUserInput("11",cellName,viewName);
          fullPathName = dmCGenObjRel(cellName, viewName, 0);
          if(!fullPathName)
            printf("dmCGenObjRel returns NULL\n");
          else
            printf("dmCGenObjRel: %s\n", fullPathName);
          break;
        case '8':
          getUserInput("11",cellName,viewName);
          fullPathName = dmCGenObjRel(cellName, viewName, 1);
          if(!fullPathName)
            printf("dmCGenObjRel returns NULL\n");
          else
            printf("dmCGenObjRel: %s\n", fullPathName);
          break;
	case '~':
	  printf("library: ");
          scanf("%s",libName);
  	  dmsCGetLibPath(libName,&fullPathName); 
	  if(!fullPathName)
	    printf("dmCGetLibraryPath returns NULL\n");
	  else 
	    printf("%s is from: %s\n", libName, fullPathName);
	  break;
#endif

/*---------------------------------------------*/
        case '0': /* get multiple extensions */
            strcpy(itkCommand,"GetMultipleExts");
            timeBegin();
            if ((error=dmsCGetMultipleExts(&extList))){
                timeEnd();
                fprintf(timeFp,"%s output: %s took %f\n",itkCommand,
                                                extList,timeElapsed());
                printf("ERROR# %d: %s\n",error,dmsCReportMessage());
                break;
            } else {
                timeEnd();
                fprintf(timeFp,"%s output: %s took %f\n",itkCommand,
                                                extList,timeElapsed());
                printf("OK: %s\n",dmsCReportMessage());
                printf("The Multiple extensions are: %s\n", extList);
            }
            break;
/*-------------------------------------------------------------*/
 
/*
            dmsCGetLastCellViewInfo(&info);
            if (noPrintFlag==FWITKOFF){
               lP = "";
               lN = "";
               cN = info->cellName;
               vN = info->viewName;
               vS = "";
               uS = "";
               fP = "";   
            switch (info->location){
               case idmsCvNotFound:
                  break;
               case idmsCvInWorklib:
               case idmsCvInRwlib:
               case idmsCvInRolib:
                   lP = info->libPath;
                   lN = info->libName;
                   uS = info->coUser;
                   vS = info->verName;
*/
                  /* break missing by intention !!! */    
/*
               default:
                  fP = info->fullPathName;
                  break; 
            }

               printf("Information: [%d][%s][%s][%s][%s][%s][%s]\n",
                        info->location, lP, lN, cN, vN, vS, uS);
               printf("FullPathName: %s\n", fP);
            }
           if (!(plib_id = coralCGetLibId(cellName,viewName,"", &error))){
               printf("ERROR# %d: %s\n",error,dmsCReportMessage());
	       printf("Can't delete_version since couldn't get LibID\n");
               break;
            }
	    timeBegin();
	    printf("** Can't implement at this time\n");
*/
	    /***
            if (dmDeleteVersion(libID,cellName,viewName,info->verName))
	       printf("*SUCCESS: %s: version deletion: \n",itkCommand);
            else { printf("* ERROR ----- Delete Not happened \n");
	           printf("  ERROR REPORT: %s\n",voGetWarn());
	           printf("\n");
            }
	    ****/
/*
	    timeEnd();

	    fprintf(timeFp,"%s: Policy info: %s took %f\n",itkCommand,
	    				     dmsBufPtr,timeElapsed());
	    break;
*/


/*---------------------------------------------*/
/*------ New NEW END by Shesha (end)   ------ */
/*---------------------------------------------*/
/*	case '1' : *//* Set *//*
	    strcpy(itkCommand,command);
	    getSourceInfo(sCellName,sViewName);
	    getDestInfo(dCellName,dViewName);
	    getRelName(relName);

	    if (error=dmsCSetRelation(NULL,sCellName,sViewName,NULL,NULL,
				      dCellName,dViewName,NULL,relName))
		printf("OK: %s\n",dmsCReportMessage());
	    else printf("ERROR %d: %s\n",error,dmsCReportMessage());
	    break;
	case '2' : *//* Get *//*
	    strcpy(itkCommand,command);
	    getSourceInfo(sCellName,sViewName);
	    getRelationName(relName);
	    
	    if (error=dmsCGetRelation(NULL,sCellName,sViewName,NULL,NULL,
				      &dCellName,&dViewName,NULL,relName))
		printf("OK: %s\n",dmsCReportMessage());
	    else printf("ERROR %d: %s\n",dmsCReportMessage());
	    break;
	case '3' : *//* Delete *//*
	    strcpy(itkCommand,command);
	    getSourceInfo(sCellName,sViewName);
	    getDestInfo(dCellName,dViewName);

	    if (error=dmsCDeleteRelation(NULL,sCellName,sViewName,NULL,NULL,
					 dCellName,dViewName,NULL,relName))
	        printf("OK: %s\n",dmsCReportMessage());
	    else printf("ERROR %d: %s\n",dmsCReportMessage());
	    break;
*/	    
	}

	if ((iFlag==FWITKOFF)&&(noPrintFlag==FWITKOFF))
	    printf("ITK> Command was %s %s %s\n",itkCommand,cellName,viewName);
	dmsCGetLastCellViewInfo(&info);
	if (noPrintFlag==FWITKOFF){
               lP = "";
               lN = "";
               cN = info->cellName;
               vN = info->viewName;
               vS = "";
               uS = "";
               fP = "";
            switch (info->location){
               case idmsCvNotFound:
                  break;
               case idmsCvInWorklib:
               case idmsCvInRwlib:
               case idmsCvInRolib:
                   lP = info->libPath;
                   lN = info->libName;
                   uS = info->coUser;
                   vS = info->verName;
                  /* break missing by intention !!! */
               default:
                  fP = info->fullPathName;
                  break;
            }
               printf("Information: [%d][%s][%s][%s][%s][%s][%s]\n",
                        info->location, lP, lN, cN, vN, vS, uS);
               printf("FullPathName: %s\n", fP);

	} else noPrintFlag=FWITKOFF;
    }

    /* terminate iDMS procedural interface after all dms calls */
    if ((error=dmsCPIExitDMS()))
	printf("ERROR# %d: %s\n",error,dmsCReportMessage());
    else printf("OK: %s\n",dmsCReportMessage());
    /* make post-exit call to induce error message */
    if ((error=dmsCInfoCellView(0,"blah","dude",&info)))
        printf("ERROR# %d: %s\n",error,dmsCReportMessage());
    else printf("OK: %s\n",dmsCReportMessage());
/*    printTrack(tracker); */
    fclose(timeFp);
} /* main */

