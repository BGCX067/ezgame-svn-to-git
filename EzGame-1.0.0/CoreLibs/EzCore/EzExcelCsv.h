#pragma once

#include "EzCoreLibType.h"
#include "EzFile.h"
#include "EzExcelCsvLine.h"
#include "EzString.h"
#include "EzArray.h"
#include "EzRefObject.h"


EzSmartPointer(EzExcelCsv);

class EZCORE_ENTRY EzExcelCsv : public EzRefObject
{
public:
	EzExcelCsv(EzFile* file);
	~EzExcelCsv(void);

	const EzStringc& getString(int line, EzStringc field);
	const char* getText(int line, EzStringc field);
	const float getFloat(int line, EzStringc field);
	const int getInt(int line, EzStringc field);
	const bool getBool(int line, EzStringc field);
	const unsigned int size() { return (unsigned int) m_aCsvLine.size(); }



protected:
	unsigned int getCol(EzStringc field);

	EzArray<EzExcelCsvLine*>  m_aCsvLine;
	EzExcelCsvLine			  m_ExcelCsvColnumInfo;
};


EZCORE_ENTRY EzExcelCsv* CreateCSVReader(const char* fileName);

