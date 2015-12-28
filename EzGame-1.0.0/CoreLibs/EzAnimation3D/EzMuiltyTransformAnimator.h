#ifndef _EZTRANSFORMCONTROLLERFORBLEND_H_INCLUDE_
#define _EZTRANSFORMCONTROLLERFORBLEND_H_INCLUDE_

#include "EzInterpAnimator.h"
#include "EzBlendTransformInterpolator.h"

class EzAVObject;

EzSmartPointer(EzMuiltyTransformAnimator);


class EzMuiltyTransformAnimator : public EzInterpAnimator
{
	EzDeclareRTTI;

public:
	EzMuiltyTransformAnimator(u16 usCount);
	~EzMuiltyTransformAnimator(void);

	void update(f32 fTime);
	EzBlendInterpolator* getBlendInterpolator(u16 usIndex) const;
	EzAVObject* getInterpolatorTarget(u16 usIndex);
	void setInterpolatorTarget(u16 usIndex, EzAVObject* pkTarget);
	u16 getInterpolatorTargetIndex(EzAVObject* pkObj);
	u16 addInterpolatorTarget(EzAVObject* pTarget);

	virtual EzInterpolator* getInterpolator(u16 usIndex = 0) const;
	virtual void setInterpolator(EzInterpolator* interpolator, u16 usIndex = 0);

protected:
	EzBlendTransformInterpolator*	m_pBlendInterps;
	EzAVObject**					m_ppTargets;
	u16								m_usNumInterps;
};

inline EzBlendInterpolator* EzMuiltyTransformAnimator::getBlendInterpolator(u16 usIndex) const
{
	return &m_pBlendInterps[usIndex];
}

inline EzAVObject* EzMuiltyTransformAnimator::getInterpolatorTarget(u16 usIndex)
{
	EZASSERT(usIndex < m_usNumInterps);
	return m_ppTargets[usIndex];
}

inline void EzMuiltyTransformAnimator::setInterpolatorTarget(u16 usIndex, EzAVObject* pkTarget)
{
	EZASSERT(usIndex < m_usNumInterps);
	m_ppTargets[usIndex] = pkTarget;
	//pTarget->setSelectiveUpdateTransformsOverride(true);
}

inline EzInterpolator* EzMuiltyTransformAnimator::getInterpolator(unsigned short usIndex) const
{
	return &m_pBlendInterps[usIndex];
}

inline void EzMuiltyTransformAnimator::setInterpolator(EzInterpolator* pkInterpolator, unsigned short usIndex)
{
}

inline u16 EzMuiltyTransformAnimator::getInterpolatorTargetIndex(EzAVObject* pkObj)
{
	for (u16 us = 0; us < m_usNumInterps; us++)
	{
		if (m_ppTargets[us] == pkObj)
		{
			return us;
		}
	}

	return USHRT_MAX;
}

inline u16 EzMuiltyTransformAnimator::addInterpolatorTarget(EzAVObject* pTarget)
{
	u16 usIndex;
	for (usIndex = 0; usIndex < m_usNumInterps; usIndex++)
	{
		if (m_ppTargets[usIndex] == NULL)
		{
			m_ppTargets[usIndex] = pTarget;
			//pkTarget->setSelectiveUpdateTransformsOverride(true);
			return usIndex;
		}
	}

	return USHRT_MAX;
}


#endif
