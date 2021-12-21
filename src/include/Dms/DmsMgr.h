//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                      (c) 1995 Intel Corp.                           //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id: DmsMgr.h,v 1.3 1997/06/06 01:34:17 ychee Exp $ //

#ifndef DMS_MGR_H
#define DMS_MGR_H

#include <Boo/BooBasics.h>
#include <Dms/DmsLib.h>

// -----------------------------------------------------------------
// --- forward declarations
// -----------------------------------------------------------------
class DmsPath;

//
// =================================================================
// CLASS: DmsMgr
// =================================================================
// Provides service to initialize the classes which are associated
// with the DMS path services
//
// OVERVIEW
//   DmsMgr is an implementation as well as interface class.
//
//   DmsMgr (or its derived class) is a static object and will be
// destroyed when the application exits.
//
// EXAMPLE
//   The following example will correctly initialize the DMS code:
//
//  EXAMPLE
//   #include <Dms/DmsMgr.h>
//   void main() {
//     DmsMgr::init();  // initialize DMS
//
//     // application stuff start here
//
//   }
//  END
//
// USAGE
//
// To use the search path services, the user should instantiate an object
// of a class derived from DmsPath.  The derived class should provide a
// "path" function.
//
// =================================================================
//
class __declspec(DECL_DMS) DmsMgr {
  BOO_CAST(DmsMgr, BOO_CAST_NONE);
public: 
  //  ---
  //+ Member functions
  //  ---

  //. Get the dmsnology file path
  virtual DmsPath& getPath() = 0;

  // ---
  //+ Static member functions
  // ---

  //. Initialize DmsMgr.  This is defined in a derived implementation
  //  class
  static bool init();
  
  //. Return the shared DmsMgr object.
  //  [warn] . Return value could be invalid - '0'
  static DmsMgr& getMgr();

protected:
  // ---
  //+ Constructors and destructor
  // ---

  //. Default constructor
  DmsMgr();
  //. Constructor
  DmsMgr(DmsMgr* mgr);
  //. Destructor
  virtual ~DmsMgr();

  // ---
  //+ Member functions
  // ---

  //. Initializes implementation specific data
  virtual bool init_() = 0;

protected: // data
  static DmsMgr* mgr_;

private: // friends
private: // disallowed methods
private: // methods
private: // data
};

#endif // DMS_MGR_H

