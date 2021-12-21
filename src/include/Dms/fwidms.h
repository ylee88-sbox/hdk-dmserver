/*$Header: /home/lsi/lsi/cvsroot-nike/nike/nfl/include/Dms/fwidms.h,v 1.1.1.1 1996/11/15 02:12:58 etse Exp $
*/
/*$Log: fwidms.h,v $
 *Revision 1.1.1.1  1996/11/15 02:12:58  etse
 *Imported sources
 *
 * Revision 1.1  1996/09/05  19:28:03  ychee
 * First commit
 *
#Revision 1.4  1993/04/02  17:52:59  cchiu
#Cleanup 2.0, beautify 2.1
#
#Revision 1.3  1993/02/21  10:33:09  dhejmad
#typo
#
#Revision 1.2  93/02/21  10:25:52  dhejmad
#added dmsCGetLastCellViewInfo line
#
#Revision 1.1  93/01/04  09:48:54  cchiu
#Initial revision
#
#Revision 1.7  92/11/16  18:21:38  cchiu
#not define DMSINFO here.
#
#Revision 1.6  92/11/16  17:53:24  cchiu
#define DMSINFO in here instead of fwierr.h.
#
#Revision 1.5  92/11/09  16:38:52  cchiu
#Added some new stuffs.
#
#Revision 1.4  92/10/12  13:12:31  cchiu
#dmsCReadonlyCellViewType added.
#
#Revision 1.3  92/09/24  09:49:56  cchiu
#2.1 standard procedures are in.
#
#Revision 1.2  92/09/22  17:59:43  cchiu
#New 2.1 stuffs are in, includes standard 2.1 and extended 2.1 procedures.
#
#Revision 1.1  92/09/11  16:23:18  dhejmad
#Initial revision
#
*/

/************ old comments from 2.0 
#Revision 1.1  92/09/11  11:42:36  cchiu
#Initial revision
#
#Revision 1.7  92/09/10  11:25:06  dhejmad
#added unlockcellview to call
#
#Revision 1.6  92/09/09  11:24:21  cchiu
#Added dmsCVersionNumber for getting iDMS version number.
#
#Revision 1.5  92/09/09  08:25:54  dhejmad
#added attribute PI calls
#
#Revision 1.4  92/08/06  08:07:01  dhejmad
#*** empty log message ***
#
#Revision 1.3  92/05/12  15:25:50  cchiu
#Add dmsCSetToolName and dmsCGetLibId.
#*/

/* STANDARD 2.1 PROCEDURES */

extern "C" {


extern int dmsCPIInitDMS(const char* toolname, const char* bootname, int nullgtmode);

extern int dmsCPIExitDMS();

extern int dmsCCreateCellView();

extern int dmsCCreateUnixFile();

extern int dmsCReadonlyCellView();

extern int dmsCReadonlyCellViewType(const char *cellname, 
				    const char* viewtype, 
				    const char **viewname, 
				    const char **fullpath);

extern int dmsCCheckoutCellView();

extern int dmsCLoadCellView();

extern int dmsCEditCellView();

extern int dmsCReadOrEditByPolicy();

extern int dmsCSaveCellView();

extern int dmsCCheckinCellView();

extern int dmsCUpdateCellView();

extern int dmsCInstallCellView();

extern int dmsCPurgeCellView();

extern int dmsCCancelCheckoutCellView();

extern int dmsCInfoCellView();

extern int dmsCGetLastCellViewInfo();

extern char *dmsCReportMessage();

extern int dmsCParsePath();

extern char *dmsCPrintPath();

extern char *dmsCVersionNumber();

extern char *dmsCGetSearchPath();

extern int dmsCGetWorkingDir();

extern int dmsCGetWorkingLib();

extern int dmsCPrintIdmsLog();

extern int dmsCSetToolName();

extern int dmsCGetLibId();

extern char *dmsCGetPolicyViewList();

/* ADVANCE 2.1 PROCEDURES */

extern int dmsCSetAttribute();

extern int dmsCSetAttributeList();

extern int dmsCGetAttribute();

extern int dmsCGetAllAttributes();

extern int dmsCDeleteAttribute();

extern int dmsCGetMultipleViews();

extern int dmsCSetMultipleViews();

/* EXTENDED 2.1 PROCEDURES */

extern int dmsCReadonlyCellViewByLib();

extern int dmsCReadonlyCellViewTypeByLib();

extern int dmsCCheckoutCellViewByLib();

extern int dmsCEditCellViewByLib();

extern int dmsCReadOrEditByPolicyByLib();

extern int dmsCInstallCellViewByLib();

extern int dmsCCheckinCellViewByLib();

extern int dmsCUpdateCellViewByLib();

extern int dmsCCancelCheckoutCellViewByLib();

extern int dmsCGetLibIdByLib();


}

