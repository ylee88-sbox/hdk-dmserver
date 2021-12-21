//////////////// Intel Corporation Proprietary Information. /////////////
//                                                                     //
// This listing is supplied under the terms of a  license  agreement   //
// with Intel Corporation and may not be copied nor disclosed except   //
// in accordance with the terms of the agreement.                      //
//                  (c) 1995 - 1996 Intel Corp.                        //
//////////////// Intel Corporation Proprietary Information. /////////////
//
// $Id: DmsTemplatesSimple.cpp,v 1.3 1997/11/10 19:51:08 ychee Exp $ //

#ifndef DMS_TEMPLATES_CPP
#define DMS_TEMPLATES_CPP

// We only need the rest of this stuff for the old HP CC
#if defined(OH_MACH_HPPA_10_01)

#define BOO_INSTANTIATE_TEMPLATE 1
#define OS_NO_AUTO_INSTANTIATE 1

// Instantiate the following templates if using shared libraries
#ifdef SHLIB

#include <map>
#include <list>
#include <Boo/BooBasics.h>

BOO_TEMPLATE_INST((std::list<std::string>));
BOO_TEMPLATE_INST((std::list<std::string>::iterator));
BOO_TEMPLATE_INST((std::list<std::string>::const_iterator));

BOO_TEMPLATE_INST((std::map<std::string, std::string,std::less<std::string> >));
BOO_TEMPLATE_INST((std::map<std::string,std::list<std::string>,std::less<std::string> >));

template <class T> class _dummy1 {
 std::string s_;
 std::string::iterator d1_;
 std::string::const_iterator d2_;
};

// string
BOO_TEMPLATE_INST((_dummy1<void>));

#endif

#undef OS_NO_AUTO_INSTANTIATE
#undef BOO_INSTANTIATE_TEMPLATE

#endif // OH_MACH_HPPA_10_01

#endif // DMS_TEMPLATES_CPP

