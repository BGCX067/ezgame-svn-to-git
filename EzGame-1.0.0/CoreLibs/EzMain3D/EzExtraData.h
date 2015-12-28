#pragma once

#include "EzObject.h"


class EzExtraData : public EzObject 
{
	EzDeclareRTTI;

public:
	EzExtraData(void);
	~EzExtraData(void);

	EzExtraData* GetNextExtraData() const;
	void SetNextExtraData( EzExtraData * obj );

public:
	EzExtraData*  NextExtraData;
};


