#pragma once
#include "dbcontrol.h"
#include "CppSQLite3.h"
#include "SystemLogger.h"
#include <exception>
//#include <winsock2.h>
#include <windows.h>
#include <string>
#include <stdio.h>
#include "utf8conv.h"
using namespace std;
using namespace utf8util;

extern CSystemLogger gSystemLogger;

using namespace std;


template <typename _QueryType>
class CSqliteDB :public CDBControl<_QueryType>
{
public:
	CSqliteDB();
	virtual ~CSqliteDB(void);

	bool Connect(void);
	bool Connect(char* dbName);
	bool Connect(char* serverName, char* dbName, char* userName, char* pwd, bool isSecure);
	bool CreateTable(const char* tbname, const char* fieldstr);
	bool CreateTable(const char* sqlstr);
	bool DropTable(const char* tbname);
	bool BeginTransaction();
	bool CommitTransaction();
	bool CreateView(char* dbName);
	int Excute(const char* sqlstr);
	int Row(const char* sqlstr);
	int saveData(const char* tbname,const char* username, const char*key, const char*level);
	int saveOPT(const char* tbname, const char* username, const char*opt, const char*time);
	int saveCountData(const char* tbname, const char* date, const char*total, const char*OKCount, const char*OKCountRate, const char* Time);
	int updateUserData(const char* username,const char * key,const char* level,int id);
	int updateUserData(const char* username, const char* level,int id);
	int QueryRow(const char*field);
	int QueryRecordRow(const char*field, const char* crtProj);
	int DeleteExpRecord(const char*field, const char* crtProj);
	QUERY_USER_RESULT GetResult(int m_Row);
	QUERY_USER_RESULT GetPassWord(const char* username);
	int DeleteRow(int id);
	_QueryType Query(const char* sqlstr);
	bool Close(void);
	__int64 GetLastRowID();
	bool TableExists(const char* tbname)
	{
		return m_db.tableExists(tbname);
	}

protected:
	bool Open(void);
	bool Open(char* dbname);
	bool Lock();
	void Unlock();

private:
	CppSQLite3DB m_db;
	HANDLE       m_hfile;
};


template <typename _QueryType>
int CSqliteDB<_QueryType>::DeleteExpRecord(const char*field, const char* crtProj)
{
	int ret = 0;
	try
	{
		ret = m_db.deleteRecord(field, crtProj);

	}
	catch (CppSQLite3Exception& e)
	{
		std::string strMsg = e.errorMessage();
		std::wstring wstrMsg = localUtils::utils::Utf8ToUnicode(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;
}


template <typename _QueryType>
int CSqliteDB<_QueryType>::QueryRecordRow( const char*field, const char* crtProj)
{
	int ret = 0;
	try
	{
		ret = m_db.getRecordRow(field, crtProj);
	}
	catch (CppSQLite3Exception& e)
	{
		std::string strMsg = e.errorMessage();
		std::wstring wstrMsg = localUtils::utils::Utf8ToUnicode(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;
}


template <typename _QueryType>
int CSqliteDB<_QueryType>::saveCountData(const char* tbname, const char* date, const char*total, const char*OKCount, const char*OKCountRate, const char* Time)
{
	int ret = 0;
	try
	{
		ret = m_db.saveCountDataInstant(tbname,date,total,OKCount,OKCountRate,Time);
	}
	catch (CppSQLite3Exception& e)
	{
		std::string strMsg = e.errorMessage();
		std::wstring wstrMsg = localUtils::utils::Utf8ToUnicode(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;

}

template <typename _QueryType>
int CSqliteDB<_QueryType>::saveOPT(const char* tbname, const char* username, const char*opt, const char*time)
{
	int ret = 0;
	try
	{
		ret = m_db.saveUserOPT(tbname, username, opt, time);
	}
	catch (CppSQLite3Exception& e)
	{
		std::string strMsg = e.errorMessage();
		std::wstring wstrMsg = localUtils::utils::Utf8ToUnicode(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;
}

template <typename _QueryType>
bool CSqliteDB<_QueryType>::CreateView(char* dbName)
{
	bool ret = false;
	try
	{
		ret = m_db.CreateUserView(dbName);
	}
	catch (CppSQLite3Exception& e)
	{
		std::string strMsg = e.errorMessage();
		std::wstring wstrMsg = localUtils::utils::Utf8ToUnicode(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;

}

template <typename _QueryType>
CSqliteDB<_QueryType>::CSqliteDB()
{
	m_hfile = NULL;
}

template <typename _QueryType>
CSqliteDB<_QueryType>::~CSqliteDB(void)
{
	m_db.close();
}

template <typename _QueryType>
bool CSqliteDB<_QueryType>::Open(void)
{
	try
	{
		m_db.open(m_strDbName.c_str());
	}
	catch (CppSQLite3Exception& e)
	{
		TCHAR msg[MSG_LEN], file[FILE_LEN], func[FUNC_LEN] ;
		wstring wstrTmp = utf8util::UTF16FromUTF8(e.errorMessage());
		_stprintf_s(msg, MSG_LEN, _T("%s"), wstrTmp.c_str());
		wstring wstrFile = localUtils::utils::Utf8ToUnicode(__FILE__);
		_stprintf_s(file, FILE_LEN, _T("%s"), wstrFile.c_str());
		wstring wstrFuncName = localUtils::utils::Utf8ToUnicode(__FUNCTION__);
		_stprintf_s(func, FUNC_LEN, _T("%s"), wstrFuncName.c_str());
		CString strMsg = msg;
		wstring wstrMsg = CT2W(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return false;
	}
	return true;
}

template <typename _QueryType>
bool CSqliteDB<_QueryType>::Open(char* dbname)
{              
	SetDBName(dbname);
	return Open();
}

template <typename _QueryType>
__int64 CSqliteDB<_QueryType>::GetLastRowID()
{
	return m_db.lastRowId();
}

template <typename _QueryType>
bool CSqliteDB<_QueryType>::Connect(void)
{
	extern CSystemManager gSystemMgr;
	wstring wstrPath = gSystemMgr.m_wstrCrtDirectory;
	wstrPath.append(L"\data");
	wstrPath.append(L"\\DataBase.db");
	string strUAC = localUtils::utils::Unicode2Utf8(wstrPath);
	char* cUAC = const_cast<char*>(strUAC.c_str());
	return Open(cUAC);
}

template <typename _QueryType>
bool CSqliteDB<_QueryType>::Connect(char* dbName)
{
	
	return Open(dbName);
}

template <typename _QueryType>
bool CSqliteDB<_QueryType>::Connect(char* serverName, char* dbName, char* userName, char* pwd, bool isSecure)
{
	return Open(dbName);
}

template <typename _QueryType>
bool CSqliteDB<_QueryType>::CreateTable(const char *tbname, const char *fieldstr)
{
	bool exists = m_db.tableExists(tbname);
	if (!exists)
	{
		std::string sqlStr("Create Table ");
		sqlStr.append(tbname);
		sqlStr.append("(");
		sqlStr.append(fieldstr);
		sqlStr.append(");");
		try
		{
			m_db.execDML(sqlStr.c_str());
		}
		catch (CppSQLite3Exception& e)
		{
			wstring wstrMsg(L"Create Table [");
			wstrMsg.append(utf8util::UTF16FromUTF8(tbname));
			wstrMsg.append(L"] failed - ");
			wstrMsg.append(utf8util::UTF16FromUTF8(e.errorMessage()));
			gSystemLogger.QueueUpLog(wstrMsg);
			return false;
		}
	}
	return true;
}

template <typename _QueryType>
bool CSqliteDB<_QueryType>::DropTable(const char *tbname)
{
	bool exists = m_db.tableExists(tbname);
	if (exists)
	{
		std::string sqlStr("drop table ");
		sqlStr.append(tbname);
		try
		{
			m_db.execDML(sqlStr.c_str());
		}
		catch (CppSQLite3Exception& e)
		{
			wstring wstrMsg(L"Drop Table [");
			wstrMsg.append(utf8util::UTF16FromUTF8(tbname));
			wstrMsg.append(L"] failed - ");
			wstrMsg.append(utf8util::UTF16FromUTF8(e.errorMessage()));
			gSystemLogger.QueueUpLog(wstrMsg);
			return false;
		}
	}
	return true;
}

template <typename _QueryType>
bool CSqliteDB<_QueryType>::CreateTable(const char *sqlstr)
{
	try
	{
		m_db.execDML(sqlstr);
	}
	catch (CppSQLite3Exception& e)
	{
		wstring wstrMsg = utf8util::UTF16FromUTF8(e.errorMessage());
		wstrMsg.append(L" - Create Table failed ! - ");
		wstrMsg.append(utf8util::UTF16FromUTF8(sqlstr));
		gSystemLogger.QueueUpLog(wstrMsg);
	}
	return true;
}

template <typename _QueryType>
bool CSqliteDB<_QueryType>::BeginTransaction()
{
	int nRet = 0;
	try
	{
		nRet = m_db.execDML("BEGIN TRANSACTION");
	}
	catch(CppSQLite3Exception& e)
	{
		string strMsg = e.errorMessage();
		wstring wstrMsg = CA2W(strMsg.c_str());
		wstrMsg.append(L" - Error Code : ");
		wstring wstrErrCode = boost::lexical_cast<wstring>(e.errorCode());
		wstrMsg.append(wstrErrCode);
		gSystemLogger.QueueUpLog(wstrMsg);
		return false;
	}
	return (nRet == 0);
}

template <typename _QueryType>
bool CSqliteDB<_QueryType>::CommitTransaction()
{
    int nRet = 0;
	try
	{
		m_db.execDML("COMMIT TRANSACTION"); 
	}
	catch(CppSQLite3Exception&)
	{
		return false;
	}
	return (nRet == 0); 
}


template <typename _QueryType>
int CSqliteDB<_QueryType>::Excute(const char* sqlstr)
{
	int ret = 0;
	try
	{
		ret = m_db.execDML(sqlstr);
	}
	catch (CppSQLite3Exception& e)
	{
		wstring wstrMsg = utf8util::UTF16FromUTF8(e.errorMessage());
		wstrMsg.append(L" - Execute Command [");
		wstrMsg.append(utf8util::UTF16FromUTF8(sqlstr));
		wstrMsg.append(L"] failed!");
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;
}

template <typename _QueryType>
int CSqliteDB<_QueryType>::Row(const char* sqlstr)
{
	int ret = 0;
	try
	{
		ret = m_db.getRowNew(sqlstr);
	}
	catch (CppSQLite3Exception& e)
	{
		wstring wstrMsg = utf8util::UTF16FromUTF8(e.errorMessage());
		wstrMsg.append(L" - Execute Command [");
		wstrMsg.append(utf8util::UTF16FromUTF8(sqlstr));
		wstrMsg.append(L"] failed!");
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;
}

template <typename _QueryType>
int CSqliteDB<_QueryType>::saveData(const char* tbname, const char* username,const char*key,const char*level)
{
	int ret = 0;
	try
	{
		ret = m_db.saveUserData(tbname,username,key,level);
	}
	catch (CppSQLite3Exception& e)
	{
		std::string strMsg = e.errorMessage();
		std::wstring wstrMsg = localUtils::utils::Utf8ToUnicode(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;
}


template <typename _QueryType>
int CSqliteDB<_QueryType>::updateUserData(const  char* username,const char * key,const char* level,int id)
{
	int ret = 0;
	try
	{
		ret = m_db.updateUserPassWord(username,key,level,id);
	}
	catch (CppSQLite3Exception& e)
	{
		std::string strMsg = e.errorMessage();
		std::wstring wstrMsg = localUtils::utils::Utf8ToUnicode(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;
}
template <typename _QueryType>
int CSqliteDB<_QueryType>::updateUserData(const char* username,const char* level,int id)
{
	int ret = 0;
	try
	{
		ret = m_db.updateUserPassWord(username, level,id);
	}
	catch (CppSQLite3Exception& e)
	{
		std::string strMsg = e.errorMessage();
		std::wstring wstrMsg = localUtils::utils::Utf8ToUnicode(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;
}


template <typename _QueryType>
int CSqliteDB<_QueryType>::QueryRow(const char*field)
{
	int ret = 0;
	try
	{
		ret = m_db.getRow(field);
	}
	catch (CppSQLite3Exception& e)
	{
		std::string strMsg = e.errorMessage();
		std::wstring wstrMsg = localUtils::utils::Utf8ToUnicode(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;
}

template <typename _QueryType>
QUERY_USER_RESULT CSqliteDB<_QueryType>::GetResult(int m_Row)
{
	QUERY_USER_RESULT ret = { 0, "", "", "" };
	try
	{
		ret = m_db.getUserResult(m_Row);
	}
	catch (CppSQLite3Exception& e)
	{
		std::string strMsg = e.errorMessage();
		std::wstring wstrMsg = localUtils::utils::Utf8ToUnicode(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;
}

template <typename _QueryType>
QUERY_USER_RESULT CSqliteDB<_QueryType>::GetPassWord(const char* username)
{
	QUERY_USER_RESULT ret = { 0,"","","" };
	try
	{
		ret = m_db.getPassWord(username);
	}
	catch (CppSQLite3Exception& e)
	{
		std::string strMsg = e.errorMessage();
		std::wstring wstrMsg = localUtils::utils::Utf8ToUnicode(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;
}

template <typename _QueryType>
int CSqliteDB<_QueryType>::DeleteRow( int id)
{
	int ret = 0;
	try
	{
		ret = m_db.deleteUserRow(id);
	}
	catch (CppSQLite3Exception& e)
	{
		std::string strMsg = e.errorMessage();
		std::wstring wstrMsg = localUtils::utils::Utf8ToUnicode(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return ret;
	}
	return ret;
}

template <typename _QueryType>
_QueryType CSqliteDB<_QueryType>::Query(const char* sqlstr)
{
	_QueryType result;
	try
	{
		result = m_db.execQuery(sqlstr);
	}
	catch (CppSQLite3Exception& e)
	{
		wstring wstrMsg = utf8util::UTF16FromUTF8(e.errorMessage());
		wstrMsg.append(L" - Execute Command [");
		wstrMsg.append(utf8util::UTF16FromUTF8(sqlstr));
		wstrMsg.append(L"] failed!");
		gSystemLogger.QueueUpLog(wstrMsg);
	}
	return result;
}


template <typename _QueryType>
bool CSqliteDB<_QueryType>::Close(void)
{
	if (m_hfile != NULL && INVALID_HANDLE_VALUE != m_hfile)
	{
		CloseHandle(m_hfile);
		m_hfile = NULL;
	}
	try
	{
		m_db.close();
	}
	catch(CppSQLite3Exception& e)
	{
		std::string strMsg = e.errorMessage();
		std::wstring wstrMsg = localUtils::utils::Utf8ToUnicode(strMsg);
		gSystemLogger.QueueUpLog(wstrMsg);
		return false;
	}
	return true;
}