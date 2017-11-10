#pragma once

#include "json/cJSON.h"

#define IME_MAX_PURVIEW					( 5 )
#define IME_MAX_THREADS					( 10 )
// Defined file information for CMD, the file transfer cmd ONLY can be run in UI thread.
//#define FILE_KEY_SIZE	( 6 )
typedef struct tagFileInfo 
{
	S_FileTransInfo sFile;
//	TCHAR		szKey[6 + 1];
	int			i32Type;	// For image
	DWORD		dwCounter;	// timer counter, for check the file task
	I_EIMCmd*	pICmd;		// For rfind
//	TCHAR		szKey[FILE_KEY_SIZE + 1];
	int			i32Ratio;	// ratio
}S_FileInfo, *PS_FileInfo;
typedef std::map<UINT64, S_FileInfo>	MapFiles;
typedef MapFiles::iterator				MapFilesIt;
typedef std::pair<UINT64, S_FileInfo>	PairFiles;

typedef struct tagFileTrans
{
	I_EIMEngine*	pIEgn;
//	I_eIMP2P*		pIP2P;
	I_EIMLogger*	pILog;
	I_EIMEventMgr*  pIEMgr;
	I_eIMBatchInternetFile* pIBIFile;
	MapFiles mapFiles;
//	MapFiles mapP2pFiles;
	CRITICAL_SECTION csMapFiles;	
	BOOL			bExit;
}S_FileTrans, *PS_FileTrans;

typedef struct tagFileTransNotify
{
	DWORD			 dwThreadId;  
	PS_FileTrans	 psFileTrans;
	PS_EIMStatNotify psStat;
}S_FileTransNotify, *PS_FileTransNotify;

// Ref: LOGINACK
typedef struct tagUpdateTimestamps
{
	TUpdateTimeStamp rUpdateTimeInfo;
	DWORD dwTTSvrNow;			// 服务器当前时间
	DWORD dwTTSysNow;			// 系统当前时间，用于计算实际的命令发送时间
	DWORD dwTTShowDeptConfig;	// 部门是否显示配置更新时间
	UINT32	dwRobotInfoUpdatetime;		// 机器人信息最后更新时间戳
}S_UpdateTTs, *PS_UpdateTTs;
typedef const struct tagUpdateTimestamps* PS_UpdateTTs_;

#define IME_UPDATE_CONTS_FLAG_FINISH	( 0x00000000 )	// Finished flag
#define IME_UPDATE_CONTS_FLAG_NOT_START	( 0x00000001 )	// Not start flag
#define IME_RESTART_UPDATE_DELAY		( 5 * 60 * 1000 )	// 5 min
typedef struct tagUpdateContsInfo
{
	UINT32	dwId;				// This time update id(use timestamp)
	UINT32	dwType;				// See: IME_UPDATE_CONTS_TYPE_*
	UINT32	dwElapse;			// Counterdown for IME_RELOAD_CONTS_TYPE_NOW
	UINT32	dwFlag;				// Flag: 1 not finish
	UINT32  dwTime;				// Start time(GetTickCount)
}S_UpdateContsInfo, *PS_UpdateContsInfo;

// 控制参数
typedef struct tagEmpControlParam
{
	UINT16 wGroupMaxMemberNum;//讨论组最大人数：2字节整数
	UINT16 wPCMaxSendFileSize;//PC文件最大发送大小：单位为M，整数，2字节
	UINT8  cMobileMaxSendFileSize;//移动客户端文件最大发送大小：单位为M，整数，1字节
	UINT8  cPCGetStatusInterval;//PC客户端拉取时间间隔：单位分，整数，1字节
	UINT8  cPCSubscribeInterval;//PC客户端临时订阅超时时间：单位为分，整数，1字节（该值同时为超时检查间隔）
	UINT8  cMobileGetStatusInterval;//移动客户端拉取状态超时时间：单位为分，整数，1字节（该值同时为超时检查间隔
	UINT16  wPCTempSubscribeMaxNum;//PC客户端临时订阅列表最大人数（用户ID订阅方式）：整数,2字节，超过则清除最开始的人
	UINT16  wGetStatusMaxNum;//客户端状态拉取最大人数:  2字节，整数
	UINT16  wPCAliveMaxInterval;//PC客户端心跳包时间间隔：单位为秒，整数，2字节
	UINT16  wMobileAliveMaxInterval;//mobile客户端心跳包时间间隔：单位为秒，整数，2字节
	UINT16  wPCSMSMaxLength;//PC短信每条发送字符最大值：2字节，单条拆分字符数：2字节
	UINT8   cMobileServiceExpiry;//移动客户端接入服务地址有效时间：单位小时，1字节
	UINT16  wMobileUploadRecentContact;//移动端登录后，获取状态时，上传的最近联系人，最大人数，2字节
	UINT16  wPCUploadRecentContact;//PC登录后，获取状态时，上传的最近联系人，最大人数，2字节
	UINT8   cModifyPersonalAuditPeriod;//修改个人资料审核时间：1字节，小时
    UINT8	cMsgSynType;//消息同步类型
}S_EmpControlParam, *PS_EmpControlParam;

// 员工权限
typedef struct tagEmpPurview
{
	UINT32 dwID;		//权限ID
	UINT32 dwParameter; //属性
}S_EmpPurview, *PS_EmpPurview;

/*
typedef struct tagLoginAck
{
	INT8	ret;
	char	aszRetDesc[RET_DESC_MAXLEN];		// 返回值描述
	UINT32	dwSessionID;						// http session id
	char	aszAuthToken[AUTHTOKEN_MAXLEN];		// 登录认证token,做单点登陆用
	UINT32	dwCompID;							// 企业ID
	UINT32	uUserId;							// 用户ID
	char	aszUserName[USERNAME_MAXLEN];		// 用户名字
	INT8	sex;								// 性别

	S_UpdateTTs  sUpdateTTs;

	UINT32 dwPersonalDisplay;					//个人资料显示：4字节（每位控制一个字段，最多控制32个字段）
	UINT32 dwPersonalEdit;						// 个人资料可编辑：4字节（每位控制一个字段，最多控制32个字段）
	INT8   cDeptUserLanguageDisplay;			//部门、人员中英文显示：1字节（2位代表部门中文-00为纯中文/01为中文+英文，
												//2位代表部门英文-00为纯英文/01为英文+中文，
												//2位代表人员中文-00为纯中文/01为中文+英文，
												//2位代表人员英文-00为纯英文/01为英文+中文）
	UINT16 wPurview;							// 用户权限
	S_EmpPurview sEmpPurview[IME_MAX_PURVIEW];
	S_EmpControlParam sControlParam;
}S_LoginAck, *PS_LoginAck;
*/
// Declare
class I_ClientAgent;
class I_EIMLogger;
class C_UIWaitOverMgr;
class C_eIMSaveData;
class I_EIMUrlFileMgr;

typedef struct tagUpdateReadMsg
{
    QSID  qsid;
    QMID qmid;
    bool operator==(const tagUpdateReadMsg &t1)const{
        if (qsid == t1.qsid && qmid == t1.qmid)
        {
            return true;
        }
        return false;
    }  
}S_UpdateReadMsg;

typedef struct tagReadMsgNotice
{
    UINT32 dwSenderID;
    UINT32 dwRecverID;
    UINT64 dwMsgID;
    UINT8  cMsgType; 
    UINT32 dwTime; 
    bool operator==(const tagReadMsgNotice &t1)const{
        if (dwSenderID == t1.dwSenderID && dwRecverID == t1.dwRecverID
            && dwMsgID == t1.dwMsgID && cMsgType == t1.cMsgType && dwTime == t1.dwTime)
        {
            return true;
        }
        return false;
    } 
    tagReadMsgNotice& operator=(const MSGREADNOTICE& t1){
         dwSenderID = t1.dwSenderID; 
         dwRecverID = t1.dwRecverID;
         dwMsgID = t1.dwMsgID; 
         cMsgType = t1.cMsgType;
         dwTime = t1.dwTime;
        return *this;
    }  
}S_ReadMsgNotice;

class C_eIMEngine: public I_EIMEngine
{
private:
	C_PluginDll		m_DllClientAgent;		// Client Agent Dll
	C_PluginDll		m_DllLogger;			// Logger Dll
	C_PluginDll		m_DllP2P;				// P2P Dll
	C_PluginDll		m_DllSQLite3;			// SQLite3 Dll
	C_PluginDll		m_DllIFile;				// Internet File Dll

	I_EIMEventMgr*	m_pIEventMgr;			// Event Manager Interface
	I_EIMLogger*	m_pILogger;				// Logger Interface
	I_ClientAgent*	m_pIClientAgent;		// ClientAgent Interface
	I_eIMP2P*		m_pIP2P;				// P2P Interface
	I_EIMContacts*	m_pIContacts;			// Contacts Interface
	I_EIMMsgMgr*	m_pIMsgMgr;				// MsgMgr Interface
	I_EIMHistory*	m_pIHistory;			// History Interface
	I_EIMPinyin*	m_pIPinyin;				// Pinyin Interface
	I_eIMBatchInternetFile* m_pIBIFile;		// Batch internet file Interface
	I_EIMSubScribeMgr*	m_pSubscribeMgr;// State subscribe mgr interface
	C_UIWaitOverMgr*	m_pUIWaitOverMgr;	//UI wait over mgr
	C_eIMSaveData*		m_pSaveData;
    I_EIMUrlFileMgr*      m_pUrlFileMgr;
	// Attribute
	typedef std::map< eIMString, eIMString >	MapAttribStr;
	typedef MapAttribStr::iterator				MapAttribStrIt;

	MapAttribStr	m_mapAttrStr;			// String attribute map
	BOOL			m_bInited;				// Initial flag
	BOOL			m_bOnlineHasSynInfo;	// online to syn some data just one time

	DWORD			m_dwLoginTimes;			// Login times
	E_EngineStatus	m_eEngineStatus;		// Login status
	BOOL			m_bIsUserOffline;		// user own do offline

	int				m_iCheckDelay;			// delaytime
	int				m_i32ReconnTimes;		// 用于修正国美的反复【登录，断线】，限制自动重连次数（默认10次），登录完成时会清0（OnLoadContactEnd）
	int				m_iSynRoamDataStep;		// SYN roam data step

	int				m_iWaitType;
	int				m_iWaitSecond;
	UINT16			m_wRegularGroupCount;	// Offline regular group syn count

	// Login information
	LOGINACK		m_sLoginAck;			// Login Ack information
	S_UpdateTTs		m_sLastUpdateTTs;		// Last update timestamp
	S_EmpInfo		m_sLoginEmpInfo;		// Login emp information
	confUserInfoNotice m_sMeetingAccountInfo;
	S_UpdateContsInfo m_sUpdateContsInfo;	// Update contacts information

	// Thread
	typedef std::list< I_EIMCmd* >	ListTask;
	typedef ListTask::iterator		ListTaskIt;

	struct tagWorkThreadData;
	typedef unsigned (C_eIMEngine::*PFN_EngineThread)(tagWorkThreadData*);
	typedef struct tagWorkThreadData
	{
		void*			 pThis;				// This 
		DWORD			 dwFlag;			// Thread flag
		BOOL			 bExit;				// Exit thread flag, TRUE: exit
		uintptr_t		 hThread;			// Thread handle
		unsigned int	 u32Tid;			// Thread Id
		HANDLE			 hTaskEvent;		// Task event
		ListTask		 listTask;			// Task list
		CRITICAL_SECTION csTask;			// Task cs
		PFN_EngineThread pThreadFun;		// Thread function
	}S_WorkThreadData, *PS_WorkThreadData;

	S_WorkThreadData	m_aThreads[IME_MAX_THREADS];	// Engine threads
    //URL文件下载
    std::list<T_UrlFileTranInfo> m_urlFileDownLst;
protected:
	friend C_eIMSessionMgr;

	// Thread stub
	static unsigned __stdcall _StartThread( void* pvData );
	static bool     __stdcall _FileTransStatNotifyCb(PS_EIMStatNotify psStat, void *pvUserData);
	static bool     __stdcall _StatNotifyCB(S_EIMStatNotify psStat, void *pvUserData);

	static bool		__stdcall _ParseContDeptCallback(PS_ContsData_ psCData, void* pvUserData);
	static bool		__stdcall _ParseContUserDeptCallback(PS_ContsData_ psCData, void* pvUserData);
	static bool		__stdcall _ParseContUsersCallback(PS_ContsData_ psCData, void* pvUserData);

	static size_t   _cUrlWrite(void *ptr, size_t size, size_t nmemb, FILE *stream);
	static size_t   _cUrlRead(void *ptr, size_t size, size_t nmemb, void *userp);
	static int	    _cUrlStatus(TCHAR *progress_data, curl_off_t llDnldTotal, curl_off_t llDnldNow, curl_off_t /*fUpldTotal*/, curl_off_t /*fUpldNow*/);
	static int		_cUrlUploadStatus(int* pnUserData, curl_off_t llDnldTotal, curl_off_t llDnldNow, curl_off_t fUpldTotal, curl_off_t fUpldNow);
	static size_t   _write_data(void* data, size_t size, size_t nmemb, void* content);

	static int		__stdcall _EmpCb(PS_EmpInfo_ psEmpInfo, void* pvUserData, DWORD dwPyOrder);

	void Data_Maneger(std::string data,char* confid,enumcurlMeetingType i32Retry=eMeeting_default);
	// Thread work route
	unsigned _DoMgrThread(PS_WorkThreadData pWTD);
	unsigned _DoSendThread(PS_WorkThreadData pWTD);
	unsigned _DoRecvThread(PS_WorkThreadData pWTD);
	unsigned _DoDispThread(PS_WorkThreadData pWTD);
	unsigned _DoDbThread(PS_WorkThreadData pWTD);
	unsigned _DoStateSubscribeThread(PS_WorkThreadData pWTD);
	unsigned _DoUIWaitOverTimeThread(PS_WorkThreadData pWTD);
    unsigned _DoDownloadUrlFileThread(PS_WorkThreadData pWTD);
    unsigned _DoCUrlThread(PS_WorkThreadData pWTD);

	BOOL _DoFileAction(I_EIMCmd* pICmd, I_EIMLogger* pILog);

	inline BOOL	_IsInit();
	//inline BOOL	_InitClient( int i32AccMode );	// 1: CMNET 2: CMWAP
	//inline int	_Connect(int i32Retry, BOOL bRecon );
	//inline BOOL _CheckVersion();
	DWORD	_GetCmdId(const TCHAR* pszXml);
	BOOL	_InstallConcatsDb(QEID qeid, const char* pszDownloadUrl = NULL, const char* pszPsw = NULL);
	BOOL	_GetContsPathFile(const char* pszUrl, eIMStringA& szZipFileA, eIMStringA& szUnzipDir, eIMStringA& szUnzipFileName, eIMString& szZipFile);
	
	inline int	_Login(BOOL bReconn);
	inline BOOL	_CreateThread( DWORD dwFlag );
	inline BOOL	_LoadProtocolConfigFile();
	inline int  _AddCmd(E_ThreadIdx eThreadIdx, I_EIMCmd* pCmd, BOOL bAddRef = FALSE);
	inline int	_AddCmd(E_ThreadIdx eThreadIdx, DWORD dwCmdId, const void* pvData);
	inline BOOL _GetAttrString(TiXmlElement* pEl, eIMString& szAtrr);
	inline eIMString _MakeSubSendCmd(const eIMString& szCmdAttr, const eIMString& szMsgUIAttr, const eIMString& szSubEl, const eIMString& szItemAttr );
    inline eIMString MakeReciveSubSendCmd(const eIMString& szCmdAttr, const eIMString& szMsgUIAttr, const eIMString& szSubEl, const eIMString& szItemAttr );
	inline BOOL	_StopThread();
	inline BOOL _InitDb(BOOL bGlobal, QEID qeid, BOOL bBkupdb=FALSE);
	inline void _InitUserEnv();
    inline eIMString GetReciveItems(const TiXmlHandle& h);

	// 同步数据
	void _UpdatePersonalInfo(DWORD dwTime);					// 更新个人信息				step0
	void _UpdatePersonalAvatar(DWORD dwTime);				// 更新个人头像信息			step0
	void _UpdateDepts(DWORD dwTime);						// 获取部门信息				step1
	void _UpdateDeptEmps(DWORD dwTime);						// 获取部门用户对应关系数据	step2
	void _UpdateRegularGroupInfo(DWORD dwTime);				// 获取固定组信息			step3 
	void _UpdateEmps( DWORD dwTime);						// 获取用户数据				step3
	void _LoadContact();									// 加载联系人				step3（UI）

	void _GetEmpState();									// 获取用户状态				step4
	void _UpdateOthersAvatorTS(DWORD dwTime);				// 更新其他人头像时间戳信息 step4
	void _GetOfflineMsg();									// 获取离线消息				step4

	void _GetVirtualGroupMsg();                             // 获取虚拟组信息
	void _UpdateRoamData();									// 同步漫游数据//暂未调用

	void OnReloadContacts(int i32Type);
	//void OnSyncId();
	void OnClearEmpsAfter();
	void OnLoginStatus(PS_LoginStatus psStatus);
	void OnGetGroupInfo(QSID qsid);
	void DoGetVGroupInfo(DWORD dwTime);
	void OnSendCmd(I_EIMCmd* lpvParam);
	void OnLoginAck(LOGINACK* lpvData);			// 登录应答
	void OnLogoutAck(LOGOUTACK* lpvParam);
	void OnModifyInfoNotice(MODIINFONOTICE* psNotice);

	void OnCreateGroupAck(CREATEGROUPACK* lpsAck);
	void OnGetGroupInfoAck(GETGROUPINFOACK* psAck);

	void OnBroadcastNotice(BROADCASTNOTICE* psNotice);
	void OnMsgRead(MSGREAD* psRead);
	void OnMsgReadNotice(BROADCASTNOTICE* psNotice);
	void OnCmdModifySelfInfoNotice(RESETSELFINFONOTICE* psNotice);

	void OnGetUserStateAck(GETUSERSTATELISTACK* psAck);			// 用户状态回包（可能接收完成）
	void OnSynRoamDataAck(ROAMDATASYNCACK* psAck);				// 同步漫游数据应答
	void OnGetRobotInfoRsp();									// 同步机器人数据完成
	void OnGetDeptListEnd(int i32Result);						// 部门数据接收完成               step1 end
	void OnGetDeptEmpsEnd(int i32Result);						// 部门用户对应关系数据接收完成	  step2 end
	void OnGetEmpsEnd(int i32Result, BOOL bForceReload);				// 用户数据接收完成				  step3 end
	void OnGetUserIconListEnd(int i32Result);					// 获取用户头像修改列表结构
	void OnLoadContactEnd();									// 加载组织架构结束
	void OnGetEmployeeAck(PS_EmpInfo pEmpInfo);					// 获取用户资料
	void OnGetDeptShowAck(GETDEPTSHOWCONFIGACK* pAck);			// 获取部门显示配置完成
    void OnSaveMeetingAccountInfo(confUserInfoNotice* pAck);

	void OnCoLastTimeNotice(COMPLASTTIMENOTICE* lpvParam);
	void OnCheckTimeRsp(CHECK_TIME_RESP* psRsp);
	void OnFileTransferAction(PS_FileTransInfo psFTI);
    void OnUrlFileTransferAction(T_UrlFileTranInfo* pTUrlFileInfo);
	void OnFileTransfer( S_FileTransInfo* sFileTransInfo );		//传输过程
	void OnFileTransStatNotify(PS_FileTransNotify psFTN);
	void OnGetDataListTypeAck(GETDATALISTTYPEACK* pAck);
	void UpdateTimsstamp();
	C_eIMEngine(void);

public:
	static C_eIMEngine& GetObject();
	~C_eIMEngine(void);
	inline BOOL InitDb(BOOL bGlobal, QEID qeid, BOOL bBkupdb=FALSE){ return _InitDb(bGlobal, qeid, bBkupdb); }

	BOOL IsManualRefreshConts() { return m_sUpdateContsInfo.dwTime > 0; }
	BOOL InitEnv(LOGINACK* pLoginAck);

	void UpdateEmps( DWORD dwTime)		{ _UpdateEmps(dwTime);		}
	void UpdateDepts(DWORD dwTime)		{ _UpdateDepts(dwTime);		}						
	void UpdateDeptEmps(DWORD dwTime)	{ _UpdateDeptEmps(dwTime);	}		
	int	AddCmd(E_ThreadIdx eThreadIdx, DWORD dwCmdId, const void* pvData) { return _AddCmd(eThreadIdx, dwCmdId, pvData); }
	int AddCmd(E_ThreadIdx eThreadIdx, I_EIMCmd* pCmd, BOOL bAddRef) { return _AddCmd(eThreadIdx, pCmd, bAddRef); }
    int  _cUrlDownload(const char* pszUrl, const TCHAR* pszSaveAs, int i32Retry=0, DWORD dwTimeout=30);
	int _cUrlMeeting(const char* pszUrl,char* cjson,char* confid = NULL,enumcurlMeetingType i32Retry=eMeeting_default, DWORD dwTimeout=30);
    bool GetFisrtDownUrlInfo(T_UrlFileTranInfo&);
    void StopDownUrlThread();
    virtual BOOL IsFace(const TCHAR* pszFace);
    void SendUrlDownProgress(TCHAR *pUrl, int iProgress);
	//void DownloadFileRequest(const TCHAR* pszUrl, const TCHAR* pszFileName, int i32CmdId);

	BOOL CheckeFileIsExist(char* confid,char* token);
	//void OnCreateMeeting(S_Meeting_create* meetingStartInfo);
	//void OnChangeMeetingMember(S_MeetingMemberChange* changeMeetingMember);
	//void OnChangeMeetingLevel(S_MeetingLevel* changeMeetingLevel);
	//void OnDeleteMeeting(S_MeetingCancel* meetingCancel);
	//void OnReGetMeetingInfo(S_ReGetMeetingInfo* sRegetMeetingInfo);
	//void OnReceiveMeeting(S_ReceiveMeeting* sReceiveMeeting);
	//void OnMeetingRead(S_MeetingRead* smeetingRead);
	//void OnGetMeetingVideoUrl(S_MeetingGetVideo* sMeetingGetVideo);
	//void OnGetMeetingRoomlist(S_Meeting_GetRoomlist* GetRoomlistInfo);
	//const char* getLaunchString();
    bool IsStopDownUrl(TCHAR* pszUrl);
    void SendAllMsgReadError();
    static int     __stdcall GetAllMsgReadError(LPCTSTR pszXml, void *pvUserData);
    
public:
	// I_EIMEngine::***
	DECALRE_PLUGIN_(C_eIMEngine)
	BOOL Emp2EmpInfo(const EMPLOYEE* psEmp, PS_EmpInfo psEmpInfo, QDID qdid);
	BOOL DownloadContactFiles(GETDATALISTTYPEACK* pAck, I_SQLite3* pIDb);
	BOOL Unzip(const char* pszZipFile, const char* pszUnzipFile, const char* pszPsw = NULL);


	void UpdateTimestamp(I_SQLite3* pDB);						// 更新已经保存的时间戳
	PS_UpdateTTs_ GetUpdateTimestamp();
	virtual QSID NewID(QEID eidRecver=0, QEID eidSender=0);

	C_eIMSaveData* GetEIMSaveDataPtr();

	virtual DWORD GetTimeNow();

	virtual int Command(
		const TCHAR*	pszXml,
		E_ThreadIdx eThreadId = eTHREAD_FLAG_SEND_IDX
		);

	// Attribute /////////////////////////////////////////////////////////////////////
	virtual const TCHAR* GetAttributeStr(
		const TCHAR*	pszName,
		const TCHAR*	pszDefault = NULL
		);

	virtual const BOOL SetAttributeStr(
		const TCHAR*	pszName,
		const TCHAR*	pszValue
		);

	virtual const int GetAttributeInt(
		const TCHAR*	pszName,
		int				i32Default = 0
		);

	virtual const BOOL SetAttributeInt(
		const TCHAR*	pszName,
		const int		i32Value
		);

	/// I_EIMPlugin ///////////////////////////////////////////////////////////////////////////
		virtual BOOL InitPlugin(		// Initial plugin, succeed if return TRUE
//Del 	I_EIMPluginMgr*	pPluginMgr,		// Tool manager interface	//Single line deleted by lwq on 2013/11/26 9:03:54
		I_EIMEventMgr*	pEventMgr,		// Event manager interface
		LPCTSTR			lpszConfigFile	// Config file path, for plugin[set] manager only now
		);
		
	virtual void OnEvent(				// Event response function
		unsigned int	u32Event, 		// Event ID
		void*			lpvParam		// Event parameter
		);
	
	virtual BOOL GetPluginInfo(			// Get plugin information, succeed if return TRUE
		PS_PluginDllInfo	pPluginInfo	// Return plugin information
		);
		
	virtual void FreePlugin();			// Free plugin

	//////////////////////////////////////////////////////////////////////////////
	virtual int Login(
		const TCHAR* pszName, 
		const TCHAR* pszPsw
		);

	virtual int	Logout(
		int		nStatus, 
		BYTE	cManual
		);

	virtual PS_EmpInfo_ GetLoginEmp();
	void SetLoginEmp(PS_EmpInfo_ pLoginEmpInfo);

	eIMString AddFontElement(E_SessionType eSType, BOOL bSend, TiXmlElement* pParentEl );
	virtual DWORD GetPurview(DWORD u32Type);
	void UpdateLoginState(E_EngineStatus eState);

	virtual void AddFontElement(E_SessionType eSType, BOOL bSend, void* pParentEl);

	//virtual void GetLaunchString(S_GetLaunchString* getLaunchString);
	virtual int  _cUrlUpload(const char* pszUrl,const TCHAR* pszSaveAs,S_UploadMeetingFile* sUpload = NULL,int i32Retry=0, DWORD dwTimeout=30);
	virtual void SetUrlFileInfo(const S_UrlFileInfo& sUrlInfo);
	virtual BOOL GetLocalPath(const S_UrlFileInfo& sUrlInfo, PFN_GetLocalPath pfnGetLocalPath, void* pvUserData);
	virtual void DelCollectMsg(const std::list<UINT64>& CollectIdLst);
	//virtual void ChangeFile(char* confId,const char* filelist);
   //virtual int deleteMeetingFile(const char* confid,const char* filelist);
   int SaveNeedUpdateRead(S_UpdateReadMsg readMsg);
   bool GetUpdateRead(QSID qSid, std::list<S_UpdateReadMsg>& readMsgLst);
   bool isNeedUpdateGroup(QMID qmid);
   void SaveReadMsgNotice(S_ReadMsgNotice msgReadNotice);
   void GetReadMsgNotice(QMID qmid, std::list<S_ReadMsgNotice>& msgReadNoticeLst);
   LONGLONG GetFileSize(const TCHAR* lpszFileFullPath);
   BOOL GetFileName(const TCHAR* pszFile,eIMString& szName);
   //BOOL CheckeFileIsExist(char* confid,char* token,long timeKey,int fileIndex);
private:
    std::list<S_UpdateReadMsg>  m_needUpdateReadMap;
    std::list<S_ReadMsgNotice> m_msgReadNoticeLst;
	//const char* m_pMeetingLaunchString; 

	int   m_OldProgressValue;
};

