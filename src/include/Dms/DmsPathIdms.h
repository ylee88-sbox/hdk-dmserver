//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                      (c) 1996 Intel Corp.                           //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id: DmsPathIdms.h,v 1.6 1997/08/01 23:49:51 vanco Exp $ //

#ifndef DMS_PATH_IDMS_H
#define DMS_PATH_IDMS_H

#include <stl.h>
#include <Boo/BooBasics.h>
#include <Dms/DmsPath.h>

//
// =================================================================
// CLASS: DmsPathIdms
// =================================================================
// DmsPathIdms provides an interface to the iDMS search path services.
//
// =================================================================
//

class DmsPathIdms : public DmsPath {
  BOO_CAST(DmsPathIdms, BOO_CAST_BASE(DmsPath));
public:
  friend class DmsMgrIdms;
  
  //. Get the path for a particular name/view pair
  virtual bool getPath(const std::string& name, const std::string& view,
                       const std::string& hint,std::string& path) const;

  //. Dummy 
  virtual bool getDirs(const std::string& s1,std::string& s2) const;
 
  //. Dummy procedure. Does not do anything 
  virtual bool setDirs(const std::string& view, const std::string& exts,
                       const std::string& pathVar, const std::string& dirs);
                       
protected:
  // ---
  //+ Constructors and destructor
  // ---

  //. Default constructor
  DmsPathIdms();
  //. Constructor
  DmsPathIdms(DmsPathIdms* path);
  //. Destructor
  virtual ~DmsPathIdms();

  // ---
  //+ Member functions
  // ---

  //. Initializes implementation specific data
  virtual bool init_();

private: // disallowed methods
private: // methods
private: // data
};

#endif // DMS_PATH_IDMS_H
