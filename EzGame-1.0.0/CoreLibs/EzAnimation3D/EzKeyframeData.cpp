#include "EzKeyframeData.h"



EzImplementRTTI(EzKeyframeData, EzObject);

EzKeyframeData::EzKeyframeData(void)
: m_pQuatRotateKeys(0), m_eQuatRotateKeysType(NOINTERP), m_uiQuatRotateKeysSize(0),
	m_pTranslationsKeys(0), m_eTranslationsKeysType(NOINTERP), m_uiTranslationsKeysSize(0),
	m_pScalesKeys(0),m_eScaleKeysType(NOINTERP), m_uiScaleKeysSize(0)	
{
}

EzKeyframeData::~EzKeyframeData(void)
{
}

void EzKeyframeData::setQuatRotateKeys(SAniKeyFrame<EzQuaternion>* pQuatRotateKeys, u32 uiKeyMaxNum, EANI_KEYFREAMTYPE eType)
{
	if(!pQuatRotateKeys || uiKeyMaxNum == 0)
	{
		m_pQuatRotateKeys = NULL;
		m_eQuatRotateKeysType = NOINTERP;
		m_uiQuatRotateKeysSize = 0;
		return;
	}

	m_pQuatRotateKeys = pQuatRotateKeys;
	m_eQuatRotateKeysType = eType;
	m_uiQuatRotateKeysSize = uiKeyMaxNum;
}

SAniKeyFrame<EzQuaternion>* EzKeyframeData::getQuatRotateKeys(u32& uiKeyMaxNum, EANI_KEYFREAMTYPE& eType)
{
	uiKeyMaxNum = m_uiQuatRotateKeysSize;
	eType = m_eQuatRotateKeysType;
	return m_pQuatRotateKeys;
}

void EzKeyframeData::setTranslateKeys(SAniKeyFrame<EzPoint3>* pTranslateKeys, u32 uiKeyMaxNum, EANI_KEYFREAMTYPE eType)
{
	if(!pTranslateKeys || uiKeyMaxNum == 0)
	{
		m_pTranslationsKeys = NULL;
		m_eTranslationsKeysType = NOINTERP;
		m_uiTranslationsKeysSize = 0;
		return;
	}

	m_pTranslationsKeys = pTranslateKeys;
	m_eTranslationsKeysType = eType;
	m_uiTranslationsKeysSize = uiKeyMaxNum;
}

SAniKeyFrame<EzPoint3>* EzKeyframeData::getTranslateKeys(u32& uiKeyMaxNum, EANI_KEYFREAMTYPE& eType)
{
	eType = m_eTranslationsKeysType;
	uiKeyMaxNum = m_uiTranslationsKeysSize;

	return m_pTranslationsKeys;

}

void EzKeyframeData::setScaleKeys(SAniKeyFrame<f32>* pScalesKeys, u32 uiKeyMaxNum, EANI_KEYFREAMTYPE eType)
{
	if(!pScalesKeys || uiKeyMaxNum == 0)
	{
		m_pScalesKeys = NULL;
		m_eScaleKeysType = NOINTERP;
		m_uiScaleKeysSize =	0;
	}

	m_pScalesKeys = pScalesKeys;
	m_eScaleKeysType = eType;
	m_uiScaleKeysSize =	uiKeyMaxNum;
}

SAniKeyFrame<f32>* EzKeyframeData::getScaleKeys(u32& uiKeyMaxNum, EANI_KEYFREAMTYPE& eType)
{
	uiKeyMaxNum = m_eScaleKeysType;
	eType = m_eScaleKeysType;

	return m_pScalesKeys;

}

