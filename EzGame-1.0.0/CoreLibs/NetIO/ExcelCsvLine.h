#pragma once

#include <string>
#include <vector>

class CExcelCsvLine
{
public:
	CExcelCsvLine(void);
	~CExcelCsvLine(void);

	void EncodeCSV(std::string str);

	inline size_t GetColCount() { return m_VectorString.size(); }
	const char* GetText(size_t iCol);
	const float GetFloat(size_t iCol);
	const int GetInt(size_t iCol);
	bool GetBool (size_t iCol);

	const std::string GetString(size_t iCol);
	inline bool CheckParam (size_t iCol);

protected:


	char			null_buffer[1];
	std::string		null_string;
	std::vector<std::string>	m_VectorString;
};
