#pragma once
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <wininet.h>
#include "Typedef.h"
#include "json/json.h"

//每次读取的字节数
#define HTTP_READ_BUFFER_SIZE        16*1024
#define CONNECT_PAGE_NUM			 6
#define HTTP_HEADER_SIZE			 200

enum HttpInterfaceError
{
	Hir_Success = 0,        //成功
	Hir_InitErr,            //初始化失败
	Hir_ConnectErr,         //连接HTTP服务器失败
	Hir_SendErr,            //发送请求失败
	Hir_QueryErr,           //查询HTTP请求头失败
	Hir_404,                //页面不存在
	Hir_IllegalUrl,         //无效的URL
	Hir_CreateFileErr,      //创建文件失败
	Hir_DownloadErr,        //下载失败
	Hir_QueryIPErr,         //获取域名对应的地址失败
	Hir_SocketErr,          //套接字错误
	Hir_UserCancel,         //用户取消下载
	Hir_BufferErr,          //文件太大，缓冲区不足
	Hir_HeaderErr,          //HTTP请求头错误
	Hir_ParamErr,           //参数错误，空指针，空字符
	Hir_ParserErr,			//数据解析异常
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
	// 创建AOI服务器
	bool CreatAOIServer();

	// 连接VIQ服务器
	bool ConnectVIQServer();

	// 获取产品信息
	bool GetProductInfomation(BASEINFO& Info,string& errcode);

	// 获取质检规则
	bool GetDetectRules(BASEINFO& Info, string& errcode);

	// 获取检测结果
	bool GetDetectResult(BASEINFO& Info, string& errcode);

	// 发送检测请求
	bool SendMetaData(BASEINFO Info, string& errcode);

	// 解析接收数据
	bool ParseRecieveInfo(string strPostData, BASEINFO& info);

	// 发送检测结果 D04 测试用
	bool PostDetectResult(BASEINFO Info, string& errcode);

	// 发送重传信号 D06 测试用
	bool PostReSendInfo(BASEINFO Info, string& errcode);

	// 等待检测结果线程
	unsigned int _stdcall WaitProcResultThread();

	SOCKET				 m_srvSocket;

private:
	// 发送URL请求数据
	int SendMsg(URLPAGENAME pageName, std::string& strPostData,bool waitRsp);

	// 解析Json数据
	bool ParseJsonInfo(HTTPINFOTYPE type,string& strPostData, BASEINFO& info,string& errcode);

	// 解析路径名称
	bool ParsePageName(string strPostData, string& strPageName);

	// 解析数据
	bool ParseBodyInfo(string strPostData, string& strBodyInfo);

	// 关闭句柄 
	void Release();

	// 释放句柄 
	void ReleaseHandle(HINTERNET& hInternet);

	// 解析URL地址 
	void ParseURLWeb(std::string strUrl, std::string& strHostName, std::string& strPageName, WORD& sPort);

	// UTF-8转为GBK2312 
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


