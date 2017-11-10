
#pragma comment(lib, "shlwapi.lib")


//=============================================================================
//Function:     eIMCheckColTextNull
//Description:	Get a table's col text
//
//Parameter:
//	colText - Column text
//
//Return:
//  NULL return ""
//	Else return the input text
//=============================================================================
inline const char* eIMCheckColTextNull(const unsigned char* colText)
{
	if (!colText)
		return "";
	else
		return (const char*)colText;
}

//=============================================================================
//Function:     eIMCompareVersion
//Description:	Compare the version, the version format as: Major.Minor[.Revision[.Build]],
//				each field is a WORD value
//
//Parameter:
//	pszVerA 	- Version A
//	pszVerB		- Version B
//Return:
//	   -1		- Parameter invalid or VersionA < Version B
//		0		- VersionA       ==VersionB
//		1		- VersionA.Major > VersionB.Major
//		2		- VersionA.Minor > VersionB.Minor
//		3		- VersionA.Rev   > VersionB.Rev
//		4		- VersionA.Build > VersionB.Build
//=============================================================================
inline int eIMCompareVersion(const TCHAR* pszVerA, const TCHAR* pszVerB)
{
	if ( pszVerA == NULL || pszVerB == NULL )
		return -1;

	// Note: The variable MUST be initial, for _stscanf maybe less than 4 value to scaned.
	DWORD au32VerA[4] = { 0 };
	DWORD au32VerB[4] = { 0 };

	// Note: %u or %d is aligned by 4Byte, the variable MUSB is aligned by 4Bytes.
	_stscanf( pszVerA, _T("%u.%u.%u.%u"), &au32VerA[0], &au32VerA[1], &au32VerA[2], &au32VerA[3] );
	_stscanf( pszVerB, _T("%u.%u.%u.%u"), &au32VerB[0], &au32VerB[1], &au32VerB[2], &au32VerB[3] );

	// Check the 4 field of version
	CMP_NEQ_RET_( au32VerA[0], au32VerB[0], 1, -1 );
	CMP_NEQ_RET_( au32VerA[1], au32VerB[1], 2, -1 );
	CMP_NEQ_RET_( au32VerA[2], au32VerB[2], 3, -1 );
//	CMP_NEQ_RET_( au32VerA[3], au32VerB[3], 4, -1 );	// Not check build version
	
	return 0;	// VerA EQU VerB
}





