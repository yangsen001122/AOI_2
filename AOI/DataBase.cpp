#include "stdafx.h"
#include "SystemLogger.h"

extern CSystemLogger gSystemLogger;


CDatabase::CDatabase()
	:m_chErrMsg(nullptr),
	//m_db("myDatabase2.db"),
	m_pDB(nullptr)
{
}


CDatabase::~CDatabase()
{
	CloseDataBase();
}


void CDatabase::OpenDataBase(const char * m_db)
{
	m_nSqlite3Result = sqlite3_open(m_db, &m_pDB);// 打开数据库，不存在时创建 
	if (m_nSqlite3Result)
	{
		sqlite3_close(m_pDB);      
		gSystemLogger.SaveAndShowLog(L"Open/Create DataBase Fail！", LV_ERROR);
		return;
	}
}



void CDatabase::CloseDataBase()
{
	sqlite3_close(m_pDB);
}

void CDatabase::CreateTable(const char * create_table)
{
	m_nSqlite3Result = sqlite3_exec(m_pDB, create_table, NULL, NULL, &m_chErrMsg);
	if (m_nSqlite3Result != SQLITE_OK)
	{
		gSystemLogger.SaveAndShowLog(L"操作失败，数据库链表生成错误！", LV_ERROR);
		return;
	}
}

void CDatabase::SaveData( char * username, char * key, char * level)
{
	sprintf_s(sql, SQL_INSERT_USER_MGMT_DATA,  username, key, level);
	m_nSqlite3Result = sqlite3_exec(m_pDB, sql, 0, 0, &m_chErrMsg);
	if (m_nSqlite3Result != SQLITE_OK)
	{
		sqlite3_close(m_pDB);
		gSystemLogger.SaveAndShowLog(L"DataBase Save Data Fail！", LV_ERROR);
		return;
	}
}

void CDatabase::SaveDemuraData(char * barcode)
{
	sprintf_s(sql, SQL_INSERT_DEMURA_DATA, barcode);

	m_nSqlite3Result = sqlite3_exec(m_pDB, sql, 0, 0, &m_chErrMsg);
	if (m_nSqlite3Result != SQLITE_OK)
	{
		sqlite3_close(m_pDB);
		//gSystemLogger.SaveAndShowLog(L"DataBase Save Data Fail！", LV_ERROR);
		AfxMessageBox(_T("用户名已存在！"));
		return;
	}
}






int CDatabase::LoadDataBaseInfoCallback(void * para, int n_column, char ** column_value, char ** column_name)
{
	//para是你在 sqlite3_exec 里传入的 void * 参数
	//通过para参数，你可以传入一些特殊的指针（比如类指针、结构指针），
	//然后在这里面强制转换成对应的类型（这里面是void*类型，必须强制转换成你的类型才可用）。然后操作这些数据
	//n_column是这一条记录有多少个字段 (即这条记录有多少列)
	//char ** column_value 是个关键值，查出来的数据都保存在这里，它实际上是个1维数组（不要以为是2维数组），
	//每一个元素都是一个 char * 值，是一个字段内容（用字符串来表示，以/0结尾）
	//char ** column_name 跟 column_value是对应的，表示这个字段的字段名称
    //这里，我不使用 para 参数。忽略它的存在.
	
	CString strErrCode;
	strErrCode.Format(_T("记录包含 %d 个字段/n!"), n_column);
	gSystemLogger.QueueUpLog(strErrCode.GetString());
	for (int i = 0; i < n_column; i++)
	{
		strErrCode.Format(_T("字段名:%s,字段值:%s /n!"),column_name[i], column_value[i]);
		gSystemLogger.QueueUpLog(strErrCode.GetString());
	}
	gSystemLogger.QueueUpLog(_T("---------回调结束--------- /n"));
	return 0;
}


void CDatabase::QueryDataBase()
{
	m_nSqlite3Result = sqlite3_exec(m_pDB, SQL_QUERY_USER_MGMT_DATA, LoadDataBaseInfoCallback, NULL, &m_chErrMsg);
	if (m_nSqlite3Result != SQLITE_OK)
	{
		sqlite3_close(m_pDB);
		return;
	}
}



QUERY_RESULT CDatabase::GetTableFromDataBase(int m_Row)
{
	int index;
	QUERY_RESULT m_QueryResult;
	m_QueryResult.username = 0;
	m_QueryResult.key = 0;
	m_QueryResult.level = 0;
	m_nSqlite3Result = sqlite3_get_table(m_pDB, SQL_QUERY_USER_MGMT_DATA, &m_dbResult, &m_nRow, &m_nColumn, &m_chErrMsg);
	if (m_nSqlite3Result == SQLITE_OK)
	{
		index = m_nColumn; //前面说过 dbResult 前面第一行数据是字段名称，从 nColumn 索引开始才是真正的数据
		int order = 0;
		order = (m_Row + 1)*index;
		
		for (int i = 0; i < 1; i++)
		{
				m_QueryResult.username = m_dbResult[order];
				m_QueryResult.key= m_dbResult[order + 1];
				m_QueryResult.level=m_dbResult[order + 2];
		}
	}
	//到这里，不论数据库查询是否成功，都释放 char** 查询结果，使用 sqlite 提供的功能来释放
	sqlite3_free_table(m_dbResult);
	return m_QueryResult;
}
int CDatabase::GetTableFromDataBase()
{
	int Row=0;
	m_nSqlite3Result = sqlite3_get_table(m_pDB, SQL_QUERY_USER_MGMT_DATA, &m_dbResult, &m_nRow, &m_nColumn, &m_chErrMsg);
	if (m_nSqlite3Result == SQLITE_OK)
	{
		Row = m_nRow; //前面说过 dbResult 前面第一行数据是字段名称，从 nColumn 索引开始才是真正的数据
		sqlite3_free_table(m_dbResult);
	}
	//到这里，不论数据库查询是否成功，都释放 char** 查询结果，使用 sqlite 提供的功能来释放
	return Row;
}

void CDatabase::DeleteRowFromTable(char* deletename)
{
	sprintf_s(sql, SQL_DELETE_USER_MGMT_ROW, deletename);

	m_nSqlite3Result = sqlite3_exec(m_pDB, sql, 0, 0, &m_chErrMsg);
	if (m_nSqlite3Result != SQLITE_OK)
	{
		sqlite3_close(m_pDB);
		return;
	}
}

BOOL CDatabase::QueryRowFromTable(char* inputname)
{
	sprintf_s(sql, SQL_QUERY_USER_MGMT_ROW, inputname);
	int queryresult = 0;
	m_nSqlite3Result= sqlite3_get_table(m_pDB, sql, &m_dbResult, &m_nRow, &m_nColumn, &m_chErrMsg);
	if (m_nSqlite3Result != SQLITE_OK)
	{
		sqlite3_close(m_pDB);
		return false;
	}
	queryresult = m_nRow;
	if (queryresult !=0)
	{
		return false;
	}
	return true;
}


