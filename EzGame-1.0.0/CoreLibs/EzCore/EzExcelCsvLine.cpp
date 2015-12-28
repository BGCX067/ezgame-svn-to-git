#include "EzExcelCsvLine.h"
#include "EzFast_atof.h"


char	EzExcelCsvLine::null_buffer[1] = "";
EzStringc*	EzExcelCsvLine::null_string = NULL;

EzExcelCsvLine::EzExcelCsvLine(void)
{
	null_string = EzNew EzStringc;
}

EzExcelCsvLine::~EzExcelCsvLine(void)
{
	m_aString.clear();
}

void EzExcelCsvLine::encodeCSV(EzStringc str)
{
	int pos;
	EzStringc temp;

	str.make_upper();

	pos = str.find(",");

	while(pos != -1)
	{
        temp = str.subString(0, pos);

		m_aString.push_back(temp);

		str = str.subString(pos + 1, str.size());

		pos = str.find(",");
	}

	pos = str.find("\r");

	if(pos)
	{
		temp = str.subString(0, str.size() - 1);
	}
	else
	{
		temp = str.subString(0, str.size());
	}

	m_aString.push_back(temp);
}


bool EzExcelCsvLine::checkParam(size_t iCol)
{
    if (iCol >= m_aString.size()) return false;
	if (iCol < 0) return false;
	
	return true;
}

bool EzExcelCsvLine::getBool (size_t iCol)
{	
	if (!checkParam(iCol)) return false;
	return _stricmp(m_aString[iCol].c_str(), "true") == 0;	
}


const char* EzExcelCsvLine::getText(unsigned int iCol)
{
	if(!checkParam(iCol))
	{
		return null_buffer;
	}

	return m_aString[iCol].c_str();
}

const EzStringc& EzExcelCsvLine::getString(unsigned int iCol)
{
	if(!checkParam(iCol))
	{
		return *null_string;
	}

	return m_aString[iCol];
}


const int EzExcelCsvLine::getInt(unsigned int iCol)
{
	if(!checkParam(iCol))
	{
		return 0;
	}

	return atoi(m_aString[iCol].c_str());
}

const float EzExcelCsvLine::getFloat(unsigned int iCol)
{
	if(!checkParam(iCol))
	{
		return 0.0f;
	}

	return (float) EzFast_atof(m_aString[iCol].c_str());
}
