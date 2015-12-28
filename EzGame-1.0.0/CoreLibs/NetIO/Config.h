#pragma once

#include <string>
#include <vector>
#include <fstream>


class CConfig
{
public:
	CConfig();
	~CConfig(void);


	struct sProfileString
	{
		std::string  Key;
		std::string  Data;
	};

	struct sProfileData
	{
		std::string						SectionName;
		std::vector<sProfileString>*	pVectorProfileString;
	};

	bool LoadFromFile(const char* filename);

	bool GetProfileString(std::string strSection, std::string strKey, std::string &strData);
	int GetProfileInt(std::string strSection, std::string strKey);

protected:
	bool FindProfileString(std::vector<sProfileString>* pVectorProfileString, std::string strKey, std::string &strData);
	std::vector<sProfileString>* GetSectionVector(std::string strSectionName);
	void Release();

	std::string						m_FileName;
	std::vector<sProfileData>		m_vectorSection;

};

inline CConfig* CreateConfig(const char* filename)
{

	CConfig* cfg = new CConfig;

	if(!cfg->LoadFromFile(filename))
		return 0;

    return cfg; 
}


