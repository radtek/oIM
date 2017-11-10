#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <wtypes.h>
#include <tchar.h>

//Rename the file
int ReNameFile(LPCTSTR lpszOldName, LPCTSTR lpszNewName);

//Delete the file or Dir
int DelFile(LPCTSTR lpszName);

//Copy File or Dir
int CopyFiles(LPCTSTR lpszSrc, LPCTSTR lpszDest);

//Run Program
BOOL RunProgram(LPCTSTR lpszProPath, LPCTSTR lpszCmdLine, int i32ShowType=SW_SHOW);

BOOL CreateDir(LPCTSTR lpszNewDir);

#include "eIMUpdater/FileUtil.inl"

#endif