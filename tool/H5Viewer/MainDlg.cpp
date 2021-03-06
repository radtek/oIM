// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"
	
#ifdef DWMBLUR	//win7毛玻璃开关
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#endif
	
void OnTitleChanged(const struct _wkeClientHandler* clientHandler, const wkeString title)
{

}
void OnUrlChanged(const struct _wkeClientHandler* clientHandler, const wkeString url)
{
}

wkeClientHandler wkeCH = { OnTitleChanged, OnUrlChanged};

// 全局的 js 调用 c++ 的函数
jsValue JS_CALL jsMsgBox(jsExecState es)
{
	const wchar_t *text = jsToStringW(es, jsArg(es, 0));
	const wchar_t *title = jsToStringW(es, jsArg(es, 1));

	SOUI::SMessageBox(NULL, text, title, MB_OK);

	return jsUndefined();
}

CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_bLayoutInited = FALSE;
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

	return 0;
}

void CMainDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN) {
		OnBtnWebkitGo();
	} else {
		SetMsgHandled(FALSE);
	}
}

HRESULT CMainDlg::OnBtnWebkitGo()
{
	SWkeWebkit *pWebKit = FindChildByName2<SWkeWebkit>(L"wke_test");
	assert(pWebKit);
	pWebKit->GetWebView()->setClientHandler(&wkeCH);
	SEdit *pEdit = FindChildByName2<SEdit>(L"edit_url");
	assert(pEdit);
	SStringT strUrl = pEdit->GetWindowText();
	pWebKit->SetAttribute(L"url", S_CT2W(strUrl), FALSE);

	return S_OK;
}

HRESULT CMainDlg::OnBtnWebkitRefresh()
{
	SWkeWebkit *pWebkit = FindChildByName2<SWkeWebkit>(L"wke_test");
	assert(pWebkit);
	pWebkit->GetWebView()->reload();

	return S_OK;
}
HRESULT CMainDlg::OnBtnWebkitBackward()
{
	SWkeWebkit *pWebkit = FindChildByName2<SWkeWebkit>(L"wke_test");
	assert(pWebkit);
	pWebkit->GetWebView()->goBack();

	return S_OK;
}
HRESULT CMainDlg::OnBtnWebkitForward()
{
	SWkeWebkit *pWebkit = FindChildByName2<SWkeWebkit>(L"wke_test");
	assert(pWebkit);
	pWebkit->GetWebView()->goForward();

	return S_OK;
}

HRESULT CMainDlg::OnBtnRunJs()
{
	// 运行 js 代码
	// 可以在浏览器地址栏输入以下代码后，点击 runJS
	// window.scrollTo(0,100)
	// document.write('<p>这是使用 document.write 写入的文本</p>')
	// document.getElementById("img_track_event_id").name
	// 其中 strValue 返回可能返回的值，比如 上一句代码执行后返回 img_track_event_name
	SEdit *pEdit = FindChildByName2<SEdit>(L"edit_url");
	assert(pEdit);
	SStringW strJs = pEdit->GetWindowText();
	SWkeWebkit* pWebkit = FindChildByName2<SWkeWebkit>(L"wke_test");
	wkeWebView pWebView = pWebkit->GetWebView();
	assert(pWebkit);
	SOUI::SStringW strValue = pWebView->runJS(strJs); ;// pWebkit->RunJS(strJs);
	SMessageBox(GetHostHwnd(), strValue, L"提示", MB_OK);

	return S_OK;
}

HRESULT CMainDlg::OnBtnRunCPlusPlus()
{
	// 运行 C++ 代码
	SWkeWebkit *pWebkit = FindChildByName2<SWkeWebkit>(L"wke_test");
	assert(pWebkit);
	wkeWebView webView = pWebkit->GetWebView();
	if (NULL != webView) {
		// 绑定 js 函数，让 js 主动调用 C++ 函数
		jsBindFunction("msgBox", jsMsgBox, 2);
		// 访问个人站，测试 js 调用 C++
		webView->loadFile(_T("Html/index.html"));
		bool b = webView->isLoaded();
	}

	return S_OK;
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

