#pragma once

#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <atlconv.h>
#include <boost\lexical_cast.hpp>
#include <boost\algorithm\string.hpp>
#include "TypeDef.h"
#include "../tinxml/tinyxml.h"
#include "utils.h"
#include"CameraSettingDlg.h"

using namespace std;
using namespace boost;

class ConfigFileManager
{
public:
	ConfigFileManager():m_bLoaded(false),m_wstrFilePath(ConfigFileManager::GetConfigFilePath())
	{
		ParseConfigFile();
	}

	~ConfigFileManager()
	{

	}

	void ParseConfigFile()
	{
		if (m_bLoaded)
		{
			return;
		}
		if (INVALID_FILE_ATTRIBUTES == (::GetFileAttributes(m_wstrFilePath.c_str())))
		{
			AfxMessageBox(_T("CANNOT find AOI.exe.config"), MB_OK | MB_ICONERROR);
			return;
		}
		string strPath = localUtils::utils::Unicode2Utf8(m_wstrFilePath);
		TiXmlDocument configFile(strPath);
		if (!configFile.LoadFile())
		{
			//TODO:Message box here
			return;
		}
		TiXmlNode* pRootNode = configFile.FirstChild("configuration");
		if (pRootNode == nullptr)
		{
			//TODO:
			return;
		}
		ParseCommonSetting(pRootNode);
		ParseHelpInfo(pRootNode);
		ParsePgSetting(pRootNode);
		ParseStationSetting(pRootNode);
		PraseCrtProjectConfig();
		m_bLoaded = true;
	}

	wstring GetCommonSetting(wstring wstrKey)
	{
		if (m_dictCommonSetting.empty())
		{
			ParseConfigFile();
		}
		wstring wstrRet(L"");
		if(m_dictCommonSetting.find(wstrKey) != m_dictCommonSetting.end())
		{
			wstrRet = m_dictCommonSetting[wstrKey];
		}
		return wstrRet;
	}

	wstring GetHelpInfo(wstring wstrKey)
	{
		if (m_dictHelpInfo.empty())
		{
			ParseConfigFile();
		}
		wstring wstrRet(L"");
		if (m_dictHelpInfo.find(wstrKey) != m_dictHelpInfo.end())
		{
			wstrRet = m_dictHelpInfo[wstrKey];
		}
		return wstrRet;
	}

	wstring GetCurrentProjectName()
	{
		return m_wstrCrtProjectName;
	}

	void SaveCurrentProjectName()
	{
		//TODO:
	}

	std::map<int, PG_IMAGE>* GetPgSeqSetting()
	{
		return &m_dictSeqIndex_PgImage;
	}

	std::map<int, PG_IMAGE>* GetPgSetting()
	{
		return &m_dictIndex_PgImage;
	}

	std::map<int, STATION_CONFIG>* GetStaionConfigArray()
	{
		return &m_dictStationConfig;
	}

	PROJECT* GetCurrentProjectConfig()
	{
		return &m_crtProjectConfig;
	}

	bool UpdatevActionGroup(int nStationIndex, std::vector<SNAP_ACTION>& newActionSetting, wstring strProjectName, int nPatternIndex, wstring strCameraName)
	{
		if (m_dictStationConfig.find(nStationIndex) != m_dictStationConfig.end())
		{
			std::vector<SNAP_ACTION> oldSetting = m_dictStationConfig[nStationIndex].m_dictProject[strProjectName].m_vPattern[nPatternIndex].m_dictCameraName_SAA[strCameraName];
			m_dictStationConfig[nStationIndex].m_dictProject[strProjectName].m_vPattern[nPatternIndex].m_dictCameraName_SAA[strCameraName] = newActionSetting;
			// Save the new values into CONFIG file.
			if (CheckFileExists(m_wstrFilePath))
			{
				string strConfigFilePath = localUtils::utils::Unicode2Utf8(m_wstrFilePath);
				TiXmlDocument config(strConfigFilePath);
				if (!config.LoadFile())
					return false;
				TiXmlNode* pNode = config.FirstChild("configuration");
				if (pNode == nullptr)
					return false;
				auto pStationNode = pNode->FirstChild("StationSetting");
				if (pStationNode == nullptr)
					return false;
				auto pStationElement = pStationNode->ToElement();
				for (auto pElement = pStationElement->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
				{
					int nRetIndex = 1;
					pElement->QueryIntAttribute("index", &nRetIndex);
					if (nRetIndex == nStationIndex)
					{
						TiXmlNode* pProjectSetting = pElement->FirstChildElement("ProjectSetting");
						if (pProjectSetting == nullptr)
							return false;
						auto pProjectSettingElements = pProjectSetting->ToElement();
						for (auto pProjectSettingElement = pProjectSettingElements->FirstChildElement(); pProjectSettingElement; pProjectSettingElement = pProjectSettingElement->NextSiblingElement())
						{
							std::string strProjectNameutf8 = localUtils::utils::Unicode2Utf8(strProjectName);
							std::string strPrjName = "";
							pProjectSettingElement->QueryStringAttribute("name", &strPrjName);
							if (strPrjName == strProjectNameutf8)
							{
								TiXmlNode* pPatternList = pProjectSettingElement->FirstChildElement("PatternList");
								if (pPatternList == nullptr)
									return false;
								auto pPatternElements = pPatternList->ToElement();
								for (auto pPatternElement = pPatternElements->FirstChildElement(); pPatternElement; pPatternElement = pPatternElement->NextSiblingElement())
								{
									int nPatternIndexXml = 1;
									pPatternElement->QueryIntAttribute("index", &nPatternIndexXml);
									if (nPatternIndexXml == nPatternIndex)
									{
										//TODO:
										TiXmlNode* pCameraParameter = pPatternElement->FirstChildElement("CameraParameter");
										if (pCameraParameter == nullptr)
											return false;
										auto pCameras = pCameraParameter->ToElement();
										for (auto pCamera = pCameras->FirstChildElement(); pCamera; pCamera = pCamera->NextSiblingElement())
										{
											std::string strCamName = "";
											pCamera->QueryStringAttribute("name", &strCamName);
											string strNameUtf8 = localUtils::utils::Unicode2Utf8(strCameraName);
											if (strNameUtf8 == strCamName)
											{
												auto pAlgorithms = pCamera->ToElement();
												int nCrtTime = 0;
												for (auto pAlgorithm = pAlgorithms->FirstChildElement(); pAlgorithm; pAlgorithm = pAlgorithm->NextSiblingElement())
												{
													if (pAlgorithm != nullptr)
													{
														pAlgorithm->SetAttribute("focal_length", newActionSetting[nCrtTime].m_nFocalPos);
														pAlgorithm->SetAttribute("exposure_time", newActionSetting[nCrtTime].m_nExposureTime);
														pAlgorithm->SetAttribute("gain", newActionSetting[nCrtTime].m_nGain);
														pAlgorithm->SetAttribute("whitebalance_r", newActionSetting[nCrtTime].m_nWhiteBalanceR);
														pAlgorithm->SetAttribute("whitebalance_g", newActionSetting[nCrtTime].m_nWhiteBalanceG);
														pAlgorithm->SetAttribute("whitebalance_b", newActionSetting[nCrtTime].m_nWhiteBalanceB);
														auto pInterfaces = pAlgorithm->ToElement();
														int nCrtInterfaceNum = 0;
														for (auto pInterface = pInterfaces->FirstChildElement(); pInterface; pInterface = pInterface->NextSiblingElement())
														{
 															wstring strParam = newActionSetting[nCrtTime].m_vAlgoFunc[nCrtInterfaceNum].m_vParamStr[0];
 															std::string strTemp(strParam.length(), ' ');
															strTemp = localUtils::utils::Unicode2Utf8(strParam);
															for (int n = 0; n < newActionSetting[nCrtTime].m_vAlgoFunc[nCrtInterfaceNum].m_vParam.size(); n++)
															{
																ostringstream oss;
																oss << newActionSetting[nCrtTime].m_vAlgoFunc[nCrtInterfaceNum].m_vParam[n].m_value;
																strTemp = strTemp + "," + oss.str();
															}
															pInterface->SetAttribute("param", strTemp);
															nCrtInterfaceNum++;
														}
														nCrtTime++;
													}
												}
											}												
										}
									}
								}
							}
						}
					}
				}
				config.SaveFile();
				ParseConfigFile();
				return true;
			}
			else
			{
				return false;
			}
			//}
		}
		else
		{
			return false;
		}
 		return false;
	}

	bool UpdateDeviceAction(int nStationIndex, bool bSideLightOn, wstring strProjectName, int nPatternIndex)
	{
		if (m_dictStationConfig.find(nStationIndex) != m_dictStationConfig.end())
		{
			bool oldSetting = m_dictStationConfig[nStationIndex].m_dictProject[strProjectName].m_vPattern[nPatternIndex].m_bSideLightOn;
			if (oldSetting == bSideLightOn)
			{
				return true;
			}
			else
			{
				m_dictStationConfig[nStationIndex].m_dictProject[strProjectName].m_vPattern[nPatternIndex].m_bSideLightOn = bSideLightOn;
				// Save the new values into CONFIG file.
				if (CheckFileExists(m_wstrFilePath))
				{
					string strConfigFilePath = localUtils::utils::Unicode2Utf8(m_wstrFilePath);
					TiXmlDocument config(strConfigFilePath);
					if (!config.LoadFile())
						return false;
					TiXmlNode* pNode = config.FirstChild("configuration");
					if (pNode == nullptr)
						return false;
					auto pStationNode = pNode->FirstChild("StationSetting");
					if (pStationNode == nullptr)
						return false;
					auto pStationElement = pStationNode->ToElement();
					for (auto pElement = pStationElement->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
					{
						int nRetIndex = 1;
						pElement->QueryIntAttribute("index", &nRetIndex);
						if (nRetIndex == nStationIndex)
						{
							TiXmlNode* pProjectSetting = pElement->FirstChildElement("ProjectSetting");
							if (pProjectSetting == nullptr)
								return false;
							auto pProjectSettingElements = pProjectSetting->ToElement();
							for (auto pProjectSettingElement = pProjectSettingElements->FirstChildElement(); pProjectSettingElement; pProjectSettingElement = pProjectSettingElement->NextSiblingElement())
							{
								std::string strProjectNameutf8 = localUtils::utils::Unicode2Utf8(strProjectName);
								std::string strPrjName = "";
								pProjectSettingElement->QueryStringAttribute("name", &strPrjName);
								if (strPrjName == strProjectNameutf8)
								{
									TiXmlNode* pPatternList = pProjectSettingElement->FirstChildElement("PatternList");
									if (pPatternList == nullptr)
										return false;
									auto pPatternElements = pPatternList->ToElement();
									for (auto pPatternElement = pPatternElements->FirstChildElement(); pPatternElement;pPatternElement = pPatternElement->NextSiblingElement())
									{
										int nPatternIndexXml = 1;
										pPatternElement->QueryIntAttribute("index", &nPatternIndexXml);
										if (nPatternIndexXml == nPatternIndex)
										{
										}
									}
								}
							}							
						}
					}
					config.SaveFile();
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
		return false;
	}

	bool UpdateCameraHeight(wstring strProjectName,int& nCameraHeight)
	{
		for (int i = 0;i<m_dictStationConfig.size();i++)
		{
			if (nCameraHeight == m_dictStationConfig[i].m_dictProject[strProjectName].m_nMainCameraHeight)
			{
				continue;
			}
			else
			{
				m_dictStationConfig[i].m_dictProject[strProjectName].m_nMainCameraHeight = nCameraHeight;
				// Save the new values into CONFIG file.
				if (CheckFileExists(m_wstrFilePath))
				{
					string strConfigFilePath = localUtils::utils::Unicode2Utf8(m_wstrFilePath);
					TiXmlDocument config(strConfigFilePath);
					if (!config.LoadFile())
						return false;
					TiXmlNode* pNode = config.FirstChild("configuration");
					if (pNode == nullptr)
						return false;
					auto pStationNode = pNode->FirstChild("StationSetting");
					if (pStationNode == nullptr)
						return false;
					auto pStationElement = pStationNode->ToElement();
					for (auto pElement = pStationElement->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
					{
						TiXmlNode* pProjectSetting = pElement->FirstChildElement("ProjectSetting");
						if (pProjectSetting == nullptr)
							return false;
						auto pProjectSettingElements = pProjectSetting->ToElement();
						for (auto pProjectSettingElement = pProjectSettingElements->FirstChildElement(); pProjectSettingElement; pProjectSettingElement = pProjectSettingElement->NextSiblingElement())
						{
							std::string strProjectNameutf8 = localUtils::utils::Unicode2Utf8(strProjectName);
							std::string strPrjName = "";
							pProjectSettingElement->QueryStringAttribute("name", &strPrjName);
							if (strPrjName == strProjectNameutf8)
							{
								pProjectSettingElement->SetAttribute("camera_height", nCameraHeight);
							}
						}
					}
					config.SaveFile();
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		return false;
	}

	bool UpdataCrtProjectName(wstring strProjectName)
	{
		for (int i = 0; i<m_dictStationConfig.size(); i++)
		{
			if (strProjectName == m_dictStationConfig[i].m_wstrCrtProjectName)
			{
				continue;
			}
			else
			{
				m_dictStationConfig[i].m_wstrCrtProjectName = strProjectName;
				// Save the new values into CONFIG file.
				if (CheckFileExists(m_wstrFilePath))
				{
					string strConfigFilePath = localUtils::utils::Unicode2Utf8(m_wstrFilePath);
					TiXmlDocument config(strConfigFilePath);
					if (!config.LoadFile())
						return false;
					TiXmlNode* pNode = config.FirstChild("configuration");
					if (pNode == nullptr)
						return false;
					auto pStationNode = pNode->FirstChild("StationSetting");
					if (pStationNode == nullptr)
						return false;
					auto pStationElement = pStationNode->ToElement();
					for (auto pElement = pStationElement->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
					{
						std::string strProjectNameutf8 = localUtils::utils::Unicode2Utf8(strProjectName);
						pElement->SetAttribute("crt_project", strProjectNameutf8);
					}
					config.SaveFile();
				}
				else
				{
					return false;
				}
			}
		}
		m_wstrCrtProjectName = strProjectName;
		PraseCrtProjectConfig();
		return true;
	}

	bool CreateNewProject(const PROJECT& project)
	{
		string strPath = localUtils::utils::Unicode2Utf8(m_wstrFilePath);
		TiXmlDocument configFile(strPath);
		if (!configFile.LoadFile())
		{
			return false;
		}
		TiXmlNode* pRootNode = configFile.FirstChild("configuration");
		if (pRootNode == nullptr)
		{
			return false;
		}
		TiXmlNode* pStationNode = pRootNode->FirstChild("StationSetting");
		if (pStationNode == nullptr)
			return false;

		TiXmlNode* pStationElement = pStationNode->ToElement();
		if (pStationElement == nullptr)
			return false;
		for (auto pElement = pStationElement->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
		{
			int nStationIndex = -1;
			pElement->QueryIntAttribute("index", &nStationIndex);

			TiXmlNode* pProjectSettingNode = pElement->FirstChildElement("ProjectSetting");
			if (pProjectSettingNode == nullptr)
				return false;

			TiXmlElement*  ptrProject = new TiXmlElement("Project");
			string strProjectName = localUtils::utils::Unicode2Utf8(project.m_wstrProjectName);
			ptrProject->SetAttribute("name", strProjectName);
			string strHeight = boost::lexical_cast<string>(project.m_nMainCameraHeight);
			ptrProject->SetAttribute("camera_height", strHeight);

			TiXmlElement* ptrScreenNode = new TiXmlElement("ScreenParam");
			//
			TiXmlElement* childNode1 = new TiXmlElement("dimension_width");
			TiXmlText* value1 = new TiXmlText(boost::lexical_cast<string>(project.m_screenParam.m_nDimensionWidth));
			childNode1->LinkEndChild(value1);
			ptrScreenNode->LinkEndChild(childNode1);
			//
			TiXmlElement* childNode2 = new TiXmlElement("dimension_height");
			TiXmlText* value2 = new TiXmlText(boost::lexical_cast<string>(project.m_screenParam.m_nDimensionHeight));
			childNode2->LinkEndChild(value2);
			ptrScreenNode->LinkEndChild(childNode2);
			//
			TiXmlElement* childNode3 = new TiXmlElement("width_s");
			TiXmlText* value3 = new TiXmlText(boost::lexical_cast<string>(project.m_screenParam.m_nResolutionWidth_S));
			childNode3->LinkEndChild(value3);
			ptrScreenNode->LinkEndChild(childNode3);
			//
			TiXmlElement* childNode4 = new TiXmlElement("height_s");
			TiXmlText* value4 = new TiXmlText(boost::lexical_cast<string>(project.m_screenParam.m_nResolutionHeight_S));
			childNode4->LinkEndChild(value4);
			ptrScreenNode->LinkEndChild(childNode4);
			//
			TiXmlElement* childNode5 = new TiXmlElement("station_name");
			TiXmlText* value5 = new TiXmlText(localUtils::utils::Unicode2Utf8(project.m_screenParam.m_strStationName));
			childNode5->LinkEndChild(value5);
			ptrScreenNode->LinkEndChild(childNode5);
			//
			TiXmlElement* childNode6 = new TiXmlElement("product_name");
			TiXmlText* value6 = new TiXmlText(localUtils::utils::Unicode2Utf8(project.m_screenParam.m_strProductName));
			childNode6->LinkEndChild(value6);
			ptrScreenNode->LinkEndChild(childNode6);
			//ScreenParam
			ptrProject->LinkEndChild(ptrScreenNode);
			//Pattern List
			TiXmlElement* ptrPatternListNode = new TiXmlElement("PatternList");

			PROJECT tmpProject = project;
			CreateFramePatternList(ptrPatternListNode, tmpProject, nStationIndex);

			ptrProject->LinkEndChild(ptrPatternListNode);

			pProjectSettingNode->LinkEndChild(ptrProject);
		}
		configFile.SaveFile();
		ReloadConfigFile();
		return true;
	}

	bool DeleteProject(wstring wstrProjectName,bool bReload = true)
	{
		string strPath = localUtils::utils::Unicode2Utf8(m_wstrFilePath);
		TiXmlDocument configFile(strPath);
		if (!configFile.LoadFile())
		{
			return false;
		}
		TiXmlNode* pRootNode = configFile.FirstChild("configuration");
		if (pRootNode == nullptr)
		{
			return false;
		}
		TiXmlNode* pStationNode = pRootNode->FirstChild("StationSetting");
		if (pStationNode == nullptr)
			return false;

		TiXmlNode* pStationElement = pStationNode->ToElement();
		if (pStationElement == nullptr)
			return false;
		for (auto pElement = pStationElement->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
		{
			int nStationIndex = -1;
			pElement->QueryIntAttribute("index", &nStationIndex);

			TiXmlNode* pProjectSettingNode = pElement->FirstChildElement("ProjectSetting");
			if (pProjectSettingNode == nullptr)
				return false;

			string strTargetProject = localUtils::utils::Unicode2Utf8(wstrProjectName);
			TiXmlElement* pProjectElementList = pProjectSettingNode->ToElement();
			for (auto pElement = pProjectElementList->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
			{
				string strName;
				pElement->QueryStringAttribute("name", &strName);
				if (strName == strTargetProject)
				{
					pProjectElementList->RemoveChild(pElement);
					break;
					//pProjectSettingNode->RemoveChild();
				}
			}
		}
		configFile.SaveFile();
		if (bReload)
		{
			ReloadConfigFile();
		}
		return true;
	}

	bool ModifyProject(const PROJECT& project)
	{
		if (!DeleteProject(project.m_wstrProjectName))
		{
			//TODO:
			return false;
		}
		if (!CreateNewProject(project))
		{
			return false;
		}
		return true;
	}

	static wstring GetConfigFilePath()
	{
		TCHAR  moduleFileName[MAX_PATH];
		::GetModuleFileName(NULL, moduleFileName, MAX_PATH);
		wstring filePath(moduleFileName);
		filePath.append(L".config");
		return filePath;
	}

    bool ReplaceCamera(wstring wstrOldCameraName, const CAMERA_SETTING& newSetting)
    {
        //TODO:
        return false;
    }

    bool UpdateCamera(const CAMERA_SETTING& newSetting)
    {
        //TODO:
        return false;
    }

    bool AddNewCamera(const CAMERA_SETTING& newSetting)
    {
        //TODO
        return false;
    }

    bool DeleteCamera(wstring wstrCameraName)
    {
        //TODO:
        return false;
    }
private:

	void ResetInternalData()
	{
		m_bLoaded = false;
		m_dictCommonSetting.clear();
		m_dictAxisGeneralSetting.clear();
		m_dictIndex_PgImage.clear();
		m_dictSeqIndex_PgImage.clear();
		m_dictStationConfig.clear();
	}

	void ReloadConfigFile()
	{
		ResetInternalData();
		ParseConfigFile();
	}

	bool CheckFileExists(const wstring& path)
	{
		if (INVALID_FILE_ATTRIBUTES == (::GetFileAttributes(path.c_str())))
		{
			return false;
		}
		return true;
	}

	void ParseCommonSetting(TiXmlNode* pRootNode)
	{
		TiXmlNode* pCommonNode = pRootNode->FirstChild("CommonSetting");
		if (pCommonNode == nullptr)
			return;

		string key;
		string strValue;
		TiXmlElement* pElementList = pCommonNode->ToElement();
		if (pElementList != nullptr)
		{
			for (TiXmlElement* pElement = pElementList->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
			{
				pElement->QueryValueAttribute("key", &key);
				pElement->QueryValueAttribute("value", &strValue);
				if (!key.empty() && !strValue.empty())
				{
					wstring wsKey = localUtils::utils::Utf8ToUnicode(key);
					wstring wsValue = localUtils::utils::Utf8ToUnicode(strValue);
					m_dictCommonSetting[wsKey] = wsValue;
				}
			}
		}
	}

	void ParseHelpInfo(TiXmlNode* pRootNode)
	{
		TiXmlNode* pHelpNode = pRootNode->FirstChild("HelpInfo");
		if (pHelpNode == nullptr)
			return;

		string key;
		string strValue;
		TiXmlElement* pElementList = pHelpNode->ToElement();
		if (pElementList != nullptr)
		{
			for (TiXmlElement* pElement = pElementList->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
			{
				pElement->QueryValueAttribute("key", &key);
				pElement->QueryValueAttribute("value", &strValue);
				if (!key.empty() && !strValue.empty())
				{
					wstring wsKey = localUtils::utils::Utf8ToUnicode(key);
					wstring wsValue = localUtils::utils::Utf8ToUnicode(strValue);
					m_dictHelpInfo[wsKey] = wsValue;
				}
			}
		}
	}

	void ParsePgSetting(TiXmlNode* pRootNode)
	{
		TiXmlNode* pPgNode = pRootNode->FirstChild("PGSetting");
		if (pPgNode == nullptr)
		{
			return;
		}
		TiXmlElement* pElementList = pPgNode->ToElement();
		if (pElementList != nullptr)
		{
			for (TiXmlElement* pElement = pElementList->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
			{
				PG_IMAGE pgImg;
				pElement->QueryIntAttribute("index", &pgImg.m_nIndex);
				pElement->QueryIntAttribute("seq_index", &pgImg.m_seqIndex);
				string strName = "";
				pElement->QueryValueAttribute("name", &strName);
				pgImg.m_wstrName = localUtils::utils::Utf8ToUnicode(strName);
				string strRgb = "";
				pElement->QueryValueAttribute("RGB", &strRgb);
				string strType = "";
				pElement->QueryValueAttribute("Type", &strType);
				pgImg.m_wstrType = localUtils::utils::Utf8ToUnicode(strType);
				if (strRgb != "")
				{
					std::vector<string> vColor = localUtils::utils::SplitString(strRgb, ",");
					if (vColor.size() == 3)
					{
						int red = boost::lexical_cast<int>(vColor[0]);
						int green = boost::lexical_cast<int>(vColor[1]);
						int blue = boost::lexical_cast<int>(vColor[2]);
						pgImg.m_color = RGB(red, green, blue);
					}
				}
				m_dictSeqIndex_PgImage[pgImg.m_seqIndex] = pgImg;
				m_dictIndex_PgImage[pgImg.m_nIndex] = pgImg;
			}
		}
	}

	void ReadIntValue(TiXmlNode* pParentNode, string strChildName,long& retValue)
	{
		string strValue = "";
		auto pNode = pParentNode->FirstChildElement(strChildName);
		if (pNode != nullptr)
		{
			strValue = pNode->FirstChild()->Value();
			retValue = boost::lexical_cast<long>(strValue);
		}
	}

	void ParseCameraSetting(TiXmlElement* pStation, STATION_CONFIG& stationConfig)
	{
		TiXmlNode* pCameraSetting = pStation->FirstChildElement("CameraSetting");
		if (pCameraSetting == nullptr)
			return;
		TiXmlElement* pCameraList = pCameraSetting->ToElement();
		for (auto pElement = pCameraList->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
		{
			CAMERA_SETTING cameraSetting;
			pElement->QueryIntAttribute("index", (int*)&cameraSetting.m_nIndex);
			string strCameraName;
			pElement->QueryStringAttribute("name", &strCameraName);
			cameraSetting.m_wstrCameraName = localUtils::utils::Utf8ToUnicode(strCameraName);
			pElement->QueryBoolAttribute("main_camera", &cameraSetting.m_bMainCamera);
            pElement->QueryIntAttribute("type", &cameraSetting.m_type);
			pElement->QueryIntAttribute("width", (int*)&cameraSetting.m_nWidth);
			pElement->QueryIntAttribute("height", (int*)&cameraSetting.m_nHeight);
			pElement->QueryIntAttribute("serial_port", &cameraSetting.m_nSerialPort);
			pElement->QueryIntAttribute("BuffSize", &cameraSetting.m_nBuffSize);
			pElement->QueryIntAttribute("baudrate", &cameraSetting.m_nBaudrate);
			pElement->QueryIntAttribute("databits", &cameraSetting.m_nDatabits);
			pElement->QueryIntAttribute("stopsbits", &cameraSetting.m_nStopsbits);
			string strConfigFile;
			pElement->QueryStringAttribute("config_file", &strConfigFile);
			cameraSetting.m_wstrConfigFile = localUtils::utils::Utf8ToUnicode(strConfigFile);
			stationConfig.m_dictCamara[cameraSetting.m_wstrCameraName] = cameraSetting;
		}
	}

	void ReadScreenParam(TiXmlElement* pProjectElement, PROJECT& project)
	{
		TiXmlNode* pScreenParamNode = pProjectElement->FirstChildElement("ScreenParam");
		if (pScreenParamNode == nullptr)
			return;

		SCREEN_PARAM screenParam;
		ReadIntValue(pScreenParamNode, "dimension_width", screenParam.m_nDimensionWidth);
		ReadIntValue(pScreenParamNode, "dimension_height", screenParam.m_nDimensionHeight);
		ReadIntValue(pScreenParamNode, "width_s", screenParam.m_nResolutionWidth_S);
		ReadIntValue(pScreenParamNode, "height_s", screenParam.m_nResolutionHeight_S);
		//auto pFunc = pScreenParamNode->FirstChildElement();
		string strValue = "";
		auto pNodeStation = pScreenParamNode->FirstChildElement("station_name");
		if (pNodeStation != nullptr)
		{
			strValue = pNodeStation->GetText();
			screenParam.m_strStationName = localUtils::utils::Utf8ToUnicode(strValue);
		}
		auto pNode = pScreenParamNode->FirstChildElement("product_name");
		if (pNode != nullptr)
		{
			strValue = pNode->GetText();
			screenParam.m_strProductName = localUtils::utils::Utf8ToUnicode(strValue);
		}
		project.m_screenParam = screenParam;
	}

	void ReadPatternFrameList(TiXmlElement* pProjectElement, PROJECT& project)
	{
		TiXmlNode* pPattermListNode = pProjectElement->FirstChildElement("PatternList");
		if (pPattermListNode == nullptr)
			return;
		TiXmlElement* pPatternList = pPattermListNode->ToElement();
		for (auto pElement = pPatternList->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
		{
			FRAME_PATTERN  framePattern;
			pElement->QueryIntAttribute("index", &framePattern.m_nIndex);
			pElement->QueryIntAttribute("seq_index", &framePattern.m_nSeqIndex);
			pElement->QueryBoolAttribute("side_light", &framePattern.m_bSideLightOn);
			pElement->QueryIntAttribute("width_r", &framePattern.m_nWidth_R);
			pElement->QueryIntAttribute("height_r", &framePattern.m_nHeight_R);
			TiXmlNode* pCameraParamNode = pElement->FirstChildElement("CameraParameter");
			if (pCameraParamNode != nullptr)
			{
				TiXmlElement* pCameraList = pCameraParamNode->ToElement();
				for (auto pElement1 = pCameraList->FirstChildElement(); pElement1; pElement1 = pElement1->NextSiblingElement())
				{
					string strCameraName;
					std::vector<SNAP_ACTION> vActionGp;
					bool bMainCamera = false;
					pElement1->QueryStringAttribute("name", &strCameraName);
					pElement->QueryBoolAttribute("main_camera", &bMainCamera);

					TiXmlElement* pAlgoList = pElement1->ToElement();
					if (pAlgoList != nullptr)
					{
						for (auto pAlgoElement = pAlgoList->FirstChildElement(); pAlgoElement; pAlgoElement = pAlgoElement->NextSiblingElement())
						{
							SNAP_ACTION actionGp;
							pAlgoElement->QueryIntAttribute("focal_length", &actionGp.m_nFocalPos);
							pAlgoElement->QueryIntAttribute("exposure_time", &actionGp.m_nExposureTime);
							pAlgoElement->QueryIntAttribute("gain", &actionGp.m_nGain);
							pAlgoElement->QueryIntAttribute("whitebalance_r", &actionGp.m_nWhiteBalanceR);
							pAlgoElement->QueryIntAttribute("whitebalance_g", &actionGp.m_nWhiteBalanceG);
							pAlgoElement->QueryIntAttribute("whitebalance_b", &actionGp.m_nWhiteBalanceB);
							std::vector<FUNC_INFO> vAlgoFunc;
							TiXmlElement* pFuncElement = pAlgoElement->ToElement();
							if (pFuncElement != nullptr)
							{
								for (auto pFunc = pFuncElement->FirstChildElement(); pFunc; pFunc = pFunc->NextSiblingElement())
								{
									FUNC_INFO funcInfo;
									pFunc->QueryIntAttribute("index", &funcInfo.m_funcIndex);

									string strFuncName;
									pFunc->QueryStringAttribute("name", &strFuncName);
									funcInfo.m_wstrFuncName = localUtils::utils::Utf8ToUnicode(strFuncName);

									string strParam;
									pFunc->QueryStringAttribute("param", &strParam);
									wstring wstrParam = localUtils::utils::Utf8ToUnicode(strParam);
									funcInfo.m_vParamStr = localUtils::utils::SplitString(wstrParam, L",");
									vAlgoFunc.push_back(funcInfo);
								}
								actionGp.m_vAlgoFunc = vAlgoFunc;
							}
							vActionGp.push_back(actionGp);
						}
					}
					wstring wstrCameraName = localUtils::utils::Utf8ToUnicode(strCameraName);
					framePattern.m_dictCameraName_SAA[wstrCameraName] = vActionGp;
				}
			}
			project.m_vPattern.push_back(framePattern);
		}
	}

	void ParseProjectSetting(TiXmlElement* pStation, STATION_CONFIG& config)
	{
		TiXmlNode* pProjectSettingNode = pStation->FirstChildElement("ProjectSetting");
		if (pProjectSettingNode == nullptr)
			return;
		TiXmlElement* pProjectElementList = pProjectSettingNode->ToElement();
		for (auto pElement = pProjectElementList->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
		{
			PROJECT project;
			string strName;
			int nCameraHeight = -1;
			pElement->QueryStringAttribute("name", &strName);
			pElement->QueryIntAttribute("camera_height", &nCameraHeight);
			project.m_wstrProjectName = localUtils::utils::Utf8ToUnicode(strName);
			project.m_nMainCameraHeight = nCameraHeight;
			ReadScreenParam(pElement, project);
			ReadPatternFrameList(pElement, project);
			config.m_dictProject[project.m_wstrProjectName] = project;
		}
	}

	void ParseStationConfig(TiXmlElement* pStation, STATION_CONFIG& config)
	{
		ParseCameraSetting(pStation, config);
		ParseProjectSetting(pStation, config);
	}

	void ParseStationSetting(TiXmlNode* pRootNode)
	{
		if (pRootNode == nullptr)
			return;
		TiXmlNode* pStationNode = pRootNode->FirstChild("StationSetting");
		if (pStationNode == nullptr)
			return;

		TiXmlNode* pStationElement = pStationNode->ToElement();
		if (pStationElement == nullptr)
			return;
		for (auto pElement = pStationElement->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
		{
			STATION_CONFIG stationConfig;
			pElement->QueryIntAttribute("index", &stationConfig.m_nStationIndex);
			string strCrtProjectName;
			pElement->QueryStringAttribute("crt_project", &strCrtProjectName);
			stationConfig.m_wstrCrtProjectName = localUtils::utils::Utf8ToUnicode(strCrtProjectName);
			m_wstrCrtProjectName = stationConfig.m_wstrCrtProjectName;
			ParseStationConfig(pElement, stationConfig);
			m_dictStationConfig[stationConfig.m_nStationIndex] = stationConfig;
		}
	}

	void PraseCrtProjectConfig()
	{
		std::map<int, STATION_CONFIG>* ptrDict = GetStaionConfigArray();
		if (ptrDict != nullptr && !ptrDict->empty())
		{
			STATION_CONFIG& station = ptrDict->begin()->second;
			if (station.m_dictProject.find(m_wstrCrtProjectName) != station.m_dictProject.end())
			{
				m_crtProjectConfig = station.m_dictProject[m_wstrCrtProjectName];
			}
		}
	}

	string GetTargetCameraName(int nStationIndex, wstring wstrSymbolName,bool& bMainCamera)
	{
		bMainCamera = false;
		string strRet;
		if (m_dictStationConfig.find(nStationIndex) != m_dictStationConfig.end())
		{
			STATION_CONFIG& stationConfig = m_dictStationConfig[nStationIndex];
			wstring wstrRet;
			CString strName=L"";
			// 辅相机
			for (int index =0 ; index< stationConfig.m_dictCamara.size(); index++)
			{
				strName.Format(_T("%d"), index);
				if (wstrSymbolName.find(strName) != wstring::npos)
				{
					for (auto it = stationConfig.m_dictCamara.begin(); it != stationConfig.m_dictCamara.end(); it++)
					{
						if (it->second.m_wstrCameraName.find(strName) != wstring::npos)
						{
							wstrRet = it->first;
							strRet = localUtils::utils::Unicode2Utf8(wstrRet);
							return strRet;
						}
					}
					break;
				}
			}
			// 主相机
			for (auto it = stationConfig.m_dictCamara.begin(); it != stationConfig.m_dictCamara.end(); it++)
			{
				for (int index = 0; index< stationConfig.m_dictCamara.size(); index++)
				{
					strName.Format(_T("%d"), index);
					if (it->second.m_wstrCameraName.find(strName) != wstring::npos)
					{
						break;
					}
					else if (index == (stationConfig.m_dictCamara.size()-1))
					{
						wstrRet = it->first;
						strRet = localUtils::utils::Unicode2Utf8(wstrRet);
						bMainCamera = true;
					}
				}
			}
		}
		return strRet;
	}

	void CreateFramePatternList(TiXmlElement* ptrPatternListNode, PROJECT& project, int nStationIndex)
	{
		for (size_t t = 0; t < project.m_vPattern.size(); t++)
		{
			FRAME_PATTERN crtPattern = project.m_vPattern[t];

			TiXmlElement* ptrPattern = new TiXmlElement("Pattern");
			ptrPattern->SetAttribute("index", crtPattern.m_nIndex);
			ptrPattern->SetAttribute("seq_index", crtPattern.m_nSeqIndex);
			string bStr = (crtPattern.m_bSideLightOn ? "true" : "false");
			ptrPattern->SetAttribute("side_light", bStr);

			// CameraParameter Node
			TiXmlElement* ptrCameraParam = new TiXmlElement("CameraParameter");
			for (auto it = crtPattern.m_dictCameraName_SAA.begin(); it != crtPattern.m_dictCameraName_SAA.end(); it++)
			{
				bool bMainCamera = false;
				string strCameraName = GetTargetCameraName(nStationIndex,it->first, bMainCamera);
				TiXmlElement* ptrCamera = new TiXmlElement("Camera");
				ptrCamera->SetAttribute("name", strCameraName);
				string strFlag = (bMainCamera ? "true" : "false");
				ptrCamera->SetAttribute("main_camera", strFlag);
				//Add snap action
				std::vector<SNAP_ACTION>& vSnap = it->second;
				for (size_t t = 0; t < vSnap.size(); t++)
				{
					SNAP_ACTION& snap = vSnap[t];
					TiXmlElement* ptrAlgo = new TiXmlElement("Algorithm");
					if (bMainCamera)
					{
						ptrAlgo->SetAttribute("focal_length", snap.m_nFocalPos);
					}
					ptrAlgo->SetAttribute("exposure_time", snap.m_nExposureTime);
					ptrAlgo->SetAttribute("gain", snap.m_nGain);
					ptrAlgo->SetAttribute("whitebalance_r", snap.m_nWhiteBalanceR);
					ptrAlgo->SetAttribute("whitebalance_g", snap.m_nWhiteBalanceG);
					ptrAlgo->SetAttribute("whitebalance_b", snap.m_nWhiteBalanceB);
					for (size_t i = 0; i < snap.m_vAlgoFunc.size(); i++)
					{
						FUNC_INFO& funcInfo = snap.m_vAlgoFunc[i];
						TiXmlElement* ptrInterface = new TiXmlElement("interface");
						ptrInterface->SetAttribute("index", funcInfo.m_funcIndex);
						string strTmp = localUtils::utils::Unicode2Utf8(funcInfo.m_wstrFuncName);
						ptrInterface->SetAttribute("name", strTmp);
						strTmp = funcInfo.PackageDefaultParam();
						ptrInterface->SetAttribute("param", strTmp);
						strTmp = localUtils::utils::Unicode2Utf8(funcInfo.m_wstrToolName);
						ptrInterface->SetAttribute("nickname", strTmp);
						ptrAlgo->LinkEndChild(ptrInterface);
					}

					ptrCamera->LinkEndChild(ptrAlgo);
				}
				ptrCameraParam->LinkEndChild(ptrCamera);
			}
			ptrPattern->LinkEndChild(ptrCameraParam);

			//Finally add the pattern node into PatternList
			ptrPatternListNode->LinkEndChild(ptrPattern);
		}
	}
private:
	bool m_bLoaded;
	wstring m_wstrFilePath;
	wstring m_wstrCrtProjectName;
	std::map<wstring, wstring> m_dictCommonSetting;
	std::map<wstring, wstring>  m_dictHelpInfo;
	std::vector<AXIS_GENERAL_SETTING> m_dictAxisGeneralSetting;
	std::map<int, PG_IMAGE>       m_dictSeqIndex_PgImage;
	std::map<int, PG_IMAGE>       m_dictIndex_PgImage;
	std::vector<PG_PROXY_SETTING> m_vPgProxySetting;
	std::map<int, STATION_CONFIG> m_dictStationConfig;
	PROJECT                       m_crtProjectConfig;

	//deleted functions
	ConfigFileManager(const ConfigFileManager&) = delete;
	ConfigFileManager(ConfigFileManager&&) = delete;
	ConfigFileManager& operator=(const ConfigFileManager&) = delete;
	ConfigFileManager& operator=(ConfigFileManager&&) = delete;
public:

};
