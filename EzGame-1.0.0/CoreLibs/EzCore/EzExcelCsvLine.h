#pragma once

#include "EzString.h"
#include "EzArray.h"
#include "EzRefObject.h"
#include "EzSmartPointer.h"


EzSmartPointer(EzExcelCsvLine);

class EZCORE_ENTRY EzExcelCsvLine : public EzRefObject
{
public:
	EzExcelCsvLine(void);
	~EzExcelCsvLine(void);

	void encodeCSV(EzStringc str);

	inline unsigned int getColCount() { return m_aString.size(); }
	const char* getText(unsigned int iCol);
	const float getFloat(unsigned int iCol);
	const int getInt(unsigned int iCol);
	bool getBool (unsigned int iCol);

	const EzStringc& getString(unsigned int iCol);
	inline bool checkParam (unsigned int iCol);

	static char			null_buffer[1];
	static EzStringc*	null_string;

protected:


	EzArray<EzStringc>			m_aString;
};


