#ifndef __IEIMUI_SESSION_INVITE_WND_H__
#define __IEIMUI_SESSION_INVITE_WND_H__

#include <vector>

//��������Ϣ
typedef enum tagInviteSelType
{
	eSelectEmps,								// By employee
	eSelectDeps,								// By department
}E_InviteSelType,* PS_InviteSelType;

/*���������ݣ��������봰����չ�ӿ����ݽṹ*/
typedef struct tagInviteItemData
{
	enum{	
		eEmp,
		eDep,
	} eDataType;				//��������

	union {
		DWORD dwDeptId;
		DWORD dwEmpId;
	} unDataId;					//�������������������ж�ѡ���Id

}S_InviteItemData,*PS_InviteItemData;


// ����ص�����ָ����Ĭ��ʵ��
typedef BOOL (__stdcall* PFN_InvaliteCallback)(const VectSessionMember& vectAll, const VectSessionMember& vectAdd, const VectSessionMember& vectDel, void* pvInfo);
typedef struct tagInviteInfo
{
	HWND hParent;
	QEID eidCreater;
	E_InviteSelType	  eSelType;	// Invite select type
	VectSessionMember vectAll;	// Initial or result
	VectSessionMember vectAdd;	// Add member;
	VectSessionMember vectDel;	// Del member
	PFN_InvaliteCallback pfnCallback;
	tagInviteInfo()
		: hParent(NULL)
		, eSelType(eSelectEmps)
		, pfnCallback(NULL)
		, eidCreater(0)
	{}
}S_InviteInfo, *PS_InviteInfo;

// ��չ����ص�����ָ����Ĭ��ʵ��
typedef std::vector<S_InviteItemData>	VectInviteData;
typedef VectInviteData::iterator		VectInviteDataIt;
typedef VectInviteData::const_iterator	VectInviteDataIt_;
typedef BOOL (__stdcall* PFN_InvaliteCallbackEx)(const VectInviteData& vectAll, const VectInviteData& vectAdd, const VectInviteData& vectDel, void* pvInfo);
typedef struct tagInviteInfoEx
{
	HWND hParent;
	QEID eidCreater;
	E_InviteSelType	eSelType;	// Invite select type
	VectInviteData	vectAllEx;	// Initial or result
	VectInviteData	vectAddEx;	// Add member;
	VectInviteData	vectDelEx;	// Del member
	PFN_InvaliteCallbackEx pfnCallbackEx;
	tagInviteInfoEx()
		: hParent(NULL)
		, eSelType(eSelectEmps)
		, pfnCallbackEx(NULL)
		, eidCreater(0)
	{}
}S_InviteInfoEx, *PS_InviteInfoEx;

inline BOOL __stdcall InvaliteCallback(const VectSessionMember& vectAll, const VectSessionMember& vectAdd, const VectSessionMember& vectDel, void* pvInfo)
{
	if ( PS_InviteInfo psInfo= (PS_InviteInfo)pvInfo)
	{
		psInfo->vectAll.assign(vectAll.begin(), vectAll.end());
		psInfo->vectAdd.assign(vectAdd.begin(), vectAdd.end());
		psInfo->vectDel.assign(vectDel.begin(), vectDel.end());
		return TRUE;
	}
	return FALSE;
}

inline BOOL __stdcall InvaliteCallbackEx(const VectInviteData& vectAllEx, const VectInviteData& vectAddEx, const VectInviteData& vectDelEx, void* pvInfoEx)
{
	if ( PS_InviteInfoEx psInfoEx= (PS_InviteInfoEx)pvInfoEx)
	{
		psInfoEx->vectAllEx.assign(vectAllEx.begin(), vectAllEx.end());
		psInfoEx->vectAddEx.assign(vectAddEx.begin(), vectAddEx.end());
		psInfoEx->vectDelEx.assign(vectDelEx.begin(), vectDelEx.end());
		return TRUE;
	}

	return FALSE;
}

// ʹ������ʾ�� ///////////////////////////////////////////////////////////////
// S_InviteInfo sInfo;
// sInfo.vectAll.assign(m_vectInit.begin(), m_vectInit.end());
// sInfo.pfnCallback = InvaliteCallback;
// if ( hpt.ShowInviteWnd(sInfo) )
// { ... } 
//
#endif	// __IEIMUI_SESSION_INVITE_WND_H__