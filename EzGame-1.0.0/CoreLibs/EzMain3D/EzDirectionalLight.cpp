#include "EzDirectionalLight.h"


EzImplementRTTI(EzDirectionalLight, EzLight);

EzDirectionalLight::EzDirectionalLight(void)
: m_vDirection(1.0f, 0.0f, 0.0f)
{
	setEffectType(DIR_LIGHT);

}

EzDirectionalLight::~EzDirectionalLight(void)
{
}

void EzDirectionalLight::updateWorldData()
{
	EzLight::updateWorldData();

	m_vDirection.X = m_World.getToMatrixRotation()[0];
	m_vDirection.Y = m_World.getToMatrixRotation()[1];
	m_vDirection.Z = m_World.getToMatrixRotation()[2];

}
