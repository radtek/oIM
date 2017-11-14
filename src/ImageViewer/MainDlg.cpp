// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"

#ifdef DWMBLUR	//win7毛玻璃开关
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#endif

#define SWITCH_NEXT	(-1)	// Switch to next image
#define SWITCH_PREV (-2)	// Switch to prev image

CMainDlg::CMainDlg() 
	: SHostWnd(_T("LAYOUT:XML_MAINWND"))
	, m_bLayoutInited(FALSE)
	, m_pImageSwitcher(NULL)
	, m_pPrevImageBtn(NULL)
	, m_pNextImageBtn(NULL)
{
}

CMainDlg::~CMainDlg()
{
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	#ifdef DWMBLUR	//win7毛玻璃开关
	MARGINS mar = {5,5,30,5};
	DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
	#endif

	SetMsgHandled(FALSE);
	return 0;
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;
	m_pImageSwitcher = FindChildByName2<SImageSwitcher>(R.name.imageSwitcher);
	m_pPrevImageBtn  = FindChildByName2<SButton>(R.name.btn_prev);
	m_pNextImageBtn  = FindChildByName2<SButton>(R.name.btn_next);
	 
	for ( VectImageIt it = g_sParams.vectImage.begin(); it != g_sParams.vectImage.end(); it++ )
	{
		m_pImageSwitcher->InsertImage(*it);
	}

	SwitchImage(g_sParams.u64Fid);
	return 0;
}

void CMainDlg::OnLanguageBtnCN()
{
	OnLanguage(1);
}
void CMainDlg::OnLanguageBtnJP()
{
	OnLanguage(0);
}
void CMainDlg::OnLanguage(int nID)
{
	ITranslatorMgr *pTransMgr = SApplication::getSingletonPtr()->GetTranslator();
	SASSERT(pTransMgr);
	bool bCnLang = nID == 1;

	pugi::xml_document xmlLang;
	if (SApplication::getSingletonPtr()->LoadXmlDocment(xmlLang, bCnLang ? _T("lang_cn") : _T("lang_en"), _T("translator")))
	{
		CAutoRefPtr<ITranslator> lang;
		pTransMgr->CreateTranslator(&lang);
		lang->Load(&xmlLang.child(L"language"), 1);//1=LD_XML
		pTransMgr->SetLanguage(lang->name());
		pTransMgr->InstallTranslator(lang);
		SDispatchMessage(UM_SETLANGUAGE,0,0);
	}
}

//TODO:消息映射
void CMainDlg::OnClose()
{
	CSimpleWnd::DestroyWindow();
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;
	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}

void CMainDlg::OnPrevImage()
{
	SwitchImage(SWITCH_PREV);
}

void CMainDlg::OnNextImage()
{
	SwitchImage(SWITCH_NEXT);
}

BOOL CMainDlg::SwitchImage(int i)
{
	SStringT szTooltip;
	SStringT szPageN = GETSTRING(R.string.tipPageN);
	size_t nCount = m_pImageSwitcher->GetCount();
	int i32CurSel = m_pImageSwitcher->GetCurSel();

	if ( i == SWITCH_NEXT )
		m_pImageSwitcher->Switch(i32CurSel - 1);
	else if ( i == SWITCH_PREV )
		m_pImageSwitcher->Switch(i32CurSel + 1);
	else
		m_pImageSwitcher->Switch(i, FALSE);
	
	i32CurSel = m_pImageSwitcher->GetCurSel();
	szTooltip.Format(szPageN, i32CurSel + 1, nCount);
	if ( i32CurSel == 0 )
	{
		m_pPrevImageBtn->SetToolTipText(szTooltip);
		m_pNextImageBtn->SetToolTipText(GETSTRING(R.string.tipLast));
	}
	else if ( i32CurSel == nCount -1 )
	{
		m_pNextImageBtn->SetToolTipText(szTooltip);
		m_pPrevImageBtn->SetToolTipText(GETSTRING(R.string.tipInRecord));
	}
	else
	{
		m_pNextImageBtn->SetToolTipText(szTooltip);
		m_pPrevImageBtn->SetToolTipText(szTooltip);
	}

	return FALSE;
}
