#ifndef __IEIMZIP_2013_11_05_BY_HDP__
#define __IEIMZIP_2013_11_05_BY_HDP__


#include <windows.h>
#include <tchar.h>

#define INAME_EIMZIP		_T("SXIT.EIMZip")	// eIMZip interface name

/////////////////////////错误定义///////////////

#define IMZIP_OK										(0)
#define IMZIP_ERROR										(-1)

#define IMUZIP_OK										(0)
#define IMUZIP_ERROR									(-1)

#define IMZIP_ZIP_OPEN_OK								0x00


#define IMZIP_FILE_NULL									0x100
#define IMZIP_ZIPFILE_NULL								0x101
#define IMZIP_FILE_OPEN_ERROR							0x102
#define IMZIP_ZIP_FILE_OPEN_ERROR						0x103
#define IMZIP_ALLOC_ERROR								0x104
#define IMZIP_ZIP_CLOSE_IN_ERROR						0x105
#define IMZIP_FUN_COMPRESSION_PARAMETER_2_3_ERROR		0x106
#define	IMZIP_FUN_COMPRESSION_PARAMETER_2_ERROR			0x107



#define	IMUZIP_MAKE_DIR_ERROR							0x200
#define	IMUZIP_HANDLE_HAVE_OPEN							0x201
#define IMUZIP_CLOSE_ERROR								0x202
#define	IMUZIP_NO_GET_CURRENT_FILE_INFO					0x203
#define IMUZIP_ALLOC_ERROR								0x204
#define	IMUZIP_DECODE_ERROR								0x205
#define IMUZIP_OPEN_FILE_ERROR							0x206
#define	IMUZIP_READ_CURRENT_FILE_ERROR					0x207
#define	IMUZIP_WRITE_FILE_ERROR							0x208
#define	IMUZIP_CLOSE_UZIP_ERROR							0x209
#define IMUZIP_GET_ZIP_INFO_ERROR						0x210


#define IMZIP_READ_FILE_ERROR							(-100)
#define IMZIP_WRITE_ZIP_ERRPR							(-101)
#define IMZIP_COMPRESSING_FILE_OPEN_ERROR				(-102)


#define IMZIP_ZIP_HANDLE_HAVE_OPEN						0x300
#define IMZIP_UNZIP_HANDLE_HAVE_OPEN					0x301


#define IMZIP_FILE_HAVE_EXIST							0x05


//////////////////////////开关定义//////////////


//////////////////////////宏变量定义///////////////

#define IMZIP_MAX_WRITE_BUFFER_SIZE						(16*1024)
#define	IMZIP_MAXPATHNAME								256

typedef struct tagZipState
{
	//unsigned int u32FileTotalSize;			//文件总体大小
	//unsigned int u32ZipFileTotalSize;			//Zip文件的总体大小
	//unsigned int u32FileSizeCompressing;		//压缩，或解压中文件大小变化
	//unsigned int u32ZipFileCompressing;		//压缩，或解压中Zip文件大小变化
	//unsigned int u32Speed;					//压缩速率KB为单位
	//unsigned int u32Compressibility;			//压缩率
	char		 szFileName[522];				//当前压缩解压的文件名称
	//unsigned int u32Time;						//压缩解压处理的时间用秒为单位
}S_ZipState,*PS_ZipState;

enum E_WRITE_ZIP_MODE
{
	COVER_BEFORE_ZIP = 1,						//压缩时覆盖之前的zip
	APPEND_TO_ZIP = 2							//压缩时追加到给定zip
};
enum E_DECOMPRESSION_FILE_MODE
{
	COVER_BEFORE_FILE		= 0,				//解压后覆盖之前的文件
	NO_COVER_BEFORE_FILE	= 1					//如果有不做覆盖
};

typedef int (__stdcall *PFN_ZipStateCallback)(S_ZipState *psZipState);

class __declspec(novtable) I_eIMZip : public I_EIMUnknown
{
public:
	/*Return : right IMZIP_OK , other IMZIP_ERROR*/
	virtual int Compression(
		const TCHAR *pszZipPath ,										/*path of zip file*/
		int			i32FilePathCount,									/*the number of path,0 indicates dir*/
		const TCHAR *pszFileList ,										/*each path ends in '\0',all ends in '\0\0'*/
		E_WRITE_ZIP_MODE	i32WriteType,								/* 2 indicate append , other over*/
		const char *pszPassword,										/*@#￥%……&*GHJasdK0909 aso*/
		PFN_ZipStateCallback pfnZipStateCallback						//回调函数返回压缩过程中状态信息
		) = 0;


	/*Return : right IMZIP_OK , other IMZIP_ERROR*/
	virtual int Decompression(
		const TCHAR  *pszZipPath,										/*path of zip file*/
		const TCHAR  *pszDir,											/*The destination of decompression file*/

		E_DECOMPRESSION_FILE_MODE	i32FileMode,						/*0 indicate cover , 1 indicate no cover exist ,
																		return IMZIP_FILE_HAVE_EXIST if file have existed*/

		const char  *pszPassWord,										/*@#￥%……&*GHJasdK0909 aso*/
		PFN_ZipStateCallback pfnZipStateCallback						//回调函数返回解压过程中状态信息
		) = 0;
	

	virtual int MemCompression(
		const char *pi8MemBuf,											//Buf will be Compressed in Mem
		int i32MemBufLen,												//Lenth of MemBuf
		char **pi8MemCompressed,										//A pointer to be compressed Mem
		int *i32MemCompressedLenth,										//the lenth of compressed Mem
		const char *pszPassWord,										//
		PFN_ZipStateCallback pfnZipStateCallback						//
		) = 0;



	virtual int MemDecompression(
		const char *pi8MemBuf,											//
		int i32MemBufLen,												//
		char **pi8MemDecompressed,										//
		int *i32MemDecompressedLenth,									//
		const char *pszPassWord,										//
		PFN_ZipStateCallback pfnZipStateCallback						//
		) = 0;

};



#endif //__ICLIENT_AGENT_2013_09_06_BY_YFGZ__