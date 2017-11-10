#pragma once

class C_BaseWaitOverItem;

typedef unsigned long UI_BUSINESS_ID;	//Wait over time ID

typedef enum tagWaitOverType
{
	eWaitType_None,		//None type
	eWaitType_Msg,		//Msg wait
	eWaitType_Group,	//Create group wait
	eWaitType_GetDataListType,	// GetDataListTime
	eWaitType_Normal,
}E_WaitOverType,PE_WaitOverType;

typedef vector<C_BaseWaitOverItem*> VectBaseWaitOverItems;
typedef vector<C_BaseWaitOverItem*>::iterator VectBaseWaitOverItemIt;

typedef struct tagSendMsgFailedAck
{
	I_EIMCmd* pMsgCmd;
	I_EIMCmd* pAckCmd;
}SEND_MSG_FAILED_ACK, *PSEND_MSG_FAILED_ACK;

/*Wait over mgr class*/
class C_UIWaitOverMgr
{
public:
	~C_UIWaitOverMgr();
	static C_UIWaitOverMgr& GetObject();

	BOOL Init(I_EIMEventMgr* pIEventMgr);
	BOOL UnInit();

	void CheckWaitOverItem();
	BOOL AddWaitOverItem( C_BaseWaitOverItem* pBaseWaitOverItem );
	BOOL DelWaitOverItem( E_WaitOverType eWaitOverType,unsigned long long TypeId );
	void SetWaitOverItem( E_WaitOverType eWaitOverType,unsigned long long TypeId );
	void DelSetFailedItem( E_WaitOverType eWaitOverType,unsigned long long TypeId, I_EIMCmd* lpvParam);
    I_EIMCmd* GetWaitOverItemCmd(UINT64 u64MsgId);
private:
	C_UIWaitOverMgr();

	C_EIMLock m_Lock;
	VectBaseWaitOverItems m_vectWaitItems;
	I_EIMEventMgr* m_pIEventMgr;
};

/*Base wait over item*/
class C_BaseWaitOverItem
{

public:
	C_BaseWaitOverItem();
	virtual ~C_BaseWaitOverItem();

	/*Virtual wait type*/
	virtual E_WaitOverType GetWaitOverType()
	{return eWaitType_None;}

	/*Get business id */
	virtual UI_BUSINESS_ID GetBusinessId()
	{ return m_Cmd->GetBusinessId();}

	/*Set start time*/
	void SetStartTime(unsigned int u32StartTime)
	{m_u32StartTime = u32StartTime;}

	/*Get start time*/
	unsigned int GetStartTime()
	{return m_u32StartTime;}

	/*Get cmd*/
	virtual I_EIMCmd* GetData()
	{ return m_Cmd; }

	/*Set cmd*/
	virtual void SetData( I_EIMCmd* pIEIMCmd )
	{ m_Cmd = pIEIMCmd; pIEIMCmd->AddRef(); }

	/*Get type id*/
	virtual unsigned long long GetTypeId()
	{ return m_Cmd->GetBusinessTypeId(); }

	/*Set Elapse*/
	virtual unsigned int GetElapse() = 0;

	virtual void DoTimeout(I_EIMEventMgr* m_pIEventMgr, bool bTimeout=true) = 0;

private:
	unsigned int m_u32StartTime;
	I_EIMCmd* m_Cmd;

};

/*Message wait over*/
class C_MsgWaitOverItem : public C_BaseWaitOverItem
{
public:
	//C_MsgWaitOverItem();
	//~C_MsgWaitOverItem();

	virtual E_WaitOverType GetWaitOverType(){ return eWaitType_Msg; }
	
	virtual unsigned int GetElapse();

	virtual void DoTimeout(I_EIMEventMgr* pIEventMgr, bool bTimeout=true)
	{
        I_EIMCmd* pCmd = GetData();
        if(CMD_FAVORITE_MODIFY_REQ == pCmd->GetCmdId())
        {
            //收藏消息超时
        }
        else
		    SAFE_SEND_EVENT_(pIEventMgr, bTimeout ? EVENT_UI_WAIT_OVERTIME : EVENT_UI_WAIT_SUCCESS, pCmd);
	}

};

/*Create group over*/
class C_CreateGroupWaitOverItem : public C_BaseWaitOverItem
{
public:
	virtual E_WaitOverType GetWaitOverType(){ return eWaitType_Group; }

	virtual unsigned int GetElapse();
	virtual void DoTimeout(I_EIMEventMgr* pIEventMgr, bool bTimeout=true)
	{
		SAFE_SEND_EVENT_(pIEventMgr, bTimeout ? EVENT_UI_WAIT_OVERTIME : EVENT_UI_WAIT_SUCCESS, GetData());
	}
};

/*Create group over*/
class C_GetDataListTypeWaitOverItem : public C_BaseWaitOverItem
{
public:
	virtual E_WaitOverType GetWaitOverType(){ return eWaitType_GetDataListType; }

	virtual unsigned int GetElapse(){ return 10000; }

	virtual void DoTimeout(I_EIMEventMgr* m_pIEventMgr, bool bTimeout=true)
	{
		if ( bTimeout )
		{
			C_eIMEngine& eng = Eng;
			int i32Times = eng.GetAttributeInt(IME_GET_DATA_LIST_TYPE) -1;
			if ( i32Times > -6 )
			{
				if ( i32Times > -3 )
					i32Times = -3;

				eng.SetAttributeInt(IME_GET_DATA_LIST_TYPE, i32Times);
				eng.UpdateDepts(C_eIMEngine::GetObject().GetPurview(GET_LTS_DEPT));
			}
		}
	}
};

/*ContactUpdater*/
class C_ContactUpdateWaitOverItem : public C_BaseWaitOverItem
{
private:
	DWORD m_dwElapse;
public:
	C_ContactUpdateWaitOverItem(DWORD dwElapse):m_dwElapse(dwElapse){};
	virtual E_WaitOverType GetWaitOverType(){ return eWaitType_GetDataListType; }

	virtual unsigned int GetElapse(){ return m_dwElapse; }

	virtual void DoTimeout(I_EIMEventMgr* m_pIEventMgr, bool bTimeout=true)
	{
		if ( bTimeout )
		{
			C_eIMEngine& eng = Eng;
			int i32Times = eng.GetAttributeInt(IME_UPDATE_CONTACTS_TYPE) -1;
			if ( i32Times > -6 )
			{
				if ( i32Times > -3 )
					i32Times = -3;

				eng.SetAttributeInt(IME_UPDATE_CONTACTS_TYPE, i32Times);
				SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_RELOAD_CONTACTS, (void*)IME_UPDATE_CONTS_TYPE_NOW);
			}
		}
	}
};

/*C_GetDeptShowConfigOverItem wait over*/
class C_GetDeptShowConfigOverItem : public C_BaseWaitOverItem
{
private:
	DWORD m_dwElapse;

public:
	C_GetDeptShowConfigOverItem(DWORD dwElapse = 5 * 1000)
		: m_dwElapse(dwElapse)
	{
	}

	virtual E_WaitOverType GetWaitOverType()
	{ 
		return eWaitType_Normal; 
	}
	
	virtual unsigned int GetElapse()
	{ 
		return m_dwElapse; 
	}

	virtual void DoTimeout(I_EIMEventMgr* pIEventMgr, bool bTimeout=true)
	{
		if ( bTimeout )
		{
			C_eIMEngine& eng = Eng;
			int i32Info = eng.GetAttributeInt(IME_GET_DEPTUSER_VISABLE_INFO) -1;
			if ( i32Info > -6 )
			{	// Timeout retry 3 times[-3 => -6)
				if ( i32Info > -3 )
					i32Info = -3;

				eng.SetAttributeInt(IME_GET_DEPTUSER_VISABLE_INFO, i32Info);
				SAFE_SEND_EVENT_(pIEventMgr, EVENT_GET_DEPT_EMPS_END, (void*)0);
			}
		}
	}

};