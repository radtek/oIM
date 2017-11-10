#pragma once

namespace SOUI
{
	enum SkinType
	{
		color,
		sys
	};

	struct SkinSaveInf
	{
		COLORREF color;
		SStringW filepath;
		RECT margin;
	};
	struct SkinLoadInf
	{
		COLORREF color;
		SStringW filepath;
		RECT margin;
	};
	interface ISetOrLoadSkinHandler
	{
		virtual bool SaveSkin(SkinType, SkinSaveInf&) = NULL;		
	};

	class  SDemoSkin :
		public SSkinImgFrame
	{
		SOUI_CLASS_NAME(SDemoSkin, L"demoskin")
	public:
		SDemoSkin();
		SDemoSkin(ISetOrLoadSkinHandler *iSkinHander);
	public:
		virtual bool SetImage(IBitmap *pImg);
		///
		bool SetImage(SStringW imgfile);
		bool SetColor(COLORREF bkColor);
		void SetHander(ISetOrLoadSkinHandler*skinhander);
		virtual SIZE GetSkinSize();
		virtual BOOL IgnoreState();
		bool SaveSkin();
		bool LoadSkin(SkinType, SkinLoadInf& saveInf);
		virtual int GetStates();
		//��֧���Զ�ɫ�� 
		virtual void OnColorize(COLORREF cr)
		{}
	protected:
		virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState, BYTE byAlpha);
		
		ISetOrLoadSkinHandler *m_ISetOrLoadSkinHandler;		
		CSize m_csSize;		
		bool m_bIsColor;
		COLORREF m_bkColor;
		bool m_bAdaptiveImg;		
		SStringW m_FilePath;	
	};
}
