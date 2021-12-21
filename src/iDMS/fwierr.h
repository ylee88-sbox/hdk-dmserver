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
# $Id: fwierr.h,v 1.1 1997/12/12 17:25:21 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
/*
#Revision 1.1  92/11/09  15:57:18  cchiu
#Initial revision
#*/


#ifndef  _H_FWIERR
#define  _H_FWIERR

/* DMSINFO definition for dmsCInfoCellView() */
typedef struct dmsInfo {
  int  location; /* 0: not exist 1: workdir 2: unixdir 3: RW 4: RO 5: create */
  char libPath[1024];
  char libName[1024];
  char cellName[1024];
  char viewName[1024];
  char verName[1024];
  char coUser[1024];
  char fullPathName[1024];
} DMSINFO;

/* no error */
#define idmsSuccess                          0

/* DMS Error */
#define idmsNotInit                          109

/* Pre-trigger Failure */
#define idmsPreTriggerFailed                 110
#define idmsPostTriggerFailed                123

/* initialization/reparse */
#define idmsEnvironmentVaribaleDmspathNotSet 6
#define idmsDmspathSyntaxError               36

/* createcellview/install */
#define idmsCanNotOpenWorklibForWrite        81

/* craetecellview */
#define idmsCvCanNotBeCreatedInWorklib       83
#define idmsCvAlreadyExistsInWorklib         84

/* install */
#define idmsCvCanNotBeInstalled              102
#define idmsCvNotExistInWorklibToInstall     99

/* createunixfile */
#define idmsFileCanNotBeCreatedInWorkdir     100
#define idmsFileAlreadyExistsInWorkdir       40

/* readonly */
#define idmsCvNotFoundOnRwpaths              29
#define idmsCvNotInstalledToRead             33
#define idmsMultipleViewMapError             104

/* checkout/edit/checkin/update/cancel checkout */
#define idmsCvNotFoundInAnyLibrary           46

/* checkout/edit/update */
#define idmsCvWasCheckedOutByAnotherUser     57
#define idmsCvCanNotBeCheckedOut             32
#define idmsCanNotCheckoutCvInRoLibrary      28
#define idmsIllegalCvCopyInWorkdir           103

/* load/edit */
#define idmsFileInWorkdirIsNotReadable       95
#define idmsFileNotExistInWorkdirToLoad      92

/* save */
#define idmsFileInWorkdirIsNotWriteable      96
#define idmsFileNotExistInWorkdirToSave      94

/* checkin/update */
#define idmsCvCanNotBeCheckedIn              31
#define idmsCvIsNotInstalledToCheckin        33
#define idmsCanNotCheckinZeroLengthFile      5
#define idmsCanNotCheckinNotWriteableFile    44
#define idmsNoFileInWorkdirToBeCheckedIn     34

/* checkin */
#define idmsCanNotCheckinCvInRoLibrary       97

/* update */
#define idmsCanNotUpdateCvInRoLibrary        98

/* cancel checkout */
#define idmsCvCheckoutCanNotBeCancelled      20

/* checkin/cancel checkout */
#define idmsCvWasNotCheckedOut               75

/* info */
#define idmsSearchWholeRwpaths               0
#define idmsSearchLibOnly                    1

#define idmsInfoCvNotFoundOnRwpaths          0
#define idmsInfoCvNotFoundInAnyLibrary       1
#define idmsInfoCvIsInWorkdir                2
#define idmsInfoCvCheckedOutByMe             3
#define idmsInfoCvIsInReadonlyUnixdir        4
#define idmsInfoCvIsInReadonlyLibrary        5
#define idmsInfoCvCheckedOutByAnotherUser    6
#define idmsInfoCvCanBeCheckedOut            7
#define idmsInfoUnknownEvent                 77

/* attribute pi error codes */
# define idmsFailure 	                     105
# define idmsBadparameter                    106
# define idmsError 	                     107

/* last cellview location */
#define idmsCvNotFound                       0
#define idmsCvInWorkdir                      1
#define idmsCvInUnixdir                      2
#define idmsCvInRwlib                        3
#define idmsCvInRolib                        4
#define idmsCvInWorklib                      5

/* iDMS Graphic Mode */
#define idmsGraphicMode			     0
#define idmsNullgtMode			     1



#endif
