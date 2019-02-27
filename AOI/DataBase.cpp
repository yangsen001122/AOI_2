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
	m_nSqlite3Result = sqlite3_open(m_db, &m_pDB);// �����ݿ⣬������ʱ���� 
	if (m_nSqlite3Result)
	{
		sqlite3_close(m_pDB);      
		gSystemLogger.SaveAndShowLog(L"Open/Create DataBase Fail��", LV_ERROR);
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
		gSystemLogger.SaveAndShowLog(L"����ʧ�ܣ����ݿ��������ɴ���", LV_ERROR);
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
		gSystemLogger.SaveAndShowLog(L"DataBase Save Data Fail��", LV_ERROR);
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
		//gSystemLogger.SaveAndShowLog(L"DataBase Save Data Fail��", LV_ERROR);
		AfxMessageBox(_T("�û����Ѵ��ڣ�"));
		return;
	}
}






int CDatabase::LoadDataBaseInfoCallback(void * para, int n_column, char ** column_value, char ** column_name)
{
	//para������ sqlite3_exec �ﴫ��� void * ����
	//ͨ��para����������Դ���һЩ�����ָ�루������ָ�롢�ṹָ�룩��
	//Ȼ����������ǿ��ת���ɶ�Ӧ�����ͣ���������void*���ͣ�����ǿ��ת����������Ͳſ��ã���Ȼ�������Щ����
	//n_column����һ����¼�ж��ٸ��ֶ� (��������¼�ж�����)
	//char ** column_value �Ǹ��ؼ�ֵ������������ݶ������������ʵ�����Ǹ�1ά���飨��Ҫ��Ϊ��2ά���飩��
	//ÿһ��Ԫ�ض���һ�� char * ֵ����һ���ֶ����ݣ����ַ�������ʾ����/0��β��
	//char ** column_name �� column_value�Ƕ�Ӧ�ģ���ʾ����ֶε��ֶ�����
    //����Ҳ�ʹ�� para �������������Ĵ���.
	
	CString strErrCode;
	strErrCode.Format(_T("��¼���� %d ���ֶ�/n!"), n_column);
	gSystemLogger.QueueUpLog(strErrCode.GetString());
	for (int i = 0; i < n_column; i++)
	{
		strErrCode.Format(_T("�ֶ���:%s,�ֶ�ֵ:%s /n!"),column_name[i], column_value[i]);
		gSystemLogger.QueueUpLog(strErrCode.GetString());
	}
	gSystemLogger.QueueUpLog(_T("---------�ص�����--------- /n"));
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
		index = m_nColumn; //ǰ��˵�� dbResult ǰ���һ���������ֶ����ƣ��� nColumn ������ʼ��������������
		int order = 0;
		order = (m_Row + 1)*index;
		
		for (int i = 0; i < 1; i++)
		{
				m_QueryResult.username = m_dbResult[order];
				m_QueryResult.key= m_dbResult[order + 1];
				m_QueryResult.level=m_dbResult[order + 2];
		}
	}
	//������������ݿ��ѯ�Ƿ�ɹ������ͷ� char** ��ѯ�����ʹ�� sqlite �ṩ�Ĺ������ͷ�
	sqlite3_free_table(m_dbResult);
	return m_QueryResult;
}
int CDatabase::GetTableFromDataBase()
{
	int Row=0;
	m_nSqlite3Result = sqlite3_get_table(m_pDB, SQL_QUERY_USER_MGMT_DATA, &m_dbResult, &m_nRow, &m_nColumn, &m_chErrMsg);
	if (m_nSqlite3Result == SQLITE_OK)
	{
		Row = m_nRow; //ǰ��˵�� dbResult ǰ���һ���������ֶ����ƣ��� nColumn ������ʼ��������������
		sqlite3_free_table(m_dbResult);
	}
	//������������ݿ��ѯ�Ƿ�ɹ������ͷ� char** ��ѯ�����ʹ�� sqlite �ṩ�Ĺ������ͷ�
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


