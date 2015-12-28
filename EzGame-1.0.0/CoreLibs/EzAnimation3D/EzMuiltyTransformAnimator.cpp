#include "EzMuiltyTransformAnimator.h"
#include "../EzMain3D/EzAVObject.h"
#include "../EzCore/EzDebugConsole.h"


EzImplementRTTI(EzMuiltyTransformAnimator, EzInterpAnimator);

EzMuiltyTransformAnimator::EzMuiltyTransformAnimator(u16 usCount)
{
	m_usNumInterps = usCount;
	if (usCount == 0)
	{
		return;
	}

	m_pBlendInterps = EzNew EzBlendTransformInterpolator[usCount];
	for (u16 us = 0; us < usCount; us++)
	{
		m_pBlendInterps[us].addRef();
	}

	m_ppTargets = EzAlloc(EzAVObject*,usCount);
	memset(m_ppTargets, 0, usCount * sizeof(EzAVObject*));
}

EzMuiltyTransformAnimator::~EzMuiltyTransformAnimator(void)
{

#ifdef _DEBUG
	for (unsigned short us = 0; us < m_usNumInterps; us++)
	{
		EZASSERT(m_pBlendInterps[us].getRefCount() == 1);
	}
#endif // _DEBUG

	EzDelete[] m_pBlendInterps;
	EzFree(m_ppTargets);
}



void EzMuiltyTransformAnimator::update(f32 fTime)
{

	EzTransform Value;

	for (unsigned short us = 0; us < m_usNumInterps; us++)
	{
		// We need to check the UpdateSelected flag before updating the
		// interpolator. For instance, BoneLOD might have turned off that
		// bone.
		//if (m_ppkTargets[us] == NULL ||
		//	!m_ppkTargets[us]->GetSelectiveUpdate())
		//{
		//	continue;
		//}

		if (m_pBlendInterps[us].update(fTime, Value))
		{
			if (Value.isTranslateValid())
			{
				m_ppTargets[us]->setPosition(Value.getTranslation());
			}
			if (Value.isRotateValid())
			{
				m_ppTargets[us]->setRotation(Value.getToMatrixRotation());
			}
			if (Value.isScaleValid())
			{
				m_ppTargets[us]->setScale(Value.getScale());
			}
		}
	}
}
