#include "stdafx.h"
#include "MathUtil.h"
#include "excelcsvline.h"

CExcelCsvLine::CExcelCsvLine(void)
{
	null_buffer[0] = 0;
}

CExcelCsvLine::~CExcelCsvLine(void)
{
	m_VectorString.clear();
}

void CExcelCsvLine::EncodeCSV(std::string str)
{
	std::string::size_type pos;
	std::string temp;

	pos = str.find(",");

	while(pos != std::string::npos)
	{
        temp = str.substr(0, pos);

		m_VectorString.push_back(temp);

		str = str.substr(pos + 1, str.find("\n"));

		pos = str.find(",");
	}

	pos = str.find("\n");

	temp = str.substr(0, pos - 1);

	m_VectorString.push_back(temp);
}


bool CExcelCsvLine::CheckParam(size_t iCol)
{
    if (iCol >= m_VectorString.size()) return false;
	if (iCol < 0) return false;
	
	return true;
}

bool CExcelCsvLine::GetBool (size_t iCol)
{	
	if (!CheckParam(iCol)) return false;
	return _stricmp(m_VectorString[iCol].c_str(), "true") == 0;	
}


const char* CExcelCsvLine::GetText(size_t iCol)
{
	if(!CheckParam(iCol))
	{
		return null_buffer;
	}

	return m_VectorString[iCol].c_str();
}

const std::string CExcelCsvLine::GetString(size_t iCol)
{
	if(!CheckParam(iCol))
	{
		return null_string;
	}

	return m_VectorString[iCol];
}


const int CExcelCsvLine::GetInt(size_t iCol)
{
	if(!CheckParam(iCol))
	{
		return 0;
	}

	return atoi(m_VectorString[iCol].c_str());
}

const float CExcelCsvLine::GetFloat(size_t iCol)
{
	if(!CheckParam(iCol))
	{
		return 0.0f;
	}

	return (float) fast_atof(m_VectorString[iCol].c_str());
}

