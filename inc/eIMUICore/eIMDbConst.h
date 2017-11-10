// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: eIMUI is a public module
// ***Please use UPPER case of SQL keyword
// It is combined the SQL by programme when without end with ';'
//
// Auth: yfgz
// Date: 
// 
// History: 
//    2014/7/2 yfgz Finished first version
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIM_Database_CONST_HEADER_2014_07_02_YFGZ__
#define __EIM_Database_CONST_HEADER_2014_07_02_YFGZ__

#include <windows.h>
#include <tchar.h>

#define DB_BUSY_TIMEOUT			( 1000 * 30 )				// 30s
// Database name define
#define USERS_DB_FILE			_T("Users.db")				// [1]Users db file, global, $(eIMData)
#define CONTACTS_DB_FILE		_T("Contacts.db")			// [1]Contacts db file, user, $(LoginId)
#define MSG_DB_FILE				_T("msg.db")				// [N]Msg db file, user, $(LoginId) 

#define MAIN_DB_AS_NAME			"main"						// Attach as name
#define MSG_DB_AS_NAME			"msg_"						// Attach as name
#define BKUP_DB_AS_NAME			"bkup_"						// Attach as name

#define DB_FLAG_DELETED			( 0x00080000 )				// The same as eSEFLAG_DELETE of E_SEFlag in IeIMSessionMgr.h
#define DB_FLAG_UPDATED			( 0x00100000 )				// The same as eSEFLAG_UPDATE of E_SEFlag in IeIMSessionMgr.h, need in db, I_EIMLoginUserMgr Upgrade CSAIR flag
#define DB_FLAG_MSG_NEW			( 0x00200000 ) 				// The same as eSEFLAG_NEW of E_SEFlag in IeIMSessionMgr.h
#define DB_FLAG_MSG_OFFLINE		( 0x00400000 )				// The same as eSEFLAG_OFFLINE of E_SEFlag in IeIMSessionMgr.h
#define DB_FLAG_SHOW_DEPT		( 1 )					// Show Department(hide employee)
#define DB_FLAG_SHOW_DEPTUSER	( 2 )					// Show department and user

#define NO_ORDERS_VALUE			0	//( 0xFFFF )			// No orders flag
#define CHECK_MAP_NO_ORDERS(V)	((V) == 0 ? NO_ORDERS_VALUE : (V))	// Default no orders value(map 0 to 0xFFFF for simple sort)

// SQL
const char* const kWalCheckpoint = "PRAGMA wal_checkpoint(FULL);";

// Backup
#define DB_FIELD_TIMESTAMP	"timestamp"

const char* const kGetLastRecordTT = "SELECT timestamp FROM %s ORDER BY timestamp DESC LIMIT 1;";
const char* const kIsValidTable    = "SELECT * FROM %s LIMIT 1;";
const char* const kBkupRecordsByTT = "REPLACE INTO %s (SELECT * FROM %s WHERE timestamp>=%lld);";
const char* const kBkupRecordsAll  = "REPLACE INTO %s (SELECT * FROM %s);";
const char* const kDelRecords      = "DELETE FROM %s;";

// users.t_login_user =========================================================
//const char* const kSelectMsgdb = "SELECT msgdb FROM t_login_user WHERE uid_=%lu";
//const char* const kUpdateMsgdb = "UPDATE t_login_user SET msgdb='%q' WHERE uid_=%lld";
const char* const kUpdateLoginUserSetFlag	= "UPDATE t_login_user SET flag=%d  WHERE eid_=%ld;";
const char* const kUpdateLoginUserClearFlag	= "UPDATE t_login_user SET flag=flag&~%d WHERE eid_=%ld;";
const char* const kDeleteLoginUserName		= "DELETE FROM t_login_user WHERE name='%q';";
const char* const kSelectLoginUserDbVer		= "SELECT db_ver FROM t_login_user WHERE name='%q';";
const char* const kUpdateLoginUserDbVer		= "UPDATE t_login_user SET db_ver=%d WHERE name='%q';";
const char* const kSelectLoginUserNameFlag	= "SELECT name, eid_, flag FROM t_login_user ORDER BY timestamp DESC;";
const char* const kSelectLoginUserPsw		= "SELECT psw FROM t_login_user WHERE name='%q' ORDER BY timestamp DESC;";
const char* const kReplaceLoginUser			= "REPLACE INTO t_login_user(eid_, name, psw, timestamp, flag) VALUES (@eid_, @name, @psw, @timestamp, @flag);";
const char* const kSelectLoginUserId		= "SELECT eid_ FROM t_login_user WHERE name='%q' ORDER BY timestamp DESC;";

// contacts.t_timestamp ============================================================
const char* const kReplaceTimestamp = "REPLACE INTO t_timestamp(eid_, co, dept, emps, dept_emps, personal_info, comcontact, comdept, personal_attention, globalcomcontact, others_avatar, personal_avatar, vgts, emp_rank, emp_duty, emp_area, speciallist, specialwhitelist, robot, showdept) VALUES (@eid_, @co, @dept, @emps, @dept_emps, @personal_info, @comcontact, @comdept, @personal_attention, @globalcomcontact, @others_avatar, @personal_avatar, @vgts, @emp_rank, @emp_duty, @emp_area, @speciallist, @specialwhitelist, @robot, @showdept);";
const char* const kSelectTimestamp  = "SELECT co, dept, emps, dept_emps, personal_info, comcontact, comdept, personal_attention, globalcomcontact, others_avatar, personal_avatar, vgts, emp_rank, emp_duty, emp_area, speciallist, specialwhitelist,robot, showdept FROM t_timestamp WHERE eid_=%u;";


// contacts.t_dept ============================================================
// flag see: E_SEFlag in file "IeIMSessionMgr.h"

const char* const kInsertDept	= "INSERT INTO t_dept(did_, name, ename, pid, tel, orders, pinyin, pinyin_s, flag, coid, update_time) VALUES (@did_, @name, @ename, @pid, @tel, @orders, @pinyin, @pinyin_s, @flag, @coid, @update_time);";
const char* const kUpdateDept	= "UPDATE OR REPLACE t_dept SET did_=@did_, name=@name, ename=@ename, pid=@pid, tel=@tel, orders=@orders, pinyin=@pinyin, pinyin_s=@pinyin_s, flag=@flag, coid=@coid, update_time=@update_time WHERE did_=@qdid;";
const char* const kSelectDept	= "SELECT did_, name, ename, pid, tel, orders, pinyin, pinyin_s, flag, "
	"(SELECT t2.did_ FROM t_dept t2 WHERE t1.did_=t2.pid AND NOT(t2.flag & 0x00080000) ORDER BY t2.orders, t2.pinyin LIMIT 1) AS fcid, update_time, show "
	"FROM t_dept t1 WHERE t1.coid=%u AND NOT(t1.flag & 0x00080000) ORDER BY t1.pid, t1.orders, t1.pinyin;";
const char* const kBkupDept     = "CREATE TABLE t_dept_bkup AS SELECT * FROM t_dept;";
const char* const kClearDept	= "DELETE FROM t_dept;";
//const char* const kDropBkupDept	= "DROP TABLE IF EXISTS t_dept_bkup;";
const char* const kCleanupDept	= "DELETE FROM t_dept WHERE (flag & 0x00080000);";
const char* const kUpdateDeptUserShowDefalutFlag = "UPDATE t_dept SET show=%u;";
const char* const kUpdateDeptUserShowFlag = "UPDATE t_dept SET show=%u WHERE did_=%u;";

// constats.t_session =========================================================
const char* const kSelectSessionSid		= "SELECT sid_, type, title, remark, flag, cid, ctime, utime, off_msg, new_msg, sutime, (SELECT toptime FROM t_session_top WHERE t_session_top.sid_=t_session.sid_) AS toptime FROM t_session WHERE sid_=%lld;";
//const char* const kSelectSessionNewMsg= "SELECT SUM(new_msg) AS off_msgs FROM t_session WHERE (flag&%d);";
//const char* const kSelectSessionOffMsg= "SELECT SUM(off_msg) AS off_msgs FROM t_session WHERE (flag&%d);";
const char* const kSelectSessionFlag	= "SELECT sid_, type, title, remark, flag, cid, ctime, utime, off_msg, new_msg, sutime, virgroupid, (SELECT toptime FROM t_session_top WHERE t_session_top.sid_=t_session.sid_) AS toptime FROM t_session WHERE (flag&%d) ORDER BY toptime DESC,%s %s;";
const char* const kSelectSessionFlag2	= "SELECT sid_, type, title, remark, flag, cid, ctime, utime, off_msg, new_msg, sutime, virgroupid, (SELECT toptime FROM t_session_top WHERE t_session_top.sid_=t_session.sid_) AS toptime FROM t_session WHERE (flag&%d OR flag&%d) ORDER BY toptime DESC,%s %s;";
const char* const kReplaceSession		= "REPLACE INTO t_session("
	"sid_ ,type, title, remark, flag, cid, ctime, utime, off_msg, new_msg, sutime,virgroupid"
	") VALUES ("
	"@sid_, @type, @title, @remark, @flag, @cid, @ctime, @utime, @off_msg, @new_msg, @sutime,@virgroupid);";
const char* const kUpdateSessionSidFieldInt = "UPDATE t_session SET %s=%d WHERE sid_=%lld";
const char* const kUpdateSessionSidFieldInt2= "UPDATE t_session SET %s=%s+%d,flag=flag|%d WHERE sid_=%lld";
const char* const kUpdateSessionSidFieldStr = "UPDATE t_session SET %s='%q' WHERE sid_=%lld;";
const char* const kUpdateSessionSetFlag		= "UPDATE t_session SET flag=flag|%d  WHERE sid_=%lld;";
const char* const kUpdateSessionClearFlag	= "UPDATE t_session SET flag=flag&~%d WHERE sid_=%lld;";

const char* const kSelectComInfoSessionSid = "SELECT a.sid_,a.flag from t_session a inner join (select sid_ from t_session_emp where eid_=%u) b on a.sid_=b.sid_ where a.flag&%u=%d";
const char* const kDeleteComInfoByType = "UPDATE t_session SET flag=flag|%d WHERE sid_ in (select sid_ from t_session where type = %d);";

const char* const kSelectGroupTitle = "SELECT title from t_session where (title like '%s(%%)' and type=3)";

const char* const kGetVirGroupId = "Select virgroupid FROM t_session WHERE sid_=%lld ;";
const char* const kGetSidbyVirGroupid = "SELECT s.sid_ from t_session s,t_session_emp e where s.sid_ = e.sid_ and s.type = %d and s.virgroupid=%lld and e.eid_=%u;";
const char* const kDeleteSessionMsg = "DELETE FROM t_session where type = 6;";
const char* const kDeleteSession_emp = "DELETE FROM t_session_emp where sid_ in (SELETE sid_ FROM t_session WHERE type=6);";
const char* const kDeleteSession_records = "DELETE FROM t_session_records WHERE sid in (SELETE sid_ FROM t_session WHERE type=6);";
const char* const kUpdateReceiptMsgFlag = "UPDATE t_session_records SET flag=flag|%d  WHERE msgid_=%lld;";
const char* const kSelectReceiptMsgFlag = "SELECT flag from t_session_records where msgid_=%lld;";

// constacts.t_session_top ====================================================
const char* const kReplaceSessionTop     = "REPLACE INTO t_session_top(sid_, toptime) VALUES (%lld, %u);";
const char* const kDeleteSessionTop      = "DELETE FROM t_session_top WHERE sid_=%lld;";
const char* const kSelectSessionTop      = "SELECT sid_, toptime FROM t_session_top t INNER JOIN t_session s ON t.sid_=s.sid_ AND NOT(s.flag & 0x00080000) ORDER BY t.toptime DESC;";
const char* const kSelectSessionTopSid   = "SELECT toptime FROM t_session_top  t INNER JOIN t_session s ON t.sid_=s.sid_ AND NOT(s.flag & 0x00080000) WHERE t.sid_=%lld;";
const char* const kSelectSessionTopCount = "SELECT COUNT(*) FROM t_session_top t INNER JOIN t_session s ON t.sid_=s.sid_ AND NOT(s.flag & 0x00080000);";

// constacts.t_session_emp ====================================================
const char* const kUpdateSessionRecord = "update t_session_records set sid=%lld where sid=%lld;";
const char* const kDropTempTable = "DROP TABLE IF EXISTS t_session_emp_temp;";
//const char* const kUpdateSessionFile = "update t_session_files set sid=%lld where sid=%lld;";
const char* const kDeleteSession = "delete from t_session where sid_=%lld;";
const char* const kUpdateSession = "update t_session set utime=%u where sid_=%lld;";
const char* const kDeleteSessionEmp = "delete from t_session_emp where sid_=%lld ;";
const char* const kGetSameSidTempTable = "select sid_,utime from t_session where sid_ in (select sid_ from t_session_emp_temp where eid_=%u) order by utime asc;";
const char* const kGetSameEidTempTable = "select eid_,count(sid_) as num from t_session_emp_temp group by eid_ having(num >1);";
const char* const kCreateTempTable = "CREATE TABLE t_session_emp_temp(sid_ BIGINT,eid_ INTEGER);";
const char* const kInsertTempTableValue = "INSERT INTO t_session_emp_temp(sid_,eid_) VALUES (%lld,%u);";
const char* const kSelectSessionMembers		 = "SELECT eid_ FROM t_session_emp WHERE sid_=%lld AND flag<>%d ORDER BY eid_;";
const char* const kSelectSessionAllMembers   = "SELECT eid_ FROM t_session_emp WHERE sid_=%lld ORDER BY eid_;";
const char* const kSelectSessionGroupMembersSingleRepair = "SELECT sid_,eid_ FROM t_session_emp where sid_ in (select sid_ from t_session where type=0) and eid_<>%u;";
const char* const kSelectSessionGroupMembersSingle = "SELECT e.sid_, COUNT(e.eid_) AS num FROM t_session s INNER JOIN t_session_emp e ON s.sid_=e.sid_ AND NOT(e.flag & 0x00080000) AND s.type=%d GROUP BY e.sid_ HAVING(num=%d) ORDER BY s.utime DESC;";
const char* const kSelectSessionGroupMembers = "SELECT e.sid_, COUNT(e.eid_) AS num FROM t_session s INNER JOIN t_session_emp e ON s.sid_=e.sid_ AND NOT(e.flag & 0x00080000) AND s.type=%d AND s.virgroupid=%lld GROUP BY e.sid_ HAVING(num=%d) ORDER BY s.utime DESC;";
const char* const kSelectSessionGroupMembersOderCreator = "SELECT e.sid_, COUNT(e.eid_) AS num,1 As MyOrder,s.utime FROM t_session s "
	"INNER JOIN t_session_emp e ON s.sid_=e.sid_ AND NOT(e.flag & 0x00080000)  where s.cid=%u GROUP BY e.sid_ HAVING(num=%d) "
	"union all SELECT e.sid_, COUNT(e.eid_) AS num, 0 As MyOrder,s.utime FROM t_session s "
	"INNER JOIN t_session_emp e ON s.sid_=e.sid_ where s.cid<>%u GROUP BY e.sid_ HAVING(num=%d) ORDER BY MyOrder DESC, s.utime DESC;";
const char* const kReplaceSessionEmp		 = "REPLACE INTO t_session_emp(sid_, eid_, flag) VALUES (@sid_, @eid_, @flag);";       //单人会话
const char* const kReplaceSessionEmp2		 = "REPLACE INTO t_session_emp(sid_, eid_, flag, attrib) VALUES (@sid_, @eid_, @flag, @attrib);";   //群组会话
const char* const kClearSessionEmp           = "Delete from t_session_emp where sid_=%lld;";
const char* const kSelectRgroupAdmines		 = "SELECT eid_,attrib FROM t_session_emp WHERE sid_=(select sid from t_session_records where msgid_=%lld);";
//get virtualGroup updatetime
const char* const kvirtualGroupUpdatetime = "SELECT max(update_time) FROM t_groupinfo_virtual ;";
const char* const kvirtualGroupId = "Select groupid FROM t_groupmember_virtual WHERE userid=%ld ;";
const char* const kvirtualGroupWaitTip = "Select waiting_prompt FROM t_groupinfo_virtual WHERE main_userid=%ld ;";
const char* const kUpdateGroupInfo = "INSERT INTO t_groupinfo_virtual values(%d,'%s',%d,%d,%d,'%s','%s','%s',%d,%d,%d);";
const char* const kUpdateGroupInfo_delete = "DELETE FROM t_groupinfo_virtual WHERE groupid='%s';";
const char* const kUpdateGroupMember = "INSERT INTO t_groupmember_virtual values('%s',%d,%d,%d,%d);";
const char* const kUpdateGroupMember_delete = "DELETE FROM t_groupmember_virtual WHERE groupid='%s';";
const char* const kgetvirGroupmainid = "SELECT main_userid from t_groupinfo_virtual";
const char* const kgetvirGroupGroupId = "SELECT groupid from t_groupinfo_virtual";
const char* const kgetvirGroupUserId = "SELECT userid from t_groupmember_virtual";
const char* const kisVirGroupSession = "SELECT sid_ from t_session where sid_=%lld and type=%d;";
// constacts.t_dept_emp =======================================================
const char* const kReplaceDeptEmp = "REPLACE INTO t_dept_emp(did_, eid_, orders, flag, update_time) VALUES (@did_, @eid_, @orders, @flag, @update_time);";
const char* const kSelectDeptEmp  = "SELECT d.eid_, d.orders FROM t_dept_emp d INNER JOIN t_emp e ON d.eid_=e.eid_ WHERE d.did_=%u AND NOT(d.flag & 0x00080000) ORDER BY d.orders, e.pinyin;";
const char* const kBkupDeptEmp    = "CREATE TABLE t_dept_emp_bkup AS SELECT * FROM t_dept_emp;";
const char* const kClearDeptEmp   = "DELETE FROM t_dept_emp;";
const char* const kClearInValidDeptEmps = "DELETE FROM t_dept_emp WHERE did_ not in (SELECT did_ From t_dept)  ;";
const char* const kDropBkupDeptEmp= "DROP TABLE IF EXISTS t_dept_emp_bkup;";
const char* const kCleanupDeptEmp = "DELETE FROM t_dept_emp WHERE (flag & 0x00080000);";


// constacts.t_emp ============================================================
// Update employee information by DeptEmps
const char* const kInsertEmpDept = "INSERT INTO t_emp(eid_, name, ename, ucode, pinyin, pinyin_s, sex, logo, flag, rank_id, prof_id, aera_id"
	") VALUES ("
	"@eid_, @name, @ename, @ucode, @pinyin, @pinyin_s, @sex, @logo, @flag, @rank_id, @prof_id, @aera_id);";
const char* const kUpdateEmpDept = "UPDATE OR REPLACE t_emp SET eid_=@eid_, name=@name, ename=@ename, ucode=@ucode, pinyin=@pinyin, pinyin_s=@pinyin_s, sex=@sex, logo=@logo, flag=@flag, rank_id=@rank_id, prof_id=@prof_id, aera_id=@aera_id WHERE eid_=@qeid;";

const char* const kInsertUserInfo = "INSERT INTO t_emp(pinyin, pinyin_s, eid_, ucode, name, ename, sex, flag, duty, email, tel,phone,address,postalcode,fax,update_time,hiredate,sign"
    ") VALUES ("
    "@pinyin, @pinyin_s, @eid_, @ucode, @name, @ename, @sex, @flag, @duty, @email, @tel, @phone, @address, @postalcode, @fax, @update_time, @hiredate,@sign);";

const char* const kUpdateUserInfo = "UPDATE  t_emp SET pinyin=@pinyin, pinyin_s=@pinyin_s, eid_=@eid_,  ucode=@ucode, name=@name, ename=@ename, sex=@sex, flag=@flag, duty=@duty, email=@email, tel=@tel,phone=@phone,address=@address,postalcode=@postalcode,fax=@fax,update_time=@update_time, hiredate=@hiredate,sign=@sign  WHERE eid_=@qeid;";

const char* const kSelectEmpInfoByEmp = "SELECT ucode,name FROM t_emp WHERE eid_=%u";
const char* const kBkupEmp    = "CREATE TABLE t_emp_bkup AS SELECT * FROM t_emp;";
const char* const kClearEmp   = "DELETE FROM t_emp;";
const char* const kDropBkupEmp= "DROP TABLE IF EXISTS t_emp_bkup;";
const char* const kCleanupEmp = "DELETE FROM t_emp WHERE (flag & 0x00080000);";

// Update employee information by employees
const char* const kReplaceEmp ="REPLACE INTO t_emp("
	"eid_, name, ename, ucode, pinyin, pinyin_s, sex, address, flag, duty, tel, phone, email, postalcode, fax, home_tel, emgr_phone, logo, sign, logotime, update_time, hiredate"
	") VALUES ("
	"@eid_, @name, @ename, @ucode, @pinyin, @pinyin_s, @sex, @address, @flag, @duty, @tel, @phone, @email, @postalcode, @fax, @home_tel, @emgr_phone, @logo, @sign, @logotime, @update_time, @hiredate);";
const char* const kUpdateEmpFiledInt = "UPDATE t_emp SET %s=%d WHERE eid_=%u";
const char* const kUpdateEmpFiledStr = "UPDATE t_emp SET %s='%q' WHERE eid_=%u;";

const char* const kSelectEmpsPid    = "SELECT e.eid_, e.ucode, e.name, e.pinyin, e.pinyin_s, e.birthday, e.sex, e.duty, e.email, e.tel, e.home_tel, e.phone, e.emgr_phone, e.logo, e.sign, e.flag, e.rank_id, e.prof_id, e.aera_id, d.did_, d.orders, e.address, e.postalcode, e.fax, e.logotime, e.update_time, e.hiredate, e.ename, (SELECT show FROM t_dept t WHERE d.did_=t.did_) AS show FROM t_dept_emp d INNER JOIN t_emp e ON d.eid_=e.eid_ GROUP BY e.eid_;"; 
const char* const kSelectEmpPidByID = "SELECT e.eid_, e.ucode, e.name, e.pinyin, e.pinyin_s, e.birthday, e.sex, e.duty, e.email, e.tel, e.home_tel, e.phone, e.emgr_phone, e.logo, e.sign, e.flag, e.rank_id, e.prof_id, e.aera_id, d.did_, d.orders, e.address, e.postalcode, e.fax, e.logotime, e.update_time, e.hiredate, e.ename, (SELECT show FROM t_dept t WHERE d.did_=t.did_) AS show FROM t_dept_emp d INNER JOIN t_emp e ON d.eid_=e.eid_ WHERE e.eid_=%u;"; 

// Get Set emp logo time  0 need download 1 exist logo download ok
const char* const kSelectEmpLogoTime	= "SELECT logotime FROM t_emp where eid_=%u;";
const char* const kUpdateEmpLogoTime	= "UPDATE t_emp SET logotime=%u WHERE eid_=%u;";
const char* const kUpdateMutiEmpLogoTime= "UPDATE t_emp SET logotime=%u WHERE eid_ in (%s);";
const char* const kUpdateAllLogoTime	= "UPDATE t_emp SET logotime=0;";	// 把所有的头像时间复位（全部需要更新）
//  订阅相关
// constacts.t_subscribe_info==================================================
//const char* const kReplaceSubscribeInfo = "REPLACE INTO t_subscribe_info(eid_, createtime, updatetime, count) VALUES (@eid_, @createtime, @updatetime, @count);";
//const char* const kSelectSubscribeInfo = "SELECT eid_, createtime, updatetime, count from t_subscribe_info where eid_=%u;";
//const char* const kSelectAllSubscribeInfo = "SELECT eid_, createtime, updatetime, count from t_subscribe_info ORDER BY createtime ASC;";
//const char* const kDeleteSubscribeInfo = "DELETE FROM t_subscribe_info where eid_=%u;";
//const char* const kDeleteAllSubscribeData = "DELETE FROM t_subscribe_info;DELETE FROM t_expanditem_info;";

// contacts.t_expanditem_info==================================================
//const char* const kReplaceExpandItemInfo = "REPLACE INTO t_expanditem_info(id_, type, expandtime, count) VALUES (@id_, @type, @expandtime, @count);";
//const char* const kSelectExpandItemInfo = "SELECT id_, type, expandtime,count from t_expanditem_info WHERE id_=%d and type=%d;";
//const char* const kSelectAllExpandItemInfo = "SELECT id_, type, expandtime, count from t_expanditem_info;";
//const char* const kDeleteExpandItemInfo = "DELETE FROM t_expanditem_info WHERE id_=%u and type=%d;";

// 个人数据漫游
// contact.t_define_top_emp====================================================
const char* const kReplaceTopEmpInfo = "REPLACE INTO t_define_top_emp(eid, is_default, update_flag) VALUES (@eid, @is_default, @update_flag);";
const char* const kSelectTopEmpsInfo = "SELECT eid, is_default, update_flag from t_define_top_emp  inner join t_emp where  t_define_top_emp.eid = t_emp.eid_ ;";
const char* const kDeleteTopEmpInfo = "DELETE FROM t_define_top_emp WHERE eid=%u;";
const char* const kDeleteAllNoDefaultEmpInfo = "DELETE FROM t_define_top_emp WHERE is_default=0 AND update_flag=2;";
const char* const kDeleteAllDefaultEmpInfo = "DELETE FROM t_define_top_emp WHERE is_default=1;";
// contact.t_define_top_dept===================================================
const char* const kReplaceTopDeptInfo = "REPLACE INTO t_define_top_dept(did, update_flag) VALUES (@did, @update_flag);";
const char* const kSelectTopDeptsInfo = "SELECT did, update_flag from t_define_top_dept inner join t_dept where t_define_top_dept.did= t_dept.did_;";
const char* const kDeleteTopDeptInfo = "DELETE FROM t_define_top_dept WHERE did=%u;";
const char* const kDeleteAllTopDeptInfo = "DELETE FROM t_define_top_dept;";
// contact.t_define_group======================================================
const char* const kReplaceDefineGroupInfo = "REPLACE INTO t_define_group(id, alias, update_flag) VALUES (@id, @alias, @update_flag);";
const char* const kSelectDefineGroupInfo = "SELECT id, alias, update_flag from t_define_group;";
const char* const kSelectInsertDefineGroupID = "SELECT MAX(id) As id from t_define_group;";
const char* const kDeleteDefineGroupInfo = "DELETE FROM t_define_group WHERE id=%u;DELETE FROM t_define_group_emps WHERE id=%u";
// contact.t_define_group_emps=================================================
const char* const kReplaceDefineGroupEmpInfo = "REPLACE INTO t_define_group_emps(id, eid, update_flag) VALUES (@id, @eid, @update_flag);";
const char* const kSelectDefineGroupEmpsInfo = "SELECT eid, update_flag from t_define_group_emps WHERE id=%u and eid IN(SELECT eid_ from t_dept_emp);";   //SELECT eid, update_flag from t_define_group_emps g INNER JOIN t_emp e ON g.eid=e.eid_ WHERE id=%u AND NOT(e.flag & 0x00080000);
const char* const kSelectDefineGroupEmpsInfo1 = "SELECT eid, update_flag from t_define_group_emps WHERE id=%u;";
const char* const kDeleteDefineGroupEmpInfo  = "DELETE FROM t_define_group_emps WHERE id=%u and eid=%u;";
// contact.t_define_common_group======================================================
const char* const kReplaceDefineCommonGroupInfo = "REPLACE INTO t_define_common_group(id, alias, update_flag) VALUES (@id, @alias, @update_flag);";
const char* const kSelectDefineCommonGroupInfo  = "SELECT id, alias, update_flag from t_define_common_group;";
const char* const kDeleteDefineCommonGroupInfo  = "DELETE FROM t_define_common_group WHERE id=%lld;";
const char* const kSelectDefineCommonGroupInfoId= "SELECT id, alias, update_flag from t_define_common_group WHERE id=%lld;";

// contacts.t_emp======
const char* const kUpdateEmpSetFlag		= "UPDATE t_emp SET flag=flag|%d  WHERE eid_=%u;";
const char* const kUpdateEmpClearFlag	= "UPDATE t_emp SET flag=flag&~%d WHERE eid_=%u;";
const char* const kClearEmpConcernFlag  = "UPDATE t_emp SET flag=flag&~%d WHERE flag&%d=%d;";

// contacts.t_robot_info=======================================================
const char* const kReplaceRobotInfo = "REPLACE INTO t_robot_info(userid,usertype,attribute,greetings) VALUES (@userid,@usertype,@attribute,@greetings);";
const char* const kDeleteAllRobotInfo="DELETE FROM t_robot_info";
const char* const kSelectRobotInfo="SELECT userid,usertype,attribute,greetings FROM t_robot_info WHERE userid=%u";
const char* const kSelectAllRobotInfo = "SELECT userid,usertype,attribute,greetings FROM t_robot_info";
// contacts.t_update_contacts=======================================================
const char* const kSelectUpdateContactsInfo     = "SELECT id_, type, elapse, flag FROM t_update_contacts ORDER BY id_ DESC LIMIT 1;";
const char* const kSelectUpdateContactsInfoById = "SELECT id_, type, elapse, flag FROM t_update_contacts WHERE id_=%ld;";
const char* const kReplaceUpdateContactsInfo	= "REPLACE INTO t_update_contacts(id_, type, elapse, flag) VALUES (@id_, @type, @elapse, @flag);";

// msg.t_session_records ======================================================
const char* const kReplaceRecords     = "REPLACE INTO t_session_records("
	"msgid_, sid, eid, msg, timestamp, type, flag"
	") VALUES ("
	"@msgid_, @sid, @eid, @msg, @timestamp, @type, @flag);";
const char* const kInsertRecords     = "INSERT INTO t_session_records("
	"msgid_, sid, eid, msg, timestamp, type, flag"
	") VALUES ("
	"@msgid_, @sid, @eid, @msg, @timestamp, @type, @flag);";

const char* const kUpdateRecordsSetFlag		= "UPDATE t_session_records SET flag=flag|%d  WHERE msgid_=%lld;";
const char* const kUpdateRecordsClearFlag	= "UPDATE t_session_records SET flag=flag&~%d WHERE msgid_=%lld;";

const char* const kUpdateRecordsSetFalgByQSID	= "UPDATE t_session_records SET flag=flag|%d  WHERE sid=%lld;";
const char* const kUpdateRecordsClearFlagByQSID = "UPDATE t_session_records SET flag=flag&~%d WHERE sid=%lld AND flag<>0;";
const char* const kUpdateReadMsgSyncNotice		= "UPDATE t_session_records SET flag=flag&~%d WHERE sid=%lld AND flag<>0 AND timestamp<=%u;";

const char* const kUpdateRecordsType = "UPDATE t_session_records SET type=%d, msg='%s' WHERE msgid_=%lld;";

const char* const kSelectRecords		= "SELECT sid, eid, msg, timestamp, type, flag, msgid_ FROM t_session_records WHERE %s ORDER BY timestamp %s LIMIT %u OFFSET %u;";	
const char* const kSelectRecords2		= "SELECT sid, eid, msg, timestamp, type, flag, msgid_ FROM t_session_records WHERE %s AND %s ORDER BY timestamp %s LIMIT %u OFFSET %u;";	
const char* const kSelectRecordsName	= "EXISTS(SELECT sid_ FROM t_session_emp INNER JOIN t_emp ON t_session_emp.eid_=t_emp.eid_ WHERE (t_emp.name LIKE '%%%q%%' or t_emp.ucode LIKE '%%%q%%') and sid_=%lld)";
const char* const kSelectRecordsNameText= "msg LIKE '%%%q%%' AND EXISTS(SELECT sid_ FROM t_session_emp INNER JOIN t_emp ON t_session_emp.eid_=t_emp.eid_ WHERE (t_emp.name LIKE '%%%q%%' or t_emp.ucode LIKE '%%%q%%') and sid_=%lld)";
const char* const kSelectRecordsDate	= "SELECT timestamp FROM t_session_records WHERE sid=%lld AND timestamp>=%u AND timestamp<=%u GROUP BY timestamp;";
const char* const kSelectRecordsCount	= "SELECT count(msgid_) FROM t_session_records WHERE sid=%lld AND timestamp>=%u AND timestamp<=%u;";
const char* const kSelectRecordsFlag	= "SELECT sid, eid, msg, timestamp, type, flag, msgid_ FROM t_session_records WHERE (%s) and flag&%d ORDER BY timestamp %s LIMIT %u OFFSET %u;";
const char* const kSelectRecordsQmid	= "SELECT sid FROM t_session_records WHERE msgid_=%lld;";
const char* const kSelectRecordsQmid2	= "SELECT sid, eid, msg, timestamp, type, flag, msgid_ FROM t_session_records WHERE msgid_=%lld;";	
const char* const kSelectRecordsLastId	= "SELECT msgid_ FROM t_session_records WHERE ((msgid_>>61)=3) ORDER BY msgid_ DESC LIMIT 1;";	// type is PC
const char* const kSelectRecordsSessionLastMsgTime = "SELECT timestamp FROM t_session_records WHERE sid=%lld ORDER BY timestamp DESC LIMIT 1;";
const char* const kSelectRecordsBySid   =  "SELECT msg FROM t_session_records WHERE sid=%lld;";	
const char* const kSelectRecordsLastMsgSid = "SELECT sid, eid, msg, timestamp, type, flag, msgid_ FROM t_session_records WHERE sid=%lld AND type>=0 AND type<=5 ORDER BY timestamp DESC LIMIT 1;";	

const char* const kSelectMsgidByFilefid = "SELECT msgid FROM t_session_files WHERE fid_=%lld ;";
const char* const kSelectMsgByFilefid = "SELECT msg FROM t_session_records WHERE msgid_ = (SELECT msgid FROM t_session_files WHERE fid_=%lld) ;";
const char* const kSelectSidByFilefid = "SELECT sid FROM t_session_records WHERE msgid_ = (SELECT msgid FROM t_session_files WHERE fid_=%lld) ;";
const char* const kSelectFileInfo = "SELECT name , size FROM t_session_files WHERE msgid = %lld ;";

const char* const kCreateMsgTempTable = "CREATE TABLE IF NOT EXISTS t_session_msg_temp (msg TEXT);";
const char* const kInsertIntoMsgTempTable = "INSERT INTO t_session_msg_temp (msg) VALUES ('%q');";
const char* const kClearMsgTempTable = "DELETE FROM t_session_msg_temp";
const char* const kDropMsgTempTable = "DROP TABLE IF EXISTS t_session_msg_temp;";
//const char* const kSelectImageMsg = "SELECT msg FROM t_session_records WHERE timestamp %s (select timestamp from t_session_records where msg like '%%%q%%')"
const char* const kSelectImage1 = "SELECT msg from t_session_records where "
	" timestamp %s (select timestamp from t_session_records where msgid_=%lld) and sid=%lld order by timestamp %s limit %d;";
const char* const kSelectImage2 = "SELECT msg from t_session_msg_temp where msg like '%%<MsgImage%%' limit %d; ";
//collectmsg sql
const char* const kSelectMaxCollectId = " SELECT max(collectid_) FROM t_msg_collects ";
const char* const kSelectCollectItemCount = "SELECT count(collectid_) FROM t_msg_collects";
const char* const kSelectCollects = "SELECT * FROM t_msg_collects;";
const char* const kSelectCollectsType = "SELECT * FROM t_msg_collects WHERE type=%u;";
const char* const kInsertCollectsTime     = "REPLACE INTO t_collect_times(eid, collecttime) VALUES  (@eid, @collecttime);";
const char* const kInsertCollects     = "REPLACE INTO t_msg_collects("
    "collectid_, msg, title, isgroup, timestamp, type, sendid, needremind,sid) \
    VALUES  (@collectid_,   @msg, @title, @isgroup, @timestamp, @type, @sendid, @needremind,@sid);";
const char* const kDeleteCollectItem = "DELETE From t_msg_collects WHERE collectid_=%llu;";
const char* const kSelectContents = "SELECT msg FROM t_msg_collects WHERE collectid_=%llu;";
const char* const kSelectTime = "SELECT collecttime FROM t_collect_times WHERE eid=%u;";

const char* const kSelectItemData = "SELECT * FROM t_msg_collects WHERE collectid_=%u;";

const char* const kInsertUrlFileItem = "INSERT INTO t_url_file(msgid, downurl, loaclPath) VALUES  (@msgid,   @downurl, @loaclPath);";
const char* const kSelectLocalPath = "select loaclPath from t_url_file where msgid=%lld and downurl='%s';";

//const char* const kNewSelectRecords			= "select sid_,utime from t_session where t_session.[type] = %d AND utime>0 %s %s %s %s";
const char* const kNewSelectRecords2		= "SELECT sid_, type, title, cid, utime from t_session where (t_session.[type] = %d or t_session.[type]=%d) AND utime>0 %s %s %s %s";
const char* const kNewSelectRecords3		= "SELECT sid_, type, title, cid, utime from t_session where (t_session.[type] = %d or t_session.[type]=%d or t_session.[type]=%d) AND utime>0 %s %s %s %s";
const char* const kNewSelectRecords4		= "SELECT sid_, type, title, cid, utime from t_session where utime>0 %s %s %s %s";
const char* const kNewRecordsAddAccountLike = "AND sid_ in(select distinct sid_ from t_session_emp inner join t_emp on t_session_emp.[eid_] = t_emp.[eid_] where t_emp.[name] like '%%%q%%' or t_emp.ucode like '%%%q%%')";
const char* const kNewRecordsAddContentLike = "AND t_session_records.msg like '%%%q%%') ";
const char* const kNewRecordsContentLike    = "AND sid_ in(select distinct sid  from t_session_records where t_session_records.msg like '%%%q%%') ";
const char* const kNewSelectTitle           = "AND t_session.[title] like '%%%q%%' ";
const char* const kNewRecordsOrderBy		= "ORDER BY utime DESC;";

const char* const kSelectVirgroupSingle     = "select sid_ from t_session where type=6 and sid_ in " 
	" (select sid_ from t_session_emp where " 
	" eid_ = (select eid_ from t_session_emp  where "
	" sid_ = %lld and eid_ <> %u) and sid_<> %lld); ";
const char* const kNewSelectMsgCount2		= "SELECT count(msgid_) FROM t_session_records WHERE sid=%lld or sid=%lld %s %s %s;";

const char* const kNewSelectMsgCount		= "SELECT count(msgid_) FROM t_session_records WHERE sid=%lld %s %s %s;";
const char* const kNewSelectMsgDetail		= "SELECT sid, eid, msg, timestamp, type, flag, msgid_ FROM t_session_records WHERE sid=%lld %s %s %s ORDER BY timestamp ASC %s;";
const char* const kNewSelectMsgLimite       = "LIMIT %u OFFSET %u";
const char* const kNewMsgAddAccountLike		= "AND t_session_records.eid in (select eid_ from t_emp where t_emp.[name] like '%%%q%%' or t_emp.[ucode] like '%%%q%%') ";
const char* const kNewMsgAddContentLike		= "AND t_session_records.msg like '%%%s%%' ";
const char* const kNewMsgAddFileContentLike	= "AND (t_session_records.msg like '%%<MsgAnnex%%' OR t_session_records.msg like '%%<MsgVoice%%' OR t_session_records.msg like '%%<MsgVideo%%')";

const char* const kNewMsgAddContentTextLike = "AND t_session_records.msg like '%%%s%%' AND t_session_records.msg like '%%%s%%'";
const char* const kNewMsgAddFileContentTextLike	= "AND t_session_records.msg like '%%%s%%' AND (t_session_records.msg like '%%<MsgAnnex%%' OR t_session_records.msg like '%%<MsgVoice%%' OR t_session_records.msg like '%%<MsgVideo%%')";

const char* const kSelectEmpBySid           = "SELECT flag FROM t_session WHERE sid_=%lld;";
// msg.t_sms_records
const char* const kInsertSmsRecords     = "INSERT INTO t_sms_records("
	"msgid_, eid_or_phone, type, msg, timestamp, offmsg_flag, newmsg_flag, flag"
	") VALUES ("
	"@msgid_, @eid_or_phone, @type, @msg, @timestamp, @offmsg_flag, @newmsg_flag, @flag);";

const char* const kSelectSmsRecordsCount		= "SELECT eid_or_phone,count(*) as count,type,timestamp,msgid_ FROM t_sms_records %s %s %s union all SELECT eid_or_phone,count(*) as count,type,timestamp,msgid_ FROM t_sms_records %s %s %s;";
const char* const kSelectSmsRecordsJuseEidCount		= "SELECT eid_or_phone,count(*) as count,type,timestamp FROM t_sms_records %s %s %s ;";
const char* const kSelectSmsRecords				= "SELECT msgid_, eid_or_phone, type, msg, timestamp, offmsg_flag, newmsg_flag, flag FROM t_sms_records %s %s;";
const char* const kSelectSmsRecordsAccount		= "INNER JOIN t_emp ON t_sms_records.eid_or_phone=t_emp.eid_ WHERE (t_emp.name LIKE '%%%q%%' or t_emp.ucode LIKE '%%%q%%') ";
const char* const kSelectSmsRecordsAccountContent= "INNER JOIN t_emp ON t_sms_records.eid_or_phone=t_emp.eid_ WHERE (t_emp.name LIKE '%%%q%%' or t_emp.ucode LIKE '%%%q%%') AND msg LIKE '%%%q%%' ";
const char* const kSelectSmsRecordsContent       = "WHERE msg LIKE '%%%q%%' ";
const char* const kSelectSmsAllCount             = "SELECT count(*) FROM t_sms_records where eid_or_phone=%lld AND type=%d;";

// just count
const char* const kCountSelectRecords		= "SELECT count(sid) FROM t_session_records WHERE %s LIMIT %u OFFSET %u;";	
const char* const kCountSelectRecords2		= "SELECT count(sid) FROM t_session_records WHERE %s AND %s LIMIT %u OFFSET %u;";	
const char* const kCountSelectRecordsFlag	= "SELECT count(sid) FROM t_session_records WHERE flag&%d LIMIT %u OFFSET %u;";

// msg delete and exist
const char* const kDeleteRecordsQsid	= "DELETE FROM t_session_records WHERE sid=%lld;";
const char* const kDeleteRecordsQmid	= "DELETE FROM t_session_records WHERE msgid_=%lld;DELETE FROM t_session_files WHERE msgid=%lld;";
const char* const kDeleteSMSQmid	= "DELETE FROM t_sms_records WHERE msgid_=%lld;";
const char* const kDeleteBoardcastQmid	= "DELETE FROM t_bro_records WHERE msgid_=%lld;";
const char* const kDeleteMsgNoticeQmid	= "DELETE FROM t_alert_records WHERE msgid_=%lld;";
const char* const kSelectMsgExistQmid	= "SELECT msgid_ FROM t_session_records WHERE msgid_=%lld;";
const char* const kSelectSMSExistQmid	= "SELECT msgid_ FROM t_sms_records WHERE msgid_=%lld;";
const char* const kSelectBroExistQmid	= "SELECT msgid_ FROM t_bro_records WHERE msgid_=%lld;";
const char* const kSelectMsgNoticeExistQmid	= "SELECT msgid_ FROM t_alert_records WHERE msgid_=%lld;";

// msg.t_bro_records
const char* const kSelectBroInfo		= "SELECT msgid_,recver,title,msg, timestamp from t_bro_records %s %s %s ORDER BY timestamp asc;";

// msg.t_alert_records
const char* const kSelectMsgNoticeInfo		= "SELECT msgid_,sendid,title,msg, timestamp from t_alert_records %s %s %s ORDER BY timestamp desc;";

// msg.t_session_files
const char* const kReplaceFiles				= "REPLACE INTO t_session_files("
	"fid_, msgid, name, key, size, sender, timestamp, localpath, flag"
	") VALUES ("
	"@fid_, @msgid, @name, @key, @size, @sender, @timestamp, @localpath, @flag);";

const char* const kSelectFiles				= "SELECT timestamp, localpath, sender FROM t_session_files WHERE flag&%d != %d AND timestamp >= %u AND timestamp <= %u;";
const char* const kSelectFilesFid			= "SELECT name, msgid, key, size, sender, timestamp, localpath, flag FROM t_session_files WHERE fid_=%lld;";
const char* const kSelectFilesCount			= "SELECT count(fid_) as counts FROM t_session_files WHERE timestamp >= %u AND timestamp <= %u;";
 
const char* const kUpdateFilesSetFlagTt		= "UPDATE t_session_files SET flag=flag|%d  WHERE timestamp >= %u AND timestamp <= %u;";
const char* const kUpdateFilesClearFlagTt	= "UPDATE t_session_files SET flag=flag&~%d WHERE timestamp >= %u AND timestamp <= %u;";
const char* const kUpdateFilesSetFlagFid	= "UPDATE t_session_files SET flag=flag|%d  WHERE fid_=%lld;";
const char* const kUpdateFilesClearFlagFid	= "UPDATE t_session_files SET flag=flag&~%d WHERE fid_=%lld;";


const char* const kUseSelectFilesCount      = "SELECT count(*) FROM t_session_files %s %s %s;";
const char* const kUseSelectFiles			= "SELECT fid_, t_session_files.name, msgid, key, size, sender, timestamp, localpath, t_session_files.flag  FROM t_session_files %s %s %s;";
const char* const kUseSelectFilesAccount    = "INNER JOIN t_emp ON t_session_files.sender=t_emp.eid_ WHERE (t_emp.name LIKE '%%%q%%' or t_emp.ucode LIKE '%%%q%%') ";
const char* const kUseSelectFilesAccountFile= "INNER JOIN t_emp ON t_session_files.sender=t_emp.eid_ WHERE (t_emp.name LIKE '%%%q%%' or t_emp.ucode LIKE '%%%q%%') AND name LIKE '%%%q%%' ";
const char* const kUseSelectFilesFileName   = "WHERE name LIKE '%%%q%%' ";


// msg.msg.t_p2p_files
const char* const kReplaceP2p				= "REPLACE INTO t_p2p_files("
	"fid_, msgid, name, size, receiver, sender, timestamp, token, localpath, flag"
	") VALUES ("
	"@fid_, @msgid, @name, @size, @receiver, @sender, @timestamp, @token, @localpath, @flag);";

const char* const kSelectP2p				= "SELECT timestamp, localpath, sender, receiver FROM t_p2p_files WHERE flag&%d != %d AND timestamp >= %u AND timestamp <= %u;";
const char* const kSelectP2pFid				= "SELECT name, msgid, size, receiver, sender, timestamp, token, localpath, flag FROM t_p2p_files WHERE fid_=%lld;";
const char* const kSelectP2pCount			= "SELECT count(fid_) as counts FROM t_p2p_files WHERE timestamp >= %u AND timestamp <= %u;";

const char* const kUpdateP2pSetFlagTt		= "UPDATE t_p2p_files SET flag=flag|%d  WHERE timestamp >= %u AND timestamp <= %u;";
const char* const kUpdateP2pClearFlagTt		= "UPDATE t_p2p_files SET flag=flag&~%d WHERE timestamp >= %u AND timestamp <= %u;";
const char* const kUpdateP2pSetFlagFid		= "UPDATE t_p2p_files SET flag=flag|%d  WHERE fid_=%lld;";
const char* const kUpdateP2pClearFlagFid	= "UPDATE t_p2p_files SET flag=flag&~%d WHERE fid_=%lld;";

// t_bro_records
const char* const kInsertBroRecords     = "INSERT INTO t_bro_records("
	"msgid_, recver, title, msg, type, timestamp, offmsg_flag, newmsg_flag, flag"
	") VALUES ("
	"@msgid_, @recver, @title, @msg, @type, @timestamp, @offmsg_flag, @newmsg_flag, @flag);";

// t_alert_records
const char* const kInsertAlertRecords     = "INSERT INTO t_alert_records("
	"msgid_, sendid, title, msg, type, timestamp, offmsg_flag, newmsg_flag, flag"
	") VALUES ("
	"@msgid_, @sendid, @title, @msg, @type, @timestamp, @offmsg_flag, @newmsg_flag, @flag);";

// t_msg_read
const char* const kInsertMsgRead = "INSERT INTO t_msg_read ( msgid_, receiver, readtimer, flag, timestamp) VALUES (@msgid_, @receiver, @readtimer, @flag, @timestamp);";
const char* const kCountMsgRead = "SELECT COUNT(*) AS has_read_num FROM t_msg_read WHERE msgid_=%lld AND flag=%u;";
const char* const kCountMsgReadAll = "SELECT COUNT(*) AS all_num FROM t_msg_read WHERE msgid_=%lld;";
const char* const kUpdateMsgReadFlag = "UPDATE t_msg_read SET flag=flag|%d WHERE msgid_=%lld AND receiver=%u;";
const char* const kUpdateMsgReadClearFlag = "UPDATE t_msg_read SET flag=flag&~%d WHERE msgid_=%lld AND receiver=%u;";
const char* const kSelectMsgRead = "SELECT * FROM t_msg_read WHERE msgid_=%lld;";
const char* const kUpdateMsgRead = "UPDATE t_msg_read SET flag=flag|%d, readtimer=%u WHERE msgid_=%lld AND receiver=%u;";

//t_meeting_basicinfo
const char* const kInsertMeetingBasicInfo = "REPLACE INTO t_meeting_basicinfo ( confid_, hostcode, mbrcode, conftitle, starttime, endtime, conflength, confstate, creatoracct, creatorid, "
	" conftype, isrepeat,repeatkey, confmode,reallength, mbrmaxnum, mbrurl, hosturl, location,updatetype, updatetime, conflevel , isreceive ,islook,smsnotice,dnumbernum,dfilenum,realstarttime,realendtime) VALUES (@confid_, @hostcode, @mbrcode, @conftitle, "
	" @starttime, @endtime, @conflength, @confstate, @creatoracct, @creatorid, @conftype, @isrepeat,@repeatkey, @confmode, @reallength, @mbrmaxnum, @mbrurl, @hosturl, @location,@updatetype, @updatetime,@conflevel , @isreceive ,@islook,@smsnotice,@dnumbernum,@dfilenum,@realstarttime,@realendtime);";
const char* const kDeleteMeetingBasicInfo = "DELETE FROM t_meeting_basicinfo WHERE confid_='%s';";
const char* const kUpdateMeetingBasicInfo = "UPDATE t_meeting_basicinfo SET confid_=@confid_, hostcode=@hostcode, mbrcode=@mbrcode, conftitle=@conftitle, starttime=@starttime, endtime=@endtime, conflength=@conflength, confstate=@confstate, creatoracct=@creatoracct, creatorid=@creatorid,conftype=@conftype, isrepeat=@isrepeat, repeatkey=@repeatkey,"
	" confmode=@confmode, reallength=@reallength, mbrmaxnum=@mbrmaxnum, mbrurl=@mbrurl, hosturl=@hosturl, location=@location, updatetype=@updatetype, updatetime=@updatetime, conflevel=@conflevel , isreceive=@isreceive ,islook=@islook,smsnotice=@smsnotice,dnumbernum=@dnumbernum,dfilenum=@dfilenum,realstarttime=@realstarttime,realendtime=@realendtime WHERE confid_=@confid_; ";
const char* const kDeleteMeetingMbr = "DELETE FROM t_meeting_member WHERE confid='%s';";
const char* const kDeleteMeetingFileInfo = "DELETE FROM t_meeting_file WHERE confid='%s';";
const char* const kDeleteFileOnPathTable = "DELETE FROM t_meeting_filepath WHERE confid='%s';";
const char* const kSelectUpdateTime = "SELECT MAX(%s) from %s WHERE %s<%d;";
const char* const kInsertMeetingMbrInfo = "REPLACE INTO t_meeting_member (confid , mbrid ,dwoperaid ,isaccept, updatetime ,updatetype,conflevel,cisread) VALUES (@confid , @mbrid , @dwoperaid ,@isaccept,@updatetime ,@updatetype,@conflevel,@cisread);";
//const char* const kUpdateMeetingMbrInfo = "UPDATE t_meeting_member SET confid=@confid , mbrid=@mbrid ,dwoperaid=@dwoperaid ,isaccept=@isaccept, updatetime=@updatetime ,updatetype=@updatetype,conflevel=@conflevel WHERE confid='%s';";
const char* const kDeleteMeetingMbrInfo = "DELETE FROM t_meeting_member WHERE confid='%s' AND mbrid=%u;";
const char* const kInsertMeetingFileInfo = "REPLACE INTO t_meeting_file (confid , userid ,cterminal ,copertype, updatetime ,filesize, filename ,url) VALUES (@confid , @userid ,@cterminal ,@copertype, @updatetime ,@filesize, @filename ,@url);";
const char* const kDeleteMeetingFile = "DELETE FROM t_meeting_file WHERE confid='%s' AND url='%s';";
const char* const kSelectBasicInfo = "SELECT confid_,hostcode,mbrcode,conftitle,starttime,endtime,conflength,confstate,creatoracct,creatorid,conftype,isrepeat,repeatkey,confmode,reallength,mbrmaxnum,mbrurl,hosturl,location,confremark,updatetype,"
	" updatetime,smsnotice,dnumbernum,dfilenum,videourl,RealStartTime,RealEndTime,sharedocurl,videodownurl FROM t_meeting_basicinfo WHERE confid_='%s';";
const char* const kSelectBasicMbrInfo = "SELECT * FROM t_meeting_member INNER JOIN t_emp ON t_meeting_member.mbrid = t_emp.eid_ where  confid='%s' ORDER BY replace(t_emp.ucode,'v_','') ASC;";
const char* const kSelectAllBasicInfo = "SELECT confid_,hostcode,mbrcode,conftitle,starttime,endtime,conflength,confstate,creatoracct,creatorid,conftype,isrepeat,repeatkey,confmode,reallength,mbrmaxnum,mbrurl,hosturl,location,confremark,updatetype,"
	" updatetime,smsnotice,dnumbernum,dfilenum,videourl,RealStartTime,RealEndTime FROM t_meeting_basicinfo;";
const char* const kSelectTadayMeetingCount = "SELECT count(confid_) from t_meeting_basicinfo WHERE starttime BETWEEN %lld AND %lld ;";
const char* const kSelectMeetingFiles = "SELECT url,filename,filesize,updatetime,confid FROM t_meeting_file WHERE confid='%s' ORDER BY updatetime ASC;";
const char* const kSelectMeetingFilePath = "SELECT path FROM t_meeting_filepath WHERE confid='%s' AND token='%s' AND flag=0;";
const char* const kUpdateMeetingLevel = "UPDATE t_meeting_member SET conflevel=%d WHERE confid='%s' AND mbrid=%d ;";
const char* const kSelectMeetingLevel = "SELECT conflevel FROM t_meeting_member WHERE confid='%s' AND mbrid=%d ;";
const char* const kSelectMeetingCreator = "SELECT creatorid FROM t_meeting_basicinfo WHERE confid_='%s';";
const char* const kUpdateMeetingFileInfo = "INSERT INTO t_meeting_filepath (confid ,token ,key,path,flag) VALUES (@confid ,@token ,@key,@path,@flag);";
const char* const kUpdateMeetingIsRead =   "UPDATE t_meeting_member SET cisread=%d WHERE confid='%s' AND mbrid=%d ;";
const char* const kSelectMeetingIsRead = "SELECT cisread FROM t_meeting_member WHERE confid='%s' AND mbrid=%d ;";
const char* const kSelectMeetingIsAccept = "SELECT isaccept FROM t_meeting_member WHERE confid='%s' AND mbrid=%d ;";
const char* const kCheckeFileExist1 = "SELECT * FROM t_meeting_file WHERE confid='%s' AND url='%s'; ";
const char* const kCheckeFileExist2 = "SELECT * FROM t_meeting_filepath WHERE confid='%s' AND token='%s' AND key >= %ld AND flag=0; ";
const char* const kCheckeFileExist3 = "SELECT * FROM t_meeting_filepath WHERE confid='%s' AND token='%s' AND key >= %ld AND flag=1; ";
const char* const kDeletePathTableFile = "DELETE FROM t_meeting_filepath WHERE confid='%s' AND token='%s' AND flag=0;";
const char* const kClearpathTable = "DELETE FROM t_meeting_filepath WHERE flag=1;";
const char* const kSetMeetingUrl = "UPDATE t_meeting_basicinfo SET %s='%s' WHERE confid_='%s';";
const char* const kUpdateBasicRemark = "UPDATE t_meeting_basicinfo SET confremark='%s' WHERE confid_='%s';";
const char* const kInsertBasicRemark = "INSERT INTO t_meeting_basicinfo (confremark,confid_) VALUES ('%s','%s');";
const char* const kSelectMeetingExist = "SELECT confid_ FROM t_meeting_basicinfo WHERE confid_='%s';";
const char* const kSelectMeetingExist1 = "SELECT confid FROM t_meeting_member WHERE confid='%s' AND mbrid=%d;";

const char* const kSelectSessionList1 = "SELECT sid_, type, title, remark, flag, cid, ctime, utime, off_msg, new_msg, sutime, virgroupid, (SELECT toptime FROM t_session_top WHERE t_session_top.sid_=t_session.sid_) AS toptime FROM t_session WHERE utime>0 ORDER BY utime desc limit 0,%d ;";
const char* const kSelectSessionList2 = "SELECT sid_, type, title, remark, flag, cid, ctime, utime, off_msg, new_msg, sutime, virgroupid, (SELECT toptime FROM t_session_top WHERE t_session_top.sid_=t_session.sid_) AS toptime FROM t_session WHERE utime>0 AND utime < %u  ORDER BY utime desc limit 0,%d ;";
const char* const kSelectSingleMailBySid = "SELECT sex FROM t_emp WHERE eid_= (SELECT em.eid_ FROM t_session_emp em INNER JOIN "
	" t_session ts ON em.sid_=ts.sid_ WHERE ts.sid_='%lld' AND em.eid_ <> %u);";
const char* const kSelectSessionMsgCount = "SELECT count(msgid_) FROM t_session_records WHERE sid=%lld;";

const char* const kSelectSessionValid = "SELECT COUNT(sid_) FROM t_session WHERE sid_=%lld AND NOT(flag & 0x00080000);";
#endif // __EIM_Database_CONST_HEADER_2014_07_02_YFGZ__