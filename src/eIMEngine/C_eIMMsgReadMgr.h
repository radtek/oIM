#ifndef C_EIM_MSG_READ_MGR_H
#define C_EIM_MSG_READ_MGR_H

#include "eIMEngine/IeIMMsgReadMgr.h"

class C_eIMMsgReadMgr : public I_EIMMsgReadMgr
{
private:
	DWORD		m_dwRef;
	I_SQLite3*	m_pIDb;

public:
	DECALRE_PLUGIN_(C_eIMMsgReadMgr)

	C_eIMMsgReadMgr();
	~C_eIMMsgReadMgr();

	virtual BOOL Init(I_SQLite3* pIContsDb = NULL);
	virtual BOOL Uninit();

	virtual int		Set(PS_MsgRead psMsgRead);
	int		AddMsgReadEmps(QMID qmid, VectSessionMember& vectEmps);
	BOOL	UpdateFlag( QMID qmid, QEID qeid, DWORD dwFlag , BOOL bSet=TRUE );
	BOOL	UpdateFlagAndReadTime(QMID qmid, QEID qeid, DWORD dwFlag, DWORD dwTime);

	virtual int	GetHasMsgReadCount( QMID qmid );
	virtual int	GetMsgReadCount( QMID qmid );
	virtual int GetMsgRead(QMID qmid, PFN_1to100READ pfnRead, void* pvUserData);
    virtual int DeleteMsgRead(const std::list<QMID>& delMsgIdLst);

};

#endif