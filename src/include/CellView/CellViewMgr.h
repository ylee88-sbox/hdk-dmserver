//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                    (c) 1997 - 2000 Intel Corp.                      //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id$
//

#ifndef CELLVIEW_MGR_H
#define CELLVIEW_MGR_H

#include <Boo/BooIterT.h>
#include <Baa/BaaCellViewInfo.h> // to be removed after ww09
#include <Baa/BaaCellViewReader.h>
#include <Baa/BaaCellViewWriter.h>
#include <CellView/CellViewBasics.h>
#include <Els/ElsOptionList.h>
#include <map>
#include <set>
#include <list>

// -----------------------------------------------------------------
// --- Forward declarations
// -----------------------------------------------------------------
//
#ifndef  CELLVIEW_SWIG
namespace Nike {
  class INikeData;
}
#endif
class  CellViewRWMgr;
class  ElsOptionList;
class  CellViewSearchCustomizer;
class  CellViewCatalog;

//
// =================================================================
// CLASS: CellViewMgr
// =================================================================
//  This is a singleton class used to access Cellview data.
// 
//
// OVERVIEW
//      The CellViewMgr interface provides the bridge between the Data 
//  Management Engine and the application readers, and writers and the 
//  in-memory representation.  The CellViewMgr communicates with iDMS,
//  and the Cell Managers of the application to provide a transparent 
//  dispatching of read/write process for a given application.  The
//  CellViewMgr class uses the services provide by three existing blocks:  
//  iDMS, Cell Managers, and PIF.  
// =================================================================
//

typedef BaaCellViewInfo* CviPtr;

class __declspec(DECL_CELLVIEW) CellViewMgr : public BaaCellViewReader , public BaaCellViewWriter {
// The CellViewMgr class provides access to the Intel Database
// Management System (iDMS). It includes methods to get information
// from the database system and to perform operations to the database
// system.
//
// For the methods, you need a pointer to the CellViewMgr object.
// CellViewMgr is a singleton and cannot be created.
//
// To get the CellViewMgr object:
// set cvm [::boo::CellViewMgr_getCellViewMgr]	
// _________________________________________________________________

public:
#ifndef CELLVIEW_SWIG
  static const std::string CONVERT_CUTS_ARG_NAME;
  static const std::string HIERARCHY_ARG_NAME;
  static const std::string NON_CATEGORIZED_LABEL;
#endif

  // These functions are the new and preferred mechanism for doing I/O
  // operations. The client class uses getCellViewInfo first to get a
  // BaaCellViewInfo object, fills in the relevant parameters and then
  // calls the appropriate function.

  //. File Management Functions
  // [in] info              BaaCellViewInfo object containing infomation
  //                        about the file element to be operated on. The
  //                        info object must be obtained with a previous
  //                        call to getCellViewInfo.
  // [ret] .                True if success - false if error

  
  //. Open
  // [in] info              BaaCellViewInfo object containing infomation
  //                        about the file element to be operated on. The
  //                        info object must be obtained with a previous
  //                        call to getCellViewInfo.
  // [ret] .                True if success - false if error
  bool open          (BaaCellViewInfo& info);
  // Summary:
  //   Loads a cell into memory from disk for editing.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr open <info>
  //
  //   [in] <info> - pointer to BaaCellViewInfo of the cell, it cannot
  //                 be NULL info 
  //
  //   [ret] bool - true(1) if operation is successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     $cvm open $cvi
  //   }
  // _________________________________________________________________


  //. Save
  // [in] info              BaaCellViewInfo object containing infomation
  //                        about the file element to be operated on. The
  //                        info object must be obtained with a previous
  //                        call to getCellViewInfo.
  // [ret] .                True if success - false if error
  bool save          (BaaCellViewInfo& info, bool handleException=false);
  // Summary:
  //   Saves the changes made to a cell in memory to disk.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr save <info> [<exception>]
  //
  //   [in] <info> - pointer to BaaCellViewInfo of the cell, it cannot
  //                 be NULL info
  //   [in] <exception> - bool: true(1) to have a C++ expection handled
  //                            and only return false(0) if there is an
  //                            exception (recommended), false(0) to not
  //                            handle a C++ exception, which means you
  //                            will need to catch it, but can then
  //                            examine it (default)
  //                            
  //   [ret] bool - true(1) if operation is successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     $cvm save $cvi 1
  //   }
  // _________________________________________________________________



  //. Save As
  // [in] info              BaaCellViewInfo object containing infomation
  //                        about the file element to be operated on. The
  //                        info object must be obtained with a previous
  //                        call to getCellViewInfo.
  // [ret] .                True if success - false if error
  bool saveAs        (BaaCellViewInfo& info);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________


  //. Checkin
  // [in] info              BaaCellViewInfo object containing infomation
  //                        about the file element to be operated on. The
  //                        info object must be obtained with a previous
  //                        call to getCellViewInfo.
  // [ret] .                True if success - false if error
  bool checkIn       (BaaCellViewInfo& info);
  // Summary:
  //   Checks a cell into a library. Note: You need to manually
  //   refresh the DMBrowser if calling DM operations directly from
  //   CellViewMgr without the DmBrowser wrapper.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr checkIn <info>
  //
  //   [in] <info> - pointer to BaaCellViewInfo of the cell, it cannot
  //                 be NULL info 
  //
  //   [ret] bool - true(1) if operation is successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     $cvm checkIn $cvi
  //   }
  // _________________________________________________________________


  //. Checkout
  // [in] info              BaaCellViewInfo object containing infomation
  //                        about the file element to be operated on. The
  //                        info object must be obtained with a previous
  //                        call to getCellViewInfo.
  // [ret] .                True if success - false if error
  bool checkOut      (BaaCellViewInfo& info);
  // Summary:
  //   Checks a cell out from a library. Note: You need to manually
  //   refresh the DMBrowser if calling DM operations directly from
  //   CellViewMgr without the DmBrowser wrapper.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr checkOut <info>
  //
  //   [in] <info> - pointer to BaaCellViewInfo of the cell, it cannot
  //                 be NULL info 
  //
  //   [ret] bool - true(1) if operation is successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     $cvm checkOut $cvi
  //   }
  // _________________________________________________________________


  //. Cancel Checkout
  // [in] info              BaaCellViewInfo object containing infomation
  //                        about the file element to be operated on. The
  //                        info object must be obtained with a previous
  //                        call to getCellViewInfo.
  // [ret] .                True if success - false if error
  bool cancelCheckout(BaaCellViewInfo& info);
  //   Cancels the checkout of a cell from a library. This discards all
  //   the edits made to the cell since the last checkin. Note: You need
  //   to manually refresh the DMBrowser if calling DM operations directly
  //   from CellViewMgr without the DmBrowser wrapper.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr cancelCheckout <info>
  //
  //   [in] <info> - pointer to BaaCellViewInfo of the cell, it cannot
  //                 be NULL info 
  //
  //   [ret] bool - true(1) if operation is successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     $cvm cancelCheckout $cvi
  //   }
  // _________________________________________________________________


  //. Install
  // [in] info              BaaCellViewInfo object containing infomation
  //                        about the file element to be operated on. The
  //                        info object must be obtained with a previous
  //                        call to getCellViewInfo.
  // [ret] .                True if success - false if error
  bool install       (BaaCellViewInfo& info);
  //   Installs a cell from the work directory to a library. Note: You 
  //   need to manually refresh the DMBrowser if calling DM operations
  //   directly from CellViewMgr without the DmBrowser wrapper.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr install <info>
  //
  //   [in] <info> - pointer to BaaCellViewInfo of the cell, it cannot
  //                 be NULL info 
  //
  //   [ret] bool - true(1) if operation is successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     $cvm install $cvi
  //   }
  // _________________________________________________________________



  //. Delete Cell
  // [in] info              BaaCellViewInfo object containing infomation
  //                        about the file element to be operated on. The
  //                        info object must be obtained with a previous
  //                        call to getCellViewInfo.
  // [ret] .                True if success - false if error
  bool deleteCell    (BaaCellViewInfo& info);
  //   Deletes a cell from a library. If cell is not in a library,
  //   returns false(0). Note: You need to discard the
  //   cell from memory and manually refresh the DMBrowser if calling 
  //   DM operations directly from CellViewMgr without the DmBrowser
  //   wrapper.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr deleteCell <info>
  //
  //   [in] <info> - pointer to BaaCellViewInfo of the cell, it cannot
  //                 be NULL info 
  //
  //   [ret] bool - true(1) if operation is successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     $cvm deleteCell $cvi
  //   }
  // _________________________________________________________________


  //. Delete View/Version
  // [in] info              BaaCellViewInfo object containing infomation
  //                        about the file element to be operated on. The
  //                        info object must be obtained with a previous
  //                        call to getCellViewInfo.
  // [ret] .                True if success - false if error
  bool deleteCellView    (BaaCellViewInfo& info,bool whichDelete = false);
  //   Deletes a cell that is a file or a cellview version from a
  //   library. Note: You need to discard the cell from memory and
  //   manually refresh the DMBrowser if calling DM operations directly
  //   from CellViewMgr without the DmBrowser wrapper.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr deleteCellView <info> [<whichDelete>]
  //
  //   [in] <info> - pointer to BaaCellViewInfo of the cell, it cannot
  //                 be NULL info 
  //   [in] <whichDelete> - bool: false(0) to delete a file (default),
  //                         true(1) to delete a specific version of the
  //                         cell in a library
  //
  //   [ret] bool - true(1) if operation is successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     $cvm deleteCellView $cvi
  //   }
  // _________________________________________________________________



  //. Prints the information about the element to the standard Info window.
  // [in] info              BaaCellViewInfo object containing infomation
  //                        about the file element to be operated on. The
  //                        info object must be obtained with a previous
  //                        call to getCellViewInfo.
  // [ret] .                True if success - false if error
  bool printCellViewInfo (BaaCellViewInfo&info);
  // Summary:
  //   Prints out to the Message window information about the cellview.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr printCellViewInfo <info>
  // 
  //   [in] <info> - pointer to BaaCellViewInfo of the cell, it cannot
  //                 be NULL info 
  //
  //   [ret] bool - false(0) if successful in accessing file status,
  //                true(1) if not
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     $cvm printCellViewInfo $cvi
  //   }
  // _________________________________________________________________  




#ifndef CELLVIEW_SWIG

  //. Tries to create a new item according to the following rules:
  //   Workdir if dms.pth is used  or CWD otherwise.
  // [in] cellName 
  // [in] viewName
  // [out] info                BaaCellViewInfo object containing infomation
  //                           about newly created file element.
  // [ret] .                   True if success - false if error

  bool createCellView(const std::string& cellName,
                      const std::string& viewName,
                      CviPtr& info,
                      const std::string& outDir="");

  //. Retrieves information about a database item
  // [in] cellName 
  // [in] viewName
  // [out] info                BaaCellViewInfo object containing infomation
  //                           about read element
  // [in,default] versionName  Version name
  // [in,default] libName      Library name
  // [in,default] libsOnly     Search only in libraries. 
  // [ret] .                   True if success - false if error
  bool getCellViewInfo(const std::string& cellName,
                       const std::string& viewName, CviPtr& cellInfo, 
                       const std::string& versionName = CellViewVersionDefault,
                       const std::string& libName = CellViewLibraryDefault,
                       bool libsOnly=false);

  //. Retrieves information about a file item
  // [in] cellName 
  // [in] viewName
  // [out] info                BaaCellViewInfo object containing infomation
  //                           about read element
  // [ret] true if success - false if error
  
  bool getCellViewInfo(const std::string& cellName,
                       const std::string& viewName, 
                       const std::string& fullPath,
                       CviPtr& cellInfo);

#endif

  // This functions are identical as the createCellView, getCellViewInfo and getCellViewInfo
  // but are used for swig use which does not support output ptr. args or default arguments.
  // The second form of getCellViewInfo is call createCellViewInfo due to lack of polymorphic
  // functions in tcl.
  
  //. Create a cell view
  BaaCellViewInfo* createCellView(const std::string& cellName,
                                  const std::string& viewName,
                                  const std::string& outDir,
                                  bool handleException=false);
  // Summary:
  //   Creates a file on disk for a cellview.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::BaaCellViewInfo ::boo::CellViewMgr createCellView <name>
  //                                        <view> <outDir> [<exception>]
  //
  //   [in] <name> - name of cell
  //   [in] <view> - name of view
  //   [in] <outDir> - fullpath to the output directory
  //   [in] <exception> - bool: true(1) to have a C++ expection handled
  //                            and only return false(0) if there is an
  //                            exception (recommended), false(0) to not
  //                            handle a C++ exception, which means you
  //                            will need to catch it, but can then
  //                            examine it (default)
  //
  //   [ret] pointer to BaaCellViewInfo object or NULL if cannot
  //         create cellview
  // _________________________________________________________________
  // Example:
  //   set cvi [$cvm createCellView CellA lnf /local/file/ 1]
  // _________________________________________________________________



  //. Get cell view info for a cell
  BaaCellViewInfo * getCellViewInfo(std::string cellName,
                                    std::string viewName,
                                    std::string versionName =
                                    CellViewVersionDefault,
                                    std::string libName =
                                    CellViewLibraryDefault,
                                    bool libsOnly=false);
  // Summary:
  //   Returns the BaaCellViewInfo object associated with the specified
  //   cell. The cell is searched for using the dmspath.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::BaaCellViewInfo ::boo::CellViewMgr getCellViewInfo <name>
  //                        <view> [<version>] [<library>] [<libsOnly>]
  //
  //   [in] <name> - name of cell
  //   [in] <view> - name of view
  //   [in] <version> - version of the cell if cell is from a library,
  //                    default is latest
  //   [in] <library> - name of the library that the cell is in,
  //                    default is worklib
  //   [in] <libsOnly> - bool: true(1) to only look for cell in
  //                           libraries, false(0) to also look in
  //                           file system (RW paths) (default)
  //
  //   [ret] pointer to BaaCellViewInfo object or NULL if cannot find
  //         the info
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     puts "found cell $cellname.$viewname"
  //   }
  // _________________________________________________________________

  //. Create cell view info
  BaaCellViewInfo * createCellViewInfo(std::string cellName,
                                       std::string viewName,
                                       std::string fullPath);
  // Summary:
  //   Creates a BaaCellViewInfo object.
  // _________________________________________________________________
  // Syntax:
  //   BaaCellViewInfo ::boo::CellViewMgr createCellViewInfo <cellName>
  //                                      <viewName> <fullPath>
  //
  //   [in] <cellName> - name of new cell
  //   [in] <viewName> - name of new view
  //   [in] <fullPath> - fullpath to the cell's file. It should be
  //                     "directory/cellName.viewName"
  //
  //   [ret] pointer to BaaCellViewInfo object or NULL if cannot
  //         create cellview
  // _________________________________________________________________
  // Example:
  //   set cvi [$cvm createCellViewInfo "hello" "lnf"
  //            "/home/workdir/hello.lnf"]
  // _________________________________________________________________


#ifndef CELLVIEW_SWIG
  
  //. Adds a new Reader to the CellView Manager
  // [in] name      Name of the view for which the reader is being registered
  // [in] reader    An object derived from BaaCellViewReader
  // [in] replace   If replace is true an existing reader for the view will
  //                be replaced
  // [ret] .        True if success - false if error

  bool addCellViewReader(const std::string& name,
                         BaaCellViewReader& reader,
                         bool replace);
  
  //. Adds a new Reader to the CellView Manager
  // [in] name      Name of the view for which the writer is being registered
  // [in] reader    An object derived from BaaCellViewWriter
  // [in] replace   If replace is true an existing writer for the view will
  //                be replaced
  // [ret] .        True if success - false if error
  bool addCellViewWriter(const std::string& name,
                         BaaCellViewWriter& writer,
                         bool replace);

  //. Removes an existing  Reader from the CellView Manager
  // [in] name : Name of the view for which the Reader is being removed
  // [out] removed -true if reader was removed.
  // [ret] the previous reader registered if any
  BaaCellViewReader* removeCellViewReader(const std::string& name,
                                          bool &removed);

  //. Removes an existing  Writer from the CellView Manager
  // [in] name : Name of the view for which the writer is being removed
  // [out] removed -true if reader was removed.
  // [ret] the previous reader registered if any
  BaaCellViewWriter* removeCellViewWriter(const std::string& name,
                                          bool &removed);

  //. Registers a memory object (INikeData) with the BaaCellViewInfo object
  //  from where they come from or should go. This is normally used by readers/writers
  //  to find/create the object in memory after reading/saving an object.
  // [in] data : The object in memory that we want to associate with a file object.
  // [in] info : The BaaCellViewInfo that should be associated with the object in memory
  // [ret] .        True if success - false if error
  bool addCellViewMap(Nike::INikeData*data,BaaCellViewInfo*info);

  
  //. Removes the registration of a memory object (INikeData) with the BaaCellViewInfo object
  // [in] data : The object in memory that we want to associate with a file object.
  // [in,default] info : The BaaCellViewInfo that should be associated with the object in memory
  // [ret] .        True if success - false if error
  bool removeCellViewMap(Nike::INikeData*data);

  //. Retrieves the in memory object associated with this file object.
  // [in] data : The object in memory that we want to associate with a file object.
  // [in,default] info : The BaaCellViewInfo that should be associated with the object in memory
  // [ret] .        True if success - false if error

  Nike::INikeData* getCellViewMap(BaaCellViewInfo*info);

  //. Retrieves the file object associated with the given memory object.
  // [in] data : The object in memory that we want to associate with a file object.
  // [in,default] info : The BaaCellViewInfo that should be associated with the object in memory
  // [ret] .        True if success - false if error
  BaaCellViewInfo* getCellViewMap(Nike::INikeData*data);
#endif


  //. Returns whether the application is running with iDMS enabled.
  // [ret] .        True if using a valid dms.pth

  bool isiDMSRunning();
  // Summary:
  //   Checks to see if iDMS (Intel Database Management System) is running.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr isiDMSRunning
  //
  //   [ret] bool - true(1) if iDMS is running, false(0) if not
  // _________________________________________________________________
  // Example:
  //   set isRunning [$cvm isiDMSRunning]
  // _________________________________________________________________



  // The following functions controls how the CellViewMgr class manages views
  // that have different names on disk as they do in memory.

  //. Set the dms (file) viewName for a given memory view name.
  // [in] memoryViewName : The name of the view in memory
  // [in] dmsViewName    : The name of the view in on disk.
  // [ret] .             : The last registered dms view name for the given view.

  std::string setDmsTargetViewName(const std::string& memoryViewName,
                                   const std::string& dmsViewName);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________

  //. Get the dms viewName for a given memory memory view name.
  // [in] memoryViewName : The name of the view in memory
  // [ret] .             : The last registered dms view name for the given view.

  std::string getDmsTargetViewName(const std::string& memoryViewName);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________

  //. Get the memory viewName for a given memory dms view name.
  // [in] dmsViewName    : The name of the view in on disk.
  // [ret] .             : The last registered memory view name for the given view.

  std::string getMemoryTargetViewName(const std::string& dmsViewName);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________

  //. Set the memory viewName for a given memory view name.
  // [in] memoryViewName : The name of the view in memory
  // [in] dmsViewName    : The name of the view in on disk.
  // [ret] .             : The last registered memory view name for the given view.

  std::string setMemoryTargetViewName(const std::string& dmsViewName,
                                      const std::string& memoryViewName);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________
  
  void setSearchMode(bool useIdms, bool idmsFirst);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________


  void getSearchMode(bool& useIdms, bool& idmsFirst);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________

  bool getUseIdmsSearchMode();
  // Summary:
  //   Should not be used.
  // _________________________________________________________________  
  
  
  bool getIdmsFirstSearchMode();
  // Summary:
  //   For Swig Usage. Avoids having to create boolean objects in tcl.
  // _________________________________________________________________


  bool reloadDmsPath(const std::string& dmsPath, bool restart=false);
  // Summary:
  //   Reloads the dmspath file. This must be the same dmspath file
  //   that was initially loaded. Also can restart the DMS server.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr reloadDmsPath <dmspath> [<restart>]
  //
  //   [in] <dmspath> - fullpath to the original dmspath file
  //   [in] <restart> - true(1) to restart the DMS server, false(0)
  //                    to not restart the DMS server (default)
  //
  //   [ret] bool - true(1) if dmspath file is successfully parsed,
  //                false(0) if not
  // _________________________________________________________________
  // Example:
  //   set success [$cvm reloadDmsPath /home/fw/whung1/dms.pth]
  // _________________________________________________________________



  std::string printDmsPath(const std::string&viewName);
  // Summary:
  //   Returns the dmspath for the specified view.
  // _________________________________________________________________
  // Syntax:
  //   string ::boo::CellViewMgr printDmsPath <view>
  //
  //   [in] <view> - name of the view
  //
  //   [ret] string containing dmspath
  // _________________________________________________________________
  // Example:
  //   set dmspth [$cvm printDmsPath lnf]
  // _________________________________________________________________



#ifndef  CELLVIEW_SWIG
  bool printDmsPath(const std::string&viewName,std::string& printout);
#endif
  
  void getLibraries(BooIterT<std::string>& libs);
  // Summary:
  //   Returns the names of libraries visible in the dmspath.
  // _________________________________________________________________
  // Syntax:
  //   string ::boo::CellViewMgr getLibraries
  //
  //   [ret] space-separated list of libraries in dmspath
  // _________________________________________________________________
  // Example:
  //   set libraries [$cvm getLibraries]
  // _________________________________________________________________



  void getCellViews(const std::string& library,
                    const std::string& viewName,
                    BooIterT<std::string>& cellViews,
                    bool returnCOuser=0);
  // Summary:
  //   Returns a list of cells with the specified view in the given
  //   library. Optionally, includes the name of any checkout users.
  // _________________________________________________________________
  // Syntax:
  //   list ::boo::CellViewMgr getCellViews <library> <view>
  //                                          <returnCOuser>
  //
  //   [in] <library> - name of the library
  //   [in] <view> - name of the view
  //   [in] <returnCOuser> - bool: true(1) if you want the results to
  //                         include the username of a user that has the
  //                         cell checked out, false(0) to only return
  //                         cell names (default)
  //
  //   [ret] space-separated list of cell names. If <returnCOuser> is
  //         true, the list will also include the username of a user
  //         that has the cell checked out or the word "None" if the
  //         cell is not checked out. In this case, format is:
  //         cell1:user1 cell2:user2 cell3:None
  // _________________________________________________________________
  // Example:
  //   set cells [$cvm getCellViews wanda_lib lnf 1]
  // _________________________________________________________________
  

  void getUserCOViews(const std::string& library,
                    const std::string& viewName,
                    const std::string& couser, 
                    BooIterT<std::string>& cellViews);
  // Summary:
  //   Returns a list of cells with the specified view in the given
  //   library that are checked out by the given user.
  // _________________________________________________________________
  // Syntax:
  //   list ::boo::CellViewMgr getCellUserCOViews <library> <view>
  //                                              <coUser>
  //
  //   [in] <library> - name of the library, or leave blank for all
  //                    libraries
  //   [in] <view> - name of the view
  //   [in]  <coUser> - name of the checkout user, or leave blank for
  //                    the current user
  //
  //   [ret] space-separated list of cell names
  // _________________________________________________________________
  // Example:
  //   set cells [$cvm getCellUserCOViews wanda_lib lnf]
  // _________________________________________________________________
  


  void getCellVersions(const std::string& libName,
                       const std::string& cellName,
                       const std::string& cellView,
                       BooIterT<std::string>& verIt);
  // Summary:
  //   Returns a list of version numbers of the specified cellview in
  //   the specified library.
  // _________________________________________________________________
  // Syntax:
  //   list ::boo::CellViewMgr getCellVersions <library> <name> <view>
  //
  //   [in] <library> - name of the library
  //   [in] <name> - name of the cell
  //   [in] <view> - name of the view
  //
  //   [ret] space-separated list of version numbers
  // _________________________________________________________________
  // Example:
  //   set versions [$cvm getCellVersions wanda_lib nand lnf]
  // _________________________________________________________________


  void getViews(const std::string& library,
                BooIterT<std::string>& viewIt);
  // Summary:
  //   Returns a list of views registered with the DMS Manager for the
  //   given library.
  // _________________________________________________________________
  // Syntax:
  //   list ::boo::CellViewMgr getViews <library>
  //
  //   [in] <library> - name of library
  //
  //   [ret] space-separated list of views
  // _________________________________________________________________
  // Example:
  //   set views [$cvm getViews wanda_lib]
  // _________________________________________________________________


  bool setToolName(const std::string& toolName);
  // Summary:
  //   Sets the tool name used by the DM server when parsing the dmspath.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr setToolName <toolName>
  //
  //   [in] <toolName> - name of tool
  //
  //   [ret] bool - true(1) if operation is successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   set success [$cvm setToolName galaxy]
  // _________________________________________________________________


  std::string getToolName();
  // Summary:
  //   Returns the tool name used by the DM server when parsing the
  //   dmspath.
  // _________________________________________________________________
  // Syntax:
  //   string ::boo::CellViewMgr getToolName
  //
  //   [ret] name of the tool
  // _________________________________________________________________
  // Example:
  //   set tool [$cvm getToolName]
  // _________________________________________________________________


  bool setWorkDir(const std::string& dirPath);
  // Summary:
  //   Sets the work directory for all registered views. This command
  //   only works if iDMS is NOT running.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr setWorkDir <dirPath>
  //
  //   [in] <dirPath> - fullpath of the work directory
  //
  //   [ret] bool - true(1) if operation is successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   set success [$cvm setWorkDir /home/fw/whung1/workdir/]
  // _________________________________________________________________


#ifndef CELLVIEW_SWIG  
  std::string getWorkDir(const std::string& viewName = "");
#else
  std::string getWorkDir(const std::string& viewName);
  // Summary:
  //   Returns the work directory from the dmspath for the specified view.
  // _________________________________________________________________
  // Syntax:
  //   string ::boo::CellViewMgr getWorkDir <viewName>
  //
  //   [in] <viewName> - name of view
  //
  //   [ret] fullpath of work directory
  // _________________________________________________________________
  // Example:
  //   set workdir [$cvm getWorkDir lnf]
  // _________________________________________________________________

#endif

#ifndef CELLVIEW_SWIG  
  std::string getWorkLib(const std::string& viewName = "");
#else
  std::string getWorkLib(const std::string& viewName);
  // Summary:
  //   Returns the name of the work library from the dmspath for the
  //   specified view.
  // _________________________________________________________________
  // Syntax:
  //   string ::boo::CellViewMgr getWorkLib <view>
  //
  //   [in] <view> - name of the view
  //
  //   [ret] work library name
  // _________________________________________________________________
  // Example:
  //   set worklib [$cvm getWorkLib lnf]
  // _________________________________________________________________


#endif

  bool getRWPaths(const std::string& viewName,
                  BooIterT<std::string> & rwIter);
  // Summary:
  //   Returns a list of RW Paths from the dmspath for the specified view.
  // _________________________________________________________________
  // Syntax:
  //   list ::boo::CellViewMgr getRWPaths <view>
  //
  //   [in] <view> - name of the view
  //
  //   [ret] space-separated list of RW paths
  // _________________________________________________________________
  // Example:
  //   set rwpaths [$cvm getRWPaths lnf]
  // _________________________________________________________________

  bool getLibPaths(const std::string& viewName,
                   BooIterT<std::string> & lpIter);
  // Summary:
  //   Returns the library paths from the dmspath file for the specified
  //   view.
  // _________________________________________________________________
  // Syntax:
  //   list ::boo::CellViewMgr getLibPaths <view>
  //
  //   [in] <view> - name of the view
  //
  //   [ret] space-separated list of library paths
  // _________________________________________________________________
  // Example:
  //   set libpaths [$cvm getLibPaths lnf]
  // _________________________________________________________________


  bool getDmsPathViews(BooIterT<std::string> & rwIter);
  // Summary:
  //   Returns a list of all views explicitly used in the dmspath.
  // _________________________________________________________________
  // Syntax:
  //   list ::boo::CellViewMgr getDmsPathViews 
  //
  //   [ret] space-separated list of view names that are listed in
  //         the dms.pth file. 
  // _________________________________________________________________
  // Example:
  //   set views [$cvm getDmsPathsViews]
  // _________________________________________________________________




  bool getRWPathCellViews(const std::string& viewName,
                          const std::string& rwPath,
                          BooIterT<std::string> & rwIter);
  // Summary:
  //   Returns a list of cells of the specified view in the given RW Path.
  // _________________________________________________________________
  // Syntax:
  //   list ::boo::CellViewMgr getRWPathCellViews <view> <rwpath>
  //
  //   [in] <view> - name of the view
  //   [in] <rwpath> - fullpath to a directory
  //
  //   [ret] space-separated list of cell names
  // _________________________________________________________________
  // Example:
  //   set cells [$cvm getRWPathCellViews lnf /home/fw/whung1/mylnfs/]
  // _________________________________________________________________


  
  
  std::string getLastError();
  // Summary:
  //   Returns the last error reported from the iDMS (Intel Database
  //   Management System).
  // _________________________________________________________________
  // Syntax:
  //   string ::boo::CellViewMgr getLastError
  //
  //   [ret] last error message from iDMS
  // _________________________________________________________________
  // Example:
  //   set error [$cvm getLastError]
  // _________________________________________________________________


  static CellViewMgr& getCellViewMgr();
  // Summary:
  //   Returns the CellViewMgr object.
  // _________________________________________________________________
  // Example:
  //   set cvm [::boo::CellViewMgr_getCellViewMgr]	
  // _________________________________________________________________

  
  void setApplicationOpenList(bool enable=true);
  // Summary:
  //   Sets the flag to use the Application Open List during cell search.
  //   The Application Open List is a list of views that 
  //   CellViewMgr uses when it cannot locate a cell with the given view.
  //   If CellViewMgr cannot locate a cell with the given view, it will
  //   then look in the Application Open List for the given view. If it
  //   finds the view there, it will take the next view in the list
  //   and search for the cell using that view. If it still cannot find
  //   the cell, it will search using the next view in the list, and
  //   so on until the end of the list. This mechanism provides
  //   prioritized alternative views to search under if the cell cannot
  //   be found under the given view.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewMgr setApplicationOpenList [<enable>]
  //
  //   [in] <enable> - bool: true(1) to enable use of Application
  //                         Open List, false(0) to disable it
  // _________________________________________________________________
  // Example:
  //   $cvm setApplicationOpenList 1
  // _________________________________________________________________


  bool getApplicationOpenList ();
  // Summary:
  //   Checks to see if CellViewMgr is using the Application Open List
  //   for cell search. The Application Open List is a list of views that 
  //   CellViewMgr uses when it cannot locate a cell with the given view.
  //   If CellViewMgr cannot locate a cell with the given view, it will
  //   then look in the Application Open List for the given view. If it
  //   finds the view there, it will take the next view in the list
  //   and search for the cell using that view. If it still cannot find
  //   the cell, it will search using the next view in the list, and
  //   so on until the end of the list. This mechanism provides
  //   prioritized alternative views to search under if the cell cannot
  //   be found under the given view.
  // _________________________________________________________________
  //   Syntax:
  //     bool ::boo::CellViewMgr getApplicationOpenList
  //
  //     [ret] true(1) if flag is set to use Application Open List
  //           during search, false(0) if not
  // _________________________________________________________________
  //   Example:
  //     set flag [$cvm getApplicationOpenList]
  // _________________________________________________________________


  
  bool addApplicationOpenList(const std::string & dmsViewName,bool front);
  // Summary:
  //   Adds a view to the Application Open List for cell search.
  //   You have the option to place the view in the front or the back
  //   of the list. Order does matter in this list.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr addApplicationOpenList <view> <front>
  //
  //   [in] <view> - name of the view to add to the list
  //   [in] <front> - bool: true(1) to put the view in the front of the
  //                  list, false(0) to put the view in the back of the
  //                  list
  //
  //   [ret] bool - true(1) if operation is successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   set success [$cvm addApplicationOpenList lnf 0]
  // _________________________________________________________________


  bool clearApplicationOpenList();
  // Summary:
  //   Clears the Application Open List so that it is empty.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr clearApplicationOpenList
  //
  //   [ret] bool - true(1) if operation is successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   set success [$cvm clearApplicationOpenList]
  // _________________________________________________________________


  std::string getApplicationOpenListNextView(const std::string & dmsViewName);
  // Summary:
  //   Returns the next view after the given view in the Application
  //   Open List. 
  // _________________________________________________________________
  // Syntax:
  //   string ::boo::CellViewMgr getApplicationOpenListNextView <view>
  //
  //   [in] <view> - name of the view
  //
  //   [ret] name of the next view after <view> in the Application Open
  //         List, or empty string if <view> is not in the list or is
  //         the last item in the list
  // _________________________________________________________________
  // Example:
  //   set view [$cvm getApplicationOpenListNextView lnf]
  // _________________________________________________________________



  int setSaveBackups(int saveBackup);
  // Summary:
  //   Sets the number of backup files to keep when the save operation
  //   is called.
  // _________________________________________________________________
  // Syntax:
  //   int ::boo::CellViewMgr setSaveBackups <saveBackup>
  //
  //   [in] <saveBackup> - number of backups to keep
  //
  //   [ret] previous number of backup files to keep
  // _________________________________________________________________
  // Example:
  //   set prev [$cvm setSaveBackups 3]
  // _________________________________________________________________


  int getSaveBackups();
  // Summary:
  //   Returns the number of backup files to be kept when the save
  //   operation is called.
  // _________________________________________________________________
  // Syntax:
  //   int ::boo::CellViewMgr getSaveBackups
  //
  //   [ret] number of backup files
  // _________________________________________________________________
  // Example:
  //   set numbackups [$cvm getSaveBackups]
  // _________________________________________________________________


  void setSkipBackup(const std::string& viewName, bool add);
  // Summary:
  //   Disables backup creation for the given view. This is high
  //   risk since it disallows recovering data from backup files if
  //   a write operation fails. This should be used only when 
  //   performance is highest priority.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewMgr setSkipBackup <view> <add>
  //
  //   [in] <view> - name of the view, such as lnf or flash
  //   [in] <add> - bool: true(1) to enable skipping backup (backups
  //                      will not be made), false(0) to create backups
  //                      (this is the default behavior)
  // _________________________________________________________________
  // Example:
  //   $cvm setSkipBackup flash true
  // _________________________________________________________________  
  
  
  bool getSkipBackup(const std::string& viewName);
  // Summary:
  //   Returns the value of the skip backup flag for the given view.
  //   This is set with setSkipBackup.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr getSkipBackup <view>
  //
  //   [in] <viewn> - name of the view, such as lnf or flash
  //
  //   [ret] <add> - bool: true(1) if skipping backup is enabled,
  //                       false(0) if it is not enabled (backups are made)
  // _________________________________________________________________
  // Example:
  //   set skipbk [$cvm getSkipBackup flash]
  // _________________________________________________________________    


  std::string getCellNameRegex();
  // Summary:
  //   Should not be used.
  // _________________________________________________________________

  int setCellNameRegex(const std::string& reg);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________

  int isValidCellName(const std::string&cn);
  // Summary:
  //   Checks if a string contains space, tab, or carriage return, which
  //   would make it invalid as a cell name.
  // _________________________________________________________________
  // Syntax:
  //   int ::boo::CellViewMgr isValidCellName <name>
  //
  //   [in] <name> - cell name
  //
  //   [ret] bool - true(1) if name does not contain any space, tab, or
  //                carriage return and is a valid name, false(0) if
  //                name does contain space, tab, or return so is invalid
  // _________________________________________________________________
  // Example:
  //   set isvalid [$cvm isValidCellName $cname]
  // _________________________________________________________________


  std::string getCanonicalPath(const std::string&path);
  // Summary:
  //   Returns the actual physical path of a file to make sure there is
  //   not a link in the path.
  // _________________________________________________________________
  // Syntax:
  //   string ::boo::CellViewMgr getCanonicalPath <fullPath>
  //
  //   [in] <fullPath> - fullpath to a file on disk
  //
  //   [ret] canonical path of the fullpath
  // _________________________________________________________________
  // Example:
  //   set path [$cvm getCanonicalPath "/home/fw/whung1/opuslib44link/cell.lnf"]
  // _________________________________________________________________


  std::string getCanonicalDir(const std::string&dir);
  // Summary:
  //   Returns the actual physical path of a directory to make sure
  //   there is not a link in the path.
  // _________________________________________________________________
  // Syntax:
  //   string ::boo::CellViewMgr getCanonicalDir <fullPath>
  //
  //   [in] <fullPath> - fullpath to a directory on disk
  //
  //   [ret] canonical path of the directory
  // _________________________________________________________________
  // Example:
  //   set path [$cvm getCanonicalDir "/home/fw/whung1/opuslib44link/"]
  // _________________________________________________________________


  void setSpeedMode(int useCachedInfo);
  // Summary:
  //   Sets the mode of to getCellViewInfoto to Speed or Normal.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewMgr setSpeedMode <useCachedInfo>
  //
  //   [in] <useCachedInfo> - integer representing current mode of
  //                          operation:
  //                           0 - Normal mode
  //                           1 - Speed mode (using cached info)    
  // _________________________________________________________________
  // Example:
  //   $cvm setSpeedMode 1
  // _________________________________________________________________


  int getSpeedMode();
  // Summary:
  //   Returns the current mode of getCellViewInfo.
  // _________________________________________________________________
  // Syntax:
  //   integer ::boo::CellViewMgr getSpeedMode
  //
  //   [ret] integer representing current mode of operation:
  //         0 - Normal mode
  //         1 - Speed mode (using cached info)  
  // _________________________________________________________________
  // Example:
  //   set curMode [$cvm getSpeedMode]
  // _________________________________________________________________

  void setPrintCache(int print);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________
    
  int  getPrintCache();
  // Summary:
  //   Should not be used.
  // _________________________________________________________________  

  bool setViewTypeSearchList(const std::string& views);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________  
  
  bool getViewTypeSearchList(BooIterT<std::string>& cvIt);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________  
 
  void setUseViewTypeSearchList(bool enable);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________  
  
  bool getUseViewTypeSearchList();
  // Summary:
  //   Should not be used.
  // _________________________________________________________________  

  void clearCachedMap();
  // Summary:
  //   Should not be used.
  // _________________________________________________________________
  
    
  void clearCachedMapEntry(const std::string& cellName, const std::string& viewName);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________
  
    
  void printCachedMap();
  // Summary:
  //   Should not be used.
  // _________________________________________________________________  
  
  void cachingInfo(const std::string& cvlist, const std::string& library="");
  // Summary:
  //   Should not be used.
  // _________________________________________________________________  

  void getCellViewsByLib(const std::string& library, const std::string& viewName);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________  
  
  void getCellViewsForCache(const std::string& cellName, const std::string& viewName, const std::string& version);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________  
  
  bool addCachedMapEntry(BaaCellViewInfo& info);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________  
  
  bool addUnfoundCachedMapEntry(const std::string& cellName, const std::string& viewName);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________  
  

  void getAllCellViews(const std::string& viewName,BooIterT<std::string>& cvIt);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________  
  
//  void getAllCellViews(const std::string& viewName,BooIterT<BaaCellViewInfo*>& cvIt);

#ifndef CELLVIEW_SWIG  
  bool findCachedMap(const std::string& cellName, const std::string& viewName,CviPtr& pCellInfo);
  static bool init(const std::string& toolName = "",
                   const std::string& appName = "");
  CellViewRWMgr& getRWMgr();
  void lockCellView();
  void unlockCellView();
  bool closeDMSConnection();

  const std::string& getName() const;
  ~CellViewMgr();

  
  bool readCellView(BaaCellViewInfo&info);
  bool writeCellView(BaaCellViewInfo&info);
  
  bool addCellViewCallback(void(*)(const std::string& op,
                                   BaaCellViewInfo&source));
  bool removeCellViewCallback(void(*)(const std::string& op,
                                      BaaCellViewInfo&source));
  bool executeCellViewCallbacks(const std::string& op,
                                BaaCellViewInfo&source);
#endif
  //. Set a "customizer" for the cell search. 
  virtual bool setSearchCustomizer(CellViewSearchCustomizer* sc);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________


  //. Get the "customizer" for the cell search. 
  virtual CellViewSearchCustomizer* getSearchCustomizer();
  // Summary:
  //   Should not be used.
  // _________________________________________________________________


#ifndef CELLVIEW_SWIG
  bool setOptionList(const std::string& option);
  void setOptionList(ElsOptionList&opt);
  void clearOptionList();
  ElsOptionList& getOptionList();
#endif
  //. set the partial loaded flag - loaded= -1
  void setPartialLoaded(BaaCellViewInfo&info);
  // Summary:
  //   Sets the loaded flag to be partially loaded for the cell (only
  //   the interface is loaded).
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr setPartialLoaded <info>
  //
  //   [in] <info> - pointer to BaaCellViewInfo of the cell, it cannot
  //                 be NULL info
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     $cvm setPartialLoaded $cvi
  //   }
  // _________________________________________________________________


  //. set the loaded flag to 1.
  void setFullyLoaded(BaaCellViewInfo&info);
  // Summary:
  //   Sets the loaded flag to be fully loaded for the cell (content of
  //   cell is loaded).
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr setFullyLoaded <info>
  //
  //   [in] <info> - pointer to BaaCellViewInfo of the cell, it cannot
  //                 be NULL info
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     $cvm setFullyLoaded $cvi
  //   }
  // _________________________________________________________________


  //. get the partial loaded flag
  bool isPartialLoaded(BaaCellViewInfo&info);
  // Summary:
  //   Checks if the cell is partially loaded in memory (only the
  //   interface is loaded).
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr isPartialLoaded <info>
  //
  //   [in] <info> - pointer to BaaCellViewInfo of the cell, it cannot
  //                 be NULL info
  //
  //   [ret] bool - true(1) if cell is partial loaded, false(0) if not
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     $cvm isPartialLoaded $cvi
  //   }
  // _________________________________________________________________


  //. check to see if the cell is read-only.
  bool isReadOnlyCell(BaaCellViewInfo&info);
  // Summary:
  //   Checks if the cell is read only.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewMgr isReadOnlyCell <info>
  //
  //   [in] <info> - pointer to BaaCellViewInfo of the cell, it cannot
  //                 be NULL info
  //
  //   [ret] bool - true(1) if cell is read-only, false(0) if not
  // _________________________________________________________________
  // Example:
  //   # get the BaaCellViewInfo of cell with $cellname and $viewname.
  //   if [catch { set cvi [::boo::BaaCellViewInfo -this
  //      [$cvm getCellViewInfo $cellname $viewname]] } ] {
  //     set cvi [$cvm getCellViewInfo $cellname $viewname]
  //   }
  //   if { $cvi == "NULL" } { 
  //     puts "cannot find cell $cellname.$viewname"
  //   } else {
  //     $cvm isReadOnlyCell $cvi
  //   }
  // _________________________________________________________________

  std::string getLibPath(const std::string& libName);
  // Summary:
  //   Returns the library path from the dmspath file for the given
  //   library.
  // _________________________________________________________________
  // Syntax:
  //   string ::boo::CellViewMgr getLibPath <libname>
  //
  //   [in] <libname> - name of the library
  //
  //   [ret] full library path
  // _________________________________________________________________
  // Example:
  //   set libpath [$cvm getLibPath wanda_lib]
  // _________________________________________________________________  
  
  

  std::string getCategoryCellViews(const std::string& library,
                    const std::string& viewName, bool returnCOuser=0);
  // Summary:
  //   Returns a list of cell names with the given view in the given
  //   library organized by category. Can also include the user name
  //   of a user that has the cell checked out.
  // _________________________________________________________________
  // Syntax:
  //   string ::boo::CellViewMgr getCategoryCellViews <libname> <view>
  //                                                  <returnCOuser>
  //
  //   [in] <libname> - name of the library
  //   [in] <view> - name of the view                                
  //   [in] <returnCOuser> - bool: true(1) if you want the results to
  //                         include the username of a user that has the
  //                         cell checked out, false(0) to only return
  //                         cell names (default)
  //
  //   [ret] space-separated list of category names and cell names within
  //         each category. If <returnCOuser> is true, the list will also
  //         include the username of a user that has the cell checked out
  //         or the word "None" if the cell is not checked out. Format is:
  //         #category1 cell1:user1 cell2:user2 #category2 cell3:None
  // _________________________________________________________________
  // Example:
  //   set catcells [$cvm getCategoryCellViews wanda_lib lnf 1]
  // _________________________________________________________________


  //. Error/Success/Warning reporting from readers/writers
  enum Result { OK, WARN, ERR, FATAL };

#ifndef CELLVIEW_SWIG  
  //. setResult
  void setResult(Result result=OK, std::string resultStr="");
  // Summary:
  //   Should not be used.
  // _________________________________________________________________
#else
  void setResult(Result result, std::string resultStr);
#endif


  //. getResult
  Result getResult();
  // Summary:
  //   Should not be used.
  // _________________________________________________________________


  //. getResultStr
  const std::string& getResultStr();
  // Summary:
  //   Should not be used.
  // _________________________________________________________________
  

  //. set/get has new multiple associated cells
  void setHasNewMultiAssocCell(bool flag);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________
   
  
  bool getHasNewMultiAssocCell();
  // Summary:
  //   Should not be used.
  // _________________________________________________________________
  
  void setNewMultiAssocCell(BaaCellViewInfo& cvi);
  // Summary:
  //   Should not be used.
  // _________________________________________________________________

#ifndef CELLVIEW_SWIG  
  //. get next new multiple associated cell
  bool getNextNewMultiAssocCell(BaaCellViewInfo*& info, Nike::INikeData*& cell, bool reset);

  //. add transaction path
  bool addTransactionPath(BaaCellViewInfo& info, const std::string& path);

protected:
  CellViewMgr(CellViewRWMgr& rWMgr);
  std::string getInfoKey_(const BaaCellViewInfo&cellInfo);
  bool setNewPath_(BaaCellViewInfo&cellInfo, const std::string& fullPath);
  std::string getRealPath(BaaCellViewInfo&cvi);
  bool getCellViewInfos_(BooIterT<BaaCellViewInfo*>& infos);
public:  bool processRWPaths_();
protected:  
  typedef std::map<std::string,BaaCellViewInfo*,std::less<std::string> > infoMapType_;
  typedef std::map<Nike::INikeData*,BaaCellViewInfo*,std::less<Nike::INikeData*> > cVMapType_;
  
  typedef std::map<std::string,std::string,std::less<std::string> > memoryToDmsType_;
  typedef std::map<std::string,std::string,std::less<std::string> > dmsToMemoryType_;

  typedef std::list<std::string> openSearchList_;
  
  typedef std::set<void(*)(const std::string& op,BaaCellViewInfo&source)> callBackType_;

  // Key: viewname.  Info: BaaCellViewInfo.  (all have the same cellname). 
  typedef std::map<std::string,BaaCellViewInfo*,std::less<std::string> > infoList;
  // Key: cellname.  Info: infoList_. 
  typedef std::map<std::string,CellViewMgr::infoList,std::less<std::string> > cachedInfo;

  static CellViewMgr* cellViewMgr_;
  infoMapType_ infoMap_;
  cVMapType_ cvMap_;
  
  memoryToDmsType_ memoryToDms_;
  dmsToMemoryType_ dmsToMemory_;
  callBackType_ callBacks_;
  openSearchList_ applicationOpenList_;
  
  CellViewRWMgr& rWMgr_;

  bool useFallBack_;
  bool idmsFirst_;
  bool useApplicationList_;
  int  enableBackups_;
  int  useCachedInfo_;		// flag indicating mode to use cached information or query idms.
  cachedInfo cachedMap_;	// map storing cached information.
 
  std::string workDir_;
  char*  compiledReg_;
  std::string asciiReg_;
  ElsOptionList opts_;
  CellViewSearchCustomizer* searchCustomizer_; 
  //. list of views with disabled backup creation
  std::set<std::string> noBackup_;

  //. list of views in the search order.
  std::vector<std::string> viewTypeSearchList_;
  bool useViewTypeSearchList_;

  Result result_;
  std::string resultStr_;
  bool hasNewMultiAssocCell_;

#endif
  

};



//
// =================================================================
// CLASS: CellViewSearchCustomizer 
//   
// =================================================================
//
class __declspec(DECL_CELLVIEW) CellViewSearchCustomizer {
public:
  //  ---
  //+ Member functions
  //  ---

  //. Search BaaCellViewInfo
  virtual bool findCellView(BaaCellViewInfo& info);
  virtual bool checkIn(BaaCellViewInfo&info){return false; }
  
  virtual bool checkOut(BaaCellViewInfo&info){return false; }
  virtual bool cancelCheckOut(BaaCellViewInfo&info){return false; }
  virtual bool install(BaaCellViewInfo&info) {return false; }
  
 //. returns or set the value of the idmsFallback flag.
  bool isidmsFallback();
  void setidmsFallback(bool idmsFallback);

protected:
  // ---
  //+ Constructors and destructor
  // ---

  //. Default constructor
  CellViewSearchCustomizer();
  //. Destructor
  virtual ~CellViewSearchCustomizer();

  //. flag to indicate if it needs to call idmsSearch if findCellview fails. 
  bool idmsFallback_;
};

#ifndef CELLVIEW_SWIG
//
// =================================================================
// CLASS: CellViewCatalog
//   
// =================================================================
//
class __declspec(DECL_CELLVIEW) CellViewCatalog {
  BOO_CAST(CellViewCatalog, BOO_CAST_NONE);
public:
  //  ---
  //+ Member functions
  //  ---

  //. Search BaaCellViewInfo
  virtual bool findCellView(BaaCellViewInfo& info) = 0;

  //. initialize catalog [called after parsing DMSPATH]
  virtual bool initCatalog() = 0;

  //. initialize catalog [called for every path refreshed]
  virtual bool initCatalogForPath(const std::string& path, bool prepend) = 0;

  //. clear catalog info
  virtual bool clearCatalog() = 0;

  //. clear catalog for path
  virtual bool clearCatalogForPath(const std::string& path) = 0;

  //. get catalog cells
  virtual unsigned int getCellsInCatalog(const std::string& path, BooIterT<std::string>& rwIter) = 0;

  //. get catalog paths
  virtual unsigned int getCatalogPaths(BooIterT<std::string>& rwIter) = 0;

  //. check if a cell is in the catalog
  virtual bool isCellInCatalog(const std::string& cellName) = 0;

  //. return the path for a cell
  virtual std::string getFullPath(const std::string& cellName) = 0;

  //. check if this cvi belongs to this catalog and if cell is readonly. File permission
  //  check is not necessary - taken care of by CellViewMgr;
  virtual bool isReadOnly(BaaCellViewInfo& info) = 0;

  //. set/get enable
  void setEnable(bool enable) { enable_ = enable; }
  bool isEnabled() { return enable_; }

  //. get viewnames
  const std::string& getCatalogView() { return catalogView_; }
  const std::string& getTargetView() { return targetView_; }

  //. set/get search order
  void setSearchOrder(int order) 
    { searchOrder_ = (searchOrder_ < 0) ? -1 : (searchOrder_ > 0) ? 1 : 0 ; }
  int getSearchOrder() { return searchOrder_; }

  //. search orders
  bool isFirstOrder() { return (searchOrder_ < 0); }
  bool isMiddleOrder() { return (searchOrder_ == 0); }
  bool isLastOrder() { return (searchOrder_ > 0); }

protected:
  // ---
  //+ Constructors and destructor
  // ---

  //. Default constructor
  CellViewCatalog() : 
    catalogView_(""), targetView_(""), enable_(true), searchOrder_(-1) {;}
  //. Destructor
  virtual ~CellViewCatalog() {;}

  //. viewname for this catalog
  std::string catalogView_;
  //. target cell views
  std::string targetView_;
  //. enabled
  bool enable_;
  //. search order
  int searchOrder_;
};
#endif //CELLVIEW_SWIG



//
// =================================================================
// CLASS: CellViewCatalogMgr
//   
// =================================================================
//
class __declspec(DECL_CELLVIEW) CellViewCatalogMgr {
// The CellViewCatalogMgr class enables file searching through
// registered catalogs. It also provides API to query for existence
// of catalogs and files in catalogs.
//
// For the methods, you need a pointer to the CellViewCatalogMgr
// object.
// CellViewCatalogMgr is a singleton and cannot be created.
//
// To get the CellViewCatalogMgr object:
// set ctlmgr [boo::CellViewCatalogMgr_getMgr]	
// _________________________________________________________________
public:
  //--
  //+ CatalogMgr utilities
  //--

  static CellViewCatalogMgr& getMgr();
  // Summary:
  //   Returns the Cell View Catalog Manager.
  // _________________________________________________________________
  // Syntax:
  //   CellViewCatalogMgr boo::CellViewCatalogMgr_getMgr
  //
  //   [ret] pointer to CellViewCatalogMgr
  // _________________________________________________________________
  // Example:
  //   set ctlmgr [boo::CellViewCatalogMgr_getMgr]
  // _________________________________________________________________  
  

  //--
  //+ catalog attributes
  //--

  void useCatalogs(bool use);
  // Summary:
  //   Enables use of catalogs when searching for files.
  // _________________________________________________________________
  // Syntax:
  //   boo::CellViewCatalogMgr_useCatalogs <self> <use>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <use> - bool: true(1) to enable use of catalogs, false(0)
  //                to not use catalogs when searching for files
  // _________________________________________________________________
  // Example:
  //   boo::CellViewCatalogMgr_useCatalogs $ctlmgr 1
  // _________________________________________________________________  
  
  
  bool isUsingCatalogs();
  // Summary:
  //   Checks if use of catalogs is enabled when searching for files.
  // _________________________________________________________________
  // Syntax:
  //   bool boo::CellViewCatalogMgr_isUsingCatalogs <self>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [ret] bool - true(1) if catalogs have been enabled, false(0) if
  //                not
  // _________________________________________________________________
  // Example:
  //   set use [boo::CellViewCatalogMgr_isUsingCatalogs $ctlmgr]
  // _________________________________________________________________    
  

  bool hasCatalogForDmsView(const std::string& dmsView);
  // Summary:
  //   Checks if a catalog exists for the given dms view name.
  // _________________________________________________________________
  // Syntax:
  //   bool boo::CellViewCatalogMgr_hasCatalogForDmsView <self>
  //                                                       <dmsView>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name
  //
  //   [ret] bool - true(1) if a catalog exists for this dms view name,
  //                false(0) if not
  // _________________________________________________________________
  // Example:
  //   set catdms [boo::CellViewCatalogMgr_hasCatalogForDmsView \
  //               $ctlmgr "alflay"]
  // _________________________________________________________________
   
  
  bool hasCatalogForTargetView(const std::string& targetView);
  // Summary:
  //   Checks if a catalog exists for the given target/memory view name.
  // _________________________________________________________________
  // Syntax:
  //   bool boo::CellViewCatalogMgr_hasCatalogForTargetView <self>
  //                                                      <targetView>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <targetView> - memory view name
  //
  //   [ret] bool - true(1) if a catalog exists for the target/memory
  //                view name, false(0) if not
  // _________________________________________________________________
  // Example:
  //   set cattar [boo::CellViewCatalogMgr_hasCatalogForTargetView \
  //               $ctlmgr "alf"]
  // _________________________________________________________________  
  

  std::string getCatalogDmsView(const std::string& targetView);
  // Summary:
  //   Returns the corresponding dms view name for the given target/
  //   memory view name if there exists a catalog that maps the two
  //   view names.
  // _________________________________________________________________
  // Syntax:
  //   string boo::CellViewCatalogMgr_getCatalogDmsView <self>
  //                                                      <targetView>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <targetView> - memory view name
  //
  //   [ret] dms view name that has a catalog mapping for the target/
  //         memory view name.
  // _________________________________________________________________
  // Example:
  //   set dmsview [boo::CellViewCatalogMgr_getCatalogDmsView \
  //                $ctlmgr "alf"]
  // _________________________________________________________________  
  
  
  std::string getCatalogTargetView(const std::string& dmsView);
  // Summary:
  //   Returns the corresponding target/memory view name for the given
  //   dms view name if there exists a catalog that maps the two view
  //   names.
  // _________________________________________________________________
  // Syntax:
  //   string boo::CellViewCatalogMgr_getCatalogTargetView <self>
  //                                                         <dmsView>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name
  //
  //   [ret] target/memory view name that has a catalog mapping for
  //         the dms view name
  // _________________________________________________________________
  // Example:
  //   set targetview [boo::CellViewCatalogMgr_getCatalogTargetView \
  //                   $ctlmgr "alflay"]
  // _________________________________________________________________   
  

  bool getCatalogTargetViews(BooIterT<std::string>& rwIter);
  // Summary:
  //   Returns all target/memory view names that have registered catalogs.
  // _________________________________________________________________
  // Syntax:
  //   list boo::CellViewCatalogMgr_getCatalogTargetViews <self>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //
  //   [ret] list of target/memory view names
  // _________________________________________________________________
  // Example:
  //   set targetviews [boo::CellViewCatalogMgr_getCatalogTargetViews \
  //                    $ctlmgr]
  // _________________________________________________________________   
  
  
  bool getCatalogDmsViews(BooIterT<std::string>& rwIter);
  // Summary:
  //   Returns all dms view names that have registered catalogs.
  // _________________________________________________________________
  // Syntax:
  //   list boo::CellViewCatalogMgr_getCatalogDmsViews <self>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //
  //   [ret] list of dms view names
  // _________________________________________________________________
  // Example:
  //   set dmsviews [boo::CellViewCatalogMgr_getCatalogDmsViews $ctlmgr]
  // _________________________________________________________________    
  

  //. enable catalog
  bool isCatalogEnabled(const std::string& dmsView);
  // Summary:
  //   Checks if a catalog is enabled for the given dms view name.
  // _________________________________________________________________
  // Syntax:
  //   bool boo::CellViewCatalogMgr_isCatalogEnabled <self> <dmsView>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name
  //
  //   [ret] bool - true(1) if catalog is enabled for the dms view name,
  //                false(0) if not
  // _________________________________________________________________
  // Example:
  //   set caten [boo::CellViewCatalogMgr_isCatalogEnabled $ctlmgr \
  //              "alflay"]
  // _________________________________________________________________    
  
  
  bool enableCatalog(const std::string& dmsView, bool enable);
  // Summary:
  //   Enables or disables a catalog for the given dms view name.
  // _________________________________________________________________
  // Syntax:
  //   bool boo::CellViewCatalogMgr_enableCatalog <self> <dmsView>
  //                                                <enable>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name
  //   [in] <enable> - bool: true(1) to enable, false(0) to disable
  //
  //   [ret] bool - true(1) if the dms view name has a catalog and it 
  //                was successfully disabled or enabled, false(0) if not
  // _________________________________________________________________
  // Example:
  //   set ret [boo::CellViewCatalogMgr_enableCatalog $ctlmgr \
  //            "alflay" 1]
  // _________________________________________________________________   
  

  //. catalog search order
  int getCatalogOrder(const std::string& dmsView);
  // Summary:
  //   Returns the search order of the catalog for the given dms view
  //   name.
  // _________________________________________________________________
  // Syntax:
  //   integer boo::CellViewCatalogMgr_getCatalogOrder <self> <dmsView>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name of catalog
  //
  //   [ret] integer value describing search order: 
  //         0 = search before DMSPATH and runtime search directories
  //         1 = search in between the DMSPATH and runtime search
  //             directories
  //         2 = search after DMSPATH and runtime search directories
  // _________________________________________________________________
  // Example:
  //   set catorder [boo::CellViewCatalogMgr_getCatalogOrder $ctlmgr \
  //                 "alflay"]
  // _________________________________________________________________
   
  
  bool setCatalogOrder(const std::string& dmsView, int order);
  // Summary:
  //   Sets the search order of the catalog for the dms view name.
  // _________________________________________________________________
  // Syntax:
  //   bool boo::CellViewCatalogMgr_setCatalogOrder <self> <dmsView>
  //                                                  <order>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name of catalog
  //   [in] <order> - integer value describing search order to use: 
  //        0 = search before DMSPATH and runtime search directories
  //        1 = search in between the DMSPATH and runtime search
  //            directories
  //        2 = search after DMSPATH and runtime search directories
  //
  //   [ret] bool - true(1) if dms view name has a catalog and it's
  //                search order was successfully set, false(0) if not
  // _________________________________________________________________
  // Example:
  //   set ret [boo::CellViewCatalogMgr_setCatalogOrder $ctlmgr \
  //            "alflay" 2]
  // _________________________________________________________________
   

  bool refreshCatalog(const std::string& dmsView);
  // Summary:
  //   Refreshes all paths in the catalog for the given dms view name.
  //   The catalog will look at all paths for any new files or deleted
  //   files.
  // _________________________________________________________________
  // Syntax:
  //   bool boo::CellViewCatalogMgr_refreshCatalog <self> <dmsView>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name of catalog
  //
  //   [ret] bool - true(1) if dms view name has a catalog and refresh
  //                was successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   set ret [boo::CellViewCatalogMgr_refreshCatalog $ctlmgr "alflay"]
  // _________________________________________________________________
  
  
  bool refreshCatalogForPath(const std::string& dmsView, const std::string& path,
                             bool prepend);
  // Summary:
  //   Refreshes a specific path in the catalog for the given dms view
  //   name.
  // _________________________________________________________________
  // Syntax:
  //   bool boo::CellViewCatalogMgr_refreshCatalogForPath <self> 
  //                                        <dmsView> <path> <prepend>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name of catalog
  //   [in] <path> - path to add to this catalog
  //   [in] <prepend> - bool: true(1) to prepend path to already existing
  //                    paths, false(0) to
  //
  //   [ret] bool - true(1) if dms view name has a catalog and refresh
  //                was successful for this path, false(0) if not
  // _________________________________________________________________
  // Example:
  //   set ret [boo::CellViewCatalogMgr_refreshCatalogForPath $ctlmgr \
  //            "alflay" "/tmp/alf/" 1]
  // _________________________________________________________________
                             

  bool clearCatalog(const std::string& dmsView);
  // Summary:
  //   Clears all paths from a catalog for the given dms view name.
  // _________________________________________________________________
  // Syntax:
  //   bool boo::CellViewCatalogMgr_clearCatalog <self> <dmsView>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name of catalog
  //
  //   [ret] bool - true(1) if dms view name has a catalog and clear
  //                was successful, false(0) if not
  // _________________________________________________________________
  // Example:
  //   set ret [boo::CellViewCatalogMgr_clearCatalog $ctlmgr "alflay"]
  // _________________________________________________________________
  
  
  bool clearCatalogForPath(const std::string& dmsView, const std::string& path);
  // Summary:
  //   Clears a specific path from a catalog for the given dms view name.
  // _________________________________________________________________
  // Syntax:
  //   bool boo::CellViewCatalogMgr_clearCatalogForPath <self>
  //                                                  <dmsView> <path>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name of catalog
  //   [in] <path> - path to remove from this catalog
  //
  //   [ret] bool - true(1) if dms view name has a catalog and the path
  //                was successfully removed, false(0) if not
  // _________________________________________________________________
  // Example:
  //   set ret [boo::CellViewCatalogMgr_clearCatalogForPath $ctlmgr \
  //            "alflay" "/tmp/alf/"]
  // _________________________________________________________________  
  

  bool getCells(const std::string& dmsView, const std::string& path, BooIterT<std::string>& rwIter);
  // Summary:
  //   Returns all cell names that are in the given path of a catalog 
  //   for the given dms view name.
  // _________________________________________________________________
  // Syntax:
  //   list boo::CellViewCatalogMgr_getCells <self> <dmsView> <path>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name of catalog
  //   [in] <path> - path to check for cell names
  //
  //   [ret] Tcl list of cell names
  // _________________________________________________________________
  // Example:
  //   set cells [boo::CellViewCatalogMgr_getCells $ctlmgr "alflay" \
  //              "/tmp/alf/"]
  // _________________________________________________________________
  
  
  bool getPaths(const std::string& dmsView, BooIterT<std::string>& rwIter);
  // Summary:
  //   Returns all paths contained in a catalog for the given dms view
  //   name.
  // _________________________________________________________________
  // Syntax:
  //   list boo::CellViewCatalogMgr_getPaths <self> <dmsView>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name of catalog
  //
  //   [ret] Tcl list of path strings
  // _________________________________________________________________
  // Example:
  //   set paths [boo::CellViewCatalogMgr_getPaths $ctlmgr "alflay"]
  // _________________________________________________________________  
  
  
  bool hasCell(const std::string& dmsView, const std::string& cellName);
  // Summary:
  //   Checks if a cell is founded in a catalog for the given dms view
  //   name.
  // _________________________________________________________________
  // Syntax:
  //   bool boo::CellViewCatalogMgr_hasCell <self> <dmsView> <cellname>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name of catalog
  //   [in] <cellname> - name of cell to search
  //
  //   [ret] bool - true(1) if a file for the given cell name was found
  //                in the catalog, false(0) if not
  // _________________________________________________________________
  // Example:
  //   set hascell [boo::CellViewCatalogMgr_hasCell $ctlmgr "alflay" "topcell"]
  // _________________________________________________________________  
  
  
  std::string getPath(const std::string& dmsView, const std::string& cellName);
  // Summary:
  //   Returns the full path of a file for the given cell name from a
  //   catalog of the given dms view name.
  // _________________________________________________________________
  // Syntax:
  //   string boo::CellViewCatalogMgr_getPath <self> <dmsView>
  //                                            <cellname>
  //
  //   [in] <self> - pointer to the CellViewCatalogMgr singleton
  //   [in] <dmsView> - dms view name of catalog
  //   [in] <cellname> - name of cell to check
  //
  //   [ret] full path string of file if found, empty string otherwise
  // _________________________________________________________________
  // Example:
  //   set path [boo::CellViewCatalogMgr_getPath $ctlmgr "alflay" \
  //             "topcell"]
  // _________________________________________________________________    
  

#ifndef CELLVIEW_SWIG  
  //. add/remove catalog
  bool addCatalog(const std::string& dmsView, CellViewCatalog& catalog);
  CellViewCatalog* removeCatalog(const std::string& dmsView);

  //. get catalogs
  CellViewCatalog* getCatalog(const std::string& viewName, bool searchTargets=false);
  CellViewCatalog* getActiveCatalog(const std::string& viewName);

protected:
  // ---
  //+ Constructors and destructor
  // ---
  CellViewCatalogMgr();
  ~CellViewCatalogMgr();

  //. data
  bool useCatalogs_;
  //. map of targetviews to catalogs
  typedef std::map<std::string, CellViewCatalog*, std::less<std::string> > Catalogs;
  Catalogs catalogs_;

  //. singleton obj
  static CellViewCatalogMgr* mgr_;
  
#endif //CELLVIEW_SWIG
};

#endif //CELLVIEW_MGR_H
