// dui-demo.cpp : main source file
//

#include "stdafx.h"
	
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
	core.UISetInstance(hInstance);
	return core.Run();
}
