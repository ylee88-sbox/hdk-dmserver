//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                      (c) 1996 Intel Corp.                           //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id: DmsMgrIdms.h,v 1.4 1997/08/01 23:49:49 vanco Exp $ //

#ifndef DMS_MGR_IDMS_H
#define DMS_MGR_IDMS_H

#include <Dms/DmsLib.h>
#include <Dms/DmsMgr.h>

// -----------------------------------------------------------------
// --- forward declarations
// -----------------------------------------------------------------
class DmsPath;

//
// =================================================================
// CLASS: DmsMgrIdms
// =================================================================
// CnsMgrIdms
// =================================================================
//
class DmsMgrIdms : public DmsMgr {
  BOO_CAST(DmsMgrIdms, BOO_CAST_BASE(DmsMgr));
public: 
  //  ---
  //+ Member functions
  //  ---

  //. Get the dms file path
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
  DmsMgrIdms();
  //. Constructor
  DmsMgrIdms(DmsMgrIdms* mgr);
  //. Destructor
  virtual ~DmsMgrIdms();

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

#endif // DMS_MGR_IDMS_H


