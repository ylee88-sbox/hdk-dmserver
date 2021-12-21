//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                      (c) 1996 Intel Corp.                           //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id: DmsPathSimple.h,v 1.7 1997/11/10 19:50:47 ychee Exp $ //

#ifndef DMS_PATH_SIMPLE_H
#define DMS_PATH_SIMPLE_H

#include <list>
#include <map>
#include <Boo/BooBasics.h>
#include <Dms/DmsSimpleLib.h>
#include <Dms/DmsPath.h>

//
// =================================================================
// CLASS: DmsPathSimple
// =================================================================
// simplistic search path service
// 
// OVERVIEW
//
// DmsPathSimple provides a simplistic search path service which
// uses environment variables with colon-separated paths to find
// different file types.
//
// =================================================================
//

class __declspec(DECL_DMS_SIMPLE) DmsPathSimple : public DmsPath {
  BOO_CAST(DmsPathSimple, BOO_CAST_BASE(DmsPath));
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
                       const std::string& hint,std::string& path) const;

  //. Get views that have been defined (have associated extensions).
  //  The associated paths may still be empty.
  //  [out] iter    Iterator over view names
  virtual void getViews(BooIterT<std::string>& iter) const;
  
  //. Get the directories for a particular view
  //  [in] view     A view that has been defined for this object - e.g. "alf"
  //  [out] dirs    A set of directory names separated by the path separator
  //                character which indicates where 'this' will search for
  //                files
  //  [return]      FALSE is returned if the view is not defined
  virtual bool getDirs(const std::string& view,std::string& dirs) const;
  
  //. Get the extensions for a particular view
  //  [in] view     A view that has been defined for this object - e.g. "alf"
  //  [out] dirs    A set of directory names separated by the extension
  //                separator character
  //  [return]      FALSE is returned if the view is not defined
  virtual bool getExts(const std::string& view,std::string& exts) const;

  //. Set the search paths programmatically for a particular view
  //  [in] view     A view that will be or has been defined for this
  //                object - e.g. "alf"
  //  [in] exts     Extensions associated with this view; if an empty
  //                string is specified, then any existing extensions will
  //                be kept; otherwise existing extensions will be
  //                overridden.  Extensions will be set even if no
  //                directories are specified.
  //  [in] pathVar  An optional envrionment variable name which can be
  //                used to set directories - the value of the environment
  //                variable will be added to any directories specified with
  //                dirs.  Set pathVar to "" if you want the paths to come
  //                solely from the dirs argument
  //  [in] dirs     A set of directory names separated by the separator
  //                character which indicates where 'this' will search for
  //                files.  If any directories are specified, they will be
  //                appended to those specified from the pathVar (which can
  //                be a null string if you don't want to get any paths
  //                from the environment)
  //  [return]      Returns TRUE
  virtual bool setDirs(const std::string& view, const std::string& exts,
                       const std::string& pathVar, const std::string& dirs = "");

  //. Add a single search path for a particular view
  //  [in] view     A view that will be or has been defined for this
  //                object - e.g. "alf"
  //  [in] dir      A single directory name to add to the search path
  //  [in] prepend  If TRUE, the new path is added to the front of the
  //                existing search paths; otherwise it is appended to the end
  //  [return]      Returns TRUE
  virtual bool addDir(const std::string& view, const std::string& dir,
                      bool prepend = FALSE);
  //. Remove a single search path for a particular view
  //  [in] view     A view that will be or has been defined for this
  //                object - e.g. "alf"
  //  [in] dir      A single directory name to add to the search path
  //  [return]      Returns TRUE
  virtual bool remDir(const std::string& view, const std::string& dir);
  
  
  //. Add a single extension for a particular view
  //  [in] view     A view that will be or has been defined for this
  //                object - e.g. "alf"
  //  [in] dir      A single extension name to add to the list of extensions
  //                for the view
  //  [return]      Returns TRUE
  virtual bool addExt(const std::string& view, const std::string& dir);

  //. Clear all directories in oriMap(paths in dms.pth file) for a particular view 
  //  [in] view     A view that will be or has been defined for this
  //                object - e.g. "alf"
  //  [return]      Returns TRUE
  virtual bool clearDirs(const std::string& view);

  //. Add a single search path for a particular view in oriMap (path from dms.pth file).
  //  [in] view     A view that will be or has been defined for this
  //                object - e.g. "alf"
  //  [in] dir      A single directory name to add to the search path
  //  [in] prepend  If TRUE, the new path is added to the front of the
  //                existing search paths; otherwise it is appended to the end
  //  [return]      Returns TRUE
  virtual bool addOri(const std::string& view, const std::string& dir,
                      bool prepend = FALSE);

  //. Get the directories in both dirMap(env. variables) and oriMap(dms.pth files) for a particular view
  //  [in] view     A view that has been defined for this object - e.g. "alf"
  //  [out] dirs    A set of directory names separated by the path separator
  //                character which indicates where 'this' will search for
  //                files
  //  [return]      FALSE is returned if the view is not defined
  virtual bool getDirsOrig(const std::string& view,std::string& dirs) const;

  // ---
  //+ Constructors and destructor
  // ---

  //. Default constructor
  DmsPathSimple();
  //. Constructor
  DmsPathSimple(DmsPathSimple* path);
  //. Destructor
  virtual ~DmsPathSimple();

  // ---
  //+ Member functions
  // ---

  //. Initializes implementation specific data
  virtual bool init_();

private: // friends
  friend class DmsMgrSimple;

private: // disallowed methods
  
private: // methods
  bool fileExists_(const std::string& fullpath) const;
  bool getExts_(const std::string& view,std::list<std::string>& exts) const;
  bool getDirs_(const std::string& view,std::list<std::string>& dirs) const;
  bool split_(const std::string& path, char splitchar,std::list<std::string>& parts) const;
  bool getOrig_(const std::string& view,std::list<std::string>& dirs) const;
  
private: // data
  typedef std::map<std::string,std::list<std::string>,std::less<std::string> > stringListMap;

  stringListMap dirMap_; // dirs added by environment variables. 
  stringListMap extMap_; // exts added for each view.
  stringListMap oriMap_; // dirs added by dmspath file. 
};

#endif // DMS_PATH_SIMPLE_H
