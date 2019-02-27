#include "stdafx.h"
#pragma   warning(disable:4503) 
#include "HttpHelper.h"
#include <fstream>
#include <tchar.h>
#include <process.h>

#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "ws2_32.lib")

CHttpHelper::CHttpHelper(void) :m_hSession(NULL), m_hConnect(NULL)
{
	m_srvSocket = 0;
	m_AOIHostName = "127.0.0.1";
	m_AOIPort = 8080;
	memset(&m_hRequest, NULL, sizeof(m_hRequest));

	//m_VIQHostName = "127.0.0.1";
	//m_VIQPort = 8080;
	//string strURL = "/Mock/api/D0";

	m_VIQHostName = "193.168.70.1";
	m_VIQPort = 8080;
	string strURL = "/TianmaService/api/D0";

	char str[4];
	for (int i = 0; i < CONNECT_PAGE_NUM; i++)
	{
		_itoa_s(i+1, str, 10);
		m_pageName[i] = strURL + str;
		m_Action[i] = Hr_Post;
	}
	m_Action[0] = Hr_Get;
}

CHttpHelper::~CHttpHelper(void)
{
	Release();
}

// 创建AOI服务器
bool CHttpHelper::CreatAOIServer()
{
	WSADATA wsa_data;
	sockaddr_in serv_addr = {0};
	int	result = 0;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
	m_srvSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if (m_srvSocket == INVALID_SOCKET)
	{
		return false;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(m_AOIPort);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	result = ::bind(m_srvSocket, (sockaddr *) &serv_addr, sizeof(serv_addr));
	if (result == SOCKET_ERROR) 
	{
		closesocket(m_srvSocket);
		return false;
	}
	listen(m_srvSocket, 0);
	return true;
}

// 连接VIQ服务器
bool CHttpHelper::ConnectVIQServer()
{
	try
	{
	    Release();
		m_hSession = InternetOpen(_T("Http-connect"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
		if (NULL == m_hSession)
		{
			return false;
		}
		m_hConnect = InternetConnectA(m_hSession, m_VIQHostName.c_str(), m_VIQPort, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);

		if (NULL == m_hConnect)
		{
			return false;
		}
		for (int i = 0; i < CONNECT_PAGE_NUM; i++)
		{
			std::string strRequestType;
			if (Hr_Get == m_Action[i])
			{
				strRequestType = "GET";
			}
			else
			{
				strRequestType = "POST";
			}

			m_hRequest[i] = HttpOpenRequestA(m_hConnect, strRequestType.c_str(), m_pageName[i].c_str(), "HTTP/1.1", NULL, NULL, INTERNET_FLAG_RELOAD, NULL);
			if (NULL == m_hRequest)
			{
				return false;
			}
		}
	}
	catch (HttpInterfaceError)
	{
		return false;
	}
	return true;
}

// 获取产品信息
bool CHttpHelper::GetProductInfomation(BASEINFO& Info, string& errcode)
{
	string strPostData = "";
	int ret = SendMsg(PAGE_D01, strPostData, true);
	if (Hir_Success != ret)
	{
		return false;
	}
	return ParseJsonInfo(AOI_CLIENT_REC_D01, strPostData, Info, errcode);
}

// 获取检测规则
bool CHttpHelper::GetDetectRules(BASEINFO& Info, string& errcode)
{
	string strPostData = "";
	if (ParseJsonInfo(AOI_CLIENT_SEND_D02, strPostData, Info, errcode))
	{
		int ret = SendMsg(PAGE_D02, strPostData, true);
		if (Hir_Success != ret)
		{
			return false;
		}
		Info.clear();
		if (ParseJsonInfo(AOI_CLIENT_REC_D02, strPostData, Info, errcode))
		{
			return true;
		}
	}
	return false;
}

// 获取检测结果
bool CHttpHelper::GetDetectResult(BASEINFO& Info, string& errcode)
{
	string strPostData = "";
	if (ParseJsonInfo(AOI_CLIENT_SEND_D03, strPostData, Info, errcode))
	{
		int ret = SendMsg(PAGE_D03, strPostData, true);
		if (Hir_Success != ret)
		{
			return false;
		}
		Info.clear();
		if (ParseJsonInfo(AOI_CLIENT_REC_D03, strPostData, Info, errcode))
		{
			return true;
		}
	}
	return false;
}

// 发送检测请求
bool CHttpHelper::SendMetaData(BASEINFO Info, string& errcode)
{
	string strPostData = "";
	if (ParseJsonInfo(AOI_CLIENT_SEND_D05, strPostData, Info, errcode))
	{
		int ret = SendMsg(PAGE_D05, strPostData, true);
		if (Hir_Success != ret)
		{
			return false;
		}
		Info.clear();
		if (ParseJsonInfo(AOI_CLIENT_REC_D05, strPostData, Info, errcode))
		{
			return true;
		}
	}
	return false;
}

// 解析接收数据
bool CHttpHelper::ParseRecieveInfo(string strPostData, BASEINFO & info)
{
	string strRet = "";
	string parseData = "";
	if (ParseBodyInfo(strPostData, parseData))
	{
		parseData += '\0';
		strRet.append(UtfToGbk(parseData.c_str()));
		if (ParsePageName(strPostData, parseData))
		{
			return ParseJsonInfo(("/api/D06" == parseData) ? AOI_SERVE_REC_D06 : AOI_SERVE_REC_D04, strRet, info, strRet);
		}
	}
	return false;
}

bool CHttpHelper::PostDetectResult(BASEINFO Info, string& errcode)
{
	string strPostData = "";
	if (ParseJsonInfo(TEST_CLIENT_SEND_D04, strPostData, Info, errcode))
	{
		int ret = SendMsg(PAGE_D04, strPostData, false);
		if (Hir_Success != ret)
		{
			return false;
		}
	}
	return true;
}

bool CHttpHelper::PostReSendInfo(BASEINFO Info, string& errcode)
{
	string strPostData = "";
	if (ParseJsonInfo(TEST_CLIENT_SEND_D06, strPostData, Info, errcode))
	{
		int ret = SendMsg(PAGE_D06, strPostData, false);
		if (Hir_Success != ret)
		{
			return false;
		}
	}
	return true;
}

// 等待检测结果线程
unsigned int CHttpHelper::WaitProcResultThread()
{
	SOCKET	 acpt_soc; 
	sockaddr_in clientAddr;
	int clientAddrSize = sizeof(clientAddr);
	string rspStr = "";
	while (true)
	{
		acpt_soc = accept(m_srvSocket, (sockaddr*)&clientAddr, &clientAddrSize);
		if (acpt_soc == INVALID_SOCKET)
		{
			closesocket(acpt_soc);
			continue;
		}
		string strInfoRec = "";
		int result = 0;
		for (int i = 0; i < 2; i++)
		{
			char recv_buf[HTTP_READ_BUFFER_SIZE + 1] = { 0 };
			result = recv(acpt_soc, recv_buf, HTTP_READ_BUFFER_SIZE, 0);
			if (result == SOCKET_ERROR)
			{
				closesocket(acpt_soc);
				break;
			}
			string curInfoRec(recv_buf);
			strInfoRec += curInfoRec;
			i = result < HTTP_HEADER_SIZE ? 0 : 1;
		}
		if (result == SOCKET_ERROR) 
		{
			// 接收异常报错
			continue;
		}
		BASEINFO curInfo;
		ParseRecieveInfo(strInfoRec, curInfo);
		string headStr = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: ";
		string headEndstr = "\r\n\r\n";
		string bodyStr = "{ \"status\":\"SUCCESS\"}";
		rspStr = headStr + to_string(bodyStr.length()) + headEndstr + bodyStr;
		send(acpt_soc, rspStr.c_str(), (int)rspStr.length(), 0);
	}
}

// 发送URL请求数据
int CHttpHelper::SendMsg(URLPAGENAME pageName, std::string& strPostData, bool waitRsp)
{
	if (ConnectVIQServer())
	{
		string strRet = "";
		BOOL bRet = FALSE;
		if (Hr_Get == m_Action[pageName])
		{
			bRet = HttpSendRequestA(m_hRequest[pageName], NULL, 0, NULL, 0);
		}
		else
		{
			string strHeader = "Content-type:application/json;charset=UTF-8\nAccept:application/json";
			DWORD dwHeaderSize = (DWORD)strlen(strHeader.c_str());
			DWORD dwSize = (strPostData.empty()) ? 0 : (DWORD)strlen(strPostData.c_str());
			bRet = HttpSendRequestA(m_hRequest[pageName], strHeader.c_str(), dwHeaderSize, (LPVOID)strPostData.c_str(), dwSize);
		}
		if (!bRet)
		{
			return Hir_SendErr;
		}
		if (waitRsp)
		{
			char szBuffer[HTTP_READ_BUFFER_SIZE + 1] = { 0 };
			char* GbkBuffer;
			DWORD dwReadSize = HTTP_READ_BUFFER_SIZE;
			if (!HttpQueryInfoA(m_hRequest[pageName], HTTP_QUERY_RAW_HEADERS, szBuffer, &dwReadSize, NULL))
			{
				return Hir_QueryErr;
			}
			if (NULL != strstr(szBuffer, "404"))
			{
				return Hir_404;
			}

			while (true)
			{
				bRet = InternetReadFile(m_hRequest[pageName], szBuffer, HTTP_READ_BUFFER_SIZE, &dwReadSize);
				if (!bRet || (0 == dwReadSize))
				{
					break;
				}
				szBuffer[dwReadSize] = '\0';
				GbkBuffer = UtfToGbk(szBuffer);
				strRet.append(GbkBuffer);
				strPostData = strRet;
			}
		}
		return Hir_Success;
	}
	return Hir_SendErr;
}

// 解析Json数据 
bool CHttpHelper::ParseJsonInfo(HTTPINFOTYPE type, string& strPostData, BASEINFO& info, string& errcode)
{
	info.m_type = type;
	Json::Value value;
	switch (type)
	{
	case AOI_CLIENT_SEND_D02:
	case AOI_CLIENT_SEND_D03:
	case TEST_CLIENT_SEND_D04:
	case AOI_CLIENT_SEND_D05:
	case TEST_CLIENT_SEND_D06:
	{
		for (auto it = info.m_commInfo.begin(); it != info.m_commInfo.end(); it++)
		{
			value[it->first] = Json::Value(it->second);
		}
		if (type == TEST_CLIENT_SEND_D04)
		{
			for (auto it = info.m_defectsInfo.begin(); it != info.m_defectsInfo.end(); it++)
			{
				Json::Value arrayObj;
				int nSize = (int)it->second.size();
				for (int nPos = 0; nPos < nSize; nPos++)
				{
					Json::Value item;
					for (auto it1 = it->second[nPos].defCommInfo.begin(); it1 != it->second[nPos].defCommInfo.end(); it1++)
					{
						item[it1->first] = it1->second;
					}
					for (auto it2 = it->second[nPos].defSpeInfo.begin(); it2 != it->second[nPos].defSpeInfo.end(); it2++)
					{
						Json::Value item2;
						auto it3 = it2->second.begin();
						for (; it3 != it2->second.end(); it3++)
						{
							item2[it3->first] = it3->second;
						}
						item[it2->first] = item2;
					}
					arrayObj.append(item);
				}
				value[it->first] = arrayObj;
			}
		}
		else if (type == AOI_CLIENT_SEND_D05)
		{
			for (auto it = info.m_specifiedInfo.begin(); it != info.m_specifiedInfo.end(); it++)
			{
				Json::Value arrayObj;
				int nSize = (int)it->second.size();
				for (int nPos = 0; nPos < nSize; nPos++)
				{
					Json::Value item;
					for (auto it1 = it->second[nPos].begin(); it1 != it->second[nPos].end(); it1++)
					{
						item[it1->first] = it1->second;
					}
					arrayObj.append(item);
				}
				value[it->first] = arrayObj;
			}
		}
		strPostData = value.toStyledString();
	}
	break;
	case AOI_CLIENT_REC_D01:
	case AOI_CLIENT_REC_D02:
	{
		Json::Reader reader;
		if (!reader.parse(strPostData, value))
		{
			return false;
		}
		if (value["status"].isNull()|| value["status"].asString() == "ERROR"|| value["result"].isNull())
		{
			value = value["result"];
			errcode = value["message"].asString();
			return false;
		}
		int nSize = value["result"].size();
		vector<map<string, string>> curInfo(nSize);
		for (int nPos = 0; nPos < nSize; ++nPos)                //对数据数组进行遍历
		{
			if (type == AOI_CLIENT_REC_D01)
			{
				curInfo[nPos]["ProdID"] = value["result"][nPos]["ProdID"].asString();
				curInfo[nPos]["ProdName"] = value["result"][nPos]["ProdName"].asString();
				curInfo[nPos]["ProdMemo"] = value["result"][nPos]["ProdMemo"].asString();
			}
			else
			{
				curInfo[nPos]["RuleID"] = value["result"][nPos]["RuleID"].asString();
				curInfo[nPos]["RuleName"] = value["result"][nPos]["RuleName"].asString();
				curInfo[nPos]["RuleMemo"] = value["result"][nPos]["RuleMemo"].asString();
				string splitChar = ",";
				int nSize1 = value["result"][nPos]["Frames"].size();
				for (int nPos1 = 0; nPos1 < nSize1; nPos1++)
				{
					curInfo[nPos]["Frames"] += value["result"][nPos]["Frames"][nPos1].asString();
					if (nPos1 != nSize1 - 1)
					{
						curInfo[nPos]["Frames"] += splitChar;
					}
				}
			}
		}
		info.m_specifiedInfo["result"] = curInfo;
	}
	break;
	case AOI_CLIENT_REC_D03:
	case AOI_SERVE_REC_D04:
	case AOI_CLIENT_REC_D05:
	{
		Json::Reader reader;
		if (!reader.parse(strPostData, value))
		{
			return false;
		}
		if (type == AOI_CLIENT_REC_D03|| type == AOI_CLIENT_REC_D05)
		{
			if (value["status"].isNull())
			{
				return false;
			}
			try
			{
				if (value["status"].asString() == "ERROR")
				{
					if (value["result"].isNull())
					{
						return false;
					}
					else
					{
						value = value["result"];
						errcode = value["message"].asString();
					}
					return false;
				}
			}
			catch (...)
			{
				return false;
			}
			value = value["result"];
		}
		if (value["Status"].isNull())
		{
			return false;
		}
		info.m_commInfo["Status"] = value["Status"].asString();
		if (type == AOI_SERVE_REC_D04)
		{
			if (value["TaskID"].isNull() || value["MaterielID"].isNull()|| value["AOIID"].isNull())
				return false;
			info.m_commInfo["AOIID"] = value["AOIID"].asString();
			info.m_commInfo["TaskID"] = value["TaskID"].asString();
			info.m_commInfo["MaterielID"] = value["MaterielID"].asString();
		}
		int nSize = value["Defects"].size();
		vector<DEFECTINFO> curInfo(nSize);
		for (int nPos = 0; nPos < nSize; ++nPos)
		{
			curInfo[nPos].defCommInfo["DefectID"] = value["Defects"][nPos]["DefectID"].asString();
			curInfo[nPos].defCommInfo["Confidence"] = value["Defects"][nPos]["Confidence"].asString();
			curInfo[nPos].defCommInfo["Camera"] = value["Defects"][nPos]["Camera"].asString();
			curInfo[nPos].defCommInfo["Frame"] = value["Defects"][nPos]["Frame"].asString();
			curInfo[nPos].defCommInfo["Photo"] = value["Defects"][nPos]["Photo"].asString();
			map<string, string> posInfo;
			posInfo["X"] = value["Defects"][nPos]["PicPos"]["X"].asString();
			posInfo["Y"] = value["Defects"][nPos]["PicPos"]["Y"].asString();
			posInfo["Width"] = value["Defects"][nPos]["PicPos"]["Width"].asString();
			posInfo["Height"] = value["Defects"][nPos]["PicPos"]["Height"].asString();
			curInfo[nPos].defSpeInfo["PicPos"] = posInfo;
			/*posInfo["X"] = value["Defects"][nPos]["ScrPos"]["X"].asString();
			posInfo["Y"] = value["Defects"][nPos]["ScrPos"]["Y"].asString();
			posInfo["Width"] = value["Defects"][nPos]["ScrPos"]["Width"].asString();
			posInfo["Height"] = value["Defects"][nPos]["ScrPos"]["Height"].asString();
			curInfo[nPos].defSpeInfo["ScrPos"] = posInfo;*/
		}
		info.m_defectsInfo["Defects"] = curInfo;
	}
	break;
	case AOI_SERVE_REC_D06:
	{
		Json::Reader reader;
		if (!reader.parse(strPostData, value))
		{
			return false;
		}
		if (value["TaskID"].isNull()|| value["MaterielID"].isNull()||
			value["AOIID"].isNull() || value["Frame"].isNull())
		{
			return false;
		}
		info.m_commInfo["TaskID"] = value["TaskID"].asString();
		info.m_commInfo["MaterielID"] = value["MaterielID"].asString();
		info.m_commInfo["AOIID"] = value["AOIID"].asString();
		info.m_commInfo["Frame"] = value["Frame"].asString();
	}
	break;
	default:
		break;
	}
	return true;
}

bool CHttpHelper::ParsePageName(string strPostData,string& strPageName)
{
	int length = 0;
	char* begin, *end;
	char* p = const_cast<char*>(strPostData.c_str());
	begin = strchr(p, ' ');
	if (begin != NULL)
	{
		begin++;
		end = strchr(begin, ' ');
		if (end != NULL)
		{
			*end = 0;
			strPageName = begin;
			return true;
		}
	}
	return false;
}

bool CHttpHelper::ParseBodyInfo(string strPostData,string& strBodyInfo)
{
	int length = 0;
	char* begin;
	char* p = const_cast<char*>(strPostData.c_str());
	begin = strchr(p, '{');
	strBodyInfo = begin;
	return begin != NULL;
}

// 解析URL地址 
void CHttpHelper::ParseURLWeb(std::string strUrl, std::string& strHostName, std::string& strPageName, WORD& sPort)
{
	sPort = 80;
	string strTemp(strUrl);
	std::size_t nPos = strTemp.find("http://");
	if (nPos != std::string::npos)
	{
		strTemp = strTemp.substr(nPos + 7, strTemp.size() - nPos - 7);
	}

	nPos = strTemp.find('/');
	if (nPos == std::string::npos)    //没有找到
	{
		strHostName = strTemp;
	}
	else
	{
		strHostName = strTemp.substr(0, nPos);
	}

	std::size_t nPos1 = strHostName.find(':');
	if (nPos1 != std::string::npos)
	{
		std::string strPort = strTemp.substr(nPos1 + 1, strHostName.size() - nPos1 - 1);
		strHostName = strHostName.substr(0, nPos1);
		sPort = (WORD)atoi(strPort.c_str());
	}
	if (nPos == std::string::npos)
	{
		return;
	}
	strPageName = strTemp.substr(nPos, strTemp.size() - nPos);
}

// 关闭句柄 
void CHttpHelper::Release()
{
	for (int i = 0; i < 6; i++)
	{
		ReleaseHandle(m_hRequest[i]);
	}
	ReleaseHandle(m_hConnect);
	ReleaseHandle(m_hSession);

	/*closesocket(m_srvSocket);
	WSACleanup();*/
}

// 释放句柄 
void CHttpHelper::ReleaseHandle(HINTERNET& hInternet)
{
	if (hInternet)
	{
		InternetCloseHandle(hInternet);
		hInternet = NULL;
	}
}

// UTF-8转为GBK2312 
char* CHttpHelper::UtfToGbk(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[2 * len + 1];
	memset(str, 0, 2 * len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, 2 * len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}


