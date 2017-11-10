#include "stdafx.h"
#include <algorithm>
#include "C_eIMContacts.h"

#define NULL_STR	""
IMPLEMENT_PLUGIN_SINGLETON_(C_eIMContacts, INAME_EIMENGINE_CONTACTS);
C_eIMContacts& C_eIMContacts::GetObject()
{
	static C_eIMContacts obj;
	return obj;
}

BOOL C_eIMContacts::SetEventMgr(I_EIMEventMgr* pIEIMEventMgr)
{
	m_pIEIMEventMgr = pIEIMEventMgr;
	return ( !!m_pIEIMEventMgr );
}

C_eIMContacts::C_eIMContacts(void)
	: m_pIEIMEventMgr( NULL )
	, m_dwSearchDeptLastTime( 0 )
	, m_dwSearchEmpLastTime( 0 )
	, m_dwLoadContactDeptTimestamp( 0 )
	, m_dwLoadContactDeptUserTimestamp( 0 )
	, m_dwLoadEmpTimestamp( 0 )
	, m_i32Language( 0 )
	, m_pvCheckDeptPurviewUserData(NULL)
	, m_pvCheckEmpPurviewUserData(NULL)
	, m_pfnCheckDeptPurviewCb(NULL)
	, m_pfnCheckEmpPurviewCb(NULL)
{
	ZERO_ARRAY_(m_szSearchEmpsCond);
	ZERO_ARRAY_(m_szSearchDeptsCond);
	
	ZERO_STRUCT_(m_sCoInfo);

	InitializeCriticalSectionAndSpinCount( &m_csConts,  9999 );
}

C_eIMContacts::~C_eIMContacts(void)
{
	Clear( IMECFLAG_CLEAR_ALL );
	DeleteCriticalSection( &m_csConts );
}

inline BOOL C_eIMContacts::_InitPid(QDID qdidPid[IME_MAX_PID], QDID qdid)
{
	memset(qdidPid, 0xFF, sizeof(QDID) * IME_MAX_PID);		// IME_ID_NONE is 0xFFFFFFFF
	qdidPid[0] = qdid;
	return TRUE;
}

inline BOOL C_eIMContacts::_UpdateEmpPid(
	QDID qdidPid[IME_MAX_PID],
	QDID qdid,
	BOOL bAdd
	)
{
	int i32Index;

	for (i32Index = 0; i32Index < IME_MAX_PID; i32Index++)
	{
		if (qdidPid[i32Index] == qdid ||
			qdidPid[i32Index] == IME_ID_NONE )
			break;
	}

//	ASSERT_(i32Index < IME_MAX_PID);
	if ( i32Index >= IME_MAX_PID)
	{
		TRACE_(_T("Index[%d] outof the max[%d] of pid[%d]"), i32Index, IME_MAX_PID, qdid);
		return FALSE;
	}

	if ( bAdd )
	{
		if (qdidPid[i32Index] == qdid )
			return FALSE;			// Not modify, already existed

		qdidPid[i32Index] = qdid;	// Add it
	}
	else
	{
		if (qdidPid[i32Index] != qdid )
			return FALSE;			// Not modify, not exist

		if ( i32Index + 1 == IME_MAX_PID )		// is delete last pid
			qdidPid[i32Index] = IME_ID_NONE;
		else									// only copy to 
			memcpy(&qdidPid[i32Index], &qdidPid[i32Index + 1], sizeof(QDID) * (IME_MAX_PID - i32Index - 1));
	}

	return TRUE;
}

//=============================================================================
//Function:     _UpdateDeptEmployee
//Description:	Update department-employee relative
//
//Parameter:
//	dwDeptId    - Employee's parent department ID
//	dwEmpId     - Employee Id
//	bAdd        - TRUE:  Add
//				  FALSE: Delete 
//
//Return:
//      TRUE:  Succeeded
//		FALSE: Failed
//=============================================================================
BOOL C_eIMContacts::_UpdateDeptEmployee(
	DWORD	dwDeptId,
	DWORD	dwEmpId,
	BOOL	bAdd
	)
{
	MapEmpIt it = m_mapEmp.find( dwEmpId );
	if ( it == m_mapEmp.end() )
	{	// No exist in Employee memory table
		if ( bAdd )
		{
			PS_EmpInfo psEmpInfo = new S_EmpInfo; 
			ASSERT_(psEmpInfo);
			if ( psEmpInfo )
			{	// Add UGLY employee information to memory table
				memset(psEmpInfo, 0, sizeof(S_EmpInfo));
				psEmpInfo->Id			= dwEmpId;
				psEmpInfo->bitRef		= 1;
				psEmpInfo->bitPurview   = IMP_EMP_PURVIEW_DEFAULT;
				psEmpInfo->bitIntegrity = IMP_EMP_INTEGRITY_UGLY;
				_CheckEmpsNullString(*psEmpInfo);
				_InitPid(psEmpInfo->qdidPid, dwDeptId);

				m_mapEmp.insert(PairEmp(dwEmpId, psEmpInfo));
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{	// Exist in Employee memory table
		if ( bAdd )
		{
			if ( it->second->bitRef == 0 )
			{	// No reference 
				it->second->bitRef	   = 1;
				_InitPid(it->second->qdidPid, dwDeptId);
			}
			else if( _UpdateEmpPid(it->second->qdidPid, dwDeptId, TRUE) )
			{
				it->second->bitRef = ( it->second->bitRef + 1 ) <= IME_REF_MAX ? ( it->second->bitRef + 1 ) : IME_REF_MAX;
			}
		}
		else
		{	// Delete
			if ( _UpdateEmpPid(it->second->qdidPid, dwDeptId, FALSE) )
				it->second->bitRef = ( it->second->bitRef - 1 ) >= 0x0 ? ( it->second->bitRef - 1 ) : 0x0;
		}
	}

	return TRUE;
}

//=============================================================================
//Function:     _AddDepts
//Description:	Add department information 
//Important Notes: CAN NOT ADD the department, use UPDATE contact to do this
//
//Parameter:
//	sDeptEmp    - Department information to Add
//
//Return:
//      TRUE:  Succeeded
//		FALSE: Failed
//=============================================================================
BOOL C_eIMContacts::_AddDepts(
	S_DeptEmp&	sDeptEmp
	)
{
	DWORD		 dwDeptId = sDeptEmp.sDeptInfo.Id;
	MapDeptEmpIt it;
	VectEmpIdIt	 EmpIdIt;

	it = m_mapDeptEmp.find( dwDeptId );
	if ( it == m_mapDeptEmp.end() )
	{	// Not exist in DeptEmp memory table
		m_mapDeptEmp[dwDeptId]  = sDeptEmp;
		S_DeptEmp& sDE = m_mapDeptEmp[dwDeptId];	// Speed up
		sDE.pszNamePinyinS = eIMAddString_(sDeptEmp.pszNamePinyinS);
		if ( sDE.pszNamePinyinS && sDE.pszNamePinyinS[0] )
			sDE.pszNamePinyin  = strstr(sDE.pszNamePinyinS, sDeptEmp.pszNamePinyin);
		if ( sDE.pszNamePinyin == NULL || sDE.pszNamePinyin[0]=='\0' )
			sDE.pszNamePinyin  = eIMAddString_( sDeptEmp.pszNamePinyin);
		if ( sDE.pszNamePinyinS == NULL || sDE.pszNamePinyinS[0] == '\0')
			sDE.pszNamePinyinS = sDE.pszNamePinyin;

		VectEmpId& vectEmpId = sDE.vectEmpId;
		for ( EmpIdIt = vectEmpId.begin(); EmpIdIt!= vectEmpId.end(); EmpIdIt++ )
		{	// Check and update Employee memory table
			_UpdateDeptEmployee( dwDeptId, GET_QEID_(*EmpIdIt), TRUE );
		}
	}
	else
	{	// Existed in DeptEmp memory table
		memcpy( &it->second.sDeptInfo, &sDeptEmp.sDeptInfo, sizeof(sDeptEmp.sDeptInfo) );
		S_DeptEmp& sDE = it->second;
		sDE.pszNamePinyinS = eIMAddString_( sDeptEmp.pszNamePinyinS );
		if (sDE.pszNamePinyinS && sDE.pszNamePinyinS[0])
			sDE.pszNamePinyin = strstr(sDE.pszNamePinyinS, sDeptEmp.pszNamePinyin );
		if (sDE.pszNamePinyin == NULL || sDE.pszNamePinyin[0] == '\0')
			sDE.pszNamePinyin = eIMAddString_( sDeptEmp.pszNamePinyin);
		if (sDE.pszNamePinyinS || sDE.pszNamePinyinS[0] == '\0' )
			sDE.pszNamePinyinS = sDE.pszNamePinyin;

		// ****** If the department structure modified[Add/Del/Move], 
		// MUST be reload the Contacts, so not update the tree at here!
		// it->second.dwFirstChildId  = sDeptEmp.dwFirstChildId;
		// it->second.dwNextId		  = sDeptEmp.dwNextId;
		// it->second.dwParentId	  = sDeptEmp.dwParentId;
		// it->second.dwPrevId		  = sDeptEmp.dwPrevId;

		VectEmpId& vectEmpId =  sDE.vectEmpId;
		EmpIdIt = sDeptEmp.vectEmpId.begin();
		for ( ; EmpIdIt!= sDeptEmp.vectEmpId.end(); EmpIdIt++)
		{
			VectEmpIdIt	 EmpIdIt2 = vectEmpId.begin();
			for(; EmpIdIt2 != vectEmpId.end(); EmpIdIt2++ )
			{
				if ( GET_QEID_(*EmpIdIt2) == GET_QEID_(*EmpIdIt) )
					break;
			}

			if ( EmpIdIt2 == vectEmpId.end() )
			{	// Not exist in department-employee list
				vectEmpId.push_back( *EmpIdIt );
				_UpdateDeptEmployee( dwDeptId, GET_QEID_(*EmpIdIt), TRUE );
			}
		}
	}

	return TRUE;
}

//=============================================================================
//Function:     _DelDepts
//Description:	Delete department's employee
//Important Notes: CAN NOT DELETE the department, use UPDATE contact to do this
//
//Parameter:
//	sDeptEmp    - Department information to delete
//
//Return:
//      TRUE:  Succeeded
//		FALSE: Failed
//=============================================================================
BOOL C_eIMContacts::_DelDepts(
	S_DeptEmp&	sDeptEmp
	)
{
	DWORD		 dwDeptId = sDeptEmp.sDeptInfo.Id;
	MapDeptEmpIt it;
	VectEmpIdIt	 EmpIdIt;
	VectEmpIdIt	 EmpIdIt2;

	it = m_mapDeptEmp.find( dwDeptId );
	if ( it == m_mapDeptEmp.end() )
		return TRUE;	// Not exist

	EmpIdIt = sDeptEmp.vectEmpId.begin();
	for ( ; EmpIdIt != sDeptEmp.vectEmpId.end(); EmpIdIt++ )
	{
		for (EmpIdIt2 = it->second.vectEmpId.begin(); EmpIdIt2 != it->second.vectEmpId.end(); EmpIdIt2++ )
		{
			if ( GET_QEID_(*EmpIdIt2) == GET_QEID_(*EmpIdIt) )
				break;
		}

		if ( EmpIdIt2 != it->second.vectEmpId.end() )
		{
			_UpdateDeptEmployee( dwDeptId, GET_QEID_(*EmpIdIt), FALSE );
		}
	}

	return TRUE;
}

//=============================================================================
//Function:     _ModDepts
//Description:	Modify department information, same as _AddDepts 
//
//Parameter:
//	sDeptEmp    - Department information to modify
//
//Return:
//      TRUE:  Succeeded
//		FALSE: Failed
//=============================================================================
BOOL C_eIMContacts::_ModDepts(
	S_DeptEmp&	sDeptEmp
	)
{
	return _AddDepts( sDeptEmp );
}

BOOL C_eIMContacts::SetCoInfo(PS_CoInfo psCoInfo)
{
	CHECK_NULL_RET_(psCoInfo, FALSE);
	memcpy(&m_sCoInfo, psCoInfo, sizeof(m_sCoInfo));
	return TRUE;
}

//=============================================================================
//Function:     SetVirtRootDepts
//Description:	Set virtual root department, MUST be call it for operate 
//				department memory table
//
//Parameter:
//	dwFirstChildId    - The root first department Id
//
//Return:
//		TRUE: Succeeded
//		FALSE: Failed
//=============================================================================
BOOL C_eIMContacts::SetVirtRootDepts(
	DWORD	dwFirstChildId 
	)
{
	if ( dwFirstChildId != IME_ID_NONE )
	{
		S_DeptEmp sVRoot = {{IME_ID_NONE, _T(""), _T("")}, "", "", IME_ID_NONE, dwFirstChildId, IME_ID_NONE, IME_ID_NONE, };
		m_mapDeptEmp[ IME_ID_NONE ] = sVRoot;

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//Function:     SetDepts
//Description:	Set department information
//
//Parameter:
//	sDeptEmp    - Department information
//				  S_DeptEmp::vectEmpId only contained the members that will Add/Del/Mod
//	eMode       - eIMECSET_ADD: Add department and relative employee members
//				  eIMECSET_MOD: Modify department and relative employee members
//				  eIMECSET_DEL: Delete department relative employee
//	bNotify     - TRUE: Notify UI
//				  FALSE: Not notify UI
//Return:
//      TRUE:  Succeeded
//		FALSE: Failed
//=============================================================================
BOOL C_eIMContacts::SetDepts(
	S_DeptEmp&	sDeptEmp,
	E_IMECSMode eMode,
	BOOL		bNotify
	)
{
	BOOL	bRet	  = FALSE;
	DWORD	dwEId = 0;	// Event Id
	
	if ( eMode == eIMECSET_ADD )
	{
		C_EIMAutoLock al(m_csConts);
		bRet  = _AddDepts( sDeptEmp );
		dwEId = EVENT_DEPT_ADD;
	}
	else if ( eMode == eIMECSET_DEL )
	{
		C_EIMAutoLock al(m_csConts);
		bRet  = _DelDepts( sDeptEmp );
		dwEId = EVENT_DEPT_DEL;
	}
	else if ( eMode == eIMECSET_MOD )
	{
		C_EIMAutoLock al(m_csConts);
		bRet  = _ModDepts( sDeptEmp );
		dwEId = EVENT_DEPT_MOD;
	}
	else
	{
		ASSERT_(FALSE);
	}

	if ( bNotify && bRet && dwEId )
	{
		SAFE_SEND_EVENT_(m_pIEIMEventMgr, dwEId, &sDeptEmp);
	}

	return bRet;
}

BOOL C_eIMContacts::_CheckEmpsNullString(S_EmpInfo& sEmp)
{
	const TCHAR* pszNull = eIMAddString_(L"");

	if ( sEmp.pszName == NULL )			sEmp.pszName		= pszNull;
	if ( sEmp.pszEnName == NULL )		sEmp.pszEnName		= pszNull;
	if ( sEmp.pszNamePinyin == NULL )	sEmp.pszNamePinyin	= (const char*)pszNull;
	if ( sEmp.pszNamePinyinS == NULL )	sEmp.pszNamePinyinS = (const char*)pszNull;
	if ( sEmp.pszDuty == NULL )			sEmp.pszDuty		= pszNull;
	if ( sEmp.pszSign == NULL )			sEmp.pszSign		= pszNull;
	if ( sEmp.pszCode == NULL )			sEmp.pszCode		= pszNull;
	if ( sEmp.pszTel == NULL )			sEmp.pszTel			= pszNull;
//	if ( sEmp.szPsw == NULL )			sEmp.szPsw			= pszNull;
	if ( sEmp.pszPhone == NULL )		sEmp.pszPhone		= pszNull;
	if ( sEmp.pszHomeTel == NULL )		sEmp.pszHomeTel		= pszNull;
	if ( sEmp.pszEmrgTel == NULL )		sEmp.pszEmrgTel		= pszNull;
	if ( sEmp.pszEmail == NULL )		sEmp.pszEmail		= pszNull;
//	if ( sEmp.pszLogo == NULL )			sEmp.pszLogo		= pszNull;
	if ( sEmp.pszAddress == NULL )		sEmp.pszAddress		= pszNull;
	if ( sEmp.pszPostalCode == NULL )	sEmp.pszPostalCode	= pszNull;
	if ( sEmp.pszFax == NULL )			sEmp.pszFax			= pszNull;

	return TRUE;
}
//=============================================================================
//Function:     SetEmps
//Description:	Set employee information
//
//Parameter:
//	sEmp       - Employee information
//	eMode      - eIMECSET_ADD: Add employee to memory table
//				 eIMECSET_DEL: Delete employee from department, else
//				   dwParentId == IME_ID_NONE: delete from all department
//				 eIMECSET_MOD: Modify employee information
//	bNotify    - TRUE: Notify UI
//				 FALSE: Not notify UI
//Return:
//      TRUE:  Succeeded
//		FALSE: Failed
//=============================================================================
BOOL C_eIMContacts::SetEmps(
	S_EmpInfo&	sEmp, 
	E_IMECSMode eMode,
	BOOL		bNotify
	)
{
	CHECK_NULL_RET_(sEmp.Id, FALSE);
	DWORD	 dwEId = 0;
	MapEmpIt   it  = m_mapEmp.find( sEmp.Id );
	PS_EmpInfo psEmpInfo = NULL;
	eIMStringA szValueA;
	if ( it == m_mapEmp.end() )
	{
		C_EIMAutoLock al(m_csConts);
		if ( (eMode & eIMECSET_ADD) == eIMECSET_ADD || (eMode & eIMECSET_MOD) == eIMECSET_MOD )
		{
			psEmpInfo = new S_EmpInfo;
			ASSERT_(psEmpInfo);
			if( psEmpInfo )
			{
				memcpy(psEmpInfo, &sEmp, sizeof(S_EmpInfo));
				psEmpInfo->pszNamePinyinS= eIMAddString_(sEmp.pszNamePinyinS);
				if (psEmpInfo->pszNamePinyinS && psEmpInfo->pszNamePinyinS[0])					// Use py group's 
					psEmpInfo->pszNamePinyin = strstr(psEmpInfo->pszNamePinyinS, sEmp.pszNamePinyin);
				if (psEmpInfo->pszNamePinyin == NULL || psEmpInfo->pszNamePinyin[0] == '\0' )	// Use py
					psEmpInfo->pszNamePinyin = eIMAddString_(sEmp.pszNamePinyin);
				if (psEmpInfo->pszNamePinyin == NULL || psEmpInfo->pszNamePinyin[0] == '\0' )	// Use Code
					psEmpInfo->pszNamePinyin = eIMAddString_((::eIMTChar2MByte(sEmp.pszCode, szValueA), szValueA.c_str()));
				if (psEmpInfo->pszNamePinyinS == NULL || psEmpInfo->pszNamePinyinS[0] == '\0' )
					psEmpInfo->pszNamePinyinS = psEmpInfo->pszNamePinyin;
				
				_CheckEmpsNullString(sEmp);
				_InitPid(psEmpInfo->qdidPid, sEmp.qdidPid[0]);

				m_mapEmp.insert(PairEmp(sEmp.Id, psEmpInfo));
			}
		}
		else if ( (eMode & eIMECSET_MODS) == eIMECSET_MODS )
		{
			// 暂时去掉
			/*if (sEmp.bitStatus == IME_EMP_STATUS_QUIT)
				sEmp.bitStatus = IME_EMP_STATUS_OFFLINE;

			PS_EmpInfo psEmpInfo = new S_EmpInfo;
			ASSERT_(psEmpInfo);
			if (psEmpInfo)
			{
				memcpy(psEmpInfo, &sEmp, sizeof(S_EmpInfo));
				_InitPid(psEmpInfo->qdidPid, IME_ID_NONE);
				m_mapEmp.insert(PairEmp(sEmp.Id, psEmpInfo));
			}*/
		}

		return TRUE;
	}
	else
	{	
		C_EIMAutoLock al(m_csConts);
		ASSERT_(it->second);
		S_EmpInfo& sE = *it->second;
		if ( ( eMode & eIMECSET_ADD ) == eIMECSET_ADD )
		{
			if ( memcmp(&sE.Id, &sEmp.Id, (BYTE*)&sEmp.dwLogoTime - (BYTE*)&sEmp.Id + sizeof(sEmp.dwLogoTime)) == 0)
			{
			//	TRACE_(_T("No changed of emp:%u, %s"), sEmp.Id, sEmp.pszName);
				sE.bitVisible = sEmp.bitVisible;
				_InitPid(sE.qdidPid, sEmp.qdidPid[0]);
				return TRUE;
			}

			BYTE	u8Visible  = sE.bitVisible;
			BYTE	u8Ref      = sE.bitRef;
			DWORD	dwStatus   = sE.bitStatus;
			DWORD	dwLoginType= sE.bitLoginType;

			memcpy( &sE, &sEmp, sizeof( S_EmpInfo ) );
			// Restore
			sE.bitRef         = u8Ref;
			sE.bitStatus      = dwStatus;
			sE.bitLoginType   = dwLoginType;
			sE.bitVisible	  = u8Visible;
			sE.pszNamePinyinS = eIMAddString_(sEmp.pszNamePinyinS);
			if (sE.pszNamePinyinS && sE.pszNamePinyinS[0])			// Use py group's 
				sE.pszNamePinyin = strstr(sE.pszNamePinyinS, sEmp.pszNamePinyin);
			if (sE.pszNamePinyin == NULL || sE.pszNamePinyin[0]=='\0')	// Use py
				sE.pszNamePinyin = eIMAddString_(sEmp.pszNamePinyin);
			if (sE.pszNamePinyin == NULL || sE.pszNamePinyin[0] == '\0')	// Use Code
				sE.pszNamePinyin = eIMAddString_((::eIMTChar2MByte(sEmp.pszCode, szValueA), szValueA.c_str()));
			if (sE.pszNamePinyinS == NULL || sE.pszNamePinyinS[0] == '\0' )
				sE.pszNamePinyinS = sE.pszNamePinyin;

			_CheckEmpsNullString(sE);
			_InitPid(sE.qdidPid, sEmp.qdidPid[0]);
			if ( bNotify && sEmp.qdidPid[0] != IME_ID_NONE )
				dwEId = EVENT_EMP_ADD;
		}
		else if ( (eMode & eIMECSET_MOD) == eIMECSET_MOD )
		{	// Save 
			const char*	pszPinyin  = sE.pszNamePinyin;
			const char*	pszPinyinS = sE.pszNamePinyinS;
			BYTE	u8Visible  = sE.bitVisible;
			BYTE	u8Ref      = sE.bitRef;
			DWORD	dwStatus   = sE.bitStatus;
			DWORD	dwLoginType= sE.bitLoginType;
			QDID	qdidPid[IME_MAX_PID];

			memcpy(qdidPid, sE.qdidPid, sizeof(qdidPid));
			memcpy( &sE, &sEmp, sizeof( S_EmpInfo ) );
			// Restore
			sE.bitRef         = u8Ref;
			sE.bitStatus      = dwStatus;
			sE.bitLoginType   = dwLoginType;
			sE.bitVisible	  = u8Visible;
			sE.pszNamePinyin  = pszPinyin;
			sE.pszNamePinyinS = pszPinyinS;
			memcpy(sE.qdidPid, qdidPid, sizeof(qdidPid));
			
			_CheckEmpsNullString(sE);
			if ( bNotify && sEmp.qdidPid[0] != IME_ID_NONE )
				dwEId = EVENT_EMP_MOD;
		} 
		else if ( (eMode & eIMECSET_MODS) == eIMECSET_MODS )
		{
			if( eMode == eIMECSET_MODS )//兼容之前的
			{
				sE.bitStatus    = sEmp.bitStatus == IME_EMP_STATUS_QUIT ? IME_EMP_STATUS_OFFLINE : sEmp.bitStatus;
				sE.bitLoginType = sEmp.bitLoginType;
			}
			else
			{
				if( (eMode & eIMECSET_MODS_CONCERN) == eIMECSET_MODS_CONCERN ) 
					sE.bitConcern = sEmp.bitConcern;
				if( (eMode & eIMECSET_MODS_UPDATE_LOGO) == eIMECSET_MODS_UPDATE_LOGO )
					sE.dwLogoTime = sEmp.dwLogoTime;
				//...
			}
			
			if ( bNotify && sEmp.qdidPid[0] != IME_ID_NONE )
				dwEId = EVENT_EMP_MODS;
		}
		else if ( (eMode & eIMECSET_DEL) == eIMECSET_DEL )
		{
			_UpdateEmpPid(sE.qdidPid, sEmp.qdidPid[0], FALSE);
		}
		else
		{
			ASSERT_( FALSE );
			return FALSE;
		}
	}

	if (dwEId && it->second )
	{
		SAFE_SEND_EVENT_(m_pIEIMEventMgr, dwEId, it->second);
	}

	return TRUE;
}

//=============================================================================
//Function:     Clear
//Description:	Clear data
//
//Parameter:
//	dwFlag    - See: IMECFLAG_CLEAR_*
//
//Return:
//              
//=============================================================================
BOOL C_eIMContacts::Clear( 
	DWORD	dwFlag 
	)
{
	DWORD dwEId = 0;
	if ( dwFlag & IMECFLAG_CLEAR_EMPS )
	{
		SAFE_SEND_EVENT_(m_pIEIMEventMgr, EVENT_CLEAR_EMPS_BEFORE, NULL );
		C_EIMAutoLock al(m_csConts);
		for(MapEmpIt it = m_mapEmp.begin(); it != m_mapEmp.end(); it++)
		{
			if (it->second)
			{
				delete it->second;
				it->second = NULL;
			}
		}

		m_mapEmp.clear();
		dwEId = EVENT_CLEAR_EMPS_AFTER;
	}

	if ( dwFlag & IMECFLAG_CLEAR_DEPTS )
	{
		SAFE_SEND_EVENT_(m_pIEIMEventMgr, EVENT_CLEAR_DEPTS_BEFORE, NULL );
		C_EIMAutoLock al(m_csConts);
		m_mapDeptEmp.clear();
		dwEId = EVENT_CLEAR_DEPTS_AFTER;
	}

	if ( dwFlag & IMECFLAG_CLEAR_EMPS_SCACHE )
	{
		m_szSearchEmpsCond[0] = _T('\0');
		m_vectSearchEmpsCache.clear();
	}

	if ( dwFlag & IMECFLAG_CLEAR_DEPTS_SCACHE )
	{
		m_szSearchDeptsCond[0] = _T('\0');
		m_vectSearchDeptsCache.clear();
	}

	if (dwEId )
	{
		SAFE_SEND_EVENT_(m_pIEIMEventMgr, dwEId, NULL );
	}

	return TRUE;
}

//=============================================================================
//Function:     FormatError
//Description:	Format error code as description information
//
//Parameter:
//	dwError     - Error code
//	pszError    - Return the formated error description information,
//                Buffer size as ERROR_BUFFER_SIZE
//
//Return:
//		The Error description string size               
//=============================================================================
DWORD C_eIMContacts::FormatError(
	DWORD dwError, 
	TCHAR* pszError
	)
{
	return 0;
}

BOOL C_eIMContacts::SetEmpState(QEID qeid, DWORD bitStatus, DWORD bitLoginType)
{
	S_EmpInfo		 sEmp = { 0 };
	sEmp.Id			  = qeid;
	sEmp.bitStatus	  = (bitStatus & IME_EMP_STATUS_MASK);
	sEmp.bitLoginType =(bitLoginType & IME_EMP_TERM_MASK);
	return SetEmps(sEmp, eIMECSET_MODS, TRUE);
}

//=============================================================================
//Function:     GetCompanyInfo
//Description:	Get company information
//
//
//Return:
//		Return the company information              
//=============================================================================
PS_CoInfo_ C_eIMContacts::GetCompanyInfo()
{
	return &m_sCoInfo;
}

//=============================================================================
//Function:     GetDeptItem
//Description:	Get department item data
//
//Parameter:
//	psItem        -	NULL: Get the First child Item of root(eGDIM_FIRST_CHILD), else is relative it to operate.
//	eGDIMethod    - See E_GDIMethod
//
//Return:
//		Return the finded department item data if succeeded             
//		NULL otherwise
//=============================================================================
PS_DeptInfo_	C_eIMContacts::GetDeptItem(
	PS_DeptInfo_	psItem,
	E_GDIMethod		eGDIMethod
	)
{
	C_EIMAutoLock	al(m_csConts);
	MapDeptEmpIt	it = m_mapDeptEmp.end();
	PS_DeptEmp		psDEItem = (PS_DeptEmp)psItem;

	if( eGDIMethod == eGDIM_FIRST_CHILD )
	{
		if( psDEItem == NULL )
		{
			it = m_mapDeptEmp.find( IME_ID_NONE ); // Find the virtual root item
			if( it != m_mapDeptEmp.end() && it->second.dwFirstChildId != IME_ID_NONE )
				it = m_mapDeptEmp.find( it->second.dwFirstChildId );	// Get first ROOT item
		}
		else if ( psDEItem->dwFirstChildId != IME_ID_NONE )
			it = m_mapDeptEmp.find( psDEItem->dwFirstChildId );
		else
			return NULL;
	}
	else
	{ 
		if( psDEItem == NULL )
		{
			return NULL;		// Invalid parameter
		}

		if( eGDIMethod == eGDIM_PARENT )
		{
			if( psDEItem->dwParentId == IME_ID_NONE )
				return NULL;	// Already is ROOT

			it = m_mapDeptEmp.find( psDEItem->dwParentId );
		}
		else if( eGDIMethod == eGDIM_PREV )
		{
			if( psDEItem->dwPrevId == IME_ID_NONE )
				return NULL;	// Alread is First Item

			it = m_mapDeptEmp.find( psDEItem->dwPrevId );
		}
		else if( eGDIMethod == eGDIM_NEXT )
		{
			if( psDEItem->dwNextId == IME_ID_NONE )
				return NULL;	// Alread is Last Item

			it = m_mapDeptEmp.find( psDEItem->dwNextId );
		}
		else
		{
			return NULL;
		}
	}

	if( it == m_mapDeptEmp.end() )
		return NULL;

	return &it->second.sDeptInfo;
}

//=============================================================================
//Function:     GetDeptItem
//Description:	Get department information by department Id
//
//Parameter:
//	dwDeptId    - Department Id
//				  IME_ID_NONE: Get the first root department
//	eGDIMethod	- Method
//
//Return:
//      Return the Employee information if succeeded 
//		NULL otherwise
//=============================================================================
PS_DeptInfo_	C_eIMContacts::GetDeptItem(
	QDID		DeptId,
	E_GDIMethod eGDIMethod
	)
{
	C_EIMAutoLock al(m_csConts);
	MapDeptEmpIt it = m_mapDeptEmp.find( DeptId );

	if( DeptId == IME_ID_NONE &&
		it != m_mapDeptEmp.end() &&
		it->second.dwFirstChildId != IME_ID_NONE )
		it = m_mapDeptEmp.find( it->second.dwFirstChildId );	// Get first ROOT item

	if( it != m_mapDeptEmp.end() )
	{
		QDID didFind = IME_ID_NONE;
		if ( eGDIM_SELF == eGDIMethod )
			return &it->second.sDeptInfo;
		else if ( eGDIM_PARENT == eGDIMethod && it->second.dwParentId != IME_ID_NONE )
			didFind = it->second.dwParentId;
		else if ( eGDIM_FIRST_CHILD == eGDIMethod && it->second.dwFirstChildId != IME_ID_NONE )
			didFind = it->second.dwFirstChildId;
		else if ( eGDIM_PREV == eGDIMethod && it->second.dwPrevId != IME_ID_NONE )
			didFind = it->second.dwPrevId;
		else if ( eGDIM_NEXT == eGDIMethod && it->second.dwNextId != IME_ID_NONE )
			didFind = it->second.dwNextId;

		it = m_mapDeptEmp.find(didFind);
		if ( didFind == IME_ID_NONE || (it = m_mapDeptEmp.find(didFind)) == m_mapDeptEmp.end() )
		{
			const TCHAR* paszMethod[] = 
			{
				_T("eGDIM_SELF"),		// = 0,	// Get current QDID item
				_T("eGDIM_PARENT"),		// = 1,	// Get Parent item
				_T("eGDIM_FIRST_CHILD"),//,		// Get First child item
				_T("eGDIM_PREV"),		//,		// Get Prev item
				_T("eGDIM_NEXT"),		//,		// Get Next item
			};

			TRACE_(_T("Can not find QDID[%d]'s %s"), DeptId, paszMethod[eGDIMethod]);
			return NULL;
		}

		return &it->second.sDeptInfo;
	}

	return NULL;
}

//=============================================================================
//Function:     GetEmpName
//Description:	Get employee name for display
//
//Parameter:
//	EmpId    - Employee Id
//
//Return:
//      Return the Employee Name(or Code if name is NULL) if succeeded      
//		NULL otherwise
//=============================================================================
const TCHAR* C_eIMContacts::GetEmpName(
	QEID		EmpId
	)
{
	return GetEmpName(GetEmp(EmpId));
}
//=============================================================================
//Function:     GetEmp
//Description:	Get employee information by employee Id
//
//Parameter:
//	dwEmpId    - Employee Id
//
//Return:
//      Return the Employee information if succeeded      
//		NULL otherwise
//=============================================================================
PS_EmpInfo_	C_eIMContacts::GetEmp(
	QEID EmpId
	)
{
	CHECK_NULL_RET_(EmpId, FALSE);
	C_EIMAutoLock al(m_csConts);
	MapEmpIt it = m_mapEmp.find( EmpId );
	if ( it != m_mapEmp.end() )
		return it->second;

	TRACE_(_T("***Can not find qeid[%d]."), EmpId);
	return NULL;
}

//=============================================================================
//Function:     GetEmpDept
//Description:	Get employee's department
//
//Parameter:
//	dwEmpId    -  Employee Id
//
//Return:
//     Return the department informaiton if succeeded
//		NULL otherwise
//=============================================================================
PS_DeptInfo_ C_eIMContacts::GetEmpDept(
	QEID EmpId
	)
{
	C_EIMAutoLock al(m_csConts);
	MapEmpIt	it = m_mapEmp.find( EmpId );
	if ( it != m_mapEmp.end() )
	{
		MapDeptEmpIt it_ = m_mapDeptEmp.find( it->second->qdidPid[0] );
		if ( it_ != m_mapDeptEmp.end() )
		{
			return &it_->second.sDeptInfo;
		}
	}

	return NULL;
}

//=============================================================================
//Function:     EnumDepts
//Description:	Enumerate the departments
//
//Parameter:
//	psItem       - NULL: is Enumerate from root department, else is relative it.
//	i32Level     - 0: is only current level; 
//				  >0: is include sub-level numbers; 
//				  -1: all sub-level
//	pfnCB		 - Callback function to enum department, return FALSE to break
//	pvUserData	 - UserData, eg.: an container to save the result of enumerate, like VectDepts
//
//Return:
//		Return the enumerated count             
//=============================================================================
DWORD C_eIMContacts::EnumDepts( 
	PS_DeptInfo_	psItem,
	int				i32Level,
	PFN_DeptCb		pfnCb,
	void*			pvUserData
	)
{
	C_EIMAutoLock al(m_csConts);
	CHECK_NULL_RET_(pfnCb, 0);

	DWORD		dwCount  = 0;
	PS_DeptEmp	psDEItem = (PS_DeptEmp)psItem;
	MapDeptEmpIt it;
	if ( psDEItem == NULL )
	{	// from ROOT item
		it = m_mapDeptEmp.find( IME_ID_NONE );	// Virtual ROOT
		if ( it!= m_mapDeptEmp.end() )
			it = m_mapDeptEmp.find( it->second.dwFirstChildId ); // First Root Item

		for (;it!= m_mapDeptEmp.end();
			  it = m_mapDeptEmp.find( psDEItem->dwNextId ) )
		{
			dwCount++;
			psDEItem = &it->second;
			psDEItem->dwPurview = _CheckDeptPurview(&psDEItem->sDeptInfo);
			if ( (psDEItem->dwPurview & DEPT_INVISIBLE) || (psDEItem->dwPurview & DEPT_NO_ENUM) )
				continue;

			if (it->second.bitVisible && !pfnCb((PS_DeptInfo_)psDEItem, pvUserData) )
				break;

			if ( i32Level != 0 )
				dwCount += EnumDepts( &psDEItem->sDeptInfo, i32Level, pfnCb, pvUserData );

			if (psDEItem->dwNextId == IME_ID_NONE )
				break;	// Reach at tail
		}

		return dwCount;
	}

	if ( i32Level == 0 ||  psDEItem->dwFirstChildId == IME_ID_NONE )
		return dwCount;	// Stop

	// Enumerate child department
	for ( it = m_mapDeptEmp.find( psDEItem->dwFirstChildId );
		  it!= m_mapDeptEmp.end();
		  it = m_mapDeptEmp.find( psDEItem->dwNextId ) )
	{
		dwCount++;
		psDEItem = &it->second;
		if (it->second.bitVisible && !pfnCb((PS_DeptInfo_)psDEItem, pvUserData) )
			break;

		if ( i32Level - 1 != 0 )
			dwCount += EnumDepts( &psDEItem->sDeptInfo, i32Level - 1, pfnCb, pvUserData );

		if (psDEItem->dwNextId == IME_ID_NONE )
			break;	// Reach at tail
	}

	return dwCount;
}

bool CmpPinyin( UINT64 E1, UINT64 E2 )
{
	PS_EmpInfo_ psEmp1 = C_eIMContacts::GetObject().GetEmp(GET_QEID_(E1));
	PS_EmpInfo_ psEmp2 = C_eIMContacts::GetObject().GetEmp(GET_QEID_(E2));

	if (psEmp1 && psEmp2 && psEmp1->pszNamePinyin && psEmp2->pszNamePinyin)
		return strcmp(psEmp1->pszNamePinyin, psEmp2->pszNamePinyin) < 0;

	return true;
}

BOOL C_eIMContacts::_SortEmps(VectEmpId& vectEmpId)
{
	sort(vectEmpId.begin(), vectEmpId.end(), CmpPinyin);
	return TRUE;
}

//=============================================================================
//Function:     EnumEmps
//Description:	Enumerate the employees
//
//Parameter:
//	psItem       - NULL: is Enumerate from root department, else is relative it.
//	i32Level       0: is only current department's member; 
//				  >0: is include sub-department's level; 
//				  -1: all sub-level
//	pfnCB		 - Callback function to enum employee, return FALSE to break
//	pvUserData	 - UserData, eg.: an container to save the result of enumerate, like VectEmps
//
//Return:
//		Return the enumerated count             
//=============================================================================
DWORD C_eIMContacts::EnumEmps( 
	PS_DeptInfo_	psItem,
	int				i32Level,
	PFN_EmpCbEx		pfnCb,
	void*			pvUserData
	)
{
	CHECK_NULL_RET_(pfnCb, 0);
	C_EIMAutoLock al(m_csConts);

	DWORD		dwCount  = 0;
	PS_DeptEmp	psDEItem = (PS_DeptEmp)psItem;
	MapDeptEmpIt it;
	if ( psDEItem == NULL )
	{	// from ROOT item
		it = m_mapDeptEmp.find( IME_ID_NONE );	// Virtual ROOT
		if ( it!= m_mapDeptEmp.end() )
			it = m_mapDeptEmp.find( it->second.dwFirstChildId ); // First Root Item

		for (; it!= m_mapDeptEmp.end();
			  it = m_mapDeptEmp.find( psDEItem->dwNextId ) )
		{
			psDEItem = &it->second;
			dwCount += EnumEmps( &psDEItem->sDeptInfo, i32Level, pfnCb, pvUserData );
			if (psDEItem->dwNextId == IME_ID_NONE )
				break;	// Reach at tail
		}

		return dwCount;
	}

	if ( psDEItem->vectEmpId.size() > 0 && psDEItem->bitVisible == 2)
	{	// Exist employees
		if ( psDEItem->bitSorted == 0 )
		{
			psDEItem->bitSorted = 1;
			_SortEmps(psDEItem->vectEmpId);
		}

		DWORD    dwPyIndex = 0;
		MapEmpIt it2;
		for ( VectEmpIdIt it_ = psDEItem->vectEmpId.begin();
			  it_ != psDEItem->vectEmpId.end();
			  it_++, dwPyIndex++ )
		{
			it2 = m_mapEmp.find( GET_QEID_(*it_) );
			if( it2 != m_mapEmp.end() )
			{
				it2->second->dwPurview = _CheckEmpPurview(it2->second);
				if ( (it2->second->dwPurview & EMP_INVISIBLE) || (it2->second->dwPurview & EMP_NO_ENUM) )
					continue;

				dwCount++;
				if (!pfnCb(it2->second, pvUserData, dwPyIndex, GET_ORDER_(*it_)) )
					return dwCount;
			}
			else
			{
				ASSERT_(FALSE);	
			}
		}
	}

	if ( i32Level == 0 ||  psDEItem->dwFirstChildId == IME_ID_NONE )
		return dwCount;	// Stop

	// Enumerate child department
	for ( it = m_mapDeptEmp.find( psDEItem->dwFirstChildId );
		  it!= m_mapDeptEmp.end();
		  it = m_mapDeptEmp.find( psDEItem->dwNextId ) )
	{
		psDEItem = &it->second;
		dwCount += EnumEmps( &psDEItem->sDeptInfo, i32Level - 1, pfnCb, pvUserData );
		if (psDEItem->dwNextId == IME_ID_NONE )
			break;	// Reach at tail
	}
	
	return dwCount;
}

DWORD C_eIMContacts::_GetOnline(
	MapEmp&		mapEmps, 	
	DWORD*		pdwEmpCount,			
	PFN_EmpCb	pfnEmpCb,
	void*		pvUserData
	)
{
	*pdwEmpCount = 0;
	DWORD dwOnlineCount = 0;
	for(MapEmpIt it = mapEmps.begin(); it != mapEmps.end(); it++)
	{
		it->second->dwPurview = _CheckEmpPurview(it->second);
		if ( (it->second->dwPurview & EMP_INVISIBLE) || (it->second->dwPurview & EMP_NO_ENUM) )
			continue;

		if ( it->second->bitVisible )
		{
			(*pdwEmpCount)++;
			if ( it->second->bitStatus > IME_EMP_STATUS_OFFLINE )
			{
				dwOnlineCount++;
				if (pfnEmpCb && !pfnEmpCb(it->second, pvUserData, 0))
					break;
			}
		}
	}

	return dwOnlineCount;
}

BOOL C_eIMContacts::_GetDeptEmps(
	MapEmp&		mapEmps, 
	PS_DeptEmp	psItem,
	int			i32Level 
	)
{
	CHECK_NULL_RET_(psItem, FALSE);
	PS_EmpInfo psEmpInfo = NULL;

	for(VectEmpIdIt EmpIt = psItem->vectEmpId.begin(); EmpIt != psItem->vectEmpId.end(); EmpIt++)
	{
		psEmpInfo = (PS_EmpInfo)GetEmp(GET_QEID_(*EmpIt));
		if ( psEmpInfo )
			mapEmps.insert(PairEmp(GET_QEID_(*EmpIt), psEmpInfo));
	}

	if ( i32Level == 0 || psItem->dwFirstChildId == IME_ID_NONE )
		return TRUE;

	// Get child 
	MapDeptEmpIt it = m_mapDeptEmp.find( psItem->dwFirstChildId );
	while ( it != m_mapDeptEmp.end() )
	{
		_GetDeptEmps( mapEmps, (PS_DeptEmp)&it->second.sDeptInfo, i32Level - 1 );
		if ( it->second.dwNextId == IME_ID_NONE )
			return TRUE;	// Reach at tail

		it = m_mapDeptEmp.find( it->second.dwNextId );		// Get the next item
	}

	return TRUE;
}
//=============================================================================
//Function:     GetOnline
//Description:	Get online employee count and employee count
//
//Parameter:
//	psItem        - NULL: is Enumerate from root department(for Get entire company online status ), else is relative it.
//	pdwEmpCount   - Return the employee count
//				   Caller zero it at first
//	i32Level       0: is only current department; 
//				  >0: is include sub-department level ; 
//				  -1: all sub-level
//	pfnCB		 - Callback function to enum online employee, return FALSE to break
//	pvUserData	 - UserData, eg.: an container to save the result of enumerate, like VectEmps
//
//Return:
//       Return the online employee count       
//=============================================================================
DWORD C_eIMContacts::GetOnline(
	PS_DeptInfo_	psItem,
	DWORD*			pdwEmpCount,			
	int				i32Level, 
	PFN_EmpCb		pfnEmpCb,
	void*			pvUserData
	)
{
	C_EIMAutoLock al(m_csConts);
	if ( psItem == NULL )
		return _GetOnline(m_mapEmp, pdwEmpCount, pfnEmpCb, pvUserData);

	MapEmp	mapEmp;
	_GetDeptEmps(mapEmp, (PS_DeptEmp)psItem, i32Level);
	return _GetOnline(mapEmp, pdwEmpCount, pfnEmpCb, pvUserData);
}

//=============================================================================
//Function:     SetSearchDeptsCache
//Description:	Set the search department cache by the result
//
//Parameter:
//	vectDepts    - Current searched result
//	pszCond      - Current search condition
//
//Return:
//      TRUE: Cached searched result
//		FALSE: Cleared cached searched result
//=============================================================================
BOOL C_eIMContacts::_SetSearchDeptsCache(
	VectDepts&	vectDepts, 
	const TCHAR* pszCond
	)
{
	if( vectDepts.size() > 0 )
	{	// Cached it
		_tcsncpy(m_szSearchDeptsCond, pszCond, IME_SEARCH_MAX_SIZE -1);
		m_vectSearchDeptsCache.swap(vectDepts);
	}
	else
	{
		m_szSearchDeptsCond[0] = _T('\0');
		m_vectSearchDeptsCache.clear();
	}

	return m_vectSearchDeptsCache.size() > 0;
}

BOOL C_eIMContacts::_CompareDeptsCond(
	PS_DeptEmp_	psDeptEmp,
	const TCHAR* ptszCond,
	const char*  pszCond
	)
{
	CHECK_NULL_RET_(ptszCond, FALSE);
	CHECK_NULL_RET_(psDeptEmp, FALSE);

	if ( psDeptEmp->bitVisible == 0 )
		return FALSE;
	
	((PS_DeptEmp)psDeptEmp)->dwPurview = _CheckDeptPurview(&psDeptEmp->sDeptInfo);
	if ( (psDeptEmp->dwPurview & DEPT_INVISIBLE) || (psDeptEmp->dwPurview & DEPT_NO_SEARCH) )
		return FALSE;

	if ( pszCond && psDeptEmp->pszNamePinyinS && StrStrIA(psDeptEmp->pszNamePinyinS, pszCond) )
		return TRUE;

	if ( psDeptEmp->sDeptInfo.pszName && _tcsstr( psDeptEmp->sDeptInfo.pszName, ptszCond) )
		return TRUE;

	if ( psDeptEmp->sDeptInfo.pszNameEn && StrStrI( psDeptEmp->sDeptInfo.pszNameEn, ptszCond) )
		return TRUE;

	return FALSE;
}

//=============================================================================
//Function:     SearchDepts
//Description:	Search departments
//
//Parameter:
//	pszCond		 - Search condition
//	pfnCB		 - Callback function to search department, return FALSE to break
//	pvUserData	 - UserData, eg.: an container to save the result of search, like VectDepts
//
//Return:
//      Return the count of searched result        
//=============================================================================
DWORD C_eIMContacts::SearchDepts(
	const TCHAR* pszCond,
	PFN_DeptCb	pfnCb,
	void*		pvUserData
	)
{
	C_EIMAutoLock al(m_csConts);
	if( pfnCb == NULL || pszCond == NULL || pszCond[0] == _T('\0') )
		return 0;

	VectDepts vectDepts;
	// Convert to byte data if possible
	char	szSearchDeptsCond[IME_SEARCH_MAX_SIZE] = { '\0' };
	BOOL	bSearchAscii = _CanSearchAscii( pszCond, szSearchDeptsCond );

	if( m_szSearchDeptsCond[0] &&
		_tcsncicmp(pszCond, m_szSearchDeptsCond, _tcslen( m_szSearchDeptsCond )) == 0 &&
		m_vectSearchDeptsCache.size() > 0 )
	{	// Search from last time cached result
		VectSearchDeptsCacheIt it = m_vectSearchDeptsCache.begin();
		for ( ; it != m_vectSearchDeptsCache.end(); it++ )
		{
			if (!_CompareDeptsCond((PS_DeptEmp)(*it), pszCond, bSearchAscii ? szSearchDeptsCond : NULL ) )
				continue;

			vectDepts.push_back(&((PS_DeptEmp)(*it))->sDeptInfo);
			if ( !pfnCb((*it), pvUserData) )
				return vectDepts.size();
		}
	}
	else
	{	// Search from memory table
		MapDeptEmpIt it = m_mapDeptEmp.begin();
		for ( ; it != m_mapDeptEmp.end(); it++ )
		{
			if (!_CompareDeptsCond(&it->second, pszCond, bSearchAscii ? szSearchDeptsCond : NULL ))
				continue;

			vectDepts.push_back(&it->second.sDeptInfo);
			if (!pfnCb(&it->second.sDeptInfo, pvUserData) )
				return vectDepts.size();
		}
	}
	
	_SetSearchDeptsCache( vectDepts, pszCond );

	return vectDepts.size();
}

//=============================================================================
//Function:     _SetSearchEmpsCache
//Description:	Set search employee cache for speed up
//
//Parameter:
//	vectEmps    - Current searched employee result to be cached
//	pszCond     - Current searched condition
//
//Return:
//      TRUE:  Cached the search result
//		FALSE: Cleared the search result
//=============================================================================
BOOL C_eIMContacts::_SetSearchEmpsCache(
	VectEmps&	vectEmps, 
	const TCHAR* pszCond
	)
{
	if( vectEmps.size() > 0 )
	{	// Cached it
		_tcsncpy(m_szSearchEmpsCond, pszCond, IME_SEARCH_MAX_SIZE -1);
		m_vectSearchEmpsCache.swap(vectEmps);
	}
	else
	{
		m_szSearchEmpsCond[0] = _T('\0');
		m_vectSearchEmpsCache.clear();
	}

	return m_vectSearchEmpsCache.size() > 0;
}

//=============================================================================
//Function:     _CompareAddEmps
//Description:	Compare and add to searched result if possible
//
//Parameter:
//	vectEmps           - Return the search result
//	psEmpInfo          - Current Employee information
//	pszCond            - Search condition of type TCHAR
//	pu8Cond            - Search condition of type BYTE
//	bSearchByteData    - Whether search pu8Cond or not
//
//Return:
//		>0:  Added to search result
//		0: No add to search result
//=============================================================================
DWORD C_eIMContacts::_CompareEmpsCond(
	PS_EmpInfo_	psEmpInfo,
	const TCHAR* ptszCond,
	const char*	 pszCond,
	DWORD		dwFlag
	)
{
#define RET_MATCHING_RATIO_T_(C, F) return ((F)[0] ? (DWORD)(100.0f * _tcslen(C) / _tcslen(F)) : 0) 
#define RET_MATCHING_RATIO_A_(C, F) return ((F)[0] ? (DWORD)(100.0f *  strlen(C) /  strlen(F)) : 0) 

	CHECK_NULL_RET_(ptszCond, FALSE);
	CHECK_NULL_RET_(psEmpInfo, FALSE);

	if ( psEmpInfo->bitVisible == 0 )
		return 0;
	
	((PS_EmpInfo)psEmpInfo)->dwPurview = _CheckEmpPurview(psEmpInfo);
	if ( (psEmpInfo->dwPurview & EMP_INVISIBLE) || (psEmpInfo->dwPurview & EMP_NO_SEARCH) )
		return 0;

	if(IME_SEARCH_EMP_BY_UCODE == dwFlag)
	{
		if ((psEmpInfo->pszCode && StrStrI( psEmpInfo->pszCode, ptszCond )))
			RET_MATCHING_RATIO_T_(ptszCond, psEmpInfo->pszCode);

		return 0;
	}

	if(IME_SEARCH_EMP_BY_PHONE == dwFlag)
	{
		if ((psEmpInfo->pszPhone  && StrStrI( psEmpInfo->pszPhone, ptszCond )))
			RET_MATCHING_RATIO_T_(ptszCond, psEmpInfo->pszPhone);

		return 0;
	}

	if (psEmpInfo->pszCode && StrStrI( psEmpInfo->pszCode, ptszCond ))
		RET_MATCHING_RATIO_T_(ptszCond, psEmpInfo->pszCode);

	if (psEmpInfo->pszName && StrStrI( psEmpInfo->pszName, ptszCond ))
		RET_MATCHING_RATIO_T_(ptszCond, psEmpInfo->pszName);

	if (psEmpInfo->pszEnName && StrStrI( psEmpInfo->pszEnName, ptszCond ))
		RET_MATCHING_RATIO_T_(ptszCond, psEmpInfo->pszEnName);	

	if (  pszCond && psEmpInfo->pszNamePinyinS && StrStrIA(psEmpInfo->pszNamePinyinS, pszCond) )
		RET_MATCHING_RATIO_A_(pszCond, psEmpInfo->pszNamePinyinS);	
	
	if ( dwFlag == IME_SEARCH_EMP_DEFAULT_FLAG )
		return 0;

	// IME_SEARCH_EMP_SEARCHWND_FLAG
	if (psEmpInfo->pszEmail && StrStrI( psEmpInfo->pszEmail, ptszCond )) 
		RET_MATCHING_RATIO_T_(ptszCond, psEmpInfo->pszEmail);

	if (psEmpInfo->pszPhone && StrStrI( psEmpInfo->pszPhone, ptszCond ))
		RET_MATCHING_RATIO_T_(ptszCond, psEmpInfo->pszPhone);

	if (psEmpInfo->pszTel   && StrStrI( psEmpInfo->pszTel,  ptszCond ))
	   RET_MATCHING_RATIO_T_(ptszCond, psEmpInfo->pszTel);

	if (psEmpInfo->pszDuty  && StrStrI( psEmpInfo->pszDuty, ptszCond ))
	   RET_MATCHING_RATIO_T_(ptszCond, psEmpInfo->pszDuty);

	return 0;
}

//=============================================================================
//Function:     _CanSearchAscii
//Description:	Check and convert to ASCII search condition
//
//Parameter:
//	pszCond             - Search condition
//	szSearchCond		- Return ascii search condition
//
//Return:
//      TRUE	- Can be search as Ascii
//		FALSE	- Can not be search as ascii
//=============================================================================
BOOL C_eIMContacts::_CanSearchAscii(const TCHAR* pszCond, char szSearchCond[IME_SEARCH_MAX_SIZE] )
{
	for( int i32Index = 0; pszCond[i32Index] && i32Index < IME_SEARCH_MAX_SIZE;	i32Index++)
	{
		if ( pszCond[i32Index] >= 0x20 &&	// _T(' ')
			 pszCond[i32Index] <= 0x7E )	// _T('~')
			szSearchCond[i32Index] = (char)pszCond[i32Index];
		else
			return FALSE;
	}

	return TRUE;
}

//=============================================================================
//Function:     SearchEmps
//Description:	Search employees
//
//Parameter:
//	pszCond     - Search condition
//	pfnCB		- Callback function to search employee, return FALSE to break
//	pvUserData	- UserData, eg.: an container to save the result of search, like VectEmps
//
//Return:
//      Return the count of searched result        
//=============================================================================
DWORD C_eIMContacts::SearchEmps(
	const TCHAR* pszCond,
	PFN_EmpCb	pfnCb,
	void*		pvUserData,
	BYTE		byteFlag
	)
{
	C_EIMAutoLock al(m_csConts);
	if( pfnCb == NULL || pszCond == NULL || pszCond[0] == _T('\0') )
		return 0;

	VectEmps vectEmps;	// temp for cache

	// Convert to byte data if possible
	char	szSearchEmpsCond[IME_SEARCH_MAX_SIZE] = { '\0' };
	BOOL	bSearchByteData = _CanSearchAscii( pszCond, szSearchEmpsCond );
	DWORD   dwMatchRatio = 0;

	// Do search
	if( m_szSearchEmpsCond[0] &&
		_tcsncicmp(pszCond, m_szSearchEmpsCond, _tcslen( m_szSearchEmpsCond )) == 0 &&
		m_vectSearchEmpsCache.size() > 0 )
	{	// Search employee from cached result
		VectSearchEmpsCacheIt it = m_vectSearchEmpsCache.begin();
		for ( ; it != m_vectSearchEmpsCache.end(); it++ )
		{
			dwMatchRatio = _CompareEmpsCond( *it, pszCond, bSearchByteData ? szSearchEmpsCond : NULL, byteFlag );
			if (dwMatchRatio == 0)
				continue;

			vectEmps.push_back( *it);
			if ( !pfnCb(*it, pvUserData, dwMatchRatio) )
				return vectEmps.size();
		}
	}
	else
	{	// Search employee from memory table
		MapEmpIt it = m_mapEmp.begin();
		for ( ; it != m_mapEmp.end(); it++ )
		{
			dwMatchRatio = _CompareEmpsCond(it->second, pszCond, bSearchByteData ? szSearchEmpsCond : NULL, byteFlag);
			if ( dwMatchRatio == 0 )
				continue;

			vectEmps.push_back(it->second);
			if ( !pfnCb(it->second, pvUserData, dwMatchRatio) )
				return vectEmps.size();
		}
	}

	_SetSearchEmpsCache( vectEmps, pszCond );

	return vectEmps.size();
}

DWORD C_eIMContacts::GetLoadEmpTimestamp()
{
	return m_dwLoadEmpTimestamp;
}

DWORD C_eIMContacts::SetLoadEmpTimestamp(DWORD dwTime)
{
	DWORD dwOldTime = m_dwLoadEmpTimestamp;
	m_dwLoadEmpTimestamp = dwTime;
	return dwOldTime;
}


DWORD C_eIMContacts::GetLoadContactTimestamp(BOOL bDept)
{
	return bDept ? m_dwLoadContactDeptTimestamp : m_dwLoadContactDeptUserTimestamp;
}

DWORD C_eIMContacts::SetLoadContactTimestamp(DWORD dwTime, BOOL bDept)
{
	if ( bDept )
		m_dwLoadContactDeptTimestamp = dwTime;
	else
		m_dwLoadContactDeptUserTimestamp = dwTime;

	return dwTime;
}

//=============================================================================
//Function:     UpdateEmpsState
//Description:	
//
//Parameter:
//	dwModify    - eIMECSET_MODS or eIMECSET_MODS_CONCERN or eIMECSET_MODS_UPDATE_LOGO
//	dwValue     - eIMECSET_MODS: only support IME_EMP_STATUS_OFFLINE now
//				  eIMECSET_MODS_CONCERN: is a bool value
//				  eIMECSET_MODS_UPDATE_LOGO: is LogTime
//Return:
//	TRUE	- Succeeded
//  FALSE	- Failed
//=============================================================================
BOOL C_eIMContacts::UpdateEmpsState(DWORD dwModify, DWORD dwValue)
{
	C_EIMAutoLock al(m_csConts);

	TRACE_(_T("Begin modify status[0x%08X] to %d"), dwModify, dwValue);
	for ( MapEmpIt it = m_mapEmp.begin(); it != m_mapEmp.end(); it++ )
	{
		if (  it->second == NULL )
			continue;

		switch(dwModify)
		{
		case eIMECSET_MODS:	// Now only dealwith all to offline
			if ( dwValue == IME_EMP_STATUS_OFFLINE &&
				(it->second->bitStatus == IME_EMP_STATUS_ONLINE || 
				it->second->bitStatus == IME_EMP_STATUS_LEAVE) )
			{
				it->second->bitStatus = IME_EMP_STATUS_OFFLINE;
				m_pIEIMEventMgr->SendEvent( EVENT_EMP_MODS, it->second );
			}
			break;
		case eIMECSET_MODS_CONCERN:
			it->second->bitConcern = !!dwValue;
			break;
		case eIMECSET_MODS_UPDATE_LOGO:
			it->second->dwLogoTime = dwValue;
			break;
		default:
			break;
		}
	}

	TRACE_(_T("Begin modify status[0x%08X] to %d"), dwModify, dwValue);
	return TRUE;
}

const TCHAR* C_eIMContacts::GetDeptName(QDID did)
{
	if ( PS_DeptInfo_ psDInfo = GetDeptItem(did, eGDIM_SELF) )
		return GetDeptName(psDInfo);

	return NULL;
}

const TCHAR*  C_eIMContacts::GetDeptName(PS_DeptInfo_ psDeptInfo)
{
	CHECK_NULL_RET_(psDeptInfo, _T(NULL_STR));
	switch ( m_i32Language )
	{
	case 1:
		return psDeptInfo->pszName;
	case 0:
	default:
		if ( psDeptInfo->pszNameEn == NULL ||psDeptInfo->pszNameEn[0] == _T('\0') )
			return psDeptInfo->pszName;
		else
			return psDeptInfo->pszNameEn;
	}
}

const TCHAR*  C_eIMContacts::GetEmpName(PS_EmpInfo_ psEmpInfo)
{
	CHECK_NULL_RET_(psEmpInfo, _T(NULL_STR));
	switch ( m_i32Language )
	{
	case 1:
		if ( psEmpInfo->pszName == NULL || psEmpInfo->pszName[0] == _T('\0') )
			return eIMAddString_(psEmpInfo->pszCode);
		else
			return eIMAddString_(psEmpInfo->pszName);
	case 0:
	default:
		if ( psEmpInfo->pszEnName == NULL || psEmpInfo->pszEnName[0] == _T('\0') )
			return eIMAddString_(psEmpInfo->pszCode);
		else
			return eIMAddString_(psEmpInfo->pszEnName);
	}
}

int C_eIMContacts::SetLanguage(int i32Language)
{
	int i32OldLanguage = m_i32Language;
	m_i32Language = i32Language;
	return i32OldLanguage;
}

BOOL C_eIMContacts::FindEmp(QEID EmpId)
{
	CHECK_NULL_RET_(EmpId, FALSE);
	C_EIMAutoLock al(m_csConts);
	MapEmpIt it = m_mapEmp.find( EmpId );
	if ( it != m_mapEmp.end() )
		return TRUE;

	TRACE_(_T("***Can not find qeid[%d]."), EmpId);
	return FALSE;
}
	
BOOL C_eIMContacts::SetDeptCallback(PFN_CheckDeptPurviewCb pfnCb, void* pvUserData)
{
	m_pvCheckDeptPurviewUserData = pvUserData;
	m_pfnCheckDeptPurviewCb = pfnCb;
	return TRUE;
}
	
BOOL C_eIMContacts::SetEmpCallback(PFN_CheckEmpPurviewCb pfnCb, void* pvUserData)
{	
	m_pvCheckEmpPurviewUserData = pvUserData;
	m_pfnCheckEmpPurviewCb = pfnCb;
	return TRUE;
}

inline DWORD C_eIMContacts::_CheckDeptPurview(PS_DeptInfo_ psDeptInfo)
{
	if ( m_pfnCheckDeptPurviewCb )
		return m_pfnCheckDeptPurviewCb(psDeptInfo, m_pvCheckDeptPurviewUserData);

	return 0;	// 无特殊权限
}

inline DWORD C_eIMContacts::_CheckEmpPurview(PS_EmpInfo_ psEmpInfo)
{
	if ( m_pfnCheckEmpPurviewCb )
		return m_pfnCheckEmpPurviewCb(psEmpInfo, m_pvCheckEmpPurviewUserData);

	return 0;	// 无特殊权限
}

DWORD C_eIMContacts::CheckDeptPurview(PS_DeptInfo_ psDeptInfo)
{
	return _CheckDeptPurview(psDeptInfo);
}

DWORD C_eIMContacts::CheckEmpPurview(PS_EmpInfo_ psEmpInfo)
{
	return _CheckEmpPurview(psEmpInfo);
}

DWORD C_eIMContacts::CheckDeptPurview(QDID did)
{
	if ( PS_DeptInfo_ psDeptInfo = GetDeptItem(did, eGDIM_SELF) )
		return CheckDeptPurview(psDeptInfo);

	return 0;
}

DWORD C_eIMContacts::CheckEmpPurview(QEID eid)
{
	if ( PS_EmpInfo_ psEmpInfo = GetEmp(eid) )
		return CheckEmpPurview(psEmpInfo);

	return 0;
}