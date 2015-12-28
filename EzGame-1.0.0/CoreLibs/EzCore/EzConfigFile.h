#pragma once

#include "EzCoreLibType.h"
#include "EzString.h"
#include "EzArray.h"
#include "EzFile.h"
#include "EzMemObject.h"

class EZCORE_ENTRY EzConfigFile : public EzMemObject
{
public:
	EzConfigFile(EzFile* file);
	~EzConfigFile(void);

	struct SProfileString
	{
		EzStringc		Key;
		EzStringc		Data;
	};

	struct SSection
	{
		EzStringc						SectionName;
		EzArray<SProfileString>			aProfileString;
	};


	bool getProfileInt(EzStringc strSection, EzStringc strKey, int& iValue);
	bool getProfileString(EzStringc strSection, EzStringc strKey, EzStringc &strData);
	bool getProfileFloat(EzStringc strSection, EzStringc strKey, float& iValue);

protected:
	void loadFromFile(EzFile* file);

	EzStringc				m_FileName;
	EzArray<SSection>		m_aSection;

};

inline EzConfigFile* createConfigFile(const char* filename)
{
	EzFile* file;

	file = EzFile::open(filename, EzFile::READ_ONLY);

    if (!file)
		return 0;

    return EzNew EzConfigFile(file); 
}

