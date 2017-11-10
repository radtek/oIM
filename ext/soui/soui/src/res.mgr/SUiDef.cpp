#include "include\souistd.h"
#include "res.mgr\SUiDef.h"
#include "helper\SplitString.h"
#include "helper\mybuffer.h"

namespace SOUI{

	const static WCHAR KNodeUidef[]     = L"uidef" ;
	const static WCHAR KNodeString[]    = L"string";
	const static WCHAR KNodeColor[]     = L"color";
	const static WCHAR KNodeSkin[]      = L"skin";
	const static WCHAR KNodeStyle[]     = L"style";
	const static WCHAR KNodeObjAttr[]   = L"objattr";
	const static TCHAR KDefFontFace[]   = _T("����");


	static pugi::xml_node GetSourceXmlNode(pugi::xml_node nodeRoot,pugi::xml_document &docInit,IResProvider *pResProvider, const wchar_t * pszName)
	{
		pugi::xml_node     nodeData = nodeRoot.child(pszName,false);
		if(nodeData)
		{
			pugi::xml_attribute attrSrc = nodeData.attribute(L"src",false);
			if(attrSrc)
			{//���ȴ�src�������ȡ����
				SStringT strSrc = S_CW2T(attrSrc.value());
				SStringTList strList;
				if(2==ParseResID(strSrc,strList))
				{
					CMyBuffer<char> strXml;
					DWORD dwSize = pResProvider->GetRawBufferSize(strList[0],strList[1]);

					strXml.Allocate(dwSize);
					pResProvider->GetRawBuffer(strList[0],strList[1],strXml,dwSize);
					pugi::xml_parse_result result= docInit.load_buffer(strXml,strXml.size(),pugi::parse_default,pugi::encoding_utf8);
					if(result) nodeData = docInit.child(pszName);
				}
			}
		}
		return nodeData;
	}

	class SUiDefInfo : public TObjRefImpl<IUiDefInfo>
	{
	public:
		SUiDefInfo(IResProvider *pResProvide,LPCTSTR pszUidef);

		virtual SSkinPool * GetSkinPool() {return pSkinPool;}
		virtual SStylePool * GetStylePool(){return pStylePool;}
		virtual SNamedColor & GetNamedColor() {return namedColor;}
		virtual SNamedString & GetNamedString() {return namedString;}
		virtual SObjDefAttr & GetObjDefAttr(){return objDefAttr;}
		virtual FontInfo & GetDefFontInfo() { return defFontInfo;}

	protected:

		CAutoRefPtr<SSkinPool>    pSkinPool;
		CAutoRefPtr<SStylePool>   pStylePool;

		SNamedColor   namedColor;
		SNamedString  namedString;
		SObjDefAttr   objDefAttr;

		FontInfo	  defFontInfo;
	};


	SUiDefInfo::SUiDefInfo(IResProvider *pResProvider,LPCTSTR pszUidef)
	{
		SStringTList strUiDef;
		if(2!=ParseResID(pszUidef,strUiDef))
		{
			SLOGFMTW(_T("warning!!!! Add ResProvider Error."));
		}

		DWORD dwSize=pResProvider->GetRawBufferSize(strUiDef[0],strUiDef[1]);
		if(dwSize==0)
		{
			SLOGFMTW(_T("warning!!!! uidef was not found in the specified resprovider"));
		}else
		{
			pugi::xml_document docInit;
			CMyBuffer<char> strXml;
			strXml.Allocate(dwSize);

			pResProvider->GetRawBuffer(strUiDef[0],strUiDef[1],strXml,dwSize);

			pugi::xml_parse_result result= docInit.load_buffer(strXml,strXml.size(),pugi::parse_default,pugi::encoding_utf8);

			if(!result)
			{//load xml failed
				SLOGFMTW(_T("warning!!! load uidef as xml document failed"));
			}else
			{//init named objects
				pugi::xml_node root = docInit.child(KNodeUidef,false);
				if(!root)
				{
					SLOGFMTW(_T("warning!!! \"uidef\" element is not the root element of uidef xml"));
				}else
				{
					//parse default font
					pugi::xml_node xmlFont;
					xmlFont=root.child(L"font",false);

					FONTSTYLE fontStyle(0);
					if(xmlFont)
					{
						fontStyle.attr.cSize=xmlFont.attribute(L"size").as_int(12);
						fontStyle.attr.byCharset =(BYTE)xmlFont.attribute(L"charset").as_int(DEFAULT_CHARSET);
						fontStyle.attr.fBold = xmlFont.attribute(L"bold").as_bool(false);
						fontStyle.attr.fUnderline = xmlFont.attribute(L"underline").as_bool(false);
						fontStyle.attr.fStrike = xmlFont.attribute(L"strike").as_bool(false);
						fontStyle.attr.fItalic = xmlFont.attribute(L"italic").as_bool(false);
						
						defFontInfo.dwStyle = fontStyle.dwStyle;
						defFontInfo.strFaceName = S_CW2T(xmlFont.attribute(L"face").value());

						if(defFontInfo.strFaceName.IsEmpty() || !SUiDef::CheckFont(defFontInfo.strFaceName))
						{
							defFontInfo.strFaceName = KDefFontFace;
						}
					}else
					{
						fontStyle.attr.cSize = 12;
						fontStyle.attr.byCharset =DEFAULT_CHARSET;

						defFontInfo.dwStyle = fontStyle.dwStyle;
						defFontInfo.strFaceName = KDefFontFace;
					}

					//load named string
					{
						pugi::xml_document docData;
						pugi::xml_node     nodeData = GetSourceXmlNode(root,docData,pResProvider,KNodeString);
						if(nodeData)
						{
							namedString.Init(nodeData);
						}
					}

					//load named color
					{
						pugi::xml_document docData;
						pugi::xml_node     nodeData = GetSourceXmlNode(root,docData,pResProvider,KNodeColor);
						if(nodeData)
						{
							namedColor.Init(nodeData);
						}
					}

					//load named skin
					{
						pugi::xml_document docData;
						pugi::xml_node     nodeData = GetSourceXmlNode(root,docData,pResProvider,KNodeSkin);
						if(nodeData)
						{
							pSkinPool.Attach(new SSkinPool);
							pSkinPool->LoadSkins(nodeData);
							SSkinPoolMgr::getSingletonPtr()->PushSkinPool(pSkinPool);

						}
					}
					//load named style
					{
						pugi::xml_document docData;
						pugi::xml_node     nodeData = GetSourceXmlNode(root,docData,pResProvider,KNodeStyle);
						if(nodeData)
						{
							pStylePool.Attach(new SStylePool);
							pStylePool->Init(nodeData);
							SStylePoolMgr::getSingleton().PushStylePool(pStylePool);
						}
					}
					//load SWindow default attribute
					{
						pugi::xml_document docData;
						pugi::xml_node     nodeData = GetSourceXmlNode(root,docData,pResProvider,KNodeObjAttr);
						if(nodeData)
						{
							objDefAttr.Init(nodeData);
						}
					}

				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	template<> SUiDef * SSingleton<SUiDef>::ms_Singleton = NULL;

	#define HASFONT 2
	int CALLBACK DefFontsEnumProc(  CONST LOGFONT *lplf,     // logical-font data
		CONST TEXTMETRIC *lptm,  // physical-font data
		DWORD dwType,            // font type
		LPARAM lpData            // application-defined data
		)
	{
		return HASFONT;
	}

	static BOOL DefFontCheck(const SStringT & strFontName)
	{
		//ȷ���������
		HDC hdc = GetDC(NULL);
		int hasFont = EnumFonts(hdc,strFontName,DefFontsEnumProc,0);
		ReleaseDC(NULL,hdc);
		return hasFont == HASFONT;
	}

	FunFontCheck SUiDef::s_funFontCheck = DefFontCheck;


	SUiDef::SUiDef(void)
	{
	}

	SUiDef::~SUiDef(void)
	{
	}

	IUiDefInfo * SUiDef::CreateUiDefInfo(IResProvider *pResProvider, LPCTSTR pszUiDef)
	{
		return new SUiDefInfo(pResProvider,pszUiDef);
	}

	void SUiDef::SetUiDef( IUiDefInfo* pUiDefInfo )
	{
		m_pCurUiDef = pUiDefInfo;
	}

	void SUiDef::SetFontChecker(FunFontCheck fontCheck)
	{
		s_funFontCheck = fontCheck;
	}

	BOOL SUiDef::CheckFont(const SStringT & strFontName)
	{
		return s_funFontCheck(strFontName);
	}

}
