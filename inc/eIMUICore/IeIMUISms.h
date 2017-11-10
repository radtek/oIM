#ifndef I_EIM_UI_SMS
#define I_EIM_UI_SMS

#define SMS_UCODE_LEN   64
#define SMS_NAME_LEN   256 
#define SMS_PHONE_LEN   64
#define SMS_MSG_LEN		4096

//#define SMS_MAX_EMP_NUM		80    //��������
//#define SMS_PURVIEW			0x20  //����Ȩ�� //UINT16 wPurview; ����Ȩ�� 0000000000100000 ��6λΪ����Ȩ��

#pragma  pack(push, 8)

typedef enum
{
	eSMS_EMP=1,			// Ա��
	eSMS_DEPT=2,        // ����
	eSMS_USEGROUP=3,    // �Զ�����
	eSMS_PHONE=4,		// Phone
}E_RecverType;

//���Ž�����Ա��Ϣ
typedef struct tagSmsRecver
{
	tagSmsRecver(E_RecverType type, DWORD id=0, UINT64 phone=0u)
	{
		eType    = type;
		u64Phone = phone;
		qEid = 0;
		qDid = 0;

		if ( eType == eSMS_EMP )
			qEid = id;
		else if (eType == eSMS_DEPT )
			qDid = id;
	}

	E_RecverType eType;			//���˻��� ����ǵ���û��QEID����������ֻ��� ��EID������ʱ��0
	QEID    qEid;				// Employee ID
	UINT64	u64Phone;			// Phone
	QDID    qDid;				//Ϊ����ʱ��Ų���ID
	
public:
	bool operator==(const tagSmsRecver& r) const
	{
		if ( eType != r.eType )
			return false;

		if ((eType == eSMS_EMP      && qEid == r.qEid) ||
			(eType == eSMS_DEPT     && qDid == r.qDid) ||
			(eType == eSMS_USEGROUP && qDid == r.qDid) ||
			(eType == eSMS_PHONE    && u64Phone == r.u64Phone))
			return true;

		return false;
	}
}S_SmsRecver, *PS_SmsRecver;

// SMS records information
typedef struct tag_SmsRecInfo
{
	tag_SmsRecInfo()
	{
		qEid  = 0;
		memset(tszName,  0, sizeof(tszName));
		memset(tszUcode, 0, sizeof(tszUcode));
		memset(tszPhone, 0, sizeof(tszPhone));
		memset(tszMsg,	 0, sizeof(tszMsg));
	}

	QEID    qEid;						// Employee ID
	TCHAR   tszName[SMS_NAME_LEN];		// Name
	TCHAR   tszUcode[SMS_UCODE_LEN];	// UCode
	TCHAR   tszPhone[SMS_PHONE_LEN];	// Phone
	TCHAR   tszMsg[SMS_MSG_LEN];		// Msg
}S_SmsRecInfo, *PS_SmsRecInfo;


#pragma  pack(pop)

#define INAME_EIMUI_SMS			_T("SXIT.EIMUI.Sms")

class __declspec(novtable) I_eIMUISms : public I_EIMPlugin
{
public:
	//=============================================================================
	//Function:     ShowSmsDlg
	//Description:	Show SMS dialog
	//
	//Parameter:
	//	pvecRecver      - SMS receivers
	
	//Return:
	//	HRESULT     
	//=============================================================================
	virtual int ShowSmsDlg(vector<S_SmsRecver>* pvecRecver) = 0;
};

#endif