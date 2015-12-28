#pragma once

#include "EzLight.h"


EzSmartPointer(EzDirectionalLight);


class EzDirectionalLight : public EzLight
{
	EzDeclareRTTI;

public:
	EzDirectionalLight(void);
	~EzDirectionalLight(void);

	const EzPoint3& getDirection() const { return m_vDirection; }

protected:
	virtual void updateWorldData();


	EzPoint3		m_vDirection;

};

