#pragma once

#include "EzTriBasedGeom.h"

EzSmartPointer(EzTriStrips);

class EzTriStrips : public EzTriBasedGeom
{
	EzDeclareRTTI;
	EzDeclareClon(EzTriStrips);

public:
	EzTriStrips(void);
	~EzTriStrips(void);

};

