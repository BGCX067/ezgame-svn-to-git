#pragma once


#include "EzDynamicEffect.h"


EzSmartPointer(EzLight);

class EzLight : public EzDynamicEffect
{
	EzDeclareRTTI;

public:

	EzLight();
	~EzLight(void);

	inline void setAmbientColor(EzColor color) { m_AmbientColor = color; }
	inline void setDiffuseColor(EzColor color)  { m_DiffuseColor = color; }
	inline void setSpecularColor(EzColor color) { m_SpecularColor = color; }

	inline const EzColor& getAmbientColor() { return m_AmbientColor; }
	inline const EzColor& getDiffuseColor()  { return m_DiffuseColor; }
	inline const EzColor& getSpecularColor() { return m_SpecularColor; }


protected:
	EzColor		m_AmbientColor;
	EzColor		m_DiffuseColor;
	EzColor		m_SpecularColor;

};

