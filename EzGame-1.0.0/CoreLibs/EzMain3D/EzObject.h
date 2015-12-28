#pragma once

#include "EzMacros.h"
#include "../EzCore/EzRefObject.h"
#include "../EzCore/EzSmartPointer.h"

EzSmartPointer(EzObject);

class EzObject : public EzRefObject
{
	EzDeclareAbstractStream;
	EzDeclareRootRTTI(EzObject);

public:
	EzObject(void);
	virtual ~EzObject(void);

	bool isEqual(EzObject* pObject);
};

