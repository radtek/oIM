// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMProtocol is the engine function interface
// 
// Auth: yfgz
// Date:    14:06 2013/12/25
// 
// History: 
//    
//    
/////////////////////////////////////////////////////////////////////////////
#ifndef __EIMENGINE_PROTOCOL_HEADER_2013_12_25_YFGZ__
#define __EIMENGINE_PROTOCOL_HEADER_2013_12_25_YFGZ__

#include "Public\Plugin.h"
#include "eIMEngine\eIMProtocol.h"

class __declspec(novtable) I_EIMProtocol: public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     GetSubElementCount
	//Description:	Get sub-element count
	//
	//Parameter:
	//	lpszPath    - Path of parrent, NULL is the ROOT 
	//  pszName		- Element name, NULL to all sub-element
	//
	//Return:
	//		 Count the of sub-element
	//=============================================================================
	virtual int GetSubElementCount( 
		LPCTSTR lpszPath, 
		LPCTSTR pszName
		) = 0;

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
	virtual PS_PCmd_	GetCmd( 
		int i32Id 
		) = 0;

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
	virtual PS_PField_	GetField(
		PS_PCmd_	psPCmd,
		TCHAR*		pszName, 
		BOOL		bRef 
		) = 0;

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
	virtual PS_PField_	GetField( 
		int		i32Cmd,
		TCHAR*	pszName, 
		BOOL	bRef 
		) = 0;

	//=============================================================================
	//Function:     InRange
	//Description:	Check whether or not in range of value, closures
	//
	//Parameter:
	//	psPField    - Cmd/Group field 
	//	i32Value    - Value to verify
	//
	//Return:
	//		TRUE	- In range
	//		FALSE	- Out of range
	//=============================================================================
	virtual BOOL InRange( 
		PS_PField_	psPField,
		int			i32Value
		) = 0;

	//=============================================================================
	//Function:     InRange
	//Description:	Check whether or not in range of value, closures
	//
	//Parameter:
	//	psPField    - Cmd/Group field 
	//	u32Value    - Value to verify
	//
	//Return:
	//		TRUE	- In range
	//		FALSE	- Out of range
	//=============================================================================
	virtual BOOL InRange(
		PS_PField_	 psPField,
		unsigned int u32Value 
		) = 0;

	//=============================================================================
	//Function:     InRange
	//Description:	Check whether or not in range of value, closures
	//
	//Parameter:
	//	psPField    - Cmd/Group field 
	//	i64Value    - Value to verify
	//
	//Return:
	//		TRUE	- In range
	//		FALSE	- Out of range
	//=============================================================================
	virtual BOOL InRange( 
		PS_PField_	psPField,
		__int64		i64Value 
		) = 0;

	//=============================================================================
	//Function:     InRange
	//Description:	Check whether or not in range of value, closures
	//
	//Parameter:
	//	psPField    - Cmd/Group field 
	//	u64Value    - Value to verify
	//
	//Return:
	//		TRUE	- In range
	//		FALSE	- Out of range
	//=============================================================================
	virtual BOOL InRange(
		PS_PField_		 psPField, 
		unsigned __int64 u64Value
		) = 0;

	//=============================================================================
	//Function:     InRange
	//Description:	Check whether or not in range of value, closures
	//
	//Parameter:
	//	psPField    - Cmd/Group field 
	//	fValue      - Value to verify
	//
	//Return:
	//		TRUE	- In range
	//		FALSE	- Out of range
	//=============================================================================
	virtual BOOL InRange(
		PS_PField_	psPField,
		float		fValue 
		) = 0;

	//=============================================================================
	//Function:     InRange
	//Description:	Check whether or not in range of value, closures
	//
	//Parameter:
	//	psPField    - Cmd/Group field 
	//	dValue      - Value to verify
	//
	//Return:
	//		TRUE	- In range
	//		FALSE	- Out of range
	//=============================================================================
	virtual BOOL InRange(
		PS_PField_  psPField,
		double		dValue
		) = 0;


};

#endif // __EIMENGINE_PROTOCOL_HEADER_2013_12_25_YFGZ__