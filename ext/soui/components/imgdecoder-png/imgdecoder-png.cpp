
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "imgdecoder-png.h"
#include "decoder-apng.h"
#include <png.h>

namespace SOUI
{
    //////////////////////////////////////////////////////////////////////////
    //  SImgFrame_PNG
    SImgFrame_PNG::SImgFrame_PNG()
        :m_pdata(NULL)
        ,m_nWid(0)
        ,m_nHei(0)
        ,m_nFrameDelay(0)
    {

    }


    void SImgFrame_PNG::Attach( const BYTE * pdata,int nWid,int nHei,int nDelay )
    {
        m_pdata=(pdata);
        m_nWid=(nWid);
        m_nHei=(nHei);
        m_nFrameDelay=(nDelay);
    }

    BOOL SImgFrame_PNG::GetSize( UINT *pWid,UINT *pHei )
    {
        if(!m_pdata) return FALSE;
        *pWid = m_nWid;
        *pHei = m_nHei;
        return TRUE;
    }

    BOOL SImgFrame_PNG::CopyPixels( /* [unique][in] */ const RECT *prc, /* [in] */ UINT cbStride, /* [in] */ UINT cbBufferSize, /* [size_is][out] */ BYTE *pbBuffer )
    {
        if(!m_pdata || cbBufferSize != m_nHei * m_nWid *4) return FALSE;
        memcpy(pbBuffer,m_pdata,cbBufferSize);
        return TRUE;
    }

    
    //////////////////////////////////////////////////////////////////////////
    // SImgX_PNG
    int SImgX_PNG::LoadFromMemory( void *pBuf,size_t bufLen )
    {
        APNGDATA * pdata =LoadAPNG_from_memory((char*)pBuf,bufLen);
        return _DoDecode(pdata);
    }

    int SImgX_PNG::LoadFromFile( LPCWSTR pszFileName )
    {
        APNGDATA * pdata =LoadAPNG_from_file(pszFileName);
        return _DoDecode(pdata);
    }

    int SImgX_PNG::LoadFromFile( LPCSTR pszFileName )
    {
        wchar_t wszFileName[MAX_PATH+1];
        MultiByteToWideChar(CP_ACP,0,pszFileName,-1,wszFileName,MAX_PATH);
        if(GetLastError()==ERROR_INSUFFICIENT_BUFFER) return 0;
        return LoadFromFile(wszFileName);
    }

    SImgX_PNG::SImgX_PNG( BOOL bPremultiplied )
        :m_bPremultiplied(bPremultiplied)
        ,m_pImgArray(NULL)
        ,m_pngData(NULL)
    {

    }

    SImgX_PNG::~SImgX_PNG( void )
    {
        if(m_pImgArray) delete []m_pImgArray;
        m_pImgArray = NULL;
        if(m_pngData) APNG_Destroy(m_pngData);
    }

    int SImgX_PNG::_DoDecode(APNGDATA *pData)
    {
        if(!pData) return 0;
        m_pngData = pData;

        int nWid = m_pngData->nWid;
        int nHei = m_pngData->nHei;

        //swap rgba to bgra and do premultiply
        BYTE *p=m_pngData->pdata;
        int pixel_count = nWid * nHei * m_pngData->nFrames;
        for (int i=0; i < pixel_count; ++i) {
            BYTE a = p[3];
            BYTE t = p[0];
            if (a) 
            {
                p[0] = (p[2] *a)/255;
                p[1] = (p[1] * a)/255;
                p[2] =  (t   * a)/255;
            }else
            {
                memset(p,0,4);
            }
            p += 4;
        }

        p=m_pngData->pdata;
        m_pImgArray = new SImgFrame_PNG[m_pngData->nFrames];
        for(int i=0;i<m_pngData->nFrames;i++)
        {
            m_pImgArray[i].Attach(p,nWid,nHei,m_pngData->pDelay?m_pngData->pDelay[i]:0);
            p += nWid*nHei*4;
        }
        return m_pngData->nFrames;
    }

    UINT SImgX_PNG::GetFrameCount()
    {
        return m_pngData?m_pngData->nFrames:0;
    }

    //////////////////////////////////////////////////////////////////////////
    //  SImgDecoderFactory_PNG

    SImgDecoderFactory_PNG::SImgDecoderFactory_PNG()
    {

    }

    SImgDecoderFactory_PNG::~SImgDecoderFactory_PNG()
    {

    }

    BOOL SImgDecoderFactory_PNG::CreateImgX( IImgX **ppImgDecoder )
    {
        *ppImgDecoder = new SImgX_PNG(TRUE);
        return TRUE;
    }
    
  
    HRESULT SImgDecoderFactory_PNG::SaveImage(IBitmap *pImg, LPCWSTR pszFileName, const LPVOID pFormat)
    {
        if(!pImg) return E_INVALIDARG;
        
        BYTE *pData = (BYTE*)pImg->LockPixelBits();
        
        FILE *fp;  
        png_structp png_ptr;  
        png_infop info_ptr;  

        /* ����Ҫд����ļ� */  
        fp = _wfopen(pszFileName, L"wb");  
        if (fp == NULL)  
            return (E_FAIL);  

        /* ��������ʼ�� png_struct ��������Ĵ����������������ʹ��Ĭ 
        * �ϵ� stderr �� longjump() ����������Խ��������������Ϊ NULL�� 
        * ��ʹ�ö�̬���ӿ������£�����Ҳ���⺯����汾�Ƿ����ڱ���ʱ 
        * ʹ�õİ汾�Ƿ���ݡ�����Ҫ�� 
        */  
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);  

        if (png_ptr == NULL)  
        {  
            fclose(fp);  
            return (E_FAIL);  
        }  

        /* �����ڴ沢��ʼ��ͼ����Ϣ���ݡ�����Ҫ��*/  
        info_ptr = png_create_info_struct(png_ptr);  
        if (info_ptr == NULL)  
        {  
            fclose(fp);  
            png_destroy_write_struct(&png_ptr,  NULL);  
            return (E_FAIL);  
        }  

        /* ���ô�����������ڵ��� png_create_write_struct() ʱû 
        * �����ô�����������ô��δ����Ǳ���д�ġ�*/  
        if (setjmp(png_jmpbuf(png_ptr)))  
        {  
            /* ��������ܵ������ˣ���ôд���ļ�ʱ���������� */  
            fclose(fp);  
            png_destroy_write_struct(&png_ptr, &info_ptr);  
            return (E_FAIL);  
        }  

        /* ����������ƣ������ʹ�õ��� C �ı�׼ I/O �� */  
        png_init_io(png_ptr, fp);  

        /* ����һ�ָ��ӵ����� */  

        /* �����裩����������ͼ�����Ϣ����ȡ��߶ȵ������� 2^31�� 
        * bit_depth ȡֵ������ 1��2��4��8 ���� 16, ���ǿ��õ�ֵҲ������ color_type�� 
        * color_type ��ѡֵ�У� PNG_COLOR_TYPE_GRAY��PNG_COLOR_TYPE_GRAY_ALPHA�� 
        * PNG_COLOR_TYPE_PALETTE��PNG_COLOR_TYPE_RGB��PNG_COLOR_TYPE_RGB_ALPHA�� 
        * interlace ������ PNG_INTERLACE_NONE �� PNG_INTERLACE_ADAM7, 
        * �� compression_type �� filter_type Ŀǰ������ PNG_COMPRESSION_TYPE_BASE 
        * �� and PNG_FILTER_TYPE_BASE�� 
        */  

        png_uint_32 width = pImg->Width();
        png_uint_32 height = pImg->Height();
        png_uint_32 bytes_per_pixel = 4;

        png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,  
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);  

        /* д���ļ�ͷ����Ϣ�����裩 */  
        png_write_info(png_ptr, info_ptr);  

        /* ��ԭ���ݸ�ʽ��Ԥ�˵�rgba��ʽ����Ϊ��Ԥ�˵�bgra��ʽ */  
        png_bytep image = (png_bytep) new png_byte[width*height*bytes_per_pixel];  
        png_bytep dst=image;
        png_bytep src = (png_bytep)pData;
        for(png_uint_32 y=0;y<height;y++) for(png_uint_32 x=0;x<width;x++)
        {
            png_byte a = src[3];
            dst[2] = (png_byte)((int)src[0]*a/255);
            dst[1] = (png_byte)((int)src[1]*a/255);
            dst[0] = (png_byte)((int)src[2]*a/255);
            dst[3]=a;
            dst += 4;
            src += 4;
        }
        
        if (height > PNG_UINT_32_MAX/(sizeof (png_bytep)))  
            png_error (png_ptr, "Image is too tall to process in memory");  

        png_bytep *row_pointers = new png_bytep[height];  


        /* ����Щ������ָ��ָ����� "image" �ֽ������ж�Ӧ��λ�ã�����ָ��ÿ�����ص���ʼ�� */  
        for (png_uint_32 k = 0; k < height; k++)  
            row_pointers[k] = image + k*width*bytes_per_pixel;  

        /* һ�ε��þͽ�����ͼ��д���ļ� */  
        png_write_image(png_ptr, row_pointers);  
        /* �����������������д���ļ����ಿ�� */  
        png_write_end(png_ptr, info_ptr);  
        /* д��������ͷ��ѷ�����ڴ� */  
        png_destroy_write_struct(&png_ptr, &info_ptr);  

        delete []row_pointers;
        /* �ر��ļ� */  
        fclose(fp);  

        pImg->UnlockPixelBits(pData);
        return S_OK;
    }
    
    LPCWSTR SImgDecoderFactory_PNG::GetDescription() const
    {
        return DESC_IMGDECODER;
    }

    //////////////////////////////////////////////////////////////////////////
    BOOL IMGDECODOR_PNG::SCreateInstance( IObjRef **pImgDecoderFactory )
    {
        *pImgDecoderFactory = new SImgDecoderFactory_PNG();
        return TRUE;
    }

}//end of namespace SOUI

