/*****************************************************************************
属性支持：
  1、图标(logo):   session_item_logo.bkimage
  2、相框(frame):  session_item_logo.normalimage
  3、状态(status): session_item_logo.foreimage(群聊的红点，单聊的在线状态)
  4、标题(title):  session_item_title.text
*****************************************************************************/
#ifndef __EIM_LIST_CONTACT_ELEMENT_UI_HEADER_20170417_YFGZ__
#define __EIM_LIST_CONTACT_ELEMENT_UI_HEADER_20170417_YFGZ__
#pragma once

#include "Public\EventProxy.h"

typedef enum tagContactItemType
{
	eTYPE_EMP,		  	// 员工Item
	eTYPE_DEPT,		  	// 部门Item
	eTYPE_DEPT_TOP,		// 顶层部门Item
}E_ContactItemType, *PE_ContactItemType;
typedef const enum tagContactItemType* PE_ContactItemType_;

#define CONTACT_STATE_NONE			(0)	// 无状态
#define CONTACT_STATE_PC_ONLINE		(1)	// PC 在线
#define CONTACT_STATE_MOBILE_ONLINE (2)	// 手机在线
#define CONTACT_STATE_PC_LEAVE		(3)	// PC 离开
#define CONTACT_STATE_MOBILE_LEAVE	(4)	// 手机离开
#define CONTACT_STATE_OFFLINE		(7)	// 不在线
#define CONTACT_STATE_DEPT			(8)	// 是部门（在最后）

#define CONTACT_TITLE_MAX			(256)	// 通讯录Item最大长度（比用户名/部门名长）
typedef union tagSortOrder
{
	UINT64 u64Order;
	struct{
		UINT64 bitPinyin:32;
		UINT64 bitOrder:16;
		UINT64 bitRev:12;
		UINT64 bitState:4;
	};

	tagSortOrder(): u64Order(0){} 
	void SetState(DWORD dwTerm=UINT_MAX, DWORD dwStatus=UINT_MAX)
	{
		switch(dwStatus)
		{
		case IME_EMP_STATUS_ONLINE:	// 在线
			bitState = (dwTerm == IME_EMP_TERM_PC) ? CONTACT_STATE_PC_ONLINE : CONTACT_STATE_MOBILE_ONLINE;
			break;
		case IME_EMP_STATUS_LEAVE:	// 离开
			bitState = (dwTerm == IME_EMP_TERM_PC) ? CONTACT_STATE_PC_LEAVE : CONTACT_STATE_MOBILE_LEAVE;
			break;
		case UINT_MAX:
			bitState = CONTACT_STATE_DEPT;			
			break;
		default:					
			bitState = (dwTerm == UINT_MAX) ? CONTACT_STATE_NONE : CONTACT_STATE_OFFLINE;
			break;
		}
	}

	void SetOrder(DWORD dwUser, DWORD dwPinyin=UINT_MAX)
	{
		if ( dwPinyin == UINT_MAX )
		{	// 部门，不用减法
			bitOrder = dwUser;	
			bitPinyin= 0;
		}
		else
		{
			bitOrder = USHRT_MAX - dwUser;	// 减法的原因： 服务器给的是大的大前，小的在后，0是不排序
			bitPinyin= dwPinyin;
		}
	}
}U_SortOrder, *PU_SortOrder;
typedef const union tagSortOrder* PU_SortOrder_;

#define CONTACT_FLAG_MALE			(0x00000001)	// 是男性（没有设置就是女）
#define CONTACT_FLAG_PATH			(0x00000002)	// 是路径Item
#define CONTACT_FLAG_ADMIN			(0x00000004)	// 是群创建者
#define CONTACT_FLAG_SORT_ID		(0x00000100)	// 排序使用ID，否则使用拼音排序（仅仅针对员工）
#define CONTACT_FLAG_SORT_XSTATUS	(0x00000200)	// 排序不使用状态 
#define CONTACT_FLAG_SORT_XMOBILE	(0x00000400)	// 排序不区分手机（不手机优先）
#define CONTACT_FLAG_SORT_MASK		(0x00000F00)	// 排序掩码
#define	CONTACT_FLAG_DISABLE_EVENT	(0x80000000)	// 禁用所有的事件（会话搜索列表，消息管理器等可以不用响应即时状态变化）

typedef struct tagContactItemInfo
{
	E_ContactItemType eType;	// Item 类型
	union{
		QDID did;				// 部门ID
		QEID eid;				// 员工ID
	};
	DWORD  dwFlag;				// 标识
	DWORD  dwPurview;			// 权限标识
	U_SortOrder  uSortOrder;	// 排序标识
	TCHAR  szTitle[CONTACT_TITLE_MAX + 1];		// 标题

	tagContactItemInfo()
		: eType(eTYPE_EMP)
		, eid(0)
		, dwFlag(0)
		, dwPurview(0)
	{
		szTitle[0] = _T('\0');
	}

	tagContactItemInfo& operator= (const tagContactItemInfo& s)
	{
		eid     = s.eid;
		eType   = s.eType;
		dwFlag  = s.dwFlag;
		dwPurview = s.dwPurview;
		uSortOrder.u64Order = s.uSortOrder.u64Order;
		_tcsncpy(szTitle, s.szTitle, CONTACT_TITLE_MAX);
		return *this;
	}
}S_ContactItemInfo, *PS_ContactItemInfo;
typedef const struct tagContactItemInfo* PS_ContactItemInfo_;

const TCHAR* const kListContactElement =  _T("ListContactElementUI");

#if defined(EIMUICORE_EXPORTS)
class __declspec(dllexport) CListContactElementUI : public CListContainerElementUI
#else
class __declspec(dllimport) CListContactElementUI : public CListContainerElementUI
#endif
{
public:
	CListContactElementUI();
	~CListContactElementUI();

	// Event
	virtual void OnEvent(unsigned int u32Event, void* lpvParam);
	inline void OnUpdateStatus(PS_EmpInfo psEmpInfo);
	inline void OnUpdateHeadImage(QEID eid);
	inline void OnUpdateMyself(int i32Type);

	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual UiLib::unUserData GetAttribute( LPCTSTR pstrName);
	virtual void Init();
    virtual LPCTSTR GetClass() const;
	virtual void SetEnabled(bool bEnable = true);
	virtual void DoEvent(TEventUI& event);
	virtual void DrawItemText(HDC hDC, const RECT& rcItem);
	virtual UINT GetControlFlags() const;

	LPCTSTR GetLogo();
	void	SetLogo(LPCTSTR pStrImage);

	int		GetStatus();
	void	SetStatus(DWORD dwTerm=UINT_MAX, DWORD dwStatus=UINT_MAX);

	LPCTSTR GetTitle();
	BOOL	SetTitle(LPCTSTR pszTitle);

	PS_ContactItemInfo GetInfo();
	BOOL	SetInfo(const S_ContactItemInfo& sCInfo);


private:
	C_EventProxy<CListContactElementUI> m_EventProxy;
	S_ContactItemInfo	 m_UICInfo;
	CHorizontalLayoutUI* m_phlItem;		// HorizontalLayout
	CHorizontalLayoutUI* m_phlFrame;	// HorizontalLayout

	CPictureUI*	m_ppicLogo;
	CLabelUI*	m_plblStatus;
	CLabelUI*	m_plblTitle;
	CButtonUI*	m_pbtnTitle;
	CLabelUI*	m_plblFlag;

};

#endif // __EIM_LIST_CONTACT_ELEMENT_UI_HEADER_20170417_YFGZ__