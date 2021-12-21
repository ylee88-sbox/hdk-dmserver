//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                    (c) 1996 - 1998 Intel Corp.                      //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id$ //

#ifndef DMS_PATH_H
#define DMS_PATH_H

#include <Boo/BooBasics.h>
#include <Boo/BooIterT.h>
#include <Dms/DmsLib.h>

//
// =================================================================
// CLASS: DmsPath
// =================================================================
// Provides search path functionality
// 
// OVERVIEW
//
// DmsPath is an abstract base class which presents search path
// functionality. 
//
// =================================================================
//

class __declspec(DECL_DMS) DmsPath {
  BOO_CAST(DmsPath, BOO_CAST_NONE);
public: // static

  //. These constants define some commonly used view names.  The default
  //  extensions for these paths are not included as constants because
  //  they can be accessed through the getExts function.
  //  Note: The LNETCVS and SNETCVS views refer to "ln" and "sn" files,
  //  respectively.
  static const std::string ALF_VIEW_NAME;
  static const std::string ALFLAY_VIEW_NAME;
  static const std::string ALFPLN_VIEW_NAME;
  static const std::string BITMAP_VIEW_NAME;
  static const std::string LNETCVS_VIEW_NAME;
  static const std::string LNF_VIEW_NAME;
  static const std::string NTCL_VIEW_NAME;
  static const std::string SCH_VIEW_NAME;
  static const std::string SNETCVS_VIEW_NAME;
  static const std::string TECH_VIEW_NAME;
  static const std::string DEF_VIEW_NAME;
  static const std::string LEF_VIEW_NAME;
  static const std::string BODY_VIEW_NAME;

  //. These constants define separator characters (which may
  //  vary depending on the OS platform)
  static const char DMS_EXTS_SEPARATOR;
  static const char DMS_PATH_SEPARATOR;
  static const char DMS_DIR_SEPARATOR;

  //. These constants define the names of environment variables
  //  that are used to set initial values for search paths
  static const std::string ALF_ENV_NAME;
  static const std::string BITMAP_ENV_NAME;
  static const std::string LNETCVS_ENV_NAME;
  static const std::string LNF_ENV_NAME;
  static const std::string NTCL_ENV_NAME;
  static const std::string SCH_ENV_NAME;
  static const std::string SNETCVS_ENV_NAME;
  static const std::string TECH_ENV_NAME;
  static const std::string DEF_ENV_NAME;
  static const std::string LEF_ENV_NAME;
  static const std::string BODY_ENV_NAME;

  //. This constant defines the name of the current directory
  //  (which may vary depending on the OS platform)
  static const std::string DMS_CURRENT_DIR_NAME;

  //. This constant defines the name that should be used to
  //   specify an "empty" extension
  static const std::string DMS_NULL_EXT_NAME;

  //. Get the path for a particular name/view pair
  //  [in] name     The name of the cell or file without extension
  //  [in] view     The desired cell or file view (e.g. "alf", "sch", "tech"
  //  [in] hint     A specific hint path that can be used to override
  //                the defined searchpaths - i.e. if the file is found in
  //                the hint directory, that path will be returned
  //  [out] path    The pathname to the file
  //  [return]      FALSE is returned if the cell/view was not found anywhere
  virtual bool getPath(const std::string& name, const std::string& view,
                       const std::string& hint,std::string& path) const = 0;

  //. Get views that have been defined (have associated extensions).
  //  The associated paths may still be empty.
  //  [out] iter    Iterator over view names
  virtual void getViews(BooIterT<std::string>& iter) const = 0;

  //. Get the directories for a particular view
  //  [in] view     A view that has been defined for this object - e.g. "alf"
  //  [out] dirs    A set of directory names separated by the separator
  //                character which indicates where 'this' will search for
  //                files
  //  [return]      FALSE is returned if the view is not defined
  virtual bool getDirs(const std::string& view,std::string& dirs) const = 0;

  //. Get the extensions for a particular view
  //  [in] view     A view that has been defined for this object - e.g. "alf"
  //  [out] dirs    A set of directory names separated by the extension
  //                separator character
  //  [return]      FALSE is returned if the view is not defined
  virtual bool getExts(const std::string& view,std::string& exts) const = 0;

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
                       const std::string& pathVar, const std::string& dirs = "") = 0;

  //. Add a single search path for a particular view
  //  [in] view     A view that will be or has been defined for this
  //                object - e.g. "alf"
  //  [in] dir      A single directory name to add to the search path
  //  [in] prepend  If TRUE, the new path is added to the front of the
  //                existing search paths; otherwise it is appended to the end
  //  [return]      Returns TRUE
  virtual bool addDir(const std::string& view, const std::string& dir,
                      bool prepend = FALSE) = 0;
  
  //. Remove a single search path for a particular view
  //  [in] view     A view that will be or has been defined for this
  //                object - e.g. "alf"
  //  [in] dir      A single directory name to add to the search path
  //  [return]      Returns TRUE
  virtual bool remDir(const std::string& view, const std::string& dir) = 0;
  
  //. Add a single extension for a particular view
  //  [in] view     A view that will be or has been defined for this
  //                object - e.g. "alf"
  //  [in] ext      A single extension name to add to the list of extensions
  //                for the view
  //  [return]      Returns TRUE
  virtual bool addExt(const std::string& view, const std::string& ext) = 0;

  //. Clear all directories in oriMap(paths in dms.pth file) for a particular view
  //  [in] view     A view that will be or has been defined for this
  //                object - e.g. "alf"
  //  [return]      Returns TRUE
  virtual bool clearDirs(const std::string& view) = 0;

  //. Add a single search path for a particular view in oriMap (path from dms.pth file).
  //  [in] view     A view that will be or has been defined for this
  //                object - e.g. "alf"
  //  [in] dir      A single directory name to add to the search path
  //  [in] prepend  If TRUE, the new path is added to the front of the
  //                existing search paths; otherwise it is appended to the end
  //  [return]      Returns TRUE
  virtual bool addOri(const std::string& view, const std::string& dir,
                      bool prepend = FALSE) = 0;

  //. Get the directories in both dirMap(env. variables) and oriMap(dms.pth files) for a particular view
  //  [in] view     A view that has been defined for this object - e.g. "alf"
  //  [out] dirs    A set of directory names separated by the path separator
  //                character which indicates where 'this' will search for
  //                files
  //  [return]      FALSE is returned if the view is not defined
  virtual bool getDirsOrig(const std::string& view,std::string& dirs) const = 0;
 
protected:
  // ---
  //+ Constructors and destructor
  // ---

  //. Default constructor
  DmsPath();
  //. Constructor
  DmsPath(DmsPath* path);
  //. Destructor
  virtual ~DmsPath();

  friend class DmsMgrSimple;
  
  // ---
  //+ Member functions
  // ---

  //. Initializes implementation specific data
  virtual bool init_() = 0;

private: // disallowed methods
private: // methods
private: // data
};

#endif // DMS_PATH_H
