#include "EzTransformAnimator.h"
#include "../EzMain3D/EzAVObject.h"
#include "SAniKeyFrame.h"


EzImplementRTTI(EzTransformAnimator, EzKeyframeAnimator);

EzTransformAnimator::EzTransformAnimator(void)
{
}

EzTransformAnimator::~EzTransformAnimator(void)
{
}


void EzTransformAnimator::update(f32 fTime)
{

	if (dontDoUpdate(fTime))
    {
        return;
    }

	if (m_spInterpolator)
    {
		EzTransform trans;
		EzTransform old;

		EzAVObject* pTarget = (EzAVObject*) m_pTarget;

		if(m_spInterpolator->update(m_fScaledTime, trans))
		{

			if(trans.isRotateValid() && pTarget->getRotation() != trans.getRotation())
			{
				pTarget->setRotation(trans.getToMatrixRotation());
			}

			if(trans.isTranslateValid() && pTarget->getPosition() != trans.getTranslation())
			{
				pTarget->setPosition(trans.getTranslation());
			}

			if(trans.isScaleValid() && pTarget->getScale() != trans.getScale())
			{
				pTarget->setScale(trans.getScale());
			}

		}
	}
}
