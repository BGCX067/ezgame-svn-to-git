#include "EzSingleInterpAnimator.h"


EzImplementRTTI(EzSingleInterpAnimator, EzInterpAnimator);

EzSingleInterpAnimator::EzSingleInterpAnimator(void)
: m_spInterpolator(0)
{
}

EzSingleInterpAnimator::~EzSingleInterpAnimator(void)
{

}

void EzSingleInterpAnimator::update(f32 fTime)
{

}


EzInterpolator* EzSingleInterpAnimator::getInterpolator(u16 usIndex) const
{
	return m_spInterpolator;

}

void EzSingleInterpAnimator::setInterpolator(EzInterpolator* pInterpolator, u16 usIndex)
{
	EZASSERT(!m_spInterpolator);
	m_spInterpolator = pInterpolator;

}
