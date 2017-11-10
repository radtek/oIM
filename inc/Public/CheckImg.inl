

//=============================================================================
//Function:     eIMPathFileExistNoBadimg
//Description:	Check a image whether is baded
//Parameter:
//	lpszPath   - Image file path
//  
//Return:
//  TRUE	- OK
//	FALSE	- Bad
//=============================================================================
inline BOOL	eIMPathFileExistNoBadimg(LPCTSTR lpszPath)
{
	struct __stat64	statFile = { 0 };
	if ( lpszPath && lpszPath[0] && ::PathFileExists(lpszPath) &&
		!_tstat64(lpszPath, &statFile) && statFile.st_size > 0)
		return TRUE;

	return FALSE;


	//BOOL bRet = TRUE;
	//CImage image; 
	//image.Load(lpszPath);
	//if(image.IsNull()) 
	//	bRet = FALSE;
	//else
	//{
	//	image.Destroy(); 

	//	FILE* fp = NULL;
	//	fp = _wfopen(lpszPath,_T("rb"));
	//	fseek(fp,-1L,SEEK_END);
	//	char EndChar = fgetc(fp);
	//	fclose(fp);

	//	//JPG image file complete data in hexadecimal D9 end, decimal is -39
	//	//GIF image file complete data in hexadecimal 3B end, decimal is 59
	//	//PNG image ----------------------------------------, decimal is -126
	//	//BMP image ----------------------------------------, decimal is -1
	//	if (EndChar != -39 && EndChar != 59 && EndChar != -126 && EndChar != -1)
	//		bRet = FALSE;
	//	else
	//		bRet = TRUE;	
	//}

	//if (!bRet)
	//{
	//	//	DeleteFile(lpszPath);
	//}

	//return bRet;
}