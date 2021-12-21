#ifndef FWIMSG_H
#define FWIMSG_H

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
# $Id: fwimsg.h,v 1.1 1997/12/12 17:25:23 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
/*
#Revision 1.19  1993/06/16  20:42:12  cchiu
#Some messages for iRCS to report file unchanged.
#
#Revision 1.1  92/10/26  09:15:53  cchiu
#Initial revision
#*/

#include <fwierr.h>

/* DMS MESSAGES */
#define NOIDMSERROR        idmsSuccess
#define OKTYPE             1
#define WARNING            2
#define ERROR              3
#define DMSPATHOK          4
#define ZEROLENGTHFILE     idmsCanNotCheckinZeroLengthFile
#define DMSENVNOTSET       idmsEnvironmentVaribaleDmspathNotSet
#define LIBNOTONRWPATHS    7
#define CHECKOUT           8
#define CHECKIN            9
#define SEARCHPATH        10
#define WORKINGDIR        11
#define UNKNOWNCMD        12
#define EXITDMS           13
#define INITDMS           14
#define IMPORTCOUNT       15
#define EXAMINEPROP       16
#define MEMORYUSED        17
#define TOTALWILD         18
#define UNCHECKOUT        19
#define CANTUNCHECKOUT    idmsCvCheckoutCanNotBeCancelled
#define NOWORKINGDIR      21
#define DELETEOK          22
#define DELETEFAIL        23
#define WORKDIRFILEFOUND  24
#define UNIXFILEFOUND     25
#define DMSFILEFOUND      26
#define WRITEREADONLY     27
#define CHECKOUTREADONLY  idmsCanNotCheckoutCvInRoLibrary
#define FILENOTFOUND      idmsCvNotFoundOnRwpaths
#define CANTCREATE        30
#define CANNOTCHECKIN     idmsCvCanNotBeCheckedIn
#define CANNOTCHECKOUT    idmsCvCanNotBeCheckedOut
#define NOTINSTALLED      idmsCvNotInstalledToRead
#define NOCHECKINFILE     idmsNoFileInWorkdirToBeCheckedIn
#define CANTDBINIT        35
#define DMSPATHERROR      idmsDmspathSyntaxError
#define CANTREADDIRCDBA   37
#define EXPORTCOUNT       38
#define CHECKOUTDIR       39
#define EXISTALREADY      idmsFileAlreadyExistsInWorkdir
#define FETCHOK           41
#define CANTFETCH         42
#define READONLYCOPY      43
#define CANTCIREADONLY    idmsCanNotCheckinNotWriteableFile
#define NOAUDITTRAIL      45
#define NOTINANYLIB       idmsCvNotFoundInAnyLibrary
#define LIBNOTFOUND       47
#define AUDITTRAILOK      48
#define AUDITTRAILERROR   49
#define REFRESHLIBOK      50
#define UNIXCMDOK         51
#define CANTREADLIB       52
#define NOSUCHCONF        53
#define BREAKBYSEMICOL    54
#define CRTPROPINROLIB    55
#define CREATEPROPOK      56
#define FILERECHECKOUT    idmsCvWasCheckedOutByAnotherUser
#define ENABLEAUDIT       58
#define NOCVMATCH         59
#define CONFEXIST         60
#define DUMPCONFIGOK      61
#define DUMPCONFIGERROR   62
#define CREATECONFIGOK    63
#define CREATECONFIGERROR 64
#define REMOVECONFIGOK    65
#define REMOVECONFIGERROR 66
#define LISTCONFIGOK      67
#define ADDTOCONFIGOK     68
#define REPLACECVOK       69
#define CANTADDTOCONFIG   70
#define DELETEPROP        71
#define CANTDELPROP       72
#define CANTCRTPROP       73
#define DISABLEAUDIT      74
#define NOTCHECKOUT       idmsCvWasNotCheckedOut
#define RMFROMCONFIGOK    76
#define CANTRMFROMCONFIG  77
#define DIFFOK            78
#define SHOWHEADERLOG     79
#define SKIPNOLOCK        80
#define CANTOPENWORKLIB   idmsCanNotOpenWorklibForWrite
#define CREATEOK          82
#define CANNOTCREATE      idmsCvCanNotBeCreatedInWorklib
#define CVEXIST           idmsCvAlreadyExistsInWorklib
#define CANTWRITELIB      85
#define LOCKBEFORELOAD    86
#define ONLYRWLIBCANLOAD  87
#define LOCKBEFORESAVE    88
#define ONLYRWLIBCANSAVE  89
#define CREATEUNIXOK      90
#define LOADFROMWORKDIR   91
#define NOTEXISTTOLOAD    idmsFileNotExistInWorkdirToLoad
#define SAVETOWORKDIR     93
#define NOTEXISTTOSAVE    idmsFileNotExistInWorkdirToSave
#define FILENOTREADABLE   idmsFileInWorkdirIsNotReadable
#define FILENOTWRITEABLE  idmsFileInWorkdirIsNotWriteable
#define CANTCHECKINROLIB  idmsCanNotCheckinCvInRoLibrary
#define CANTUPDATEROLIB   idmsCanNotUpdateCvInRoLibrary
#define NOTEXISTTOINSTALL idmsCvNotExistInWorklibToInstall
#define CANTCREATEFILE    idmsFileCanNotBeCreatedInWorkdir
#define INSTALL           101
#define CANNOTINSTALL     idmsCvCanNotBeInstalled
#define ILLEGALCOPY       idmsIllegalCvCopyInWorkdir
#define VIEWMAPERROR      idmsMultipleViewMapError

/* attribute PI error codes */
#define IDMSFAILURE 	  idmsFailure
#define IDMSBADPARAMETER  idmsBadparameter
#define IDMSERROR 	  idmsError

#define WORKINGLIB        108

/* new error code indicating dms not initialized */
#define IDMSNOTINITIALIZED idmsNotInit

#define PRETRIGGERFAILED  idmsPreTriggerFailed
#define VERSIONNOTEXIST   111
#define LOADCONFIGOK      112
#define LOADCONFIGERROR   113
#define ADDSUBCONFIGOK    114
#define ADDSUBCONFIGERROR 115
#define RMSUBCONFIGOK     116
#define RMSUBCONFIGERROR  117
#define CRTCONFINSTOK     118
#define CRTCONFINSTERROR  119
#define CRTCONFIGCMTOK    120
#define FILENOTCHANGED    121
#define INSERTCFOK        122
#define POSTTRIGGERFAILED idmsPostTriggerFailed

/** addded **/
/* #define LIBPATHNOTFOUND   125 */
#define DISABLESYNC	  125
#define MESSAGENUMBER     126

static char dmsMessage[MESSAGENUMBER][80]={
  /*0*/
  "*IDMS OK* ",
  "*IDMS MESSAGE* ",
  "*IDMS WARNING* ",
  "*IDMS ERROR* ",
  "PARSE DMS PATH FILE O.K.\n%s",
  "CELLVIEW: %s.%s IN WORKDIR: %s IS A ZERO LENGTH FILE\n",
  "ENVIRONMENT DMSPATH NOT SET ALL POLICIES USE DEFAULTS\n",
  "LIBRARY: %s IS NOT ON YOUR RWPATHS OF TOOL: %s VIEW: %s\n",
  "CHECKOUT %s.%s %s FROM LIBRARY: %s CONFIG: %s\n",
  "CHECKIN %s.%s %s TO %s IN LIBRARY: %s CONFIG: %s\n",
  /*10*/
  "TOOL: %s LIB SEARCH PATH: %s\n",
  "TOOL: %s VIEW: %s WORKDIR: %s\n",
  "UNKNOWN COMMAND: %s RECEIVED...\n",
  "EXIT iDMS %s\n",
  "INITIALIZE iDMS %s\n",
  "TOTAL: %d CELL(S) IMPORTED BY LIST: %s TO LIBRARY: %s\n",
  "PROPERTY LIST OF LIBRARY: %s CELLVIEW: %s.%s VER: %s\n",
  "MEMORY USED: %d BYTES\n",
  "TOTAL: %d CELL(S)\n",
  "CANCEL CHECKOUT OF LIBRARY: %s CELLVIEW: %s.%s VER: %s\n",
  /*20*/
  "CAN NOT CANCEL CHECKOUT LIBRARY: %s CELLVIEW: %s.%s VER: %s\n",
  "TOOL: %s VIEW: %s WORKDIR: DEFINED BY UNKNOWN VARIABLES\n",
  "DELETE CELLVIEW: %s.%s FROM LIBRARY: %s\n",
  "CAN NOT DELETE CELLVIEW: %s.%s FROM LIBRARY: %s\n",
  "CELLVIEW: %s.%s IN WORKDIR: %s\n",
  "CELLVIEW: %s.%s IN UNIX DIRECTORY: %s\n",
  "CELLVIEW: %s.%s %s IN LIBRARY: %s CONFIG: %s\n",
  "CAN NOT CHECKIN FILE: %s.%s IN UNIX DIRECTORY: %s\n",
  "CAN NOT CHECKOUT CELLVIEW: %s.%s IN READONLY LIBRARY: %s\n",
  "CAN NOT FIND FILE: %s.%s %s IN CONFIG: %s ON DMS SEARCH PATH\n",
  /*30*/
  "CAN NOT CREATE CELLVIEW: %s.%s IN WORKLIB: %s\n",
  "CAN NOT CHECKIN CELLVIEW: %s.%s %s IN LIBRARY: %s\n",
  "CAN NOT CHECKOUT CELLVIEW: %s.%s %s IN LIBRARY: %s CONFIG: %s\n",
  "CELLVIEW: %s.%s IN LIBRARY: %s IS NOT INSTALLED OR ZERO LENGTH\n",
  "NO FILE: %s.%s IN WORKDIR: %s TO CHECKIN/UPDATE/INSTALL\n",
  "DMS INITIALIZATION FAILS\n",
  "DMS PATH FILE ERROR CONTINUE AT YOUR OWN RISK\n%s",
  "CAN NOT READ CDBA: %s.%s DIRECTLY FROM DIRECTORY: %s\n",
  "TOTAL: %d CELL(S) EXPORTED BY LIST: %s FROM LIBRARY: %s\n",
  "CAN NOT CHECKOUT CELLVIEW: %s.%s FROM READONLY DIRECTORY: %s\n",
  /*40*/
  "CELLVIEW: %s.%s ALREADY EXISTS IN WORKDIR: %s\n",
  "FETCH LIBRARY: %s CELLVIEW: %s.%s %s TO %s O.K.\n",
  "FETCH LIBRARY: %s CELLVIEW: %s.%s %s TO %s FAILED\n",
  "CHECKOUT MIGHT OVERWRITE READONLY COPY: %s.%s IN DIRECTORY: %s\n",
  "CAN NOT CHECKIN/UPDATE/INSTALL READONLY COPY: %s.%s IN DIRECTORY: %s\n",
  "NO AUDITTRAIL INFORMATION AVAILABLE IN FILE: %s\n",
  "CELLVIEW: %s.%s NOT IN ANY LIBRARY CONFIG: %s OF CURRENT LIBPATH\n",
  "LIBRARY: %s NOT ON CURRENT LIBPATH OF TOOL: %s\n",
  "QUERY LIBRARY: %s AUDITTARIL O.K.\n",
  "QUERY LIBRARY: %s AUDITTARIL ERROR\n",
  /*50*/
  "REFRESH LIBRARY: %sO.K.\n",
  "EXECUTE UNIX COMMAND: %s O.K.\n",
  "CAN NOT OPEN LIBRARY: %s FOR READ\n",
  "NO SUCH CONFIG: %s IN LIBRARY: %s\n",
  "COMMAND ABORTED BY SEMICOLON\n",
  "CAN NOT CREATE PROPERTY IN READONLY LIBRARY: %s CELLVIEW: %s.%s\n",
  "CREATE PROPERTY: %s LIBRARY: %s CELLVIEW: %s.%s VER: %s O.K.\n",
  "LIBRARY: %s CELLVIEW: %s.%s %s WAS CHECKED OUT TO %s BY %s\n",
  "ENABLE AUDITTRAIL FOR LIBRARY: %s\n",
  "NO MATCHED CELLVIEW: %s.%s ON RWPATHS OF TOOL: %s\n",
  /*60*/
  "CONFIG: %s ALREADY EXISTS IN LIBRARY: %s\n",
  "DUMP CONFIG: %s IN LIBRARY: %s TO FILE: %s O.K.\n",
  "DUMP CONFIG: %s IN LIBRARY: %s TO FILE: %s ERROR\n",
  "CREATE CONFIG: %s IN LIBRARY: %s O.K.\n",
  "CREATE CONFIG: %s IN LIBRARY: %s ERROR\n",
  "REMOVE CONFIG: %s FROM LIBRARY: %s O.K.\n",
  "REMOVE CONFIG: %s FROM LIBRARY: %s ERROR\n",
  "LIST ALL CONFIG IN LIBRARY: %s O.K.\n",
  "ADD LIBRARY: %s CELLVIEW: %s.%s %s TO CONFIG: %s O.K.\n",
  "REPLACE LIBRARY: %s CELLVIEW: %s.%s %s IN CONFIG: %s O.K.\n",
  /*70*/
  "CAN NOT ADD LIBRARY: %s CELLVIEW: %s.%s %s TO CONFIG: %s\n",
  "REMOVE PROPERTY: %s LIBRARY: %s CELLVIEW: %s.%s VER: %s O.K.\n",
  "CAN NOT REMOVE PROPERTY: %s LIBRARY: %s CELLVIEW: %s.%s VER: %sn",
  "CAN NOT CREATE PROPERTY: %s LIBRARY: %s CELLVIEW: %s.%s VER: %s\n",
  "DISABLE AUDITTRAIL FOR LIBRARY: %s\n",
  "LIBRARY: %s CELLVIEW: %s.%s %s WAS NOT CHECKED OUT\n",
  "REMOVE LIBRARY: %s CELLVIEW: %s.%s FROM CONFIG: %s O.K.\n",
  "CAN NOT REMOVE LIBRARY: %s CELLVIEW: %s.%s FROM CONFIG: %s\n",
  "DIFF LIBRARY: %s CELLVIEW: %s.%s %s AND %s O.K.\n",
  "SHOW HEADER AND LOG OF CELLVIEW: %s.%s O.K.\n",
  /*80*/
  "SKIP LIBRARY: %s CELLVIEW: %s.%s BECAUSE YOU DO NOT HAVE LOCK\n",
  "CAN NOT OPEN WORKLIB: %s FOR WRITE ON SEARCH PATH: %s\n",
  "CREATE CELLVIEW: %s.%s IN LIBRARY: %s O.K.\n",
  "CAN NOT CREATE CELLVIEW: %s.%s IN LIBRARY: %s\n",
  "CELLVIEW: %s.%s ALREADY EXISTS IN LIBRARY: %s\n",
  "CAN NOT OPEN LIBRARY: %s ON PATH: %s FOR WRITE\n",
  "MUST CHECKOUT CELLVIEW: %s.%s IN LIBRARY: %s BEFORE LOAD\n",
  "CAN NOT LOAD READONLY CELLVIEW: %s.%s FROM %s FOR EDIT\n",
  "CELLVIEW: %s.%s WAS NOT CHECKED OUT FOR EDIT IN LIBRARY: %s\n",
  "CELLVIEW: %s.%s IS NOT EDITABLE IN READONLY LIBRARY: %s\n",
  /*90*/
  "UNIX FILE: %s.%s CREATED IN WORKDIR: %s O.K.\n",
  "LOAD CELLVIEW: %s.%s FROM WORKDIR: %s O.K.\n",
  "CELLVIEW: %s.%s DOES NOT EXIST IN WORKDIR: %s TO LOAD\n",
  "SAVE CELLVIEW: %s.%s TO WORKDIR: %s O.K.\n",
  "CELLVIEW: %s.%s DOES NOT EXIST IN WORKDIR: %s TO SAVE\n",
  "CELLVIEW: %s.%s EXISTS IN WORKDIR: %s BUT NOT READABLE\n",
  "CELLVIEW: %s.%s EXISTS IN WORKDIR: %s BUT NOT WRITEABLE\n",
  "CAN NOT CHECKIN CELLVIEW: %s.%s IN READONLY LIBRARY: %s\n",
  "CAN NOT UPDATE CELLVIEW: %s.%s IN READONLY LIBRARY: %s\n",
  "CELLVIEW: %s.%s NOT EXIST IN WORKLIB: %s TO BE INSTALLED\n",
  /*100*/
  "CAN NOT CREATE UNIX FILE: %s.%s IN WORKDIR: %s\n",
  "INSTALL %s.%s 1.1 IN LIBRARY: %s CONFIG: %s\n",
  "CAN NOT INSTALL CELLVIEW: %s.%s IN LIBRARY: %s CONFIG: %s\n",
  "CAN NOT CHECKOUT LIBRARY: %s CELLVIEW: %s.%s ALREADY EXISTS IN WORKDIR\n",
  "CAN NOT RETRIEVE TOOL: %s VIEWTYPE: %s INFORMATION FROM MAP\n",
  "THE CALL FAILED\n",
  "BAD PARAMETERS PASSED TO PROCEDURE\n",
  "ERROR IN THE PROCEDURE - LOOK FOR DIAGNOTICS\n",
  "TOOL: %s VIEW: %s WORKLIB: %s\n",
  "IDMS IS NOT INITIALIZED\n",
  /* 110 */
  "COULD NOT EXECUTE PRE-TRIGGER\n",
  "CELLVIEW: %s.%s IN LIB: %s DOES NOT HAVE VER: %s\n",
  "LOAD CONFIG FILE: %s INTO LIBRARY: %s O.K.\n",
  "LOAD CONFIG FILE: %s INTO LIBRARY: %s ERROR\n",
  "ADD SUB-CONFIG: %s of LIB: %s INTO CONFIG: %s of LIB: %s O.K.\n",
  "ADD SUB-CONFIG: %s of LIB: %s INTO CONFIG: %s of LIB: %s ERROR\n",
  "REMOVE SUB-CONFIG: %s of LIB: %s FROM CONFIG: %s of LIB: %s O.K.\n",
  "REMOVE SUB-CONFIG: %s of LIB: %s FROM CONFIG: %s of LIB: %s ERROR\n",
  "CREATE CONFIG INSTANCE: %s FROM LIB: %s CONFIG: %s O.K.\n",
  "CREATE CONFIG INSTANCE: %s FROM LIB: %s CONFIG: %s ERROR\n",
  /* 120 */
  "CREATE COMMENT: \"%s\" FOR LIB: %s CONFIG: %s O.K.\n",
  "NO CHECKIN ON UNCHANGED FILE: %s.%s IN WORKDIR: %s\n",
  "CELLVIEW ENTRIES IN LIB: %s CONFIG: %s WERE INSERTED TO CONFIG: %s\n",
  "COULD NOT EXECUTE POST-TRIGGER\n",
  "4.4 lib: %s path not found\n",
  "Library operations not available - Running in Synchronicity Disabled mode\n"
  };

#endif
