#include "EzConfigFile.h"
#include "../EzCore/EzFast_atof.h"


EzConfigFile::EzConfigFile(EzFile* file)
{
	loadFromFile(file);
}

EzConfigFile::~EzConfigFile(void)
{
	m_aSection.clear();
}

bool EzConfigFile::getProfileInt(EzStringc strSection, EzStringc strKey, int& iValue)
{
	EzStringc  value;

	if(getProfileString(strSection, strKey, value))
	{
		iValue = atoi(value.c_str());
		return true;
	}

	iValue = 0;

	return false;
}

bool EzConfigFile::getProfileFloat(EzStringc strSection, EzStringc strKey, float& iValue)
{
	EzStringc  value;

	if(getProfileString(strSection, strKey, value))
	{
		iValue = EzFast_atof(value.c_str());
		return true;
	}

	iValue = 0;

	return false;
}

bool EzConfigFile::getProfileString(EzStringc strSection, EzStringc strKey, EzStringc &strData)
{

	for(size_t i = 0 ; i < m_aSection.size(); i++)
	{
		if(strSection == m_aSection[i].SectionName)
		{
			for(unsigned int n = 0; n < m_aSection[i].aProfileString.size(); ++n)
			{
				if(m_aSection[i].aProfileString[n].Key == strKey)
				{
					strData = m_aSection[i].aProfileString[n].Data;
					return true;
				}
			}

			break;
		}
	}

	return false;
}


void EzConfigFile::loadFromFile(EzFile* file)
{
	unsigned int uSectionCount  = -1;
	bool bContinue = true;

	EzStringc  Linedata;

	while(bContinue)
	{
		Linedata = "";

		if(file->gets(Linedata) == -1)
		{
			bContinue = false;
		}

		unsigned int uPos = Linedata.findFirstCharNotInList(" \t\r", 3);

		if(uPos == -1)
			continue;

		if(Linedata[uPos] == ';') 
			continue;  // It is a comment

		else if(Linedata[uPos] == '[')   // It is a section
		{
			unsigned int uBeginSection;
			unsigned int uEndSection;

			uBeginSection = Linedata.findNextCharNotInList(uPos + 1, " \t", 2);
			uEndSection = Linedata.findNext(']', uBeginSection);

			SSection  section;
			section.SectionName = Linedata.subString(uBeginSection, uEndSection - uBeginSection);

			m_aSection.push_back(section);

			uSectionCount++;
			continue;
		}
		else
		{
			unsigned int uBeginItem = uPos;
			unsigned int uEndItem;
			EzStringc SectionKey;
			EzStringc SectionItem;

			SProfileString Profile;

			uEndItem = Linedata.findNext('=', uPos);
			SectionKey = Linedata.subString(uBeginItem, uEndItem - uBeginItem);
			Profile.Key = SectionKey.trim();

			uBeginItem = Linedata.findNextCharNotInList(uEndItem + 1, " \t", 2);

			SectionItem = Linedata.subString(uBeginItem, Linedata.size() - uBeginItem);

			Profile.Data = SectionItem.trim();
			m_aSection[uSectionCount].aProfileString.push_back(Profile);
		}
	}

}

