//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                      (c) 1996 Intel Corp.                           //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id: CellViewDmsMgr.h,v 1.3 1997/08/01 23:49:50 vanco Exp $ //

#ifndef CELLVIEW_DMS_MGR_SIMPLE_H
#define CELLVIEW_DMS_MGR_SIMPLE_H


#include <Dms/DmsMgr.h>
#include <CellView/CellViewLib.h>

// -----------------------------------------------------------------
// --- forward declarations
// -----------------------------------------------------------------
class DmsPath;

//
// =================================================================
// CLASS: CellViewDmsMgr
// =================================================================
// IDMS Simple DMS Manager
// =================================================================
//
class __declspec(DECL_CELLVIEW) CellViewDmsMgr : public DmsMgr {
  BOO_CAST(CellViewDmsMgr, BOO_CAST_BASE(DmsMgr));
public: 
  //  ---
  //+ Member functions
  //  ---

  //. Get the dmsnology file path
  virtual DmsPath& getPath();

  // ---
  //+ Static member functions
  // ---

  //. Init
  static bool init();

protected:
  // ---
  //+ Constructors and destructor
  // ---

  //. Default constructor
  CellViewDmsMgr();
  //. Constructor
  CellViewDmsMgr(CellViewDmsMgr* mgr);
  //. Destructor
  virtual ~CellViewDmsMgr();

  // ---
  //+ Member functions
  // ---

  //. Initializes implementation specific data
  virtual bool init_();

protected: // data
private: // friends
private: // disallowed methods
private: // methods
private: // data
  DmsPath* path_;
};

#endif // DMS_MGR_SIMPLE_H


