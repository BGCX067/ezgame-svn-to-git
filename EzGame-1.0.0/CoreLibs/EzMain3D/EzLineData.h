#pragma once

#include "EzGeometryData.h"


EzSmartPointer(EzLineData);

class EzLineData : public EzGeometryData
{
	EzDeclareRTTI;
	EzDeclareCopyMember(EzLineData);


public:
	EzLineData(void);
	~EzLineData(void);


	virtual void getIndexData(unsigned short* pIdxBuffer);

	inline void setConnectFlag(bool* pbConnectFlag) { m_pbConnectFlag = pbConnectFlag; }

protected:

	bool* m_pbConnectFlag;
	
};

