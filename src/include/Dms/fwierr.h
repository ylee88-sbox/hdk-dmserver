/*$Header: /home/lsi/lsi/cvsroot-nike/nike/nfl/include/Dms/fwierr.h,v 1.1.1.1 1996/11/15 02:12:58 etse Exp $*/
/*$Log: fwierr.h,v $
 *Revision 1.1.1.1  1996/11/15 02:12:58  etse
 *Imported sources
 *
 * Revision 1.1  1996/09/05  19:28:04  ychee
 * First commit
 *
#Revision 1.13  1994/10/28  16:50:25  fw
#added protection for multiple inclusions
#
#Revision 1.12  1993/06/15  16:41:09  cchiu
#enlarge  the dmsInfo internal std::string size.
#
#Revision 1.11  1993/02/23  10:14:59  cchiu
#define idms graphic modes.
#
#Revision 1.10  93/02/18  17:56:53  cchiu
#Error number for pre-trigger.
#
#Revision 1.9  93/02/08  18:07:36  cchiu
#Minor changes.
#
#Revision 1.8  93/02/08  17:31:02  dhejmad
#added error for not initialized
#added error for not initialized
#
#Revision 1.7  93/01/28  14:36:12  cchiu
#Fix macro name idmsCvWasNotCheckout -> idmsCvWasNotCheckedOut.
#
#Revision 1.6  93/01/28  13:32:27  cchiu
#idmsCvWasNotCheckout message from notcheckouttocancel.
#
#Revision 1.5  93/01/22  15:39:17  cchiu
#Get the last cellview location macros back from fwimsg.h
#
#Revision 1.4  93/01/22  09:54:20  cchiu
#iDMS internal passing macros moved to fwimsg.h.
#
#Revision 1.3  93/01/13  13:48:22  cchiu
#define macros for locations.
#
#Revision 1.2  93/01/05  09:38:26  cchiu
#Added location and libPath to CVINFO
#
#Revision 1.1  93/01/04  10:16:30  cchiu
#Initial revision
#
#Revision 1.7  92/11/19  09:32:16  cchiu
#Modify error macros for install.
#
#Revision 1.6  92/11/16  18:21:19  cchiu
#define DMSINFo here.
#
#Revision 1.5  92/11/16  17:51:35  cchiu
#To be consistent with mainsail error macros which do not like "_".
#
#Revision 1.4  92/11/11  16:56:07  cchiu
#fullPathName be part of the DMSINFO.
#
#Revision 1.3  92/11/11  13:30:27  cchiu
#Distinguish readonly lib and dir.
#
#Revision 1.2  92/11/10  15:10:55  cchiu
#DMSINFO structure defined for info procedure.
#
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
