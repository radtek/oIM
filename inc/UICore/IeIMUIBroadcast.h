#ifndef I_EIM_UI_BROADCAST_H
#define I_EIM_UI_BROADCAST_H


//#define BRO_MAX_ID_NUM		100    //�㲥���ID��
//#define BRO_PURVIEW			0x08  //�㲥Ȩ��

#pragma  pack(push, 8)

typedef enum
{
	eBRO_EMP=1,		// Ա��
	eBRO_DEPT=2,	// ����
}E_BroRecvType;

//���Ž�����Ա��Ϣ
typedef struct tag_BroRecver
{
	E_BroRecvType eType;	// ��������
	union
	{
		QEID	eid;		// Ա��ID
		QDID	did;		// ����ID
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