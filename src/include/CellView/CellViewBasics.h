//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                  (c) 1998 Intel Corp.                               //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id: CifBasics.h,v 0.1 1998/01/19 06:34:07 jdelcan Exp $ //


#ifndef CELLVIEW_BASICS_H
#define CELLVIEW_BASICS_H
#include <Boo/BooBasics.h>
//using namespace std;

#include <CellView/CellViewLib.h>
#ifdef CELLVIEW_SWIG

__declspec(DECL_CELLVIEW) extern  std::string CellViewLibraryDefault;
__declspec(DECL_CELLVIEW) extern  std::string CellViewVersionDefault;
#else
__declspec(DECL_CELLVIEW) extern const std::string CellViewLibraryDefault;
__declspec(DECL_CELLVIEW) extern const std::string CellViewVersionDefault;

__declspec(DECL_CELLVIEW) extern const std::string SCHEMATIC_VIEW_NAME;
__declspec(DECL_CELLVIEW) extern const std::string SCHEMATIC_VIEW_EXTS;
__declspec(DECL_CELLVIEW) extern const std::string CELL_VIEW_SCHEMATIC;



__declspec(DECL_CELLVIEW) extern const std::string GCL_STROKE_VIEW_NAME;
__declspec(DECL_CELLVIEW) extern const std::string GCL_ENV_NAME;
__declspec(DECL_CELLVIEW) extern const std::string GCL_STROKE_EXT;


__declspec(DECL_CELLVIEW) extern const std::string  DEPTH_OPTION_NAME;

#endif
#endif 
