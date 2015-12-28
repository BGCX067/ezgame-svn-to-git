#pragma once

#include "../EzCore/EzTypes.h"
#include "../EzCore/EzRefObject.h"
#include "../EzCore/EzXML.h"
#include "EzCostum.h"


namespace GameEasy
{


class EzLoaderCostum : public EzRefObject
{
public:
	EzLoaderCostum(EzFile *pFile);
	~EzLoaderCostum(void);

	inline bool isOpen() { return bOpen; }
	u32 getPartsCount() { return m_aParts.size(); }
	const c8* getPartsName(u32 idx);
	const SCostumParts* getPartsData(u32 idx);
	void  setPartsChange(u32 idx, u16 item_id);

	EzCostum* getCostumObject();


protected:
	bool	Parsing();
	bool	add();
	bool	partsAttribute();
	bool	parts();
	

	bool	bOpen;

	EzArray<EzStringc>				m_aStack;

	EzXMLReaderASCII*		m_pXml;

	setting						Settingdata;
	EzArray<SCostumParts*>		m_aParts;
	EzStringc					currentenum;


};


}
