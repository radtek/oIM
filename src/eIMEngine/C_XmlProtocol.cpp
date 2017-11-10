#include "stdafx.h"
#include "C_XmlProtocol.h"

#define CHECK_ISOPEN_RET_(RET) \
	do { \
		if ( !IsOpen() ) \
		{ \
			TRACE_( _T("Protocol configuration file is not opened!") ); \
			return 0; \
		} \
	}while(0)

IMPLEMENT_PLUGIN_SINGLETON_(C_XmlProtocol, INAME_EIMENGINE_PROTOCOL);
C_XmlProtocol::C_XmlProtocol(void)
{
}

C_XmlProtocol::~C_XmlProtocol(void)
{
	Close();
}

C_XmlProtocol& C_XmlProtocol::GetObject()
{
	static C_XmlProtocol obj;
	return obj;
}

//=============================================================================
//Function:     _ParseUnit
//Description:	Parse unit string to E_PFieldType, The unit string MUST be less 
// than or equal to three letter.
//
//Parameter:
//	pszValue    - Unit string
//
//Return:
//    E_PFieldType           
//=============================================================================
E_PFieldType C_XmlProtocol::_ParseUnit( const TCHAR* pszValue, unsigned __int64& u64DefaultMax )
{
	if ( pszValue == NULL || pszValue[0] == _T('\0') )
		return ePFTYPE_UNKNOWN;

	DWORD u32Code = 0;
	for ( int i32Index = 0; pszValue[i32Index] && i32Index < sizeof(u32Code); i32Index++)
	{	// Convert to DWORD for speed up(String compare ==> switch )
		u32Code <<= 8;
		u32Code  |=  _totupper(pszValue[i32Index]) & 0xFF;
	}

	u64DefaultMax = 0;
	switch ( u32Code )
	{
	case 'A':
		return ePFTYPE_ASCII;
	case 'T8':
		return ePFTYPE_UTF8;
	case 'T16':
		return ePFTYPE_UTF16;
	case 'I8':
		u64DefaultMax = SCHAR_MAX;
		return ePFTYPE_I8;
	case 'I16':
		u64DefaultMax = SHRT_MAX;
		return ePFTYPE_I16;
	case 'I32':
		u64DefaultMax = INT_MAX; 
		return ePFTYPE_I32;
	case 'I64':
		u64DefaultMax = _I64_MAX; 
		return ePFTYPE_I64;
	case 'U8':
		u64DefaultMax = UCHAR_MAX; 
		return ePFTYPE_U8;
	case 'U16':
		u64DefaultMax = USHRT_MAX; 
		return ePFTYPE_U16;
	case 'U32':
		u64DefaultMax = UINT_MAX; 
		return ePFTYPE_U32;
	case 'U64':
		u64DefaultMax = _UI64_MAX; 
		return ePFTYPE_U64;
	case 'F':
		return ePFTYPE_FLOAT;
	case 'D':
		return ePFTYPE_DOUBLE;
	default:
		TRACE_(_T("Unknown unit \'%s\'"), pszValue );
		return ePFTYPE_UNKNOWN;
	}
}

//=============================================================================
//Function:     _GetRefId
//Description:	Get reference Id of group, Group must be defined before Cmd
//
//Parameter:
//	pszName    - Name of reference of group
//
//Return:
//      Return the reference Id        
//=============================================================================
int C_XmlProtocol::_GetRefId( const TCHAR* pszName )
{
	if ( pszName == NULL || pszName[0] == _T('\0') )
		return PROTOCOL_ID_INVALID;

	for ( MapPCmdIt it = m_mapPCmd.begin(); it != m_mapPCmd.end(); it++)
	{
		if ( _tcsnicmp(pszName, it->second->szName, TYPE_ELEMENT_NAME_SIZE ) == 0 )
			return it->second->i32Id;
	}

	return PROTOCOL_ID_INVALID;
}

//=============================================================================
//Function:     _ParseCmd
//Description:	Parse Cmd or Group
//
//Parameter:
//	pElement    - Cmd or Group element
//	psPCmd      - Return the parsed result
//
//Return:
//		TRUE	- Succeeded
//		FALSE	- Failed
//=============================================================================
BOOL C_XmlProtocol::_ParseCmd( TiXmlElement* pElement, PS_PCmd psPCmd )
{
	if ( pElement == NULL || psPCmd == NULL )
		return FALSE;

	int i32Id = 0;
	eIMString   szValue;
	const char* pszValue = NULL;

	SET_UTF8_2TVALUE_(pElement->Attribute(PROTOCOL_ATTRIB_NAME), psPCmd->szName);
	if ( szValue.size() == 0 )
		return FALSE;

	pszValue	  = pElement->Attribute(PROTOCOL_ATTRIB_ID);
	psPCmd->i32Id = pszValue ? (int)strtoul(pszValue, NULL, 0) : PROTOCOL_ID_INVALID;
	ASSERT_( psPCmd->i32Id != PROTOCOL_ID_INVALID );
	if ( psPCmd->i32Id == PROTOCOL_ID_INVALID )
		return FALSE;

	int i32Index = 0;
	int i32Default = 0;
	unsigned __int64 u64DefaultMax = 0;
	pElement = pElement->FirstChildElement();
	while ( pElement )
	{	// 	<Ref   Name="GroupName" Min=0 Max=10 />
		//	<Field Name="UserID" 	Unit="U32"  Min=0 Max=0xFFFFFFFF />
		pszValue = pElement->Value();
		ASSERT_( pszValue );
		if ( pszValue == NULL )
			return FALSE;

		// Pseudo Id or Unit
		if ( _stricmp(pszValue, PROTOCOL_ELEMENT_REF ) == 0 )	// Pseudo Id
		{
			i32Default = 1;	// Reference default is 1
			u64DefaultMax = 1;
			::eIMUTF8ToTChar(pElement->Attribute( PROTOCOL_ATTRIB_NAME ), szValue);
			psPCmd->sField[i32Index].eFieldType = (E_PFieldType)_GetRefId( szValue.c_str() );
		}
		else	// Unit
		{
			::eIMUTF8ToTChar(pElement->Attribute(PROTOCOL_ATTRIB_UNIT), szValue);
			psPCmd->sField[i32Index].eFieldType = _ParseUnit( szValue.c_str(), u64DefaultMax );
		}

		ASSERT_( psPCmd->sField[i32Index].eFieldType != ePFTYPE_UNKNOWN );

		// Name
		SET_UTF8_2TVALUE_(pElement->Attribute( PROTOCOL_ATTRIB_NAME ), psPCmd->sField[i32Index].szName);

		// Min, Max
		if ( psPCmd->sField[i32Index].eFieldType == ePFTYPE_FLOAT ||
			 psPCmd->sField[i32Index].eFieldType == ePFTYPE_DOUBLE )
		{
			double dValue = 0.0;
			
			pElement->Attribute(PROTOCOL_ATTRIB_MIN, &dValue);
			psPCmd->sField[i32Index].dMin = dValue;

			dValue = (psPCmd->sField[i32Index].eFieldType == ePFTYPE_FLOAT) ? FLT_MAX : DBL_MAX;
			pElement->Attribute(PROTOCOL_ATTRIB_MAX, &dValue);
			psPCmd->sField[i32Index].dMax = dValue;
		}
		else
		{
			pszValue = pElement->Attribute(PROTOCOL_ATTRIB_MIN);
			psPCmd->sField[i32Index].u64Min = pszValue ? (DWORD)_strtoui64(pszValue, NULL, 0) : i32Default;

			pszValue =  pElement->Attribute(PROTOCOL_ATTRIB_MAX);
			psPCmd->sField[i32Index].u64Max = pszValue ? (DWORD)_strtoui64(pszValue, NULL, 0) : u64DefaultMax;
		}

		i32Index++;
		pElement = pElement->NextSiblingElement();
	}
		
	return TRUE;
}

//=============================================================================
//Function:     _Parse
//Description:	Parse protocol config file as internal format
//
//Parameter:
//	xmlDoc    - XML document object
//
//Return:
//		TRUE  - Succeeded
//		FALSE - Failed
//=============================================================================
BOOL C_XmlProtocol::_Parse( TiXmlDocument& xmlDoc )
{
	int		i32Count = 0;
	int		i32Size  = 0;
	PS_PCmd	psPCmd	 = NULL;
	TiXmlElement* pElement = NULL;
	TiXmlElement* pElementRoot = _GetElementByPath( xmlDoc, _T(PROTOCOL_ELEMENT_ROOT));
	if ( pElementRoot == NULL )
		return FALSE;

	pElement = pElementRoot->FirstChildElement();
	while ( pElement )
	{
		i32Count = _GetSubElementCount( pElement, NULL );				// Get sub element count
		i32Size  = sizeof(S_PCmd) + sizeof(S_PField) * (i32Count - 1);	// Alloc the buffer
		psPCmd   = (PS_PCmd)new BYTE[ i32Size ];
		if ( psPCmd == NULL )
			return FALSE;

		memset( (void*)psPCmd, 0, i32Size );
		psPCmd->i32Count = i32Count;					// Set Field item count
		if ( !_ParseCmd( pElement, psPCmd ) )
		{
			delete psPCmd;
			return FALSE;
		}

		ASSERT_( m_mapPCmd.find( psPCmd->i32Id ) == m_mapPCmd.end() );	// Assert error config
		m_mapPCmd.insert( MapPCmd::value_type(psPCmd->i32Id, psPCmd) );	// Save

		pElement = pElement->NextSiblingElement();		// Next Group or Cmd
	}

	return TRUE;
}

//=============================================================================
//Function:     Open
//Description:	Open protocol config file
//
//Parameter:
//	lpszFile    - Protocol config file
//
//Return:
//  See error code of EIMERR_*  
//  EIMERR_NO_ERROR			Succeed
//	EIMERR_INVALID_PARAM	Invalid parameter 
//  EIMERR_FILE_NOT_EXIST	File not exist
//  EIMERR_OPEN_FILE_FAILED	Open failed
//=============================================================================
int C_XmlProtocol::Open(LPCTSTR lpszFile, LPCTSTR /*lpszRediFile*/ )
{
	int i32Ret = C_XmlConfiger::Open( lpszFile, NULL );
	if ( i32Ret == EIMERR_NO_ERROR && _Parse(m_XmlConfiger) )
		return i32Ret;

	Close();	// Close it when occur error
	return ( i32Ret != EIMERR_NO_ERROR ) ? i32Ret : EIMERR_OPEN_FILE_FAILED;
}

//=============================================================================
//Function:     Close
//Description:	Close protocol config file
//
//
//Return:
//		TRUE	- Succeeded
//		FALSE	- Failed
//=============================================================================
BOOL C_XmlProtocol::Close()
{
	C_XmlConfiger::Close();

	for ( MapPCmdIt it = m_mapPCmd.begin(); it != m_mapPCmd.end(); it++ )
	{
		SAFE_DELETE_PTR_( it->second );
	}

	m_mapPCmd.clear();
	
	return TRUE;
}

int C_XmlProtocol::GetSubElementCount( PCTSTR lpszPath, LPCTSTR pszName )
{
	CHECK_ISOPEN_RET_( 0 );

	return _GetSubElementCount( m_XmlConfiger, lpszPath, pszName );
}

//=============================================================================
//Function:     GetCmd
//Description:	Get Cmd/Group by Id
//
//Parameter:
//	i32Id    - Id of Cmd or Group
//
//Return:
//		NULL when not found, else is not NULL
//=============================================================================
PS_PCmd_ C_XmlProtocol::GetCmd( int i32Id )
{
	CHECK_ISOPEN_RET_( NULL );

	MapPCmdIt it = m_mapPCmd.find( i32Id );

	if ( it == m_mapPCmd.end() )
	{
		TRACE_(_T("Not find Cmd[%d]"), i32Id );
		return NULL;
	}

	return it->second;
}

//=============================================================================
//Function:     GetField
//Description:	Get Cmd/Group field
//
//Parameter:
//	psPCmd     - Cmd/Group pointer
//	pszName    - Field name
//  bRef	   - Get the reference field of the pszName
//
//Return:
//		NULL when not finded, else is not NULL            
//=============================================================================
PS_PField_ C_XmlProtocol::GetField( PS_PCmd_ psPCmd, TCHAR* pszName, BOOL bRef )
{
	CHECK_ISOPEN_RET_( NULL );

	if ( psPCmd == NULL || pszName == NULL )
	{
		TRACE_( _T("Paramenter invalid, psPCmd= %p, pszName = %s"), psPCmd, pszName );
		return NULL;
	}

	for ( int i32FieldIndex = 0; i32FieldIndex < psPCmd->i32Count; i32FieldIndex++ )
	{
		if (psPCmd->sField[i32FieldIndex].eFieldType & ePFType_PID )
		{
			if ( bRef && _tcsnicmp(pszName, psPCmd->sField[i32FieldIndex].szName, TYPE_ELEMENT_NAME_SIZE ) == 0 )
				return &psPCmd->sField[i32FieldIndex];

			PS_PField_ psField = GetField( GetCmd(psPCmd->sField[i32FieldIndex].eFieldType), pszName, bRef );
			if ( psField )
				return psField;
		}
		else
		{
			if ( _tcsnicmp(pszName, psPCmd->sField[i32FieldIndex].szName, TYPE_ELEMENT_NAME_SIZE ) == 0 )
				return &psPCmd->sField[i32FieldIndex];
		}
	}

	TRACE_( _T("Not find field[%s] of Cmd[%s]"), psPCmd->szName, pszName );
	return NULL;
}

//=============================================================================
//Function:     GetField
//Description:	Get Cmd/Group field by Id an Name
//
//Parameter:
//	i32Id      - Cmd/Group Id
//	pszName    - Field Name
//  bRef	   - Get the reference field of the pszName
//
//Return:
//		NULL when not finded, else is not NULL            
//=============================================================================
PS_PField_ C_XmlProtocol::GetField( int i32Id, TCHAR* pszName, BOOL bRef )
{
	return GetField( GetCmd(i32Id), pszName, bRef );
}

BOOL C_XmlProtocol::InRange( PS_PField_ psPField, int i32Value )
{
	return VerifyRange( psPField, i32Value );
}

BOOL C_XmlProtocol::InRange( PS_PField_ psPField, unsigned int u32Value )
{
	return VerifyRange( psPField, u32Value );
}

BOOL C_XmlProtocol::InRange( PS_PField_ psPField, __int64 i64Value )
{
	return VerifyRange( psPField, i64Value );
}

BOOL C_XmlProtocol::InRange( PS_PField_ psPField, unsigned __int64 u64Value )
{
	return VerifyRange( psPField, u64Value );
}

BOOL C_XmlProtocol::InRange( PS_PField_ psPField, float fValue )
{
	return VerifyRange( psPField, fValue );
}

BOOL C_XmlProtocol::InRange( PS_PField_ psPField, double dValue )
{
	return VerifyRange( psPField, dValue );
}

//=============================================================================
//Function:     VerifyRange
//Description:	Verify range of value
//
//Parameter:
//	psPField    - Cmd/Group field 
//	Value       - Value to verify
//
//Return:
//		TRUE	- In range
//		FALSE	- Out of range
//=============================================================================
template< class T > 
BOOL C_XmlProtocol::VerifyRange( PS_PField_ psPField, T Value )
{
	if ( psPField == NULL )
	{
		TRACE_( _T("Invalid parameter"));
		return FALSE;
	}

	if ( Value >= (T)psPField->u64Min && Value <= (T)psPField->u64Max )
		return TRUE;

	if ( psPField->eFieldType == ePFTYPE_FLOAT || psPField->eFieldType == ePFTYPE_DOUBLE ) 
		TRACE_( _T("Verify failed, [%f] not in range[%f, %f]"), Value, (T)psPField->u64Min, (T)psPField->u64Max );
	else
		TRACE_( _T("Verify failed, [%d] not in range[%d, %d]"), Value, (T)psPField->u64Min, (T)psPField->u64Max );

	return FALSE;
}

