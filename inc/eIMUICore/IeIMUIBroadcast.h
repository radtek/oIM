#ifndef I_EIM_UI_BROADCAST_H
#define I_EIM_UI_BROADCAST_H


//#define BRO_MAX_ID_NUM		100    //广播最大ID数
//#define BRO_PURVIEW			0x08  //广播权限

#pragma  pack(push, 8)

typedef enum
{
	eBRO_EMP=1,		// 员工
	eBRO_DEPT=2,	// 部门
}E_BroRecvType;

//短信接收人员信息
typedef struct tag_BroRecver
{
	E_BroRecvType eType;	// 接收类型
	union
	{
		QEID	eid;		// 员工ID
		QDID	did;		// 部门ID
	};
}S_BroRecver, *PS_BroRecver;


#pragma  pack(pop)

#define INAME_EIMUI_BRO			_T("SXIT.EIMUI.Bro")

class __declspec(novtable) I_eIMUIBroadcast : public I_EIMPlugin
{
public:
	//=============================================================================
	//Function:     ShowBroDlg
	//Description:	Show broadcast dialog
	//
	//Parameter:
	//  pvecRecver	Receivers
	//					
	//Return:
	//	HRESULT
	//=============================================================================
	virtual int ShowBroDlg(vector<S_BroRecver>* pvecRecver) = 0;
};

#endif