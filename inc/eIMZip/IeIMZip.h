#ifndef __IEIMZIP_2013_11_05_BY_HDP__
#define __IEIMZIP_2013_11_05_BY_HDP__


#include <windows.h>
#include <tchar.h>

#define INAME_EIMZIP		_T("SXIT.EIMZip")	// eIMZip interface name

/////////////////////////������///////////////

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


//////////////////////////���ض���//////////////


//////////////////////////���������///////////////

#define IMZIP_MAX_WRITE_BUFFER_SIZE						(16*1024)
#define	IMZIP_MAXPATHNAME								256

typedef struct tagZipState
{
	//unsigned int u32FileTotalSize;			//�ļ������С
	//unsigned int u32ZipFileTotalSize;			//Zip�ļ��������С
	//unsigned int u32FileSizeCompressing;		//ѹ�������ѹ���ļ���С�仯
	//unsigned int u32ZipFileCompressing;		//ѹ�������ѹ��Zip�ļ���С�仯
	//unsigned int u32Speed;					//ѹ������KBΪ��λ
	//unsigned int u32Compressibility;			//ѹ����
	char		 szFileName[522];				//��ǰѹ����ѹ���ļ�����
	//unsigned int u32Time;						//ѹ����ѹ�����ʱ������Ϊ��λ
}S_ZipState,*PS_ZipState;

enum E_WRITE_ZIP_MODE
{
	COVER_BEFORE_ZIP = 1,						//ѹ��ʱ����֮ǰ��zip
	APPEND_TO_ZIP = 2							//ѹ��ʱ׷�ӵ�����zip
};
enum E_DECOMPRESSION_FILE_MODE
{
	COVER_BEFORE_FILE		= 0,				//��ѹ�󸲸�֮ǰ���ļ�
	NO_COVER_BEFORE_FILE	= 1					//����в�������
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
		const char *pszPassword,										/*@#��%����&*GHJasdK0909 aso*/
		PFN_ZipStateCallback pfnZipStateCallback						//�ص���������ѹ��������״̬��Ϣ
		) = 0;


	/*Return : right IMZIP_OK , other IMZIP_ERROR*/
	virtual int Decompression(
		const TCHAR  *pszZipPath,										/*path of zip file*/
		const TCHAR  *pszDir,											/*The destination of decompression file*/

		E_DECOMPRESSION_FILE_MODE	i32FileMode,						/*0 indicate cover , 1 indicate no cover exist ,
																		return IMZIP_FILE_HAVE_EXIST if file have existed*/

		const char  *pszPassWord,										/*@#��%����&*GHJasdK0909 aso*/
		PFN_ZipStateCallback pfnZipStateCallback						//�ص��������ؽ�ѹ������״̬��Ϣ
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