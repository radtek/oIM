//ʹ��DLL/LIB��ʽ����SOUI���
#ifndef __COM_DEF_H__
#define __COM_DEF_H__

#ifdef __UIENGINE__	// ��UIEngine�У���Ҫ����
# define DLL_SOUI_COM
#else
# define LIB_SOUI_COM
#endif	// __UIENGINE__

#endif // __COM_DEF_H__