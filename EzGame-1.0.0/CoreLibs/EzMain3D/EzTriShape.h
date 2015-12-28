#pragma once

#include "EzTriBasedGeom.h"

class EzRender;

EzSmartPointer(EzTriShape);

class EzTriShape : public EzTriBasedGeom
{

	EzDeclareRTTI;
	EzDeclareStream;
	EzDeclareClon(EzTriShape);

public:
	EzTriShape(void);
	~EzTriShape(void);

	virtual void render(EzRenderer* pRenderer);
};


