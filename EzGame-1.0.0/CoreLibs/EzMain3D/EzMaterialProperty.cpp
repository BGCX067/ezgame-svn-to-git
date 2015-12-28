#include "EzMaterialProperty.h"
#include "EzStream.h"


EzImplementCreateObject(EzMaterialProperty)
EzImplementRTTI(EzMaterialProperty, EzProperty);

unsigned int EzMaterialProperty::m_uiGlobalRevisionID = 0;

EzMaterialProperty::EzMaterialProperty(void)
: m_bLight(true)
{
	updateRevisonID();
}

EzMaterialProperty::~EzMaterialProperty(void)
{
}

void EzMaterialProperty::loadBinary(EzStream& Stream)
{
	EzProperty::loadBinary(Stream);

	EzStreamLoad<EzColor>(Stream, m_AmbientColor);
	EzStreamLoad<EzColor>(Stream, m_DiffuseColor);
	EzStreamLoad<EzColor>(Stream, m_SpecularColor);
	EzStreamLoad<EzColor>(Stream, m_EmissiveColor);
	EzStreamLoad<float>(Stream, m_fGlossiness);

}

void EzMaterialProperty::saveBinary(EzStream& Stream)
{
	EzProperty::saveBinary(Stream);

	EzStreamWrite<EzColor>(Stream, m_AmbientColor);
	EzStreamWrite<EzColor>(Stream, m_DiffuseColor);
	EzStreamWrite<EzColor>(Stream, m_SpecularColor);
	EzStreamWrite<EzColor>(Stream, m_EmissiveColor);
	EzStreamWrite<float>(Stream, m_fGlossiness);
}


bool EzMaterialProperty::isLight() const
{
	return m_bLight;

}
void EzMaterialProperty::setLight(bool bflag)
{
	m_bLight = bflag;
}


unsigned short EzMaterialProperty::getFlags() const
{
	return m_uiFlags;
}

void EzMaterialProperty::setFlags( unsigned short n )
{
	m_uiFlags = n;
}

float EzMaterialProperty::getTransparency() const
{
	return m_fAlpha;

}
void EzMaterialProperty::setTransparency( float n )
{
	m_fAlpha = n;

}

EzColor EzMaterialProperty::getAmbientColor() const
{
	return m_AmbientColor;

}

void EzMaterialProperty::setAmbientColor( const EzColor & color )
{
	m_AmbientColor = color;

}

EzColor EzMaterialProperty::getDiffuseColor() const
{
	return m_DiffuseColor;

}

void EzMaterialProperty::setDiffuseColor( const EzColor & color )
{
	m_DiffuseColor = color;

}

EzColor EzMaterialProperty::getEmissiveColor() const
{
	return m_EmissiveColor;

}

void EzMaterialProperty::setEmissiveColor( const EzColor & color )
{
	m_EmissiveColor = color;

}

EzColor EzMaterialProperty::getSpecularColor() const
{
	return m_SpecularColor;

}

void EzMaterialProperty::setSpecularColor( const EzColor & color )
{
	m_SpecularColor = color;

}

float EzMaterialProperty::getGlossiness() const
{
	return m_fGlossiness;


}

void EzMaterialProperty::setGlossiness( float n )
{
	m_fGlossiness = n;

}
