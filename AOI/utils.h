#pragma once
#include <atlcom.h>
#include <atlbase.h>
#include <string>
#include <algorithm>
#include <map>
#include <tuple>
#include <vector>
#include <iomanip>
#include <regex>
#include <boost\algorithm\string.hpp>
#include <boost\lexical_cast.hpp>

#include "HalconCPP.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace boost;
using namespace cv;
using namespace HalconCpp;

// Language ID
const int LANG_CHS = 0x01;
const int LANG_ENU = 0x02;

enum COMPARE_TYPE
{
	COMP_INVALID = -1,
	GREAT,
	GREAT_EQUAL,
	EQUAL,
	NO_EQUAL,
	LESS,
	LESS_EQUAL
};

struct COMPARATOR
{
	COMPARATOR() :m_strComp(L">"), m_compType(GREAT)
	{
	}
	COMPARATOR(wstring strComp, COMPARE_TYPE type) :m_strComp(strComp), m_compType(type)
	{
	}

	COMPARATOR(const COMPARATOR& other)
	{
		m_strComp = other.m_strComp;
		m_compType = other.m_compType;
	}
	COMPARATOR(COMPARATOR&& other)
	{
		m_strComp = other.m_strComp;
		m_compType = other.m_compType;
	}
	COMPARATOR& operator=(const COMPARATOR& other)
	{
		if (&other == this)
			return *this;
		m_strComp = other.m_strComp;
		m_compType = other.m_compType;
		return *this;
	}
	COMPARATOR& operator=(COMPARATOR&& other)
	{
		if (&other == this)
			return *this;
		m_strComp = other.m_strComp;
		m_compType = other.m_compType;
		return *this;
	}
	wstring      m_strComp;
	COMPARE_TYPE m_compType;
};
namespace localUtils
{
	class utils
	{
	public:

		static wstring GetDayOfWeekStr(int nLangID, WORD dayOfWeek)
		{
			wstring ret;
			if (nLangID != LANG_ENU)
			{
				if (dayOfWeek == 0) ret = L"星期日";
				if (dayOfWeek == 1) ret = L"星期一";
				if (dayOfWeek == 2) ret = L"星期二";
				if (dayOfWeek == 3) ret = L"星期三";
				if (dayOfWeek == 4) ret = L"星期四";
				if (dayOfWeek == 5) ret = L"星期五";
				if (dayOfWeek == 6) ret = L"星期六";
			}
			else
			{
				if (dayOfWeek == 0) ret = L"Sun.";
				if (dayOfWeek == 1) ret = L"Mon.";
				if (dayOfWeek == 2) ret = L"Tues.";
				if (dayOfWeek == 3) ret = L"Wed.";
				if (dayOfWeek == 4) ret = L"Thur.";
				if (dayOfWeek == 5) ret = L"Fri.";
				if (dayOfWeek == 6) ret = L"Sat.";
			}
			return ret;
		}

		static void TrimString(std::vector<wstring>& vStr)
		{
			for (size_t t = 0; t < vStr.size(); ++t)
			{
				boost::trim(vStr[t]);
			}
		}

		static std::vector<wstring> SplitString(wstring strTarget, wstring strSeperator, algorithm::token_compress_mode_type type = token_compress_on, bool m_bTrim = true)
		{
			std::vector<wstring> vSplitRslt;
			boost::split(vSplitRslt, strTarget, boost::is_any_of(strSeperator), type);
			if (m_bTrim)
			{
				TrimString(vSplitRslt);
			}

			return vSplitRslt;
		}

		static std::vector<string> SplitString(string strTarget, string strSeperator, bool m_bTrim = true)
		{
			std::vector<string> vSplitRslt;
			boost::split(vSplitRslt, strTarget, boost::is_any_of(strSeperator), token_compress_on);
			return vSplitRslt;
		}

		static void TrimString(std::vector<string>& vStr)
		{
			for (size_t t = 0; t < vStr.size(); ++t)
			{
				boost::trim(vStr[t]);
			}
		}

		static HRESULT GetIDispatchMethods(IDispatch * pDisp, std::vector<std::tuple<long, wstring, wstring>>& vRetRepository)
		{
			// std::map<long, std::wstring>& methodsMap, std::map<long, std::wstring>& dictFuncIdHelpStr
			HRESULT hr = S_OK;
			HRESULT ret = S_OK;

			CComPtr<IDispatch> spDisp(pDisp);
			if (!spDisp)
				return E_INVALIDARG;

			CComPtr<ITypeInfo> spTypeInfo;
			hr = spDisp->GetTypeInfo(0, 0, &spTypeInfo);

			if (SUCCEEDED(hr) && spTypeInfo)
			{
				TYPEATTR *pTatt = nullptr;
				hr = spTypeInfo->GetTypeAttr(&pTatt);
				if (SUCCEEDED(hr) && pTatt)
				{
					FUNCDESC * fd = nullptr;
					for (int i = 0; i < pTatt->cFuncs; ++i)
					{
						hr = spTypeInfo->GetFuncDesc(i, &fd);
						if (SUCCEEDED(hr) && fd)
						{
							CComBSTR funcName;
							CComBSTR docStr;
							spTypeInfo->GetDocumentation(fd->memid, &funcName, &docStr, NULL, NULL);
							if (funcName.Length() > 0)
							{
								CComBSTR helpFuncName = funcName;
								helpFuncName.Append(L"Help");
								MEMBERID retID;
								ret = spTypeInfo->GetIDsOfNames(&helpFuncName, 1, &retID);
								if (SUCCEEDED(ret))
								{
									DISPPARAMS dispParams = { NULL, NULL, 0, 0 };
									CComVariant strHelp;
									hr = pDisp->Invoke(retID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispParams, &strHelp, NULL, NULL);
									if (SUCCEEDED(hr))
									{
										wstring wstrFuncName = funcName.m_str;
										wstring wstrHelp = COLE2T(strHelp.bstrVal);
										std::tuple<long, wstring, wstring> tmp = std::make_tuple((long)fd->memid, wstrFuncName, wstrHelp);
										vRetRepository.push_back(tmp);
									}
								}
							}

							spTypeInfo->ReleaseFuncDesc(fd);
						}
					}
					spTypeInfo->ReleaseTypeAttr(pTatt);
				}
			}
			return hr;
		}

		static HRESULT GetInterfaceHelpString(IDispatch * pDisp, wstring funcName, wstring& retStr)
		{
			if (pDisp == nullptr)
				return S_FALSE;
			wstring helpFuncName = funcName + L"Help";
			DISPID dispID;
			CComBSTR tmp(helpFuncName.c_str());
			HRESULT hr = pDisp->GetIDsOfNames(IID_NULL, &tmp, 1, LOCALE_SYSTEM_DEFAULT, &dispID);
			if (SUCCEEDED(hr))
			{
				DISPPARAMS dispParams = { NULL, NULL, 0, 0 };
				CComVariant strHelp;
				hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispParams, &strHelp, NULL, NULL);
				if (SUCCEEDED(hr))
				{
					retStr = COLE2T(strHelp.bstrVal);
					return S_OK;
				}
			}
			return S_FALSE;
		}

		static DISPID GetInterfaceDispID(IDispatch * pDisp, wstring strFuncName)
		{
			DISPID dispID = -1;
			if (pDisp == nullptr)
				return dispID;

			CComBSTR tmp(strFuncName.c_str());
			pDisp->GetIDsOfNames(IID_NULL, &tmp, 1, LOCALE_SYSTEM_DEFAULT, &dispID);
			return dispID;
		}

		static bool IsInterfaceSupported(IDispatch * pDisp, wstring funcName)
		{
			if (pDisp == nullptr)
				return false;
			DISPID dispID;
			CComBSTR tmpFuncName(funcName.c_str());
			HRESULT hr = pDisp->GetIDsOfNames(IID_NULL, &tmpFuncName, 1, LOCALE_SYSTEM_DEFAULT, &dispID);
			if (SUCCEEDED(hr))
			{
				return true;
			}
			return false;
		}

		static float ConvertStr2Value(wstring strValue)
		{
			if (strValue == L"N/A")
				return 0.0f;
			try
			{
				return boost::lexical_cast<float>(strValue);
			}
			catch (...)
			{
				return 0.0f;
			}
			return 0.0f;
		}

		static bool IsNumericString(wstring strValue, bool bIntegerOnly = false)
		{
			if (strValue.empty())
				return false;
			std::wregex wrx;
			if (bIntegerOnly)
			{
				wrx = L"\\d+$";
			}
			else
			{
				wrx = L"^[+-]?[0-9]+(?:\.[0-9]{1,6})?$";
			}
			bool ret = std::regex_match(strValue, wrx);
			return ret;
		}

		static bool IsNumericString(CString strValue, bool bIntegerOnly = false)
		{
			int len = strValue.GetLength();
			if (len <= 0)
				return false;
			wstring value = CT2W(strValue);

			std::wregex wrx;
			if (bIntegerOnly)
			{
				wrx = L"\\d+$";
			}
			else
			{
				wrx = L"^-?[0-9]+([.][0-9]+){0,1}$";
			}
			bool ret = std::regex_match(value, wrx);
			if (!ret)
				return false;

			if (value.find(L".") != wstring::npos)
			{
				std::vector<wstring> vItem = SplitString(value, L".");
				if (!vItem.empty())
				{
					wstring wstrInt = vItem[0];
					if (wstrInt.size() >= 2 && wstrInt[0] == '0')
					{
						return false;
					}
				}
			}
			else
			{
				if (value.size() >= 2 && value[0] == '0')
					return false;
			}
			return ret;
		}

		static float to_float(wstring strValue)
		{
			if (!IsNumericString(strValue))
			{
				return -9999999.9f;
			}
			try
			{
				return boost::lexical_cast<float>(strValue);
			}
			catch (...)
			{
				return -9999999.9f;
			}
			return-9999999.9f;
		}

		static long to_long(wstring wstrValue)
		{
			if (!IsNumericString(wstrValue))
			{
				return -9999999;
			}
			try
			{
				return boost::lexical_cast<long>(wstrValue);
			}
			catch (...)
			{
				return -9999999;
			}
			return-9999999;
		}

		static float to_float(CString strValue)
		{
			wstring wstrValue = CT2W(strValue);
			return to_float(wstrValue);
		}

		static int ConvertStr2Value(CString strValue)
		{
			wstring wstrValue = CT2W(strValue);
			int nRet = wcstol(wstrValue.c_str(), nullptr, 10);
			return nRet;
		}

		template<class T>
		static string to_string(const T& t)
		{
			std::ostringstream oss;
			oss << t;
			return oss.str();
		}

		template<class T>
		static wstring to_wstring(const T& t)
		{
			std::wostringstream oss;
			oss << t;
			return oss.str();
		}

		static wstring Utf8ToUnicode(const string& str)
		{
			wstring tmpStr(L"");
			int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), (int)strlen(str.c_str()), NULL, 0);
			if (wcsLen <= 0)
				return tmpStr;
			wstring retWstr(wcsLen, '\0');
			::MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), (int)strlen(str.c_str()), &retWstr[0], wcsLen);

			return retWstr;
		}

		static string Unicode2Utf8(const wstring& unicodeStr)
		{
			std::string retStr("");
			int strLen = (int)::WideCharToMultiByte(CP_UTF8, NULL, unicodeStr.c_str(), (int)wcslen(unicodeStr.c_str()), NULL, 0, NULL, NULL);
			if (strLen == 0)
				return retStr;
			std::vector<char> strBuffer(strLen, '\0');
			::WideCharToMultiByte(CP_UTF8, NULL, unicodeStr.c_str(), (int)wcslen(unicodeStr.c_str()), &strBuffer[0], strLen, NULL, NULL);

			retStr.assign(strBuffer.begin(), strBuffer.end());
			return retStr;
		}

		static wstring AnsiToUnicode(const string& str)
		{
			wstring tmpStr(L"");
			int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, str.c_str(), (int)strlen(str.c_str()), NULL, 0);
			if (wcsLen <= 0)
				return tmpStr;
			wstring retWstr(wcsLen, '\0');
			::MultiByteToWideChar(CP_ACP, NULL, str.c_str(), (int)strlen(str.c_str()), &retWstr[0], wcsLen);
			return retWstr;
		}

		static string Unicode2Ansi(const wstring& unicodeStr)
		{
			std::string retStr("");
			int strLen = (int)::WideCharToMultiByte(CP_ACP, NULL, unicodeStr.c_str(), (int)wcslen(unicodeStr.c_str()), NULL, 0, NULL, NULL);
			if (strLen == 0)
				return retStr;
			std::vector<char> strBuffer(strLen, '\0');
			::WideCharToMultiByte(CP_ACP, NULL, unicodeStr.c_str(), (int)wcslen(unicodeStr.c_str()), &strBuffer[0], strLen, NULL, NULL);

			retStr.assign(strBuffer.begin(), strBuffer.end());
			return retStr;
		}

		static string TChar2Utf8(const CString& tStr)
		{
			wstring wstr = CT2W(tStr);
			return Unicode2Utf8(wstr);
		}

		static bool CheckResult(HRESULT hr)
		{
			if (hr == S_FALSE)
				return false;
			else if (SUCCEEDED(hr))
				return true;
			else
				return false;

			return false;
		}

		static VARENUM GetType(wstring strType)
		{
			VARENUM ret;
			ret = VT_EMPTY;
			if (strType == L"B")
				return VT_UI1;
			if (strType == L"LP")
				return VT_I4;
			if (strType == L"L")
				return VT_I4;
			if (strType == L"F" || strType == L"FP")
				return VT_R4;
			if (strType.find(L"SP") != wstring::npos)
				return VT_BSTR;

			return ret;
		}

		static std::vector<COMPARATOR> SYMBOL_TYPE;

		static COMPARE_TYPE GetCompType(wstring strComp)
		{
			if (SYMBOL_TYPE.empty())
				return COMP_INVALID;
			boost::trim(strComp);
			for (auto it = SYMBOL_TYPE.begin(); it != SYMBOL_TYPE.end(); it++)
			{
				if (it->m_strComp == strComp)
				{
					return it->m_compType;
				}
			}

			return COMP_INVALID;
		}

		static std::vector<COMPARATOR> InitDictionary()
		{
			std::vector<COMPARATOR> ret;
			ret.push_back(COMPARATOR(L">", GREAT));
			ret.push_back(COMPARATOR(L">=", GREAT_EQUAL));
			ret.push_back(COMPARATOR(L"<", LESS));
			ret.push_back(COMPARATOR(L"<=", LESS_EQUAL));
			ret.push_back(COMPARATOR(L"=", EQUAL));
			ret.push_back(COMPARATOR(L"!=", NO_EQUAL));
			return ret;
		}

		static std::vector<wstring> GetAllSerialPort()
		{
			std::vector<wstring> vRet;
			HKEY hKey;
			int nRet = ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), NULL, KEY_READ, &hKey);
			if (nRet == ERROR_SUCCESS)
			{
				int i = 0;
				while (1)
				{
					TCHAR portName[256], commName[256];
					DWORD dwLong = sizeof(portName);
					DWORD dwSize = dwLong;
					int nRslt = ::RegEnumValueW(hKey, i, portName, &dwLong, NULL, NULL, (PUCHAR)commName, &dwSize);
					if (nRslt == ERROR_NO_MORE_ITEMS)
						break;
					CString sPort = commName;
					vRet.push_back(sPort.GetString());
					i++;
				}
				::RegCloseKey(hKey);
			}
			return vRet;
		}

		static string ClipString(char *str, unsigned int n)
		{
			std::vector<char> vChar(n, '\0');
			unsigned int len = (unsigned int)strlen(str);
			if (n >= len)
			{
				std::string strRet = str;
				return strRet;
			}
			int k = 0;
			for (unsigned int i = len - n; i < len; i++)
			{
				vChar[k++] = str[i];
			}
			std::string strRet;
			strRet.assign(vChar.begin(), vChar.end());

			return strRet;
		}

		static string TranslateCode(unsigned int st)
		{
			string rt;
			char ch[16];
			sprintf_s(ch, "%02x", st);
			rt += ClipString(ch, 2);
			rt += " ";
			sprintf_s(ch, "%02x", ~st);
			rt += ClipString(ch, 2);
			rt += " ";
			return rt;
		}

		static char HexChar(char c)
		{
			if ((c >= '0') && (c <= '9'))
				return c - 0x30;
			else if ((c >= 'A') && (c <= 'F'))
				return c - 'A' + 10;
			else if ((c >= 'a') && (c <= 'f'))
				return c - 'a' + 10;
			else
				return 0x10;
		}

		static string Str2Hex(string str)
		{
			std::vector<char> vData;
			int len = (int)str.length();

			for (int i = 0; i < len;)
			{
				char h = str[i];
				if (h == ' ')
				{
					i++;
					continue;
				}
				i++;
				if (i >= len)
					break;
				char l = str[i];
				int t = HexChar(h);
				int t1 = HexChar(l);
				if ((t == 16) || (t1 == 16))
					break;
				else
					t = t * 16 + t1;
				i++;
				vData.push_back((char)t);
			}
			std::string strRet;
			strRet.assign(vData.begin(), vData.end());
			return strRet;
		}

		static std::vector<unsigned char> string_to_hex(const string& str)
		{
			std::vector<unsigned char> vRet;
			for (int i = 0; i < str.size(); i++)
			{
				unsigned char ch = (unsigned char)str[i];
				vRet.push_back(ch);
			}
			return vRet;
		}

		static std::vector<unsigned char> convert_to_byte_array(const std::vector<string>& vCmdStr)
		{
			std::vector<unsigned char> vRet;
			char* ptrOffset;
			for (size_t t = 0; t < vCmdStr.size(); t++)
			{
				unsigned char ch = (unsigned char)strtol(vCmdStr[t].c_str(), &ptrOffset, 16);
				vRet.push_back(ch);
			}
			return vRet;
		}

		/*static void PrintDebugSymbol(HObject& img)
		{
			if (!img.IsInitialized())
			{
				return;
			}
			HObject ImageOneChannel;
			AccessChannel(img, &ImageOneChannel, 1);

			SetGrayval(ImageOneChannel, 0, 0, (int)'d');
			SetGrayval(ImageOneChannel, 1, 0, (int)'e');
			SetGrayval(ImageOneChannel, 2, 0, (int)'b');
			SetGrayval(ImageOneChannel, 3, 0, (int)'u');
			SetGrayval(ImageOneChannel, 4, 0, (int)'g');
		}

		static void ParseMarkedDebugSymbol(HObject& img, bool& bDebug)
		{
			if (!img.IsInitialized())
			{
				return;
			}
			HObject ImageOneChannel;
			AccessChannel(img, &ImageOneChannel, 1);
			HTuple t_d = 0;
			HTuple t_e = 0;
			HTuple t_b = 0;
			HTuple t_u = 0;
			HTuple t_g = 0;
			GetGrayval(ImageOneChannel, 0, 0, &t_d);
			GetGrayval(ImageOneChannel, 1, 0, &t_e);
			GetGrayval(ImageOneChannel, 2, 0, &t_b);
			GetGrayval(ImageOneChannel, 3, 0, &t_u);
			GetGrayval(ImageOneChannel, 4, 0, &t_g);
			char c_d = (char)t_d[0].I();
			char c_e = (char)t_e[0].I();
			char c_b = (char)t_b[0].I();
			char c_u = (char)t_u[0].I();
			char c_g = (char)t_g[0].I();
			bDebug = false;
			if (c_d == 'd' && c_e == 'e' && c_b == 'b' &&c_u == 'u' && c_g == 'g')
			{
				bDebug = true;
			}
		}

		static void PrintCrtSymbol(HObject& himg, int nStationIndex, int PatternIndex, int m_nActionGpIndex)
		{
			if (!himg.IsInitialized())
			{
				return;
			}
			SetGrayval(himg, 0, 1, nStationIndex);
			SetGrayval(himg, 1, 1, PatternIndex);
			SetGrayval(himg, 2, 1, m_nActionGpIndex);
			SetGrayval(himg, 11, 1, 4095);
			SetGrayval(himg, 12, 1, 0);
		}

		static void ParseImgSymbol(HObject& himg, int& nStationIndex, int& nPatternIndex, int& nActionGpIndex, bool& bMarked)
		{
			if (!himg.IsInitialized())
			{
				return;
			}
			HObject ImageOneChannel;
			AccessChannel(himg, &ImageOneChannel, 1);
			HTuple tStation = 0;
			HTuple tCamera = 0;
			HTuple tPattern = 0;
			HTuple tActionGroup = 0;
			HTuple t4095 = 0;
			HTuple t0 = 0;
			GetGrayval(ImageOneChannel, 0, 1, &tStation);
			GetGrayval(ImageOneChannel, 1, 1, &tPattern);
			GetGrayval(ImageOneChannel, 2, 1, &tActionGroup);
			GetGrayval(ImageOneChannel, 11, 1, &t4095);
			GetGrayval(ImageOneChannel, 12, 1, &t0);
			int n4095 = t4095[0].I();
			int n0 = t0[0].I();
			nStationIndex = tStation[0].I();
			nPatternIndex = tPattern[0].I();
			nActionGpIndex = tActionGroup[0].I();
			if (n4095 == 4095 && n0 == 0)
			{
				bMarked = true;
			}
		}

		static string ExtractTimestampString(HObject& img)
		{
			HTuple nMillSecond = 0;
			HTuple nSecond = 0;
			HTuple nMinute = 0;
			HTuple nHour = 0;
			HTuple nDay = 0;
			HTuple nYDay = 0;
			HTuple nMonth = 0;
			HTuple nYear = 0;

			GetImageTime(img, &nMillSecond, &nSecond, &nMinute, &nHour, &nDay, &nYDay, &nMonth, &nYear);

			string retStr = "";

			std::ostringstream oss;
			oss << std::setfill('0') << std::setw(4) << (int)nYear << "-";
			oss << std::setfill('0') << std::setw(2) << (int)nMonth << "-";
			oss << std::setfill('0') << std::setw(2) << (int)nDay << "  ";
			oss << std::setfill('0') << std::setw(2) << (int)nHour << ":";
			oss << std::setfill('0') << std::setw(2) << (int)nMinute << ":";
			oss << std::setfill('0') << std::setw(2) << (int)nSecond << ":";
			oss << std::setfill('0') << std::setw(3) << nMillSecond;
			retStr = oss.str();
			return retStr;
		}

		static long long ExtractTimestamp(HObject img)
		{
			if (!img.IsInitialized())
			{
				return 0;
			}
			long long ret = 0;

			HTuple nMillSecond = 0;
			HTuple nSecond = 0;
			HTuple nMinute = 0;
			HTuple nHour = 0;
			HTuple nDay = 0;
			HTuple nYDay = 0;
			HTuple nMonth = 0;
			HTuple nYear = 0;

			GetImageTime(img, &nMillSecond, &nSecond, &nMinute, &nHour, &nDay, &nYDay, &nMonth, &nYear);

			ret = (long long)nYear * 10000000000000;
			ret += (long long)nMonth * 100000000000;
			ret += (long long)nDay * 1000000000;
			ret += (long long)nHour * 10000000;
			ret += (long long)nMinute * 100000;
			ret += (long long)nSecond * 1000;
			ret += (long long)nMillSecond;

			return ret;
		}*/

		static std::string bytes_to_hexstr(unsigned char* first, unsigned char* last)
		{
			std::ostringstream oss;
			oss << std::hex << std::setfill('0');
			while (first < last)
				oss << std::setw(2) << int(*first++);
			return oss.str();
		}

		static unsigned int CRC16RTU(unsigned char *buff, unsigned int len)//crc check
		{
			unsigned char temp;
			unsigned int i;
			unsigned char j;
			unsigned int crc;
			crc = 0xFFFF;
			for (i = 0; i < len; i++)
			{
				crc = crc^buff[i];
				for (j = 0; j < 8; j++)
				{
					temp = crc % 2;
					crc >>= 1;
					if (temp)
						crc = crc ^ 0xA001;
				}
			}
			return(crc);
		}

		static bool CheckDirExist(const wstring& path)
		{
			if (INVALID_FILE_ATTRIBUTES == (::GetFileAttributes(path.c_str())))
			{
				return false;
			}
			return true;
		}

		static long GetDesignatedDiskSpace(const string &strDiskPath)
		{
			DWORD dwTotalSpace, dwFreeSpace, dwOccupiedSpace;
			ULARGE_INTEGER uiFreeByteAvailableToCaller;
			ULARGE_INTEGER uiTotalNumberOfBytes;
			ULARGE_INTEGER uiTotalNumberOfFreeBytes;
			if (GetDiskFreeSpaceEx(Utf8ToUnicode(strDiskPath).c_str(), &uiFreeByteAvailableToCaller, &uiTotalNumberOfBytes, &uiTotalNumberOfFreeBytes))
			{
				dwTotalSpace = (DWORD)(uiTotalNumberOfBytes.QuadPart / 1024 / 1024);
				dwFreeSpace = (DWORD)(uiFreeByteAvailableToCaller.QuadPart >> 20);
				dwOccupiedSpace = dwTotalSpace - dwFreeSpace;
				return (long)dwFreeSpace;//MB，not GB
			}
			return -1;
		}

		/*static IplImage MatToIplImage(Mat& cMat)
		{
			IplImage iplImg = cMat;
			return iplImg;

		}

		static  Mat IplImageToMat(IplImage *iplImg)
		{
			Mat cMat = cvarrToMat(iplImg).clone();
			return cMat;
		}*/

		static Mat HobjToMat(HObject &hObj)
		{
			HTuple hChannels = HTuple();
			HString hType;
			Mat cMat;
			Hlong hWidth(0);
			Hlong hHeight(0);
			HTuple hImgType;
			GetImageType(hObj, &hImgType);
			string sType = hImgType.S();
			//if (sType != "uint2")
			//{
			//	ConvertImageType(hObj, &hObj, "byte");
			//}
			CountChannels(hObj, &hChannels);
			if (hChannels[0].I() == 1)
			{
				HImage hImg(hObj);  
				void * ptr1 = hImg.GetImagePointer1(&hType, &hWidth, &hHeight);
				int nWidth = boost::lexical_cast<int>(hWidth);
				int nHeight = boost::lexical_cast<int>(hHeight);
				//if (sType == "uint2")
				//{
				//	cMat.create(nHeight, nWidth, CV_16UC1);
				//	ushort * ptrData = (ushort *)(ptr1);
				//	memcpy(cMat.data, ptrData, nWidth*nHeight * 2);
				//}
				//else if (sType == "byte")
				{
					cMat.create(nHeight, nWidth, CV_8UC1);
					unsigned char *ptrData = static_cast<unsigned char*>(ptr1);
					memcpy(cMat.data, ptrData, nWidth*nHeight);
				}

			}
			else if (hChannels[0].I() == 3)
			{
				void *ptrR;
				void *ptrG;
				void *ptrB;
				HImage hImg(hObj);
				hImg.GetImagePointer3(&ptrR, &ptrG, &ptrB, &hType, &hWidth, &hHeight);
				int nWidth = boost::lexical_cast<int>(hWidth);
				int nHeight = boost::lexical_cast<int>(hHeight);
				vector<Mat> dictMatChannel(3);
				cMat.create(nHeight, nWidth, CV_8UC3);
				dictMatChannel[0].create(nHeight, nWidth, CV_8UC1);
				dictMatChannel[1].create(nHeight, nWidth, CV_8UC1);
				dictMatChannel[2].create(nHeight, nWidth, CV_8UC1);
				unsigned char *pR = (unsigned char *)ptrR;
				unsigned char *pG = (unsigned char *)ptrG;
				unsigned char *pB = (unsigned char *)ptrB;
				memcpy(dictMatChannel[2].data, pR, nWidth*nHeight);
				memcpy(dictMatChannel[1].data, pG, nWidth*nHeight);
				memcpy(dictMatChannel[0].data, pB, nWidth*nHeight);
				cv::merge(dictMatChannel, cMat);
			}
			return cMat;
		}

		static	HObject MatToHobj(const Mat &cMat)
		{
			HObject hObj = HObject();
			int nHeight = cMat.rows;
			int nWidth = cMat.cols;
			int i;
			if (cMat.type() == CV_8UC3)
			{
				vector<Mat> dictMatChannel;
				split(cMat, dictMatChannel);
				Mat imgB = dictMatChannel[0];
				Mat imgG = dictMatChannel[1];
				Mat imgR = dictMatChannel[2];
				vector<uchar>vDataR(nWidth*nHeight, 0);
				vector<uchar>vDataG(nWidth*nHeight, 0);
				vector<uchar>vDataB(nWidth*nHeight, 0);

				for (i = 0; i < nHeight; i++)
				{
					memcpy(&vDataR[0]+ nWidth*i, imgR.data + imgR.step*i, nWidth);
					memcpy(&vDataG[0] + nWidth*i, imgG.data + imgG.step*i, nWidth);
					memcpy(&vDataB[0] + nWidth*i, imgB.data + imgB.step*i, nWidth);
				}
				GenImage3(&hObj, "byte", nWidth, nHeight, (Hlong)(&vDataR[0]), (Hlong)(&vDataG[0]), (Hlong)(&vDataB[0]));
				vector<uchar>().swap(vDataR);
				vector<uchar>().swap(vDataG);
				vector<uchar>().swap(vDataB);
			}
			else if (cMat.type() == CV_8UC1)
			{
				vector<uchar> vData(nWidth*nHeight, 0);
				for (i = 0; i < nHeight; i++)
				{
					memcpy(&vData[0] + nWidth*i, cMat.data + cMat.step*i, nWidth);
				}
				GenImage1(&hObj, "byte", nWidth, nHeight, (Hlong)(&vData[0]));
				vector<uchar>().swap(vData);
			}
			return hObj;
		}
	};
}
