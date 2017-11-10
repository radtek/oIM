#include <atlimage.h>  
#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib") 

#include "ImgColorToGray.h"

static CString RenamingFile(const CString& filePath)
{
	if (filePath.GetLength() == 0)
		return _T("");

	int i = filePath.GetLength() - 1;
	int j = 0;
	CString FileName = _T("");
	CString SuffixName = _T("");
	CString NewFilePath = _T("");

	while(filePath[i] != '.' && i > 0)
	{
		if (filePath[i] == '\\' || filePath[i] == '/')
		{
			NewFilePath.Format(_T("%s_gray"),filePath);
			return NewFilePath;
		}

		i--;
	}
	int k = filePath.GetLength()-i;
	FileName = filePath.Mid(0,i);
	SuffixName = filePath.Mid(i,k);

	NewFilePath.Format(_T("%s_gray%s"),FileName,SuffixName);

	return NewFilePath;
}

BOOL BitmapColorToGray(const LPCTSTR srcImgName,const LPCTSTR strGrayImgSavePath)
{
	if (!PathFileExists(srcImgName) || !PathFileExists(strGrayImgSavePath))
		return FALSE;

	CString strFileName;
	strFileName.Empty();
	strFileName = PathFindFileName(srcImgName); 
	//strFileName = RenamingFile(strFileName);

	CString resImgName;
	resImgName = strGrayImgSavePath+strFileName;  //合成保存文件的完整路径

	CImage image1; 
	image1.Load(srcImgName);
	if(image1.IsNull())
		return FALSE;

	HBITMAP hBitMap = image1.Detach(); 
	if (hBitMap == NULL)
	{
		image1.Destroy();
		return FALSE;
	}

	HDC memDC1,memDC2;
	memDC1 = ::CreateCompatibleDC(NULL); 
	if (memDC1 == NULL)
	{
		DeleteObject(hBitMap);
		return FALSE;
	}

	memDC2 = ::CreateCompatibleDC(NULL); 
	if (memDC2 == NULL)
	{

		::DeleteDC(memDC1);  
		DeleteObject(hBitMap);
		return FALSE;
	}

	::SelectObject(memDC1, hBitMap); 

	BITMAP bmpInfo;
	::GetObject(hBitMap,sizeof(BITMAP),&bmpInfo);

	HBITMAP bmp = ::CreateCompatibleBitmap(memDC1,bmpInfo.bmWidth,bmpInfo.bmHeight);
	if (bmp == NULL)
	{
		int error = GetLastError();

		::DeleteDC(memDC1);  
		::DeleteDC(memDC2);  
		DeleteObject(hBitMap);
		return FALSE;
	}
	::SelectObject(memDC2, bmp); 
	::BitBlt(memDC2, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, memDC1, 0, 0, SRCCOPY); 

	DWORD   r,g,b;   
	for (int H =0; H <= bmpInfo.bmHeight; H++)   
	{
		for(int W = 0; W <= bmpInfo.bmWidth; W ++)   
		{   
			r = GetRValue(::GetPixel(memDC2,W,H));   
			g = GetGValue(::GetPixel(memDC2,W,H));   
			b = GetBValue(::GetPixel(memDC2,W,H));     
			r = (r * 3 + g * 6 +  g) / 10;   
			g   =   r;   
			b   =   g; 

			::SetPixel(memDC2,W,H,RGB(r,g,b));
		}
	}


	CImage image2;  
	image2.Attach(bmp);  
	image2.Save(resImgName);
	image2.Detach();  

	::DeleteDC(memDC1);  
	::DeleteDC(memDC2);    
	DeleteObject(hBitMap);
	DeleteObject(bmp);

	return TRUE;
}