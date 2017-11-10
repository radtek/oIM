#pragma once
#include "eIMEngine\eIMProtocol.h"
#include "eIMEngine\IeIMProtocol.h"


class C_XmlProtocol:
	public I_EIMProtocol,
	public C_XmlConfiger 
{
private:
	// < ID, Cmd >
	typedef std::map< int, PS_PCmd_ >	MapPCmd;
	typedef MapPCmd::iterator			MapPCmdIt;

	MapPCmd	m_mapPCmd;				// Protocol cmd/group 

	// 
	E_PFieldType _ParseUnit( const TCHAR* pszValue, unsigned __int64& u64DefaultMax );
	BOOL _GetRefId( const TCHAR* pszName );
	BOOL _ParseCmd( TiXmlElement* pElement, PS_PCmd psPCmd );
	BOOL _Parse( TiXmlDocument& xmlDoc );
		
	template< class T > 
	BOOL VerifyRange( PS_PField_ psPField, T Value );

	C_XmlProtocol(void);
public:
	static C_XmlProtocol& GetObject();
	~C_XmlProtocol(void);


public:
	DECALRE_PLUGIN_(C_XmlProtocol)

	virtual int		Open(LPCTSTR lpszFile, LPCTSTR /*lpszRediFile = NULL*/);
	virtual BOOL	Close();

	virtual int		GetSubElementCount( PCTSTR lpszPath, LPCTSTR pszName );

	virtual PS_PCmd_	GetCmd( int i32Id );
	virtual PS_PField_	GetField( PS_PCmd_ psPCmd, TCHAR* pszName, BOOL bRef = FALSE );
	virtual PS_PField_	GetField( int i32Cmd, TCHAR* pszName, BOOL bRef = FALSE );
	
	virtual BOOL InRange( PS_PField_ psPField, int i32Value );
	virtual BOOL InRange( PS_PField_ psPField, unsigned int u32Value );

	virtual BOOL InRange( PS_PField_ psPField, __int64 i64Value );
	virtual BOOL InRange( PS_PField_ psPField, unsigned __int64 u64Value );

	virtual BOOL InRange( PS_PField_ psPField, float fValue );
	virtual BOOL InRange( PS_PField_ psPField, double dValue );

};

