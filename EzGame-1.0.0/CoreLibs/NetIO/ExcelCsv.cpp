#include "stdafx.h"
#include <fstream>
#include ".\excelcsv.h"
#include "ReadFile.h"
CExcelCsv* CreateCSVReader(const char* fileName)
{

	CExcelCsv* csv = new CExcelCsv();

	if(csv->LoadFileFromCSV(fileName))
	{
		return csv;
	}
	else
	{
		delete csv;
		return 0;

	}

}

CExcelCsv::CExcelCsv()
{


}

CExcelCsv::~CExcelCsv(void)
{
	for(size_t i = 0; i < m_VectorCsvLine.size() ; i++)
	{
		delete m_VectorCsvLine[i];
	}

	m_VectorCsvLine.clear();
	
}


bool CExcelCsv::LoadFileFromCSV(const char* fileName)
{
	std::string strBuffer;
	CExcelCsvLine* pCsvLineData = NULL;
	null_buffer[0] = NULL;

	CReadFile* file = new CReadFile(fileName);

	if(!file)
		return false;

	// 칼럼 정보를 저장한다.
	file->GetLine(strBuffer);

	m_ExcelCsvColnumInfo.EncodeCSV(strBuffer);

	if(!file->isEof()) 
	{
		for(int line = 1; ; line++)
		{
			if(file->isEof()) 
			{
				break;
			}


			file->GetLine(strBuffer);

			pCsvLineData = new CExcelCsvLine;

			pCsvLineData->EncodeCSV(strBuffer);

			m_VectorCsvLine.push_back(pCsvLineData);
		}
	}


	return true;
}

size_t CExcelCsv::GetCol(std::string field)
{
	for(size_t i  = 0; i < m_ExcelCsvColnumInfo.GetColCount(); i++)
	{
		if(field == m_ExcelCsvColnumInfo.GetString(i))
		{
			return i;
		}
	}

	return -1;
}


const float CExcelCsv::GetFloat(int line, std::string field)
{
	size_t Col;

	Col = GetCol(field);

	if(!m_VectorCsvLine[line]->CheckParam(Col))
	{
		return 0;
	}
	else
	{
        return m_VectorCsvLine[line]->GetFloat(Col);
	}
}

const bool CExcelCsv::GetBool(int line, std::string field)
{
	size_t Col;

	Col = GetCol(field);

	if(!m_VectorCsvLine[line]->CheckParam(Col))
	{
		return 0;
	}
	else
	{
        return m_VectorCsvLine[line]->GetBool(Col);
	}
}

const int CExcelCsv::GetInt(int line, std::string field)
{
	size_t Col;

	Col = GetCol(field);

	if(!m_VectorCsvLine[line]->CheckParam(Col))
	{
		return 0;
	}
	else
	{
        return m_VectorCsvLine[line]->GetInt(Col);
	}
}

const std::string CExcelCsv::GetString(int line, std::string field)
{
	size_t Col;

	Col = GetCol(field);

	if(!m_VectorCsvLine[line]->CheckParam(Col))
	{
		return null_string;
	}
	else
	{
        return m_VectorCsvLine[line]->GetString(Col);
	}

}
const char* CExcelCsv::GetText(int line, std::string field)
{
	size_t Col;

	Col = GetCol(field);

	if(!m_VectorCsvLine[line]->CheckParam(Col))
	{
		return null_buffer;
	}
	else
	{
        return m_VectorCsvLine[line]->GetText(Col);
	}
}