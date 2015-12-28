#include <fstream>
#include "EzMemoryDefines.h"
#include "EzExcelCsv.h"


EzExcelCsv::EzExcelCsv(EzFile* file)
{
	EzStringc strBuffer;
	EzExcelCsvLine* pCsvLineData = NULL;

	if(file->gets(strBuffer) > 0)
	{
		m_ExcelCsvColnumInfo.encodeCSV(strBuffer);
	}

	if(file->isEof()) 
	{
        return;
	}

	for(int line = 1; ; line++)
	{
		if(file->isEof()) 
		{
			break;
		}

		strBuffer.clear();

		file->gets(strBuffer);

		pCsvLineData = EzNew EzExcelCsvLine;

		pCsvLineData->encodeCSV(strBuffer);

		m_aCsvLine.push_back(pCsvLineData);
	}

}

EzExcelCsv::~EzExcelCsv(void)
{
	for(size_t i = 0; i < m_aCsvLine.size() ; i++)
	{
		EzDelete m_aCsvLine[i];
	}

	m_aCsvLine.clear();
	
}

unsigned int EzExcelCsv::getCol(EzStringc field)
{
	field.make_upper();

	for(unsigned int i  = 0; i < m_ExcelCsvColnumInfo.getColCount(); i++)
	{
		if(field == m_ExcelCsvColnumInfo.getString(i))
		{
			return i;
		}
	}

	return -1;
}


const float EzExcelCsv::getFloat(int line, EzStringc field)
{
	unsigned int col;

	field.make_upper();

	col = getCol(field);

    return m_aCsvLine[line]->getFloat(col);
}

const bool EzExcelCsv::getBool(int line, EzStringc field)
{
	unsigned int col;

	field.make_upper();

	col = getCol(field);

    return m_aCsvLine[line]->getBool(col);
}

const int EzExcelCsv::getInt(int line, EzStringc field)
{
	unsigned int col;

	field.make_upper();

	col = getCol(field);

    return m_aCsvLine[line]->getInt(col);
}

const EzStringc& EzExcelCsv::getString(int line, EzStringc field)
{
	unsigned int col;

	field.make_upper();

	col = getCol(field);

    return m_aCsvLine[line]->getString(col);
}
const char* EzExcelCsv::getText(int line, EzStringc field)
{
	unsigned int col;
	
	field.make_upper();

	col = getCol(field);

    return m_aCsvLine[line]->getText(col);
}

EzExcelCsv* CreateCSVReader(const char* fileName)
{
	EzFile* file;

	file = EzFile::open(fileName, EzFile::READ_ONLY);

	if (!file)
		return 0;

	return EzNew EzExcelCsv(file); 
}
