#include "EzTransformData.h"
#include "SAniKeyFrame.h"
#include "EzInterpolator.h"


EzImplementRTTI(EzTransformData, EzKeyframeData);


EzTransformData::EzTransformData(void)
{
}

EzTransformData::~EzTransformData(void)
{
}

EzQuaternion EzTransformData::getKeyRotations(f32 fTime, u32& uiLastIdx)
{
    if (m_uiQuatRotateKeysSize == 1)
	{
        return m_pQuatRotateKeys[0].data;
	}

	u32 uiNumKeysM1 = m_uiQuatRotateKeysSize - 1;

   f32 fLastTime = m_pQuatRotateKeys[uiLastIdx].fTime;

   if ( fTime < fLastTime )
   {
       uiLastIdx = 0;
       fLastTime = m_pQuatRotateKeys[0].fTime;
   }
    //
    u32 uiNextIdx;
    f32 fNextTime = 0.0f;
    for (uiNextIdx = uiLastIdx + 1; uiNextIdx <= uiNumKeysM1; uiNextIdx++)
    {
		fNextTime = m_pQuatRotateKeys[uiNextIdx].fTime;
        if ( fTime <= fNextTime )
            break;

        uiLastIdx++;
        fLastTime = fNextTime;
    }


	EzQuaternion retData;
    f32 fNormTime = (fTime - fLastTime)/(fNextTime - fLastTime);

	InterpFunction interp = EzInterpolator::getInterpFunctions(ROTKEY, m_eQuatRotateKeysType);

	interp(fNormTime, (const void*) &m_pQuatRotateKeys[uiLastIdx].data, (const void*) &m_pQuatRotateKeys[uiNextIdx].data, (void *) &retData);

	return retData;
	
}


EzPoint3 EzTransformData::getKeyTranslations(f32 fTime, u32& uiLastIdx)
{

    if (m_uiTranslationsKeysSize == 1)
	{
        return  m_pTranslationsKeys[0].data;
	}

	u32 uiNumKeysM1 = m_uiTranslationsKeysSize - 1;

   f32 fLastTime = m_pTranslationsKeys[uiLastIdx].fTime;

   if ( fTime < fLastTime )
   {
       uiLastIdx = 0;
       fLastTime = m_pTranslationsKeys[0].fTime;
   }
    //
    u32 uiNextIdx;
    f32 fNextTime = 0.0f;
    for (uiNextIdx = uiLastIdx + 1; uiNextIdx <= uiNumKeysM1; uiNextIdx++)
    {
		fNextTime = m_pTranslationsKeys[uiNextIdx].fTime;
        if ( fTime <= fNextTime )
            break;

        uiLastIdx++;
        fLastTime = fNextTime;
    }


	EzPoint3 retData;
    f32 fNormTime = (fTime - fLastTime)/(fNextTime - fLastTime);

	InterpFunction interp = EzInterpolator::getInterpFunctions(POSKEY, m_eTranslationsKeysType);

	interp(fNormTime, (const void*) &m_pTranslationsKeys[uiLastIdx].data, (const void*) &m_pTranslationsKeys[uiNextIdx].data, (void *) &retData);

	return retData;
	
}

f32 EzTransformData::getKeyScales(f32 fTime, u32& uiLastIdx)
{

	if (m_uiScaleKeysSize == 1)
	{
		return  m_pScalesKeys[0].data;
	}

	u32 uiNumKeysM1 = m_uiTranslationsKeysSize - 1;

	f32 fLastTime = m_pScalesKeys[uiLastIdx].fTime;

	if ( fTime < fLastTime )
	{
		uiLastIdx = 0;
		fLastTime = m_pScalesKeys[0].fTime;
	}
	//
	u32 uiNextIdx;
	f32 fNextTime = 0.0f;
	for (uiNextIdx = uiLastIdx + 1; uiNextIdx <= uiNumKeysM1; uiNextIdx++)
	{
		fNextTime = m_pScalesKeys[uiNextIdx].fTime;
		if ( fTime <= fNextTime )
			break;

		uiLastIdx++;
		fLastTime = fNextTime;
	}


	f32 retData;
	
	f32 fNormTime = (fTime - fLastTime)/(fNextTime - fLastTime);

	InterpFunction interp = EzInterpolator::getInterpFunctions(FLOATKEY, m_eScaleKeysType);

	interp(fNormTime, (const void*) &m_pScalesKeys[uiLastIdx].data, (const void*) &m_pScalesKeys[uiNextIdx].data, (void *) &retData);

	return retData;

}
