//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                  (c) 1997 Intel Corp.                               //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id$

#ifndef CELLVIEW_LIB_H
#define CELLVIEW_LIB_H
#ifdef NFL_DLL
  #if defined(PLATFORM_CELLVIEW)
    #define DECL_CELLVIEW dllexport
  #else
    #define DECL_CELLVIEW dllimport
  #endif
#else
  #define DECL_CELLVIEW
#endif



#include <CellView/CellViewBasics.h>

#endif // BOO_LIB_H
