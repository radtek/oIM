//使用DLL/LIB编译SOUI,UTILITES and COLL-MEM模块
#ifndef __CORE_DEF_H__
#define __CORE_DEF_H__

#ifdef __UIENGINE__	// 在UIEngine中，需要导出
# define DLL_CORE
#else
# define LIB_ALL
#endif // __UIENGINE__

#endif // __CORE_DEF_H__