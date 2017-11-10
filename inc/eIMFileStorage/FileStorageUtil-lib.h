// eIMFileStorage is only a helper for link eIMFileStorage*.lib by command line
// You only include this file to use eIMFileStorage 

#ifndef EIM_FILE_STORAGE_LIB_H_
#define EIM_FILE_STORAGE_LIB_H_

#include "FileStorageUtil.h"

#ifndef NO_FILE_STORAGE_LIB
#  if defined(_DEBUG) || defined(DEBUG)
#    pragma comment(lib, "eIMFileStoraged.lib")
#  else
#    pragma comment(lib, "eIMFileStorage.lib")
#  endif
#endif


#endif  // EIM_FILE_STORAGE_LIB_H_
