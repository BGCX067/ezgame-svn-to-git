#pragma once

#include "../core/Types.h"
#include "../core/CRefObject.h"
#include "../core/XML.h"
#include "CCostum.h"


namespace GameEasy
{


class CLoaderCostum : public CRefObject
{
public:
	CLoaderCostum(CFile *pFile);
	~CLoaderCostum(void);

	inline bool isOpen() { return bOpen; }
	u32 getPartsCount() { return m_aParts.size(); }
	const c8* getPartsName(u32 idx);
	const SCostumParts* getPartsData(u32 idx);
	void  setPartsChange(u32 idx, u16 item_id);

	CCostum* getCostumObject();


protected:
	bool	Parsing();
	bool	add();
	bool	partsAttribute();
	bool	parts();
	bool	makeenum();
	bool	setenum();

	u16		getIDFromEnum(const c8* name, const c8* value);
	

	bool	bOpen;

	EzArray<Isakstring>				m_aStack;

	CXMLReaderASCII*		m_pXml;

	setting						Settingdata;
	EzArray<SCostumParts*>		m_aParts;
	EzArray<SEnumList>			m_aEnumdata;
	Isakstring						currentenum;


};


}
