#include "EzTexturingProperty.h"
#include "EzStream.h"
#include "EzTextureSource.h"



EzImplementCreateObject(EzTexturingProperty);
EzImplementRTTI(EzTexturingProperty, EzProperty);

bool EzTexturingProperty::Map::operator==(const Map& Obj) const
{
    //if (((m_spTexture == 0) && (Obj.m_spTexture != 0)) ||
    //    ((m_spTexture != 0) && !m_spTexture->isEqual(Obj.m_spTexture)) ||
    //    (m_uFlags != Obj.m_uFlags))
    //{
    //    return false;
    //}

    //if (m_pTextureTransform != NULL && Obj.m_pkTextureTransform != NULL)
    //{
    //    if (*m_pTextureTransform != *Obj.m_pTextureTransform)
    //        return false;
    //}
    //else if (m_pTextureTransform == NULL && Obj.m_pTextureTransform != NULL)
    //{
    //    return false;
    //}
    //else if (m_pTextureTransform != NULL && Obj.m_pTextureTransform == NULL)
    //{
    //    return false;
    //}

    return true;
}

void EzTexturingProperty::Map::loadBinary(EzStream& Stream)
{
	bool isExternalTexture;
	EzStreamLoad<bool>(Stream, isExternalTexture);

	if(isExternalTexture)
	{
		EzStringc  Name;
		EzStreamStringLoad(Stream, Name);

		m_spTexture = createTexture(Name.c_str());
	}

	EzStreamLoad<ETEXTURE_CLAMP>(Stream, eClampMode);
	EzStreamLoad<ETEXTURE_FILTERMODE>(Stream, eFilterMode);
	EzStreamLoad<ETEXTURE_APPLY_MODE>(Stream, eApplymode);
	EzStreamLoad<bool>(Stream, hasTextureTransform);
	EzStreamLoad<EzPoint2>(Stream, translation);
	EzStreamLoad<EzPoint2>(Stream, tiling);
	EzStreamLoad<unsigned short>(Stream, usRotation);
	EzStreamLoad<unsigned short>(Stream, usTransformType);
	EzStreamLoad<EzPoint2>(Stream, centerOffset);
	EzStreamLoad<short>(Stream, uvSet);

}

void EzTexturingProperty::Map::saveBinary(EzStream& Stream)
{
	bool isExternalTexture = m_spTexture->isExternalTexture();

	EzStreamWrite<bool>(Stream, isExternalTexture);

	if(isExternalTexture)
	{
		EzStreamStringWrite(Stream, m_spTexture->getName());
	}

	EzStreamWrite<ETEXTURE_CLAMP>(Stream, eClampMode);
	EzStreamWrite<ETEXTURE_FILTERMODE>(Stream, eFilterMode);
	EzStreamWrite<ETEXTURE_APPLY_MODE>(Stream, eApplymode);
	EzStreamWrite<bool>(Stream, hasTextureTransform);
	EzStreamWrite<EzPoint2>(Stream, translation);
	EzStreamWrite<EzPoint2>(Stream, tiling);
	EzStreamWrite<unsigned short>(Stream, usRotation);
	EzStreamWrite<unsigned short>(Stream, usTransformType);
	EzStreamWrite<EzPoint2>(Stream, centerOffset);
	EzStreamWrite<short>(Stream, uvSet);
}

EzTexturingProperty::EzTexturingProperty(void)
: m_uiTextureCount(0)
{
	for(unsigned int i = 0; i < INDEX_MAX; ++i)
	{
		m_pRanderTexture[i] = NULL;
	}
}

EzTexturingProperty::~EzTexturingProperty(void)
{
	for(unsigned int i = 0; i < INDEX_MAX; ++i)
	{
		if(m_pRanderTexture[i])
		{
			EzDelete m_pRanderTexture[i];
			m_pRanderTexture[i] = NULL;
		}
	}
}

void EzTexturingProperty::loadBinary(EzStream& Stream)
{
	EzProperty::loadBinary(Stream);

	EzStreamLoad<unsigned short>(Stream, m_uiTextureCount);

	unsigned short isMap;

	for(unsigned int i = 0; i < m_uiTextureCount; ++i)
	{
		EzStreamLoad<unsigned short>(Stream, isMap);

		if(isMap == 1)
		{
			m_pRanderTexture[i] = EzNew Map;
			m_pRanderTexture[i]->loadBinary(Stream);
		}
	}


}

void EzTexturingProperty::saveBinary(EzStream& Stream)
{
	EzProperty::saveBinary(Stream);

	EzStreamWrite<unsigned short>(Stream, m_uiTextureCount);

	for(unsigned int i = 0; i < m_uiTextureCount; ++i)
	{
		if(m_pRanderTexture[i])
		{
			EzStreamWrite<unsigned short>(Stream, 1);
			m_pRanderTexture[i]->saveBinary(Stream);
		}
		else
		{
			EzStreamWrite<unsigned short>(Stream, 0);
		}
	}


}


unsigned short EzTexturingProperty::getTextureCount()
{
	return m_uiTextureCount;
}



