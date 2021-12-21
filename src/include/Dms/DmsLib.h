//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                  (c) 1997 Intel Corp.                               //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id$

#ifndef DMS_LIB_H
#define DMS_LIB_H

#include <Boo/BooBasics.h>

#ifdef NFL_DLL
#if defined(NFL_DMS)
# define DECL_DMS dllexport
#else
# define DECL_DMS dllimport
#endif
#else
#define DECL_DMS
#endif

#endif // DMS_LIB_H
