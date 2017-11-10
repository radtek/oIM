
#ifndef __EIM_COMFUN_H_
#define __EIM_COMFUN_H_

#include "UIlib.h"
#include "TinyXml/tinyxml.h"

//=============================================================================
//Function:     ParseRobotText
//Description:	Parse robot text
//Parameter:
//	szRobotXml   - Robot xml content
//  strMsgText   - Return the message text
//Return:
//  None
//=============================================================================
void ParseRobotText(const eIMString& szRobotXml,CDuiString& strMsgText);

//=============================================================================
//Function:     GetNodeText
//Description:	Get a element's text attribute
//Parameter:
//	pElementchild   - Element
//  strText  		- Return text
//Return:
// 	None
//=============================================================================
void GetNodeText(const TiXmlElement* pElementchild, CDuiString& strText);

//=============================================================================
//Function:     GetRobotCommanTypeText
//Description:	get meeting is accept
//Parameter:
//	strRobotText   	- Robot text
//  strMsgText  	- Return message text
//Return:
// 	None
//=============================================================================
void GetRobotCommanTypeText(CDuiString strRobotText, CDuiString& strMsgText);

//=============================================================================
//Function:     GetKeyValue
//Description:	Get a value of the key
//Parameter:
//	strXml  - Robot xml
//  strKey 	- Key
//Return:
//  Return the value
//=============================================================================
CDuiString GetKeyValue(CDuiString strXml, CDuiString strKey);

//=============================================================================
//Function:     DelKey
//Description:	Delete the key
//Parameter:
//	strValue 	- The value
//  strStart  	- Start position
//  strStartEnd	- End position
//  strEnd 		- End
//	strResult	- Result
//Return:
//  None
//=============================================================================
void DelKey(CDuiString strValue, CDuiString strStart, CDuiString strStartEnd,CDuiString strEnd ,CDuiString& strResult);

//=============================================================================
//Function:     GetNodeName
//Description:	Get node name
//Parameter:
//	pElement   - Element
//  szDefault  - Default
//Return:
//   Return the name
//=============================================================================
eIMString GetNodeName(const TiXmlElement *pElement, LPCTSTR szDefault);

//=============================================================================
//Function:     GetMeetingIsAccept
//Description:	get meeting is accept
//Parameter:
//	confId   - meeting id
//  qeid  -  userid
//Return:
//      1  - accept
//		0  - no
//=============================================================================
eIMString GetAttributeValue(const TiXmlElement *pElement, LPCTSTR szAttriName, LPCTSTR szDefault);

#include "ComFuns.inl"
#endif