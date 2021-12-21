//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                      (c) 1996 Intel Corp.                           //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id: CellViewDmsPath.h,v 1.7 1997/11/10 19:50:47 ychee Exp $ //

#ifndef CELL_VIEW_DMS_PATH_H
#define CELL_VIEW_DMS_PATH_H

#include <list>
#include <map>
#include <Boo/BooBasics.h>
#include <Dms/DmsPathSimple.h>
#include <CellView/CellViewLib.h>
//
// =================================================================
// CLASS: CellViewCellViewDmsPath
// =================================================================
// iDMS search path service
// 
// OVERVIEW
//
// CellViewDmsPath provides a search path service which
// uses iDMS and the RW paths to locate databases of
// different file types.
//
// =================================================================
//

class __declspec(DECL_CELLVIEW) CellViewDmsPath : public DmsPathSimple {
  BOO_CAST(CellViewDmsPath, BOO_CAST_BASE(DmsPath));
public:
  // ---
  //+ Member functions
  // ---

  //. Get the path for a particular name/view pair
  //  [in] name     The name of the cell or file without extension
  //  [in] view     The desired cell or file view (e.g. "alf", "sch", "tech"
  //  [in] hint     A specific hint path that can be used to override
  //                the defined searchpaths - i.e. if the file is found in
  //                the hint directory, that path will be returned
  //  [out] path    The pathname to the file
  //  [return]      FALSE is returned if the cell/view was not found anywhere
  virtual bool getPath(const std::string& name, const std::string& view,
                       const std::string& hint, std::string& path) const;

  // ---
  //+ Constructors and destructor
  // ---

  //. Default constructor
  CellViewDmsPath();
  //. Constructor
  CellViewDmsPath(CellViewDmsPath* path);
  //. Destructor
  virtual ~CellViewDmsPath();

  // ---
  //+ Member functions
  // ---

  //. Initializes implementation specific data
  virtual bool init_();

  virtual bool findViewFromPath(const std::string& fullPath, 
                                  std::string& cellName,std::string& viewType,const std::string&desiredView="");

private: // friends
  friend class CellViewDmsMgr;

private: // disallowed methods
  
private: // methods
  bool  fileExists_(const std::string& fullpath) const  ;
private: // data
  };

#endif // DMS_PATH_SIMPLE_H
