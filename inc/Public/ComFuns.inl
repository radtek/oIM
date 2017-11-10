#include "eIMUICore\C_HelpAPI.h"

#define ADD_NODE_START(N)				(_T("<") N _T(">"))
#define ADD_NODE_END(N)					(_T("</") N _T(">"))
#define ADD_NODE_INNERSTART(N)			(_T("<") N)
#define ADD_NODE_INNEREND				_T("/>")

#define ROBOT_RES_KEY  "robotResponse"
#define ROBOT_CONTECT_KEY "content"
#define ROBOT_COMMANTS_KEY "commands"
#define ROBOT_RELATEDQUESTIONS "relatedQuestions"
#define ROBOT_TYPE_KEY "name"

void ParseRobotText(const eIMString& szRobotXml,CDuiString& strMsgText)
{
    strMsgText= _T("");
    CDuiString strRobotXml = szRobotXml.c_str();
    CDuiString strRobotBegin = ADD_NODE_START(_T(ROBOT_RES_KEY));
    CDuiString strRobotEnd = ADD_NODE_END(_T(ROBOT_RES_KEY));
    int iFirst = strRobotXml.Find(strRobotBegin);
    int iLast = strRobotXml.Find(strRobotEnd);
    strRobotXml = strRobotXml.Mid(iFirst, iLast + strRobotEnd.GetLength() );
    TiXmlDocument xmlDoc;
    eIMStringA	  szXmlMsgA;
    if ( 0 ==::eIMTChar2MByte(strRobotXml, szXmlMsgA) )
        return;
    xmlDoc.Parse(szXmlMsgA.c_str());
    if ( xmlDoc.RootElement() == NULL ||
        _stricmp(xmlDoc.RootElement()->Value(), ROBOT_RES_KEY) != 0 )
        return ; 
    TiXmlHandle h(xmlDoc.RootElement());

    TiXmlElement* pContentNode = h.FirstChildElement(ROBOT_CONTECT_KEY).ToElement();
    TiXmlElement* pReatedQuestionsNode = h.FirstChildElement(ROBOT_RELATEDQUESTIONS).ToElement();
    if (pContentNode || pReatedQuestionsNode)
    {
        TiXmlElement* pNode = h.FirstChild().ToElement();
        int iReatedNum = 1;
        while(pNode)
        {
            eIMString szNodeName = GetNodeName(pNode, _T(""));
            if(0 == szNodeName.compare(_T(ROBOT_CONTECT_KEY)) )
            {
                CDuiString strValue;
                GetNodeText(pNode, strValue);
                CDuiString strType = GetKeyValue(strRobotXml,  _T(ROBOT_TYPE_KEY));
                if( strType.IsEmpty() && -1 < strValue.Find(_T("[link")) )
                {
                    strType = _T("menu");
                }
                strValue.Replace(_T("&#xD;"), _T(""));
                strValue.Replace(_T("&quot;"), _T("\"") );
                if( 0 == strType.Compare(_T("menu")) )
                {
                    DelKey(strValue, _T("[link submit="), _T("]"),_T("[/link]"), strValue);
                }
                DelKey(strValue, _T("<a href="), _T(">"),_T("</a>"), strValue);
                strValue.Replace(_T("[link]"), _T("") );
                strMsgText += strValue;

            }
            else if( 0 == szNodeName.compare(_T(ROBOT_RELATEDQUESTIONS)) )
            {
                CDuiString strValue;
                GetNodeText(pNode, strValue);
                CDuiString strNum;
                strNum.Format(_T("%d."), iReatedNum);
                strNum += strValue;
                strNum +=_T("\r\n");
                strMsgText += strNum;
                ++iReatedNum;
            }
            pNode = pNode->NextSiblingElement();
        }//while(pNode)
    }
    else
    {
        TiXmlElement* pCommandsNode= h.FirstChildElement(ROBOT_COMMANTS_KEY).ToElement();
        if(pCommandsNode)
        {
            TiXmlElement* pType = pCommandsNode->FirstChildElement(ROBOT_TYPE_KEY);
            if(pType)
            {
                CDuiString strValue;
                GetNodeText(pType, strValue);
                GetRobotCommanTypeText(strValue, strMsgText);
            }
        }
    }
}

void GetRobotCommanTypeText(CDuiString strRobotText, CDuiString& strMsgText)
{
    if(0 == strRobotText.Compare(_T("imgtxtmsg"))
        || 0 == strRobotText.Compare(_T("wiki")) )
    {
        strMsgText = hpi.GetErrorLangInfo(EIMERR_ROBOT_MSG_IMGTEXT, _T("[图文消息]"));
    }
    else if( 0 == strRobotText.Compare(_T("imgmsg"))  )
    {
        strMsgText = hpi.GetErrorLangInfo(EIMERR_ROBOT_MSG_IMG, _T("[图片消息]"));
    }
    else if( 0 == strRobotText.Compare(_T("videomsg"))  )
    {
        strMsgText = hpi.GetErrorLangInfo(EIMERR_ROBOT_MSG_VIDEO, _T("[视频消息]"));
    }
    else if( 0 == strRobotText.Compare(_T("musicmsg"))  )
    {
        strMsgText = hpi.GetErrorLangInfo(EIMERR_ROBOT_MSG_VOICE, _T("[语音消息]"));
    }
}

void DelKey(CDuiString strValue, CDuiString strStart, CDuiString strStartEnd, CDuiString strEnd ,CDuiString& strResult)
{
    strValue.Replace(strEnd, _T(""));
    int iLinkPos = strValue.Find(strStart);
    while(-1 < iLinkPos)
    {
        int iLast = strValue.Find(strStartEnd, iLinkPos);
        if(iLast > iLinkPos)
        {
            CDuiString strLinkSubmit = strValue.Mid(iLinkPos, iLast - iLinkPos + 1);
            strValue.Replace(strLinkSubmit, _T(""));
            iLinkPos = strValue.Find(strStart);
        }
        else
        {
            break;
        }
    }
    strResult = strValue;
}


CDuiString GetKeyValue(CDuiString strXml, CDuiString strKey)
{
    CDuiString strBegin = CDuiString(_T("<")) + strKey + CDuiString(_T(">"));
    CDuiString strEnd = CDuiString(_T("</")) + strKey + CDuiString(_T(">"));
    int iFirst = strXml.Find(strBegin);
    int iLast = strXml.Find(strEnd);
    CDuiString sValue = strXml.Mid(iFirst + strBegin.GetLength(), iLast-iFirst-strBegin.GetLength() );
    return sValue;
}


void GetNodeText(const TiXmlElement* pElementchild, CDuiString& strText)
{
    strText = _T("");
    const char* pValue = pElementchild->GetText();
    if(pValue)
    {
        eIMString szData;
        ::eIMMByte2TChar(pValue, szData );
        strText.Format(_T("%s"), szData.c_str());
    }
}


eIMString GetNodeName(const TiXmlElement *pElement, LPCTSTR szDefault)
{
    const char* pValue = pElement->Value();
    eIMString sVal;
    if(!pValue || 0==strlen(pValue))
    {
        return sVal = szDefault;
    }
    else
    {
        eIMUTF8ToTChar(pValue, sVal);
    }

    return sVal;
}

eIMString GetAttributeValue(const TiXmlElement *pElement, LPCTSTR szAttriName, LPCTSTR szDefault)
{
    eIMStringA szNameA;
    ::eIMTChar2UTF8(szAttriName, szNameA);
    const char* pszValue = pElement->Attribute(szNameA.c_str());

    eIMString sVal;
    if(pszValue == '\0' || pszValue[0] == '\0')
    {
        sVal = szDefault;
    }
    else
    {
        ::eIMUTF8ToTChar(pszValue, sVal);
    }

    return sVal;
}
