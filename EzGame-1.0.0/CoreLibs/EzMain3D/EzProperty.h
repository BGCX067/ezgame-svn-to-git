#pragma once

#include "EzObjectNET.h"


class EzObjectNET;


EzSmartPointer(EzProperty);

class EzProperty :public EzObjectNET
{
	EzDeclareStream;
	EzDeclareRTTI;

public:
	enum ETYPE
	{
		MATERIAL,
		TEXTURE,
		MAX_TYPES
	};

	EzProperty(void);
	~EzProperty(void);

	virtual bool isEqual(EzObject* obj);

	virtual ETYPE type() = 0;
};

