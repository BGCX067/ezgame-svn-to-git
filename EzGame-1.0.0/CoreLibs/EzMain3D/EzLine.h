#pragma once

#include "EzGeometry.h"

EzSmartPointer(EzLine);

class EzLine : public EzGeometry
{
	EzDeclareRTTI;
	EzDeclareClon(EzLine);


public:
	
	EzLine(EzPoint3* pVertices, unsigned short uiVertexCount, EzColor* pColor, bool* pbConnectFlag);
	~EzLine(void);

	virtual void render(EzVideoDriver* pDriver);

protected:
	EzLine(void);
};

