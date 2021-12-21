//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                    (c) 2003 - 2003 Intel Corp.                      //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id: CellViewPrimList.h //
//

#ifndef CELLVIEW_PRIMLIST_H
#define CELLVIEW_PRIMLIST_H

// -----------------------------------------------------------------
// --- Boo/system includes
// -----------------------------------------------------------------
#include <Boo/BooBasics.h>
#include <Boo/BooVersion.h>
#include <Boo/BooIterT.h> //. for input/output primlist
#include <fstream>
//. for regexp
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <string.h>

// -----------------------------------------------------------------
// --- stl includes
// -----------------------------------------------------------------
#include <set>
#include <vector>

// -----------------------------------------------------------------
// --- Forward declarations
// -----------------------------------------------------------------
#ifndef  CELLVIEW_SWIG
namespace Nike {
  class INikeData;
}
#endif

//. for swig
typedef BooIterT<std::string> CellListStrIter;

//
// =================================================================
// CLASS: CellViewCellList
// =================================================================
//   1. create a list of cell names / regexps
//   2. modify or query the list / regexps
// =================================================================
//
class __declspec(DECL_CELLVIEW) CellViewCellList {
// The CellViewCellList class manages a list of cell names that can
// include regular expressions. This list is used to filter
// cells during operations such as Open. Only cell names that match
// the names and regular expressions in the cell list will be processed.
//
// For the methods, you need a pointer to the CellViewCellList object.
//
// To get the CellViewCellList object:
// 
//
// To create a CellViewCellList object:
//   set cvcl [::boo::CellViewCellList]
// _________________________________________________________________


public:
  //. ctor/dtor
  CellViewCellList();
  // Summary:
  //   Creates a CellViewCellList object.
  // _________________________________________________________________
  // Syntax:
  //   CellViewCellList ::boo::CellViewCellList
  //
  //   [ret] pointer to new CellViewCellList
  // _________________________________________________________________  
  // Example:
  //   set cvcl [::boo::CellViewCellList]
  // _________________________________________________________________  
  
  
  ~CellViewCellList();
  // Summary:
  //   Do not use.
  // _________________________________________________________________
  
  

  //. check a cellname against the list
  //  [in] cellName	cellname to be checked
  //  [ret] .		True if it is in the list, 
  //                    false otherwise.
  bool isInCellList(const std::string& cellName, bool checkRegExp=true);
  // Summary:
  //   Checks a cell name against the cell list.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewCellList_isInCellList <self> <cellname>
  //                                             <checkRegExp>
  //
  //   [in] <self> - pointer to the CellViewCellList
  //   [in] <cellname> - name of the cell
  //   [in] <checkRegExp> - bool: true(1) to match the cell name against
  //                              regular expressions, false(0) to not
  //                              consider regular expressions
  //
  //   [ret] bool - true(1) if cell name is in cell list, false(0) if not
  // _________________________________________________________________  
  // Example:
  //   set inlist [::boo::CellViewCellList_isInCellList $cvcl "cell1" 0]
  // _________________________________________________________________
  

  //. read cellList file [file with search pattern of cell names]
  //  [in] fileName		filename is specified with full path
  //  [ret] .			status of reading cellList file.
  bool readCellList(const std::string& fileName);
  // Summary:
  //   Reads in a cell list from a file on disk to the cell list object.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewCellList_readCellList <self> <path>
  //
  //   [in] <self> - pointer to the CellViewCellList
  //   [in] <path> - absolute or relative path to file
  //
  //   [ret] bool - true(1) if cell list is successfully read, false(0)
  //                if not
  // _________________________________________________________________    
  // Example:
  //   set ret [::boo::CellViewCellList_readCellList $cvcl /
  //            "./cellnames.list"]
  // _________________________________________________________________
  

  //. set cellList list [list with search pattern of cell names]
  //  [in] list			list of search patterns
  //  [in] appendToList		flag to append to existing list.
  //                            if false, the current list will be cleared.
  //  [ret] .			true if succeeded, false otherwise
  bool setCellList(CellListStrIter& inexprs, bool appendToCurrentList);
  // Summary:
  //   Adds the given list of cell names including regular expressions
  //   to the cell list.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewCellList_setCellList <self> <cellnames>
  //                                            <append>
  //
  //   [in] <self> - pointer to the CellViewCellList
  //   [in] <cellnames> - Tcl list of cell names/regular expressions
  //   [in] <append> - bool: true(1) to append to the existing list,
  //                         false(0) to clear the existing list and
  //                         replace with new list
  //
  //   [ret] bool - true(1) if cell list is successfully set, false(0)
  //                if not
  // _________________________________________________________________      
  // Example:
  //   set list [list 0m0* inv* cell1]
  //   set ret [::boo::CellViewCellList_setCellList $cvcl $list 1
  // _________________________________________________________________
  
  

  //. return list of search patterns in cellList
  //  [in] .		BooIter for search patterns.
  void getCellList(BooIterT<std::string>& exprs);
  // Summary:
  //   Returns the list of cell names/regular expressions from the
  //   cell list.
  // _________________________________________________________________
  // Syntax:
  //   list ::boo::CellViewCellList_getCellList <self>
  //
  //   [in] <self> - pointer to the CellViewCellList
  //
  //   [ret] Tcl list of cell names from cell list
  // _________________________________________________________________
  // Example:
  //   set splist [::boo::CellViewCellList_getCellList $cvcl]
  // _________________________________________________________________
  
  

  //. returns count of search patterns in cellList
  //  [ret] . 		Number of search patterns in cellList.
  unsigned int getCellListCount();
  // Summary:
  //   Returns the number of cell names in the cell list.
  // _________________________________________________________________
  // Syntax:
  //   integer ::boo::CellViewCellList_getCellListCount <self>
  //
  //   [in] <self> - pointer to the CellViewCellList
  //
  //   [ret] number of cell names in cell list
  // _________________________________________________________________  
  // Example:
  //   set spcount [::boo::CellViewCellList_getCellListCount $cvcl]
  // _________________________________________________________________
  

  //. clear cell list
  void clearCellList();
  // Summary:
  //   Clears the cell list.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewCellList_clearCellList <self>
  //
  //   [in] <self> - pointer to the CellViewCellList
  // _________________________________________________________________  
  // Example:
  //   ::boo::CellViewCellList_clearCellList $cvcl
  // _________________________________________________________________
  

  //. add an expression to cellList
  //  [in] expr		Add a search pattern to cellList.
  //  [ret] .		False if it already exists.
  bool addSearchPattern(const std::string& expr);
  // Summary:
  //   Adds the given cell name/regular expression to the cell list.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewCellList_addSearchPattern <self> <cellname>
  //
  //   [in] <self> - pointer to the CellViewCellList
  //   [in] <cellname> - cell name (including regular expression)
  //
  //   [ret] bool - true(1) if the cell name is successfully added,
  //                false(0) if not
  // _________________________________________________________________  
  // Example:
  //   set ret [::boo::CellViewCellList_addSearchPattern $cvcl "0m0*"]
  // _________________________________________________________________
  
  

  //. remove an expression from cellList
  //  [in] expr		Remove a search pattern from the cellList.
  //  [ret] .		False if it does not exist.
  bool removeSearchPattern(const std::string& expr);
  // Summary:
  //   Removes the given cell name/regular expression from the cell
  //   list.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewCellList_removeSearchPattern <self> <cellname>
  //
  //   [in] <self> - pointer to the CellViewCellList
  //   [in] <cellname> - cell name (including regular expression)
  //
  //   [ret] bool - true(1) if the cell name is successfully removed,
  //                false(0) if not
  // _________________________________________________________________  
  // Example:
  //   set ret [::boo::CellViewCellList_removeSearchPattern $cvcl "0m0*"]
  // _________________________________________________________________
  
  

  //. check if an expression is in the cellList
  //  [in] expr		search pattern to check.
  //  [ret] .		False if search pattern is not in list, true otherwise.
  bool isExpr(const std::string& expr);
  // Summary:
  //   Checks if the given cell name/regular expression is in the cell
  //   list.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewCellList_isExpr <self> <cellname>
  //
  //   [in] <self> - pointer to the CellViewCellList
  //   [in] <cellname> - cell name (including regular expression)
  //
  //   [ret] bool - true(1) if the cell name is in the list,
  //                false(0) if not
  // _________________________________________________________________    
  // Example:
  //   set inList [::boo::CellViewCellList_isExpr $cvcl "0m0*"]
  // _________________________________________________________________
  

  //. is valid cellList
  //  [ret] .	validity of cell list.
  bool isCellListValid() { return legalList_; }
  // Summary:
  //   Checks if the cell list is valid.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewCellList_isCellListValid <self>
  //
  //   [in] <self> - pointer to the CellViewCellList
  //
  //   [ret] bool - true(1) if cell list is valid, false(0) if not
  // _________________________________________________________________    
  // Example:
  //   set isvalid [::boo::CellViewCellList_isCellListValid $cvcl]
  // _________________________________________________________________
  

  //. set name of cellList
  //  [in] name		name of cellList
  void setName(const std::string& name) { name_ = name; }
  // Summary:
  //   Sets the name of the cell list.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewCellList_setName <self> <name>
  //
  //   [in] <self> - pointer to the CellViewCellList
  //   [in] <name> - name for cell list
  // _________________________________________________________________    
  // Example:
  //  ::boo::CellViewCellList_setName $cvcl "CellList1"
  // _________________________________________________________________
  
  

  //. get name of cellList
  //  [ret] .		name of cellList
  const std::string& getName() { return name_; }
  // Summary:
  //   Returns the name of the cell list.
  // _________________________________________________________________
  // Syntax:
  //   string ::boo::CellViewCellList_getName <self>
  //
  //   [in] <self> - pointer to the CellViewCellList
  //
  //   [ret] cell list name
  // _________________________________________________________________    
  // Example:
  //  set name [::boo::CellViewCellList_getName $cvcl]
  // _________________________________________________________________

  //. set inverse mode [inverse results of list]
  //  [in] flag		Flag to enable or disable inverse mode.
  void setInverseMode(bool flag) { inverse_ = flag; }
  // Summary:
  //   Sets the CellViewCellList inverse mode.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewCellList_setInverseMode <self> <inverse>
  //
  //   [in] <self> - pointer to CellViewCellList obj
  //   [in] <inverse> - bool: true(1) to use the inverse of the lists
  //                          when matching cells, false(0) to use the
  //                          lists as specified
  // _________________________________________________________________   
  // Example:
  //   ::boo::CellViewCellList_setInverseMode $obj 1
  // _________________________________________________________________  
  
  //. get inverse mode [inverse results of list]
  //  [ret] .	Current inverse mode.
  bool getInverseMode() { return inverse_; }
  // Summary:
  //   Returns the value of the CellViewCellList inverse mode.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewCellList_getInverseMode <self>
  //
  //   [in] <self> - pointer to CellViewCellList obj
  //   
  //   [ret] bool - true(1) if inverse mode is set on, false(0) if not
  // _________________________________________________________________   
  // Example:
  //   set enb [::boo::CellViewCellList_getInverseMode $obj]
  // _________________________________________________________________
  
#ifndef CELLVIEW_SWIG
  //. read regular expressions from file into the supplied
  //  regular expression object.
  //  [in] regExp		object into which regular
  //       			exprs need to be compiled
  //  [in] f			input source file for patterns
  //  [ret] .			status of reading regexps into obj
  static bool readRegExpList(regex_t& regExp, std::ifstream& f);

  //. check if a name matches the regexp
  //  [in] regExp		regular expression
  //  [in] name			name to be matched with regExp
  //  [ret] .			true if name matches regExp, false otherwise
  static bool isInRegExpList(regex_t& regExp, const std::string& name);

#endif

#ifndef CELLVIEW_SWIG
  //. typedef for set of search patterns
  typedef std::set<std::string> PattList;
#endif

private: //. methods
#ifndef CELLVIEW_SWIG

  //. compile regexps
  static bool compileRegExp_(regex_t& re, PattList& plist);
  static bool compileRegExp_(regex_t& re, const std::string& compName);

  //. read list
  bool readList_(const std::string& fileName);

  //. get list
  static void getList_(BooIterT<std::string>& exprs, PattList& plist);

  //. check expr
  static bool checkExpr_(const std::string& expr);
#endif

private: //. data

  //. validity of primlist
  bool legalList_;
  //. inverse results
  bool inverse_;
 //. is dirty. needs recalculation.
  short isDirty_;
 
  //. name of list
  std::string name_;

#ifndef CELLVIEW_SWIG

  //. cellList regexp
  regex_t listRegExp_;

  //. set of search patterns
  PattList cellList_;

  //. BooMsg out
  static const std::string CVCL_OUT;

  //. pattern list extent
  static PattList* extent_;
 
  //. friends
  friend class CellViewPrimList;
#endif
};


//
// =================================================================
// CLASS: CellViewPrimList
// =================================================================
//  This is a singleton class used to 
//   1. set primlist options for specifying black box 
//      cell information [leaf cells].
//   2. set secondary list information for specifying mixed view
//      cell information
//   3. set synth list information for specifying which cells
//      need to be synthesized and not read from disk.
//  
// =================================================================
//
class __declspec(DECL_CELLVIEW) CellViewPrimList {
// The CellViewPrimList class is a singleton object that manages
// three specifically defined cell lists (CellViewCellLists):
// 1. Prim List - This list specifies black box cells. Only the
//    interface of these cells will be loaded, not the contents or
//    any lower hierarchy.
// 2. Sec List - This list specifies mixed view cells for PV purposes.
// 3. Synth List - This list specifying cells that need to be
//                synthesized and not read from disk.
//
// For the methods, you need a pointer to the CellViewPrimList object.
//
// To get the CellViewPrimList object:
//   set mgr [::boo::CellViewPrimList_getMgr]	
// _________________________________________________________________

public:

  //. search mode
  enum SearchMode { REGEX=0, ABS=1 };

  //. API are listed in the following order :
  //  1) CellViewPrimList Manager API
  //  2) Primlist API
  //  3) Secondary List API
  //  4) Synth List API
  //  API beginning can be searched using the index (example : "2)" )

  //--
  //+ 1) CellViewPrimList Manager - API
  //--

  //. get list manager
  //  [ret] .	Reference to PrimList mgr.
  static CellViewPrimList& getMgr();
  // Summary:
  //   Returns the CellViewPrimList manager. Use this to access the
  //   Prim List, Sec List, or Synth List.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewPrimList_getmgr
  //
  //   [ret] pointer to CellViewPrimList
  // _________________________________________________________________  
  // Example:
  //   set mgr [::boo::CellViewPrimList_getMgr]	
  // _________________________________________________________________
  

  //. set enable - default is enabled
  //  [in] flag		Flag to enable or disable primlist search.
  void setEnable(bool flag) { enable_ = flag; }
  // Summary:
  //   Sets the value of the CellViewPrimList enable flag to enable
  //   or disable use of the Prim List, Sec List, and Synth List.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewPrimList_setEnable <self> <enable>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <enable> - bool: true(1) to enable lists, false(0) to
  //                         disable lists
  // _________________________________________________________________  
  // Example:
  //   ::boo::CellViewPrimList_setEnable $mgr 1
  // _________________________________________________________________
    

  //. get enable
  //  [ret] .	Enable flag status.
  bool getEnable() { return enable_; }
  // Summary:
  //   Returns the value of the CellViewPrimList enable flag.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_getEnable <self>
  //
  //   [in] <self> - pointer to CellViewPrimList manager  
  //
  //   [ret] bool - true(1) if lists are enabled, false(0) if not
  // _________________________________________________________________  
  // Example:
  //   set enb [::boo::CellViewPrimList_getEnable]
  // _________________________________________________________________
  

  //. set inverse mode [inverse results of primlist]
  //  [in] flag		Flag to enable or disable inverse mode.
  void setInverseMode(bool flag) { getPrimListObj().setInverseMode(flag); }
  // Summary:
  //   Sets the CellViewPrimList inverse mode.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewPrimList_setInverseMode <self> <inverse>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <inverse> - bool: true(1) to use the inverse of the lists
  //                          when matching cells, false(0) to use the
  //                          lists as specified
  // _________________________________________________________________   
  // Example:
  //   ::boo::CellViewPrimList_setInverseMode $mgr 1
  // _________________________________________________________________  

  //. get inverse mode [inverse results of primlist]
  //  [ret] .	Current inverse mode.
  bool getInverseMode() { return getPrimListObj().getInverseMode(); }
  // Summary:
  //   Returns the value of the CellViewPrimList inverse mode.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_getInverseMode <self>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   
  //   [ret] bool - true(1) if inverse mode is set on, false(0) if not
  // _________________________________________________________________   
  // Example:
  //   set enb [::boo::CellViewPrimList_getInverseMode $mgr]
  // _________________________________________________________________

  //. set secondary list inverse mode [inverse results of secondary-list]
  //  [in] flag		Flag to enable or disable inverse mode.
  void setSecListInverseMode(bool flag) { getSecListObj().setInverseMode(flag); }
  // Summary:
  //   Sets the CellViewPrimList inverse mode.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewPrimList_setSecListInverseMode <self> <inverse>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <inverse> - bool: true(1) to use the inverse of the lists
  //                          when matching cells, false(0) to use the
  //                          lists as specified
  // _________________________________________________________________   
  // Example:
  //   ::boo::CellViewPrimList_setSecListInverseMode $mgr 1
  // _________________________________________________________________  
  
  //. get inverse mode [inverse results of secondary-list]
  //  [ret] .	Current inverse mode.
  bool getSecListInverseMode() { return getSecListObj().getInverseMode(); }
  // Summary:
  //   Returns the value of the CellViewPrimList inverse mode.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_getSecListInverseMode <self>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   
  //   [ret] bool - true(1) if inverse mode is set on, false(0) if not
  // _________________________________________________________________   
  // Example:
  //   set enb [::boo::CellViewPrimList_getSecListInverseMode $mgr]
  // _________________________________________________________________
  
  //. set synth list inverse mode [inverse results of synth-list]
  //  [in] flag		Flag to enable or disable inverse mode.
  void setSynthListInverseMode(bool flag) { getSynthListObj().setInverseMode(flag); }
  // Summary:
  //   Sets the CellViewPrimList inverse mode.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewPrimList_setSynthListInverseMode <self> <inverse>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <inverse> - bool: true(1) to use the inverse of the lists
  //                          when matching cells, false(0) to use the
  //                          lists as specified
  // _________________________________________________________________   
  // Example:
  //   ::boo::CellViewPrimList_setSynthListInverseMode $mgr 1
  // _________________________________________________________________  
  
  //. get inverse mode [inverse results of synth-list]
  //  [ret] .	Current inverse mode.
  bool getSynthListInverseMode() { return getSynthListObj().getInverseMode(); }
  // Summary:
  //   Returns the value of the CellViewPrimList inverse mode.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_getSynthListInverseMode <self>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   
  //   [ret] bool - true(1) if inverse mode is set on, false(0) if not
  // _________________________________________________________________   
  // Example:
  //   set enb [::boo::CellViewPrimList_getSynthListInverseMode $mgr]
  // _________________________________________________________________
  

  //. set search mode - default is regexp
  //  [in] mode		Search mode.
  void setSearchMode(SearchMode mode) { searchMode_ = mode; }
  // Summary:
  //   Sets the CellViewPrimList search mode.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewPrimList_setSearchMode <self> <mode>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <mode> - bool: true(1) to match cell names against regular
  //                       expressions, false(0) to not consider
  //                       regular expressions
  // _________________________________________________________________   
  // Example:
  //   ::boo::CellViewPrimList_setSearchMode $mgr 0
  // _________________________________________________________________    
  

  //. get search mode
  //  [ret] .	Search mode.
  bool getSearchMode() { return searchMode_; }
  // Summary:
  //   Returns the value of the CellViewPrimList search mode.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_getSearchMode <self>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //
  //   [ret] bool - true(1) if search mode is set to use regular
  //                expression, false(0) if set to ignore them
  // _________________________________________________________________   
  // Example:
  //   set enb [::boo::CellViewPrimList_getSearchMode $mgr]
  // _________________________________________________________________
  

  //. get primlist manager version
  //  [ret] .		version of primlist manager.
  int getVersion() { return ver_.getVersion(); }
  // Summary:
  //   Returns the latest version of the Prim List. Each time the Prim
  //   List is modified, it is versioned.
  // _________________________________________________________________
  // Syntax:
  //   integer ::boo::CellViewPrimList_getVersion <self>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //
  //   [ret] latest version number of Prim List
  // _________________________________________________________________    
  // Example:
  //   set ver [::boo::CellViewPrimList_getVersion $mgr]
  // _________________________________________________________________
  

  //. sets default values [enabled with empty lists].
  void setDefaults();
  // Summary:
  //   Sets the default values for CellViewPrimList. This clears all
  //   three lists and enables them. Inverse mode is set off(0) and
  //   search mode is set to use regular expressions(1).
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewPrimList_setDefaults <self>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  // _________________________________________________________________   
  // Example:
  //   ::boo::CellViewPrimList_setDefaults $mgr
  // _________________________________________________________________
  

  //. start a new session. 
  //  This method stores current settings and lists.
  //  Restores defaults so a new session can be started without
  //  being affected by previous settings.
  void startSession();
  // Summary:
  //   Starts a new session for the Prim List. This makes a backup
  //   of the current Prim List and creates a temporary empty one for
  //   use in the current session. Use endSession to restore the backup.
  // _________________________________________________________________
  // Syntax:
  //   ::boo::CellViewPrimList_startSession <self>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  // _________________________________________________________________   
  // Example:
  //   ::boo::CellViewPrimList_startSession $mgr
  // _________________________________________________________________
  
 
  //. end a newly started session.
  //  This method restores last session that was cached.
  //  [ret] .		True if this is a new session that can 
  // 			be ended. False otherwise.
  bool endSession();
  // Summary:
  //   Ends a Prim List session created using startSession and restores
  //   the backup so that the Prim List is as it was when startSession
  //   was called.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_endSession <self>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //
  //   [ret] bool - true(1) if Prim List is successfully restored,
  //                false(0) if not
  // _________________________________________________________________    
  // Example:
  //   set ret [::boo::CellViewPrimList_endSession $mgr]
  // _________________________________________________________________
  

  //. get number of cached sessions
  //  [ret] .		Number of cached sessions.
  unsigned int numSessions();
  // Summary:
  //   Returns the number of sessions that have been started and have
  //   not yet been ended for the Prim List.
  // _________________________________________________________________
  // Syntax:
  //   integer ::boo::CellViewPrimList_numSessions <self>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //
  //   [ret] number of cached Prim Lists
  // _________________________________________________________________   
  // Example:
  //   set num [::boo::CellViewPrimList_numSessions $mgr]
  // _________________________________________________________________

  //--
  //+ 2) Primlist - API
  //--

  //. check a cellname against the primlist
  //  [in] cellName	cellname to be checked
  //  [ret] .		True if it is to be a black box cell, 
  //                    false otherwise.
  bool isInPrimList(const std::string& cellName);
  // Summary:
  //   Checks if the given cell name matches an entry in the Prim List
  //   indicating it is a black box cell.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_isInPrimList <self> <cellname>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <cellname> - name of cell to match
  //
  //   [ret] bool - true(1) if cell name has a match, false(0) if not
  // _________________________________________________________________    
  // Example:
  //   set stopcell [::boo::CellViewPrimList_isInPrimList $mgr Cell1]
  // _________________________________________________________________
  
  

  //. read primlist file [file with search pattern of cell names]
  //  [in] fileName		filename is specified with full path
  //  [ret] .			status of reading primlist file.
  bool readPrimList(const std::string& fileName);
  // Summary:
  //   Reads in a cell list from a file on disk to become the Prim List.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_readPrimList <self> <path>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <path> - full path to cell list file
  //
  //   [ret] bool - true(1) if cell list is succcessfully read to Prim
  //                List, false(0) if not
  // _________________________________________________________________    
  // Example:
  //   set ret [::boo::CellViewPrimList_readPrimList $mgr ./cellnames.list]
  // _________________________________________________________________
  

  //. set primlist list [list with search pattern of cell names]
  //  [in] list			list of search patterns
  //  [in] appendToList		flag to append to existing list.
  //                            if false, the current list will be cleared.
  //  [ret] .			true if succeeded, false otherwise
  bool setPrimList(CellListStrIter& inexprs, bool appendToCurrentList);
  // Summary:
  //   Adds the given list of cell names including regular expressions
  //   to the Prim List.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_setPrimList <self> <cellnames>
  //                                            <append>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <cellnames> - Tcl list of cell names/regular expressions
  //   [in] <append> - bool: true(1) to append to the existing list,
  //                         false(0) to clear the existing list and
  //                         replace with new list
  //
  //   [ret] bool - true(1) if Prim List is successfully set, false(0)
  //                if not
  // _________________________________________________________________      
  // Example:
  //   set list [list 0m0* inv* cell1]
  //   set ret [::boo::CellViewPrimList_setPrimList $mgr $list 1]
  // _________________________________________________________________


  //. return list of search patterns in primlist
  //  [in] .		BooIter for search patterns.
  void getPrimList(BooIterT<std::string>& exprs);
  // Summary:
  //   Returns the list of cell names/regular expressions in the Prim
  //   List.
  // _________________________________________________________________
  // Syntax:
  //   list ::boo::CellViewPrimList_getPrimList <self>
  //
  //   [in] <self> - pointer to the CellViewPrimList manager
  //
  //   [ret] Tcl list of cell names from Prim List
  // _________________________________________________________________  
  // Example:
  //   set splist [::boo::CellViewPrimList_getPrimList $mgr]
  // _________________________________________________________________
  

  //. get cell list obj
  //  [ret] .		returns cell list obj
  CellViewCellList& getPrimListObj();
  // Summary:
  //    Returns the CellViewCellList object for the Prim List.
  // _________________________________________________________________
  // Syntax:
  //   CellViewCellList ::boo::CellViewPrimList_getPrimListObj <self>
  //
  //   [in] <self> - pointer to the CellViewPrimList manager
  //
  //   [ret] pointer to CellViewCellList
  // _________________________________________________________________  
  // Example:
  //   set celllist [::boo::CellViewPrimList_getPrimListObj $mgr]
  // _________________________________________________________________

  //--
  //+ 3) Secondary list - API
  //--

  //. check a cellname against the secondary list
  //  [in] cellName	cellname to be checked
  //  [ret] .		True if it is to be a mixed view cell, false otherwise.
  bool isInSecondaryList(const std::string& cellName);
  // Summary:
  //   Checks if the given cell name matches an entry in the Sec List
  //   indicating it is a mixed view cell.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_isInSecondaryList <self> <cellname>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <cellname> - name of cell to match
  //
  //   [ret] bool - true(1) if cell name has a match, false(0) if not
  // _________________________________________________________________  
  // Example:
  //   set mixcell [::boo::CellViewPrimList_isInSecondaryList $mgr Cell2]
  // _________________________________________________________________
  
  

  //. read secondary list file [file with search pattern of cell names]
  //  [in] fileName		filename is specified with full path
  //  [ret] .			status of reading secondary list file.
  bool readSecondaryList(const std::string& fileName);
  // Summary:
  //   Reads in a cell list from a file on disk to become the Sec List.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_readSecondaryList <self> <path>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <path> - full path to cell list file
  //
  //   [ret] bool - true(1) if cell list is succcessfully read to Sec
  //                List, false(0) if not
  // _________________________________________________________________    
  // Example:
  //   set ret [::boo::CellViewPrimList_readSecondaryList $mgr /
  //            ./cellnames.list]
  // _________________________________________________________________
  
  

  //. set secondary list [list with search pattern of cell names]
  //  [in] list			list of search patterns
  //  [in] appendToList		flag to append to existing list.
  //                            if false, the current list will be cleared.
  //  [ret] .			true if succeeded, false otherwise
  bool setSecondaryList(CellListStrIter& inexprs, bool appendToCurrentList);
  // Summary:
  //   Adds the given list of cell names including regular expressions
  //   to the Sec List.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_setSecondaryList <self> <cellnames>
  //                                                 <append>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <cellnames> - Tcl list of cell names/regular expressions
  //   [in] <append> - bool: true(1) to append to the existing list,
  //                         false(0) to clear the existing list and
  //                         replace with new list
  //
  //   [ret] bool - true(1) if Sec List is successfully set, false(0)
  //                if not
  // _________________________________________________________________      
  // Example:
  //   set list [list 0m0* inv* cell1]
  //   set ret [::boo::CellViewPrimList_setSecondaryList $mgr $list 1]
  // _________________________________________________________________
  
  

  //. return list of search patterns in secondary list
  //  [in] .		BooIter for search patterns.
  void getSecondaryList(BooIterT<std::string>& exprs);
  // Summary:
  //   Returns the list of cell names/regular expressions in the Sec
  //   List.
  // _________________________________________________________________
  // Syntax:
  //   list ::boo::CellViewPrimList_getSecondaryList <self>
  //
  //   [in] <self> - pointer to the CellViewPrimList manager
  //
  //   [ret] Tcl list of cell names from Sec List
  // _________________________________________________________________  
  // Example:
  //   set splist [::boo::CellViewPrimList_getSecondaryList $mgr]
  // _________________________________________________________________
  
  

  //. get cell list obj
  //  [ret] .		returns cell list obj
  CellViewCellList& getSecListObj();
  // Summary:
  //    Returns the CellViewCellList object for the Sec List.
  // _________________________________________________________________
  // Syntax:
  //   CellViewCellList ::boo::CellViewPrimList_getSecListObj <self>
  //
  //   [in] <self> - pointer to the CellViewPrimList manager
  //
  //   [ret] pointer to CellViewCellList
  // _________________________________________________________________  
  // Example:
  //   set celllist [::boo::CellViewPrimList_getSecListObj $mgr]
  // _________________________________________________________________

  //--
  //+ 4) Synth list - API
  //--

  //. check a cellname against the synth list
  //  [in] cellName	cellname to be checked
  //  [ret] .		True if it is to be a synthesized cell, false otherwise.
  bool isInSynthList(const std::string& cellName);
  // Summary:
  //   Checks if the given cell name matches an entry in the Synth List
  //   indicating it is a cell to be synthesized and not loaded from
  //   disk.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_isInSynthList <self> <cellname>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <cellname> - name of cell to match
  //
  //   [ret] bool - true(1) if cell name has a match, false(0) if not
  // _________________________________________________________________  
  // Example:
  //   set synthcell [::boo::CellViewPrimList_isInSynthList $mgr Cell2]
  // _________________________________________________________________
  
  

  //. read synth list file [file with search pattern of cell names]
  //  [in] fileName		filename is specified with full path
  //  [ret] .			status of reading synthlist file.
  bool readSynthList(const std::string& fileName);
  // Summary:
  //   Reads in a cell list from a file on disk to become the Synth
  //   List.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_readSynthList <self> <path>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <path> - full path to cell list file
  //
  //   [ret] bool - true(1) if cell list is succcessfully read to Synth
  //                List, false(0) if not
  // _________________________________________________________________    
  // Example:
  //   set ret [::boo::CellViewPrimList_readSynthList $mgr /
  //            ./cellnames.list]
  // _________________________________________________________________
  
  

  //. set synth list [list with search pattern of cell names]
  //  [in] list			list of search patterns
  //  [in] appendToList		flag to append to existing list.
  //                            if false, the current list will be cleared.
  //  [ret] .			true if succeeded, false otherwise
  bool setSynthList(CellListStrIter& inexprs, bool appendToCurrentList);
  // Summary:
  //   Adds the given list of cell names including regular expressions
  //   to the Synth List.
  // _________________________________________________________________
  // Syntax:
  //   bool ::boo::CellViewPrimList_setSynthList <self> <cellnames>
  //                                             <append>
  //
  //   [in] <self> - pointer to CellViewPrimList manager
  //   [in] <cellnames> - Tcl list of cell names/regular expressions
  //   [in] <append> - bool: true(1) to append to the existing list,
  //                         false(0) to clear the existing list and
  //                         replace with new list
  //
  //   [ret] bool - true(1) if Synth List is successfully set, false(0)
  //                if not
  // _________________________________________________________________      
  // Example:
  //   set list [list 0m0* inv* cell1]
  //   set ret [::boo::CellViewPrimList_setSynthList $mgr $list 1]
  // _________________________________________________________________
  
  

  //. return list of search patterns in synth list
  //  [in] .		BooIter for search patterns.
  void getSynthList(BooIterT<std::string>& exprs);
  // Summary:
  //   Returns the list of cell names/regular expressions in the Synth
  //   List.
  // _________________________________________________________________
  // Syntax:
  //   list ::boo::CellViewPrimList_getSynthList <self>
  //
  //   [in] <self> - pointer to the CellViewPrimList manager
  //
  //   [ret] Tcl list of cell names from Synth List
  // _________________________________________________________________  
  // Example:
  //   set splist [::boo::CellViewPrimList_getSynthList $mgr]
  // _________________________________________________________________
  
  

  //. get cell list obj
  //  [ret] .		returns cell list obj
  CellViewCellList& getSynthListObj();
  // Summary:
  //    Returns the CellViewCellList object for the Synth List.
  // _________________________________________________________________
  // Syntax:
  //   CellViewCellList ::boo::CellViewPrimList_getSynthListObj <self>
  //
  //   [in] <self> - pointer to the CellViewPrimList manager
  //
  //   [ret] pointer to CellViewCellList
  // _________________________________________________________________  
  // Example:
  //   set celllist [::boo::CellViewPrimList_getSynthListObj $mgr]
  // _________________________________________________________________
  

private: //.methods
#ifndef CELLVIEW_SWIG
  //. ctor/dtor
  CellViewPrimList();
  ~CellViewPrimList();
#endif

private: //. data
  //. enable flag for the primlist
  bool enable_;

  //. search mode used for isLeafCell
  SearchMode searchMode_;

  //. boo version
  BooVersion ver_;

#ifndef CELLVIEW_SWIG

  //. BooMsg out
  static const std::string CVPL_OUT;

  //. cached sessions
  struct SessionInfo {
  public:
    SessionInfo() : enb_(true), 
                    inv_(false), 
                    secinv_(false),
                    synthinv_(false),
                    m_(CellViewPrimList::REGEX) {;}
    //. assignment operator
    SessionInfo& operator = (const SessionInfo& s) {
      enb_ = s.enb_; inv_ = s.inv_; secinv_ = s.secinv_;
      synthinv_ = s.synthinv_; m_ = s.m_;
      pl_ = s.pl_; sl_ = s.sl_; synthl_ = s.synthl_;
      return *this;
    }
    //. data
    bool enb_, inv_, secinv_, synthinv_;
    CellViewPrimList::SearchMode m_;
    CellViewCellList::PattList pl_, sl_, synthl_;
  };
  
  typedef std::vector<SessionInfo> Sessions;
  Sessions sessions_;

#endif

  //. CellList objs
  CellViewCellList primList_, secList_, synthList_; 

  //. static mgr
  static CellViewPrimList* mgr_;
};


#endif //CELL_VIEW_PRIMLIST_H
