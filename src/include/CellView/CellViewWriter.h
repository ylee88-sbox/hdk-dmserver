/////////////// Intel Corporation Proprietary Information. //////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                  (c) 2000 - 2000 Intel Corp.                        //
//////////////// Intel Corporation Proprietary Information. /////////////

#ifndef CELLVIEW_WRITER_H
#define CELLVIEW_WRITER_H

#include <CellView/CellViewBasics.h>

//
// =================================================================
// CLASS: CellViewWriter
// =================================================================
// (class summary here)
// =================================================================
//
class __declspec(dllexport) CellViewWriter {

public:

  //.
  virtual bool writeCellView(const std::string& cellName,
                             const std::string& viewName,
                             const std::string& versionName,
                             const std::string& fullpath)=0;
  //.
  virtual const std::string& getName()=0;
};

#endif // CELLVIEW_WRITER_H
