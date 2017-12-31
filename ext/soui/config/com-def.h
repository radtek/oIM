//使用DLL/LIB方式编译SOUI组件
#ifndef __COM_DEF_H__
#define __COM_DEF_H__

#ifdef __UIENGINE__	// 在UIEngine中，需要导出
# define DLL_SOUI_COM
#else
# define LIB_SOUI_COM
#endif	// __UIENGINE__

#endif // __COM_DEF_H__