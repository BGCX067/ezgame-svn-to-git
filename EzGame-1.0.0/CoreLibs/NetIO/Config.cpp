#include "stdafx.h"
#include ".\config.h"
#include "ReadFile.h"

CConfig::CConfig()
{
}

CConfig::~CConfig(void)
{
	Release();
}


void CConfig::Release()
{
    for(size_t i = 0 ; i < m_vectorSection.size(); i++)
	{
		m_vectorSection[i].pVectorProfileString->clear();
		m_vectorSection[i].SectionName.clear();
	}

	m_vectorSection.clear();
}

bool CConfig::FindProfileString(std::vector<sProfileString>* pVectorProfileString, std::string strKey, std::string &strData)
{
	std::vector<sProfileString>::iterator  it;
	sProfileString	Temp;

	for(it = pVectorProfileString->begin(); it != pVectorProfileString->end(); it++)
	{
		if((*it).Key == strKey)
		{
			strData = (*it).Data;
			return true;
		}
	}

	return false;

}

bool CConfig::GetProfileString(std::string strSection, std::string strKey, std::string &strData)
{

	for(size_t i = 0 ; i < m_vectorSection.size(); i++)
	{
		if(strSection == m_vectorSection[i].SectionName)
		{
			return FindProfileString(m_vectorSection[i].pVectorProfileString, strKey, strData);
		}

	}

	return false;
}


int CConfig::GetProfileInt(std::string strSection, std::string strKey)
{
	std::string strData;

	for(size_t i = 0 ; i < m_vectorSection.size(); i++)
	{
		if(strSection == m_vectorSection[i].SectionName)
		{
			if(FindProfileString(m_vectorSection[i].pVectorProfileString, strKey, strData))
			{
				return atoi(strData.c_str());
			}
			else
			{
				return 0;

			}



		}

	}

	return 0;
}

std::vector<CConfig::sProfileString>* CConfig::GetSectionVector(std::string strSectionName)
{
	for(size_t i = 0; i < m_vectorSection.size(); i++)
	{
		if(strSectionName == m_vectorSection[i].SectionName)
		{
			return m_vectorSection[i].pVectorProfileString;
		}
	}

	sProfileData  Temp;

	Temp.SectionName = strSectionName;
	Temp.pVectorProfileString = new std::vector<CConfig::sProfileString>;

	m_vectorSection.push_back(Temp);

	return Temp.pVectorProfileString;
}

bool CConfig::LoadFromFile(const char* filename)
{
	Release();

	int line;
	std::vector<sProfileString>*	pVectorProfileString = GetSectionVector("");
	std::string						strBuffer;
	std::string						strSectionName;
	sProfileString					temp;

	CReadFile* file = new CReadFile(filename);

	if(!file->isOpen())
	{
		delete file;
		return false;
	}


	for(line = 1; ; line++)
	{
		file->GetLine(strBuffer);

		if(file->isEof()) break;

		std::string::size_type pos;
		pos = strBuffer.find_first_not_of(" \t");

		if((pos == std::string::npos) || (strBuffer[pos] == '\n') || (strBuffer[pos] == '\r') || (strBuffer[pos] == 0)) continue;

		if(strBuffer[pos] == '#') continue;							// 주석이다.

		else if(strBuffer[pos] == '<')								// 섹션테이터 이다.
		{
			if(strBuffer[pos+1] == '/')
			{
                pVectorProfileString = GetSectionVector("");		// 섹션 닫음;
				continue;
			}
			else
			{
                strSectionName = strBuffer.substr(pos + 1, (strBuffer.find_first_of(" >", pos) - pos) - 1);
				pVectorProfileString = GetSectionVector(strSectionName);
				continue;	
			}
		}

		temp.Key = strBuffer.substr(pos, strBuffer.find_first_of(" =\t\n\r", pos) - pos);
		pos += temp.Key.size();

        pos = strBuffer.find_first_not_of(" \t", pos);
		if((pos == std::string::npos) || (strBuffer[pos] != '='))
		{
			Release();
            return false;
		}

        pos = strBuffer.find_first_not_of(" \t", pos + 1);

		temp.Data = strBuffer.substr(pos, strBuffer.find_first_of("\n\r", pos) - pos);

		pVectorProfileString->push_back(temp);

		if(file->isEof()) break;
	}

	return true;

}