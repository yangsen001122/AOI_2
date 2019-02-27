////////////////////////////////////////////////////////////////////////////////
// CppSQLite3 - A C++ wrapper around the SQLite3 embedded database library.
#pragma once

#include "sqlite3.h"
#include <cstdio>
#include <cstring>
#include <string>
#define SQL_INSERT_USER_MGMT_DATA  "Insert into '%s' values (NULL,'%s', '%s', '%s')" 
#define SQL_UPDATE_USER_DATA     "update UAC set  username='%s',key='%s',level= '%s'where id='%d'"
#define SQL_UPDATE_USER_DATANOKEY     "update UAC set  username='%s', level= '%s'where id='%d'"
#define SQL_DELETE_USER_MGMT_ROW   "Delete from UAC where id=('%d')" 
#define SQL_DELETE_RECORD          "Delete from '%s' where date<'%s'"
#define SQL_QUERY_USER_MGMT_DATA   "Select * from UAC" 
#define SQL_QUERY_RECORD_DATA      "Select * from  '%s' where date>='%s' and date<='%s'"
#define SQL_QUERY_USER_MGMT_ROW    "Select * from UAC where username=('%s')" 
#define SQL_QUERY_RECORD            "Select * from  '%s' where date<'%s'"
#define SQL_CREATE_VIEW            "Create view if not exists view_admin as select * from UAC where id>1"
#define SQL_INSERT_USER_OPT_DATA  "Insert into '%s' values ('%s', '%s', '%s')" 
#define SQL_INSERT_COUNT_DATA     "Insert into '%s'  values (NULL,'%s', '%s', '%s','%s','%s')"
#define SQL_QUERY_RECORD_DATA_FROM_ID      "Select * from  '%s'where id='%d'" 

using namespace std;
typedef struct
{
	int id;
	const char* username;
	const char* key;
	const char* level;

}QUERY_USER_RESULT;
typedef struct
{
	int nrow;
	int nid;
	int nTotal;
	int nOkCount;
	double dCrtTime;
}QUERY_DATA_RECORD;
typedef struct
{
	const char* Id;
	const char* Date;
	const char* Total;
	const char* OkCount;
	const char* OkCountRate;
	const char* CrtTime;
}QUERY_COUNT_DATA;
#pragma comment(lib, "sqlite3.lib")

#define MSG_LEN	         1024
#define FILE_LEN         MAX_PATH
#define FUNC_LEN         64
#define TIME_LEN         20
#define CHART_TOTAL_STEP 18
#define CPPSQLITE_ERROR  1000

class CppSQLite3Exception
{
public:
    CppSQLite3Exception(const int nErrCode,char* strErrMsg,bool bDeleteMsg=true);
    CppSQLite3Exception(const CppSQLite3Exception&  e);
    virtual ~CppSQLite3Exception();
    const int errorCode() { return m_nErrCode; }
    const char* errorMessage() { return m_strErrMsg; }
    static const char* errorCodeAsString(int nErrCode);
private:
    int   m_nErrCode;
    char* m_strErrMsg;
};

class CppSQLite3Buffer
{
public:
    CppSQLite3Buffer();
    ~CppSQLite3Buffer();
    const char* format(const char* szFormat, ...);
    operator const char*() { return m_strBuffer; }
    void clear();
private:
    char* m_strBuffer;
};

class CppSQLite3Binary
{
public:
    CppSQLite3Binary();
    ~CppSQLite3Binary();
    void setBinary(const unsigned char* pBuf, int nLen);
    void setEncoded(const unsigned char* pBuf);
    const unsigned char* getEncoded();
    const unsigned char* getBinary();
    int getBinaryLength();
    unsigned char* allocBuffer(int nLen);
    void clear();

private:
    unsigned char* m_strBuffer;
    int m_nBinaryLen;
    int m_nBufferLen;
    int m_nEncodedLen;
    bool m_bEncoded;
};

class CppSQLite3Query
{
public:
    CppSQLite3Query();
    CppSQLite3Query(const CppSQLite3Query& rQuery);
    CppSQLite3Query(sqlite3* pDB, sqlite3_stmt* pVM, bool bEof, bool bOwnVM=true);
    CppSQLite3Query& operator=(const CppSQLite3Query& rQuery);
    virtual ~CppSQLite3Query();
    int numFields();
    int fieldIndex(const char* szField);
	int fieldIndex( char* szField);
    const char* fieldName(int nCol);
    const char* fieldDeclType(int nCol);
    int fieldDataType(int nCol);
    const char* fieldValue(int nField);
    const char* fieldValue(const char* szField);

    int getIntField(int nField, int nNullValue=0);
    int getIntField(const char* szField, int nNullValue=0);

	sqlite_int64 getInt64Field(int nField, sqlite_int64 nNullValue=0);
	sqlite_int64 getInt64Field(const char* szField, sqlite_int64 nNullValue=0);

    double getFloatField(int nField, double fNullValue=0.0);
    double getFloatField(const char* szField, double fNullValue=0.0);

    const char* getStringField(int nField, const char* szNullValue="");
    const char* getStringField(const char* szField, const char* szNullValue="");

    const unsigned char* getBlobField(int nField, int& nLen);
    const unsigned char* getBlobField(const char* szField, int& nLen);

	bool fieldIsNull(int nField);
    bool fieldIsNull(const char* szField);
    bool eof();
    void NextRow();
    void finalize();
private:
    void checkVM();
	sqlite3* m_ptrSqlite3;
    sqlite3_stmt* m_ptrStmt;
    bool m_bEof;
    int m_nCols;
    bool m_bOwnVM;
};

class CppSQLite3Table
{
public:
    CppSQLite3Table();
    CppSQLite3Table(const CppSQLite3Table& rTable);
    CppSQLite3Table(char** paszResults, int nRows, int nCols);
    virtual ~CppSQLite3Table();

    CppSQLite3Table& operator=(const CppSQLite3Table& rTable);
    int numFields();
    int numRows();
    const char* fieldName(int nCol);
    const char* fieldValue(int nField);
    const char* fieldValue(const char* szField);

    int getIntField(int nField, int nNullValue=0);
    int getIntField(const char* szField, int nNullValue=0);

    double getFloatField(int nField, double fNullValue=0.0);
    double getFloatField(const char* szField, double fNullValue=0.0);

    const char* getStringField(int nField, const char* szNullValue="");
    const char* getStringField(const char* szField, const char* szNullValue="");

    bool fieldIsNull(int nField);
    bool fieldIsNull(const char* szField);
    void setRow(int nRow);
    void finalize();
private:
    void checkResults();
    int m_nCols;
    int m_nRows;
    int m_nCurrentRow;
    char** m_pStrResults;
};

class CppSQLite3Statement
{
public:
    CppSQLite3Statement();
	virtual ~CppSQLite3Statement();
    CppSQLite3Statement(const CppSQLite3Statement& rStatement);
    CppSQLite3Statement(sqlite3* pDB, sqlite3_stmt* pVM);
    CppSQLite3Statement& operator=(const CppSQLite3Statement& rStatement);
    int execDML();
    CppSQLite3Query execQuery();
    void bind(int nParam, const char* szValue);
    void bind(int nParam, const int nValue);
    void bind(int nParam, const double dwValue);
    void bind(int nParam, const unsigned char* blobValue, int nLen);
    void bindNull(int nParam);

	int bindParameterIndex(const char* szParam);
    void bind(const char* szParam, const char* szValue);
    void bind(const char* szParam, const int nValue);
    void bind(const char* szParam, const double dwValue);
    void bind(const char* szParam, const unsigned char* blobValue, int nLen);
    void bindNull(const char* szParam);
	void reset();
    void finalize();

private:
    void checkDB();
    void checkVM();
    sqlite3*      m_ptrSqlite3;
    sqlite3_stmt* m_ptrStmt;
};

class CppSQLite3DB
{
public:

    CppSQLite3DB();
    ~CppSQLite3DB();

    void open(const char* szFile);
    void close();
	bool tableExists(const char* szTable);
	bool CreateUserView(char* dbName);
    int execDML(const char* szSQL);
    CppSQLite3Query execQuery(const char* szSQL);
    int execScalar(const char* szSQL, int nNullValue=0);
    CppSQLite3Table getTable(const char* szSQL);
	int getRowNew(const char* szSQL);
	QUERY_USER_RESULT getUserResult(int m_Row);
	QUERY_USER_RESULT getPassWord(const char* username);
	int deleteUserRow(int id);
	int getRow(const char*field);
	int getRecordRow(const char*field, const char* crtProj);
	int deleteRecord(const char*field, const char* crtProj);
	int saveUserData(const char* tbname,const char* username, const char*key, const char*level);
	int saveUserOPT(const char* tbname, const char* username, const char*opt, const char*time);
	int saveCountDataInstant(const char* tbname, const char* date, const char*total, const char*OKCount, const char*OKCountRate, const char* Time);
	int updateUserPassWord(const char*username, const char*key,const char*level,int id);
	int updateUserPassWord(const char*username, const char*level,int id);
    CppSQLite3Statement compileStatement(const char* szSQL);
    sqlite_int64 lastRowId();
    void interrupt() { sqlite3_interrupt(m_ptrSqlite3); }
    void setBusyTimeout(int nMillisecs);
    static const char* SQLiteVersion() { return SQLITE_VERSION; }
    static const char* SQLiteHeaderVersion() { return SQLITE_VERSION; }
    static const char* SQLiteLibraryVersion() { return sqlite3_libversion(); }
    static int SQLiteLibraryVersionNumber() { return sqlite3_libversion_number(); }
	bool IsAutoCommitOn();
private:
    CppSQLite3DB(const CppSQLite3DB& db);
    CppSQLite3DB& operator=(const CppSQLite3DB& db);
    sqlite3_stmt* compile(const char* szSQL);
    void checkDB();

    sqlite3* m_ptrSqlite3;
    int      m_nBusyTimeoutMs;
};
