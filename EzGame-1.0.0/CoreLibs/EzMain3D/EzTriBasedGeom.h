#pragma once

#include "EzGeometry.h"


EzSmartPointer(EzTriBasedGeom);

class EzTriBasedGeom : public EzGeometry
{
	EzDeclareStream;
	EzDeclareRTTI;
	EzDeclareCopyMember(EzTriBasedGeom);

public:
	EzTriBasedGeom(void);
	~EzTriBasedGeom(void);

	
};

