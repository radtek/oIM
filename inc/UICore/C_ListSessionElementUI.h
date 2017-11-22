/******************************************************************************
����֧�֣�
  1��ͼ��(logo):   session_item_logo.bkimage
  2�����(frame):  session_item_logo.normalimage
  3��״̬(status): session_item_logo.foreimage(Ⱥ�ĵĺ�㣬���ĵ�����״̬)
  4������(title):  session_item_title.text
  5��ʱ��(time):   sessoin_item_time.text
  6����Ϣ(msg):    session_item_msg.text(����Ϣ������ժҪ���ݸ�)
  7����ʶ(flag):   session_item_flag.bkimage(�Ự��ʶ�������ѣ�)
******************************************************************************/
#ifndef __EIM_LIST_SESSION_ELEMENT_UI_HEADER_20170417_YFGZ__
#define __EIM_LIST_SESSION_ELEMENT_UI_HEADER_20170417_YFGZ__
#pragma once

#include "eIMUICore\IeIMUISessionMgr.h"
#include "Public\EventProxy.h"

const TCHAR* const kMsgAnnex = _T("<MsgAnnex ");
const TCHAR* const kFileKey  = _T("Key=\"");

const TCHAR* const kListSessionElement =  _T("ListSessionElementUI");

#if defined(EIMUICORE_EXPORTS)
class __declspec(dllexport) CListSessionElementUI : public CListContainerElementUI
#else
class __declspec(dllimport) CListSessionElementUI : public CListContainerElementUI
#endif
{
public:
	CListSessionElementUI();
	~CListSessionElementUI();

	// Event
	virtual void OnEvent(unsigned int u32Event, void* lpvParam);
	inline void OnUpdateStatus(PS_EmpInfo_ psEmpInfo);
	inline BOOL OnUpdateHeadImage(QEID eid);
	inline void OnUpdateMyself(int i32Type);
	inline void OnUpdateSessionLogo(QSID sid);
	inline BOOL GetLogoPath(eIMString& szLogoPath, BOOL bCheckExist);
	inline BOOL GetSkinLogoPath(eIMString& szLogoPath, BOOL bMale);
	inline BOOL IsIncluded(QEID eid);

	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual UiLib::unUserData GetAttribute( LPCTSTR pstrName);
	virtual void Init();
	virtual LPCTSTR GetClass() const;
	virtual void DoEvent(TEventUI& event);

	LPCTSTR GetLogo();
	void	SetLogo(LPCTSTR pszImage);

	E_UserStatus GetStatus();
	void	SetStatus(E_UserStatus eStatus);

	LPCTSTR GetTitle();
	BOOL	SetTitle(LPCTSTR pszTitle);
	BOOL	SetDraft(LPCTSTR pszDraft);

	BOOL	SetTime(DWORD dwTime);
	DWORD	GetTime();

	BOOL	AddRecord(PS_MsgRecord psMsgRecord, BOOL bNewMsg = FALSE, LPCTSTR pszMsgSummary = NULL);
	DWORD   UpdateMember(const VectSessionMember& vectMember, E_OperaType eType);
	BOOL	UpdateRecord(PS_MsgRecord psMsgRecord);
	
	PS_UISessionInfo GetInfo();
	BOOL	SetInfo(const S_UISessionInfo& sSInfo);

	void	SetNewMsgCount(DWORD dwCount, BOOL bAdd=TRUE);
	DWORD	GetNewMsgCount();

	DWORD	GetTopTime();
	DWORD	SetTopTime(DWORD dwTopTime);
	QSID	GetSid();

	DWORD	SetFlag(DWORD dwFlag, BOOL bClear=FALSE);
	DWORD   GetFlag(DWORD dwFlag);

protected:
	void	SetMsgSummary(LPCTSTR pszSummary, BOOL bDraft = FALSE);

private:
	C_EventProxy<CListSessionElementUI> m_EventProxy;
	S_UISessionInfo		 m_sUISInfo;
	CHorizontalLayoutUI* m_phlItem; 
	CHorizontalLayoutUI* m_phlFrame; 
	CHorizontalLayoutUI* m_phlTitle;
	CHorizontalLayoutUI* m_phlMsg;
	CVerticalLayoutUI*	 m_pvlRight;

	int m_i32MsgCount;
	CPictureUI*	m_ppicLogo;
	CLabelUI*	m_plblStatus;
	CLabelUI*	m_plblUserStatus;
	CLabelUI*	m_plblTitle;
	CLabelUI*	m_plblTime;
	CLabelUI*	m_plblMsg;
	CLabelUI*	m_plblFlag;
	CButtonUI*	m_pbtnClose;

};

#endif __EIM_LIST_SESSION_ELEMENT_UI_HEADER_20170417_YFGZ__