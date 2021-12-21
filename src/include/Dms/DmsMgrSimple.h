//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                      (c) 1996 Intel Corp.                           //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id: DmsMgrSimple.h,v 1.3 1997/08/01 23:49:50 vanco Exp $ //

#ifndef DMS_MGR_SIMPLE_H
#define DMS_MGR_SIMPLE_H

#include <Dms/DmsSimpleLib.h>
#include <Dms/DmsMgr.h>

// -----------------------------------------------------------------
// --- forward declarations
// -----------------------------------------------------------------
class DmsPath;

//
// =================================================================
// CLASS: DmsMgrSimple
// =================================================================
// Simple DMS Manager
// =================================================================
//
class __declspec(DECL_DMS_SIMPLE) DmsMgrSimple : public DmsMgr {
  BOO_CAST(DmsMgrSimple, BOO_CAST_BASE(DmsMgr));
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
  DmsMgrSimple();
  //. Constructor
  DmsMgrSimple(DmsMgrSimple* mgr);
  //. Destructor
  virtual ~DmsMgrSimple();

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


