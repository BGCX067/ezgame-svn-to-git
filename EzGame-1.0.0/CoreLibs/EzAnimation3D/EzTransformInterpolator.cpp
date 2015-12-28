#include "EzTransformInterpolator.h"
#include "../EzMain3D/EzAVObject.h"


EzTransformInterpolator::EzTransformInterpolator(void)
: m_spData(0), m_uiLastTransIdx(0), m_uiLastRotIdx(0), m_uiLastScaleIdx(0)
{
}

EzTransformInterpolator::~EzTransformInterpolator(void)
{
}

void EzTransformInterpolator::normalizeKeys( f32 phase, f32 frequency )
{
	if ( m_spData != NULL ) 
	{
		//data->NormalizeKeys( phase, frequency );
	}

}


bool EzTransformInterpolator::update(f32 fTime, EzTransform &Value)
{

	if (!timeHasChanged(fTime))
	{
		Value = m_TransformValue;

		if (m_TransformValue.isTransformInvalid())
			return false;

		return true;
	}

	if(m_spData)
	{
	    if (m_spData->getQuatRotatekeySize() > 0)
		{
			m_TransformValue.setRotation(m_spData->getKeyRotations(fTime, m_uiLastRotIdx));
		}

	    if (m_spData->getTranslateKeySize() > 0)
		{
			m_TransformValue.setTranslation(m_spData->getKeyTranslations(fTime, m_uiLastTransIdx));
		}

		if (m_spData->getScaleKeySize() > 0)
		{
			m_TransformValue.setScale(m_spData->getKeyScales(fTime, m_uiLastScaleIdx));
		}
	}

	Value = m_TransformValue;

	if(m_TransformValue.isTransformInvalid())
	{
		return false;
	}

	m_fLastTime = fTime;

    return true;
}

void EzTransformInterpolator::setData( EzTransformData * value )
{
	m_spData = value;
}

EzTransformData* EzTransformInterpolator::getData() const
{
	return m_spData;
}






//bool EzTransformInterpolator::timeHasChanged(f32 fTime)
//{
//    return (m_fLastTime != fTime);
//}


