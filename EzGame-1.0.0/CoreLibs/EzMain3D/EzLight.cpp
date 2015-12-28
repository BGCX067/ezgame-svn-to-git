#include "EzLight.h"
#include "EzRenderer.h"



EzImplementRTTI(EzLight, EzDynamicEffect);


EzLight::EzLight()
: m_AmbientColor(0, 0, 0), m_DiffuseColor(255,255,255), m_SpecularColor(255,255,255)

{
}

EzLight::~EzLight(void)
{
}
