#pragma once

#include "ExcelCsvLine.h"
#include <vector>
#include <string>

using namespace std;

class CExcelCsv 
{
public:
	CExcelCsv();
	~CExcelCsv(void);

	const std::string GetString(int line, std::string field);
	const char* GetText(int line, std::string field);
	const float GetFloat(int line, std::string field);
	const int GetInt(int line, std::string field);
	const bool GetBool(int line, std::string field);
	const int Size() { return (int) m_VectorCsvLine.size(); }
	bool LoadFileFromCSV(const char* fileName);



protected:
	size_t GetCol(std::string field);

	char			null_buffer[1];
	std::string		null_string;

	std::vector<CExcelCsvLine*>  m_VectorCsvLine;
	CExcelCsvLine				 m_ExcelCsvColnumInfo;
};


CExcelCsv* CreateCSVReader(const char* fileName);