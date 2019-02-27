#pragma once
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <wininet.h>
#include "Typedef.h"
#include "json/json.h"

//ÿ�ζ�ȡ���ֽ���
#define HTTP_READ_BUFFER_SIZE        16*1024
#define CONNECT_PAGE_NUM			 6
#define HTTP_HEADER_SIZE			 200

enum HttpInterfaceError
{
	Hir_Success = 0,        //�ɹ�
	Hir_InitErr,            //��ʼ��ʧ��
	Hir_ConnectErr,         //����HTTP������ʧ��
	Hir_SendErr,            //��������ʧ��
	Hir_QueryErr,           //��ѯHTTP����ͷʧ��
	Hir_404,                //ҳ�治����
	Hir_IllegalUrl,         //��Ч��URL
	Hir_CreateFileErr,      //�����ļ�ʧ��
	Hir_DownloadErr,        //����ʧ��
	Hir_QueryIPErr,         //��ȡ������Ӧ�ĵ�ַʧ��
	Hir_SocketErr,          //�׽��ִ���
	Hir_UserCancel,         //�û�ȡ������
	Hir_BufferErr,          //�ļ�̫�󣬻���������
	Hir_HeaderErr,          //HTTP����ͷ����
	Hir_ParamErr,           //�������󣬿�ָ�룬���ַ�
	Hir_ParserErr,			//���ݽ����쳣
	Hir_UnknowErr,
};
enum HttpRequest
{
	Hr_Get,
	Hr_Post
};

class CHttpHelper
{
public:
	CHttpHelper(void);
	~CHttpHelper(void);

public:
	// ����AOI������
	bool CreatAOIServer();

	// ����VIQ������
	bool ConnectVIQServer();

	// ��ȡ��Ʒ��Ϣ
	bool GetProductInfomation(BASEINFO& Info,string& errcode);

	// ��ȡ�ʼ����
	bool GetDetectRules(BASEINFO& Info, string& errcode);

	// ��ȡ�����
	bool GetDetectResult(BASEINFO& Info, string& errcode);

	// ���ͼ������
	bool SendMetaData(BASEINFO Info, string& errcode);

	// ������������
	bool ParseRecieveInfo(string strPostData, BASEINFO& info);

	// ���ͼ���� D04 ������
	bool PostDetectResult(BASEINFO Info, string& errcode);

	// �����ش��ź� D06 ������
	bool PostReSendInfo(BASEINFO Info, string& errcode);

	// �ȴ�������߳�
	unsigned int _stdcall WaitProcResultThread();

	SOCKET				 m_srvSocket;

private:
	// ����URL��������
	int SendMsg(URLPAGENAME pageName, std::string& strPostData,bool waitRsp);

	// ����Json����
	bool ParseJsonInfo(HTTPINFOTYPE type,string& strPostData, BASEINFO& info,string& errcode);

	// ����·������
	bool ParsePageName(string strPostData, string& strPageName);

	// ��������
	bool ParseBodyInfo(string strPostData, string& strBodyInfo);

	// �رվ�� 
	void Release();

	// �ͷž�� 
	void ReleaseHandle(HINTERNET& hInternet);

	// ����URL��ַ 
	void ParseURLWeb(std::string strUrl, std::string& strHostName, std::string& strPageName, WORD& sPort);

	// UTF-8תΪGBK2312 
	char* UtfToGbk(const char* utf8);

private:
	string				 m_AOIHostName;
	INTERNET_PORT		 m_AOIPort;
	HINTERNET            m_hSession;
	HINTERNET            m_hConnect;
	HINTERNET            m_hRequest[CONNECT_PAGE_NUM];
	string				 m_VIQHostName;
	INTERNET_PORT		 m_VIQPort;
	string		         m_pageName[CONNECT_PAGE_NUM];
	int					 m_Action[CONNECT_PAGE_NUM];
	HttpInterfaceError   m_error;
};


