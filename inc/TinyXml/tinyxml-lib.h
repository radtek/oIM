// zlib is only a helper for link zlib*.lib by command line
// You only include this file to use zlib 

#ifndef GTEST_INCLUDE_TINYXML_LIB_H_
#define GTEST_INCLUDE_TINYXML_LIB_H_

#include "tinyxml.h"

#ifndef NO_USE_TINYXML_LIB
#  if defined(_DEBUG) || defined(DEBUG)
#    pragma comment(lib, "TinyXmlSTLd.lib")
#  else
#    pragma comment(lib, "TinyXmlSTL.lib")
#  endif
#endif	// #ifndef NO_USE_TINYXML_LIB


#endif  // GTEST_INCLUDE_TINYXML_LIB_H_
