// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMStateSubscribeMgr is the engine function DLL
// 
// Auth:	WML
// Date:     2014/9/17 21:21:21 
// 
// History: 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_SUBSCRIBEMGR_HEADER_2014_09_17_WML__
#define __EIMENGINE_SUBSCRIBEMGR_HEADER_2014_09_17_WML__

#include "Public\Plugin.h"

#define OPERATE_ADD					1
#define OPERATE_DELETE				2

#define ITEMTYPE_DEPT				1
#define ITEMTYPE_TOP_DEPT			2
#define ITEMTYPE_CUSTOMGROUP		3
#define ITEMTYPE_COMMONGROUP		4
#define ITEMTYPE_COUNT				4

#define UIPAGE_DEPT					1
#define UIPAGE_CONTACT				2

typedef std::vector<QEID>			VT_EmpID;
typedef std::map<QEID, QEID>		Map_EmpID;
typedef std::map<QDID, QDID>		Map_DeptID;

typedef enum
{
	GETSTATE_ALL = 0,
	GETSTATE_FIXSUB,
	GETSTATE_DEPT,
	GETSTATE_USER
};

class __declspec(novtable) I_EIMSubScribeMgr: public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     UpdateEmpSubcribe
	//Description:	Update emp subscribe info
	//
	//Parameter:
	//  qeid	   - EMP ID	
	//	iOperateType    - OPERATE_ADD or OPERATE_DELETE
	//  bAddTime		addtime(TRUE for single session)
	//Return:
	//	int		   
	//=============================================================================
	virtual int UpdateEmpSubcribe(QEID qeid, int iOperateType, BOOL bAddTime=FALSE)=0;

	//=============================================================================
	//Function:     UpdateEmpsSubcribe
	//Description:	Update emps subscribe info
	//
	//Parameter:
	//  vtEid			- VT_EmpID
	//	iOperateType    - OPERATE_ADD or OPERATE_DELETE
	//Return:
	//	int		   
	//=============================================================================
	virtual int UpdateEmpsSubcribe(VT_EmpID& vtEid, int iOperateType)=0;

	//=============================================================================
	//Function:     UpdateExpandItemData
	//Description:	Update Expand Item info
	//
	//Parameter:
	//  dwID	   - dept id or Custom groupid  or common dept id ----SEE ITEMTYPE_*
	//  iItemType
	//	iOperateType    - OPERATE_ADD or OPERATE_DELETE
	//Return:
	//	int
	//=============================================================================
	virtual int UpdateExpandItemData(UINT64 u64ItemID, int iItemType, int iOperateType)=0;

	//=============================================================================
	//Function:     SetUIPage
	//Description:	Set main window visiable page
	//
	//Parameter:
	//  iUIPage	   - UIpage value ----SEE UIPAGE_*
	//Return:
	//	void
	//=============================================================================
	virtual void SetUIPage(int iUIPage)=0;

	//=============================================================================
	//Function:     GetUIPage
	//Description:	Get main window visiable page
	//
	//Parameter:
	//  void
	//Return:
	//	int			- UIpage value ----SEE UIPAGE_*
	//=============================================================================
	virtual int GetUIPage()=0;
	
	//=============================================================================
	//Function:     FirstSubscribeRecentContact
	//Description:	first time subscribe recent contact state
	//
	//Parameter:
	//  void
	//Return:
	//	int
	//=============================================================================
	virtual int FirstSubscribeRecentContact()=0;

	//=============================================================================
	//Function:     CheckStateSubscribeData
	//Description:	check subscribe over time
	//
	//Parameter:
	//  void
	//Return:
	//	void
	//=============================================================================
	virtual void CheckStateSubscribeData(BOOL* pbAbort);	

	//=============================================================================
	//Function:     CheckStateGetData
	//Description:	check get status over time
	//
	//Parameter:
	//  void
	//Return:
	//	void
	//=============================================================================
	virtual void CheckStateGetData(BOOL* pbAbort);

	//=============================================================================
	//Function:     IsRobot
	//Description:	check eid is robot or not
	//
	//Parameter:
	//  eid QEID
	//Return:
	//	BOOL
	//=============================================================================
	virtual BOOL IsRobot(QEID eid);

	//=============================================================================
	//Function:     IsRobot
	//Description:	check eid is virtual group employee or not
	//
	//Parameter:
	//  eid QEID
	//Return:
	//	BOOL
	//=============================================================================
	virtual BOOL IsVirGroupMain(QEID eid) = 0;

};

#endif // __EIMENGINE_SUBSCRIBEMGR_HEADER_2014_09_17_WML__