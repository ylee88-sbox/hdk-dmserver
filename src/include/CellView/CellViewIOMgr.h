/////////////// Intel Corporation Proprietary Information. //////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                  (c) 2000 - 2000 Intel Corp.                        //
//////////////// Intel Corporation Proprietary Information. /////////////

#ifndef CELLVIEW_IO_MGR_H
#define CELLVIEW_IO_MGR_H
#include <CellView/CellViewBasics.h>

#include <Baa/BaaCellViewInfo.h> // to be removed after ww09
#include <Baa/BaaCellViewReader.h>
#include <Baa/BaaCellViewWriter.h>

// =================================================================
// CLASS: CellViewIOMgr
// =================================================================
// (class summary here)
// =================================================================
//
class CellViewIOMgr : public BaaCellViewReader, public BaaCellViewWriter {

  //.
  bool readCellView (BaaCellViewInfo&info);
  //.
  bool writeCellView(BaaCellViewInfo&info);

  //.
  bool readCellView(const std::string& cellName,
                    const std::string& viewName,
                    const std::string& versionName,
                    const std::string& fullpath,
                    const BaaCellViewInfo& info) {
      return false;
      }

  //.
  bool writeCellView(const std::string& cellName,
                     const std::string& viewName,
                     const std::string& versionName,
                     const std::string& fullpath,
                     const BaaCellViewInfo& info)
                     {

                     return false;
                     }
    
  const std::string& getName() const;
};

#endif // CELLVIEW_IO_MGR_H
