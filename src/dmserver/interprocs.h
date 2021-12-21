/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program:  Procedures header file                                         */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: interprocs.h,v 1.1.1.1 1997/02/20 19:17:22 vipulX Exp $ */
/* $Log: interprocs.h,v $
 * Revision 1.1.1.1  1997/02/20  19:17:22  vipulX
 * DMServer
 * */

#ifndef  _H_INTERPROCS
#define  _H_INTERPROCS

extern void lastInfo(void);
extern void getPolicyVwList(void); 
extern void intersInit(void);
extern void intersExit(void);
extern void getSearchPath(void);  
extern void getWorkingDir(void);
extern void getWorkingLib(void);
extern void reparse(void); 
extern void prvoidpath(void);
extern void version(void);
extern void report(void); 
extern void readOnlyCVType(void);
extern void readOnlyCV(void);
extern void setAttribute(void);
extern void getAttribute(void);
extern void createFile(void);
extern void purge(void);
extern void checkOut(void);
extern void checkIn(void);
extern void install(void);
extern void deleteAttribute(void);
extern void cancelCheckout(void);
extern void setSearchPath(void);
extern void setToolName(void);
extern void createCellView(void);
extern void edit(void);
extern void readEdit(void);
extern void save(void);
extern void load(void);
extern void update(void);
extern void infoCellView(void);
extern void printpath(void);
extern void setAttributeList(void);
extern void getAllAttributes(void);
extern void setMultipleViews(void);
extern void getMultipleViews(void);
extern void getToolName(void);
extern void printIdmsLog(void);
extern void getLibraryList(void);
extern void getCheckoutVersion(void);
extern void getCellList(void);
extern void getViewList(void);
extern void getViewCellViewList(void);
extern void getCellCellViewList(void);
extern void getVersionList(void);
extern void getExtensionTableList(void);
extern void delete(void); 
extern void getCdslibName(void);
extern void getLibPath(void);
extern void getCellViewsByLib(void);
extern void getCellViewsForCache(void);
extern void getAllCellViews(void);

#endif
