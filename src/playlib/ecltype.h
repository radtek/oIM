
#ifndef __EOSTYPE_H__
#define __EOSTYPE_H__

#include <STDIO.H>
#include <STRING.H>


#define _MNT_

/*�����������Ͷ���:*/

// Fixed size data types
typedef signed char		Int8;
typedef signed short	Int16;	
typedef signed int		Int32;

typedef unsigned char	UInt8;
typedef unsigned short  UInt16;
typedef unsigned int    UInt32;

#ifdef _MNT_

	typedef __int64				Int64;
	typedef unsigned __int64	UInt64;

#else

	typedef long long			Int64;
	typedef unsigned long long	UInt64;

#endif

#ifdef _MNT_
  #define I64CONST(x) x##I64
#else
  #define I64CONST(x) x##LL
#endif

typedef Int32 Boolean;

// Basic data types 

typedef UInt8				Char;
typedef UInt16				WChar;		// 'wide' int'l character type.

typedef Int16 				Coord;		// screen/window coordinate

typedef UInt32				LocalID;		// local (card relative) chunk ID


typedef Int32				Err;

typedef UInt8				BYTE;
typedef UInt16				WORD;
typedef unsigned long		DWORD;

typedef Int32		mode_t;
typedef Int32		kdev_t;
typedef Int32		khw_t;
typedef Int32		khwtype_t;
typedef Int32		loff_t;
typedef volatile unsigned int lock_t;

typedef Int32	FontID;   //modify by wenquan 2004-3-19

//===========eway application data type ��άӦ����������=======================================

typedef Int32   Handle; //�����


typedef UInt32	Stock24;	//bit 31-bit8 Ϊstock24 (bit23 - bit0), bit7-0 = 0;

//Stock24��������ʹ�������ֽ�(24bits)�洢��λ��ʽ���£�
//6λ���ȸ�ʽ
//	    |����|	�ݴ�	  |	��ֵ
//Bits	|23	 |22	21	20|	19	18	17	16	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0
//remark|+/- |	EXP1	  |Numeric1
//5λ���ȸ�ʽ
//	    |����|	         �ݴ�	       |  ��ֵ
//Bits	|23	 |  22	21	20|	19	18	17 |16	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0
//Remark|+/- |	1	1	1 |	EXP2	   |Numeric2

typedef Int64	Money;
//�����ֵ = Int64 (int64 in memory) / 10000; 
//��¼��ΧΪ:-92233720368577.5808��92233720368577.5807.��Ҫ8 bytes


/* min and max macros */
#ifndef __cplusplus

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

#define MIN(X,Y)	(((X)<=(Y))? (X):(Y))
#define MAX(X,Y)	(((X)>=(Y))? (X):(Y))

#endif 

#define ABS(X) (((X) >= 0) ? (X) : -(X))


/* End-Of-Line defines */
#define  EOL            '\r'               /* DOS carriage return, linefeed */
#define  SKIP_EOL(p)    (p+=2)             /* macro to skip over CR LF      */
#define  PATH_CHAR      '\\'
#define  CR             '\r'
#define  LF             '\n'
#define  CRLF           "\r\n"


/************************************************************
 * Useful Macros 
 *************************************************************/

// The min() and max() macros which used to be defined here have been removed
// because they conflicted with facilities in C++.  If you need them, you
// should define them yourself, or see PalmUtils.h -- but please read the
// comments in that file before using it in your own projects.


#define OffsetOf(type, member)	((Int32) &((((type *) 0)->member)))


/************************************************************
 * Common constants
 *************************************************************/
#ifndef NULL
#define NULL	((void *)0)
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

typedef struct{
	Int16 second;	  //�� //2 Byte
	Int16 minute;	  //�� //2 Byte
	Int16 hour;		  //ʱ //2 Byte
	Int16 day;		  //�� //2 Byte
	Int16 month;	  //�� //2 Byte
	Int16 year;		  //�� //2 Byte
	Int16 weekDay;  //���� //2 Byte
	UInt16 yDay;    //���еĵڼ��� //2 Byte
	UInt16 isDst;   //��ʱ�� //2 Byte
} DateTimeType;//18 bytes



//body of the "fmt" chunk 
typedef struct  {
	Int16 FormatTag;
	Int16 Channels;
	Int32 SamplesPerSec;
	Int32 AvgBytesPerSec;
	Int16 BlockAlign;
	Int16 BitsPerSample;
} ChunkFmtBody;

//header of the chunk
typedef struct  {
	char *chunkID;
	Int32  chunkleth;
} ChunkHdr;

//header of the Riff file
typedef struct  {
	char *fileID;
	Int32 fileleth;
	char *wavTag;
} RiffHdr;

//Header of wav file
typedef struct  {
	//	RiffHdr riffHdr;
	char fileID[4];
	Int32 fileleth;
	char wavTag[4];
	char FmtHdrID[4];
	Int32  FmtHdrLeth;
	//	ChunkHdr FmtHdr;
	Int16 FormatTag;
	Int16 Channels;
	Int32 SamplesPerSec;
	Int32 AvgBytesPerSec;
	Int16 BlockAlign;
	Int16 BitsPerSample;
	//	ChunkFmtBody FmtBody;
	char DataHdrID[4];
	Int32  DataHdrLeth;
	//	ChunkHdr DataHdr;
} WaveHdr;


#endif /* __EOSTYPE_H__ */
/*  End of File - EOSTYPE.H  */
