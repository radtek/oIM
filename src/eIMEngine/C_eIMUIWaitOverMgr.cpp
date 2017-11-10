#include "StdAfx.h"
#include "C_eIMUIWaitOverMgr.h"


C_UIWaitOverMgr::C_UIWaitOverMgr()
{
	m_pIEventMgr = NULL;
}

C_UIWaitOverMgr::~C_UIWaitOverMgr()
{
	
}

C_UIWaitOverMgr& C_UIWaitOverMgr::GetObject()
{
	static C_UIWaitOverMgr obj;
	return obj;
}

BOOL C_UIWaitOverMgr::Init(I_EIMEventMgr* pIEventMgr)
{
	ASSERT_(pIEventMgr); 
	CHECK_NULL_RET_(pIEventMgr,FALSE);
	if ( m_pIEventMgr == NULL )
	{
		m_pIEventMgr = pIEventMgr;
		pIEventMgr->AddRef();
	}

	return TRUE;
}

BOOL C_UIWaitOverMgr::UnInit()
{
	SAFE_RELEASE_INTERFACE_(m_pIEventMgr);
	return TRUE;
}

void C_UIWaitOverMgr::CheckWaitOverItem()
{
	m_Lock.Lock();

	VectBaseWaitOverItemIt it = m_vectWaitItems.begin();
	for (;it != m_vectWaitItems.end();)
	{
		DWORD u32CurTime = ::GetTickCount();
		if( (*it)->GetElapse() < (u32CurTime - (*it)->GetStartTime()))
		{
			(*it)->DoTimeout(m_pIEventMgr);	//´¥·¢³¬Ê±
			SAFE_DELETE_PTR_(*it);
			it = m_vectWaitItems.erase(it);
			continue;
		}
		it++;
	}

	m_Lock.UnLock();
}

BOOL C_UIWaitOverMgr::AddWaitOverItem(C_BaseWaitOverItem* pBaseWaitOverItem)
{
	m_Lock.Lock();

	m_vectWaitItems.push_back(pBaseWaitOverItem);

	m_Lock.UnLock();

	return TRUE;
}

BOOL C_UIWaitOverMgr::DelWaitOverItem( E_WaitOverType eWaitOverType,unsigned long long TypeId )
{
	m_Lock.Lock();

	VectBaseWaitOverItemIt it = m_vectWaitItems.begin();
	for (;it != m_vectWaitItems.end();it++)
	{
		if( ((*it)->GetWaitOverType() == eWaitOverType) && ((*it)->GetTypeId() == TypeId) )
		{
			(*it)->DoTimeout(m_pIEventMgr, false);
			SAFE_DELETE_PTR_(*it);
			m_vectWaitItems.erase(it);
			break;
		}
	}

	m_Lock.UnLock();

	return TRUE;
}

I_EIMCmd* C_UIWaitOverMgr::GetWaitOverItemCmd(UINT64 u64MsgId)
{
    m_Lock.Lock();
    I_EIMCmd* pCmd = NULL;
	VectBaseWaitOverItemIt it = m_vectWaitItems.begin();
	for (;it != m_vectWaitItems.end();it++)
	{
		if( ((*it)->GetTypeId() == u64MsgId) )
		{
			(*it)->DoTimeout(m_pIEventMgr, false);
            pCmd = (*it)->GetData();
            pCmd->AddRef();
			break;
		}
	}
    
	m_Lock.UnLock();
    return pCmd;
}

void C_UIWaitOverMgr::SetWaitOverItem( E_WaitOverType eWaitOverType,unsigned long long TypeId )
{
	m_Lock.Lock();

	VectBaseWaitOverItemIt it = m_vectWaitItems.begin();
	for (;it != m_vectWaitItems.end();it++)
	{
		if( ((*it)->GetWaitOverType() == eWaitOverType) && ((*it)->GetTypeId() == TypeId) )
		{
			(*it)->DoTimeout(m_pIEventMgr);
			SAFE_DELETE_PTR_(*it);
			m_vectWaitItems.erase(it);
			break;
		}
	}

	m_Lock.UnLock();
}

void C_UIWaitOverMgr::DelSetFailedItem( E_WaitOverType eWaitOverType,unsigned long long TypeId, I_EIMCmd* lpvParam)
{
	SEND_MSG_FAILED_ACK rAck;
	rAck.pAckCmd = lpvParam;
	m_Lock.Lock();

	VectBaseWaitOverItemIt it = m_vectWaitItems.begin();
	for (;it != m_vectWaitItems.end();it++)
	{
		if( ((*it)->GetWaitOverType() == eWaitOverType) && ((*it)->GetTypeId() == TypeId) )
		{
			rAck.pMsgCmd = (*it)->GetData();
			SAFE_SEND_EVENT_(m_pIEventMgr,EVENT_SEND_MSG_ACK,&rAck);
			SAFE_DELETE_PTR_(*it);
			m_vectWaitItems.erase(it);
			break;
		}
	}

	m_Lock.UnLock();
}

C_BaseWaitOverItem::C_BaseWaitOverItem()
{
	m_u32StartTime = 0;
	m_Cmd = NULL;
}

C_BaseWaitOverItem::~C_BaseWaitOverItem()
{
	if(m_Cmd)
		m_Cmd->Release();
}

unsigned int C_MsgWaitOverItem::GetElapse()
{
	return 40000; 
}

unsigned int C_CreateGroupWaitOverItem::GetElapse()
{
	return 20000;
}


