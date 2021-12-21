//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                  (c) 1998 Intel Corp.                               //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id: LnfNetEntries.h,v 0.1 1998/01/19 06:34:07 jdelcan Exp $ //


#ifndef CELL_VIEW_RW_MGR_H
#define CELL_VIEW_RW_MGR_H

#include <CellView/CellViewLib.h>
#include <Baa/BaaCellViewReader.h>
#include <Baa/BaaCellViewWriter.h>
#include <map>

class CellViewMgr;
//
// =================================================================
// CLASS: CellViewRWMgr
//  Persistent representation for netlist data
// OVERVIEW
// =================================================================
//


class __declspec(DECL_CELLVIEW) CellViewRWMgr {
public:
  CellViewRWMgr();
  virtual ~CellViewRWMgr();
  static bool init();
  static CellViewRWMgr& getMgr();
  
  

    //. Get the current CellView reader
  virtual BaaCellViewReader* getReader();

  //. Get the current CellView writer
  virtual BaaCellViewWriter* getWriter();

  // The following functions are included in the interface to provide
  // a mechanism for future expansion (allowing multiple CellView readers
  // and writers to co-exist in a single invocation) - they are currently
  // not implemented

  //. Add an CellView reader to the list of CellView readers managed by this class
  virtual bool addReader(const std::string& name,BaaCellViewReader& cvReader,bool replace=false);

  //. Remove an CellView reader from the list of CellView readers managed by this class
  virtual BaaCellViewReader* remReader(const std::string& name,bool&res);

  //. Set the current CellView reader
  virtual bool setReader(const std::string& name);

  
  //. Add an CellView writer to the list of CellView writers managed by this class
  virtual bool addWriter(const std::string& name,BaaCellViewWriter& cvWriter,bool replace=false);

  //. Remove an CellView writer from the list of CellView writers managed by this class
  virtual BaaCellViewWriter* remWriter(const std::string& name,bool&res);

  //. Set the current CellView writer
  virtual bool setWriter(const std::string& name);


  BaaCellViewReader* getReader(const std::string &  name);

  BaaCellViewWriter* getWriter(const std::string& name);
  

protected:
  std::map<std::string,BaaCellViewReader*,std::less<std::string> > readers_;
  std::map<std::string,BaaCellViewWriter*,std::less<std::string> > writers_;
  BaaCellViewReader* reader_;
  BaaCellViewWriter* writer_;
  static CellViewRWMgr *mgr_;
  friend class CellViewMgr;
};

#endif


