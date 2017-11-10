// zlib is only a helper for link zlib*.lib by command line
// You only include this file to use zlib 

#ifndef GTEST_INCLUDE_ZLIB_LIB_H_
#define GTEST_INCLUDE_ZLIB_LIB_H_

#include "zconf.h"
#include "zlib.h"

#ifndef NO_USE_ZLIB_LIB
#  if defined(_UNICODE) || defined(UNICODE)
#    if defined(_DEBUG) || defined(DEBUG)
#      pragma comment(lib, "zlibud.lib")
#    else
#      pragma comment(lib, "zlibu.lib")
#    endif
#  else
#    if defined(_DEBUG) || defined(DEBUG)
#      pragma comment(lib, "zlibd.lib")
#    else
#      pragma comment(lib, "zlib.lib")
#    endif
#  endif
#endif


#endif  // GTEST_INCLUDE_ZLIB_LIB_H_
