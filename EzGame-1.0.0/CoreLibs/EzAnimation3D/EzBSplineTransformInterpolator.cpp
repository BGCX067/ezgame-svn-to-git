#include "EzBSplineTransformInterpolator.h"
#include "EzBSplineBasisData.h"
#include <limits.h>

static const int SizeofQuat = 4;
static const int SizeofTrans = 3;
static const int SizeofScale = 1;

EzBSplineTransformInterpolator::EzBSplineTransformInterpolator(void)
: m_uiRotationOffset(USHRT_MAX), m_uiTranslationOffset(USHRT_MAX), m_uiScaleOffset(USHRT_MAX)
{
}

EzBSplineTransformInterpolator::~EzBSplineTransformInterpolator(void)
{
}

s32 EzBSplineTransformInterpolator::getNumControlPoints() const
{
   if (m_pBasisData)
   {
      return m_pBasisData->getNumControlPoints();
   }

   return 0;

}

void EzBSplineTransformInterpolator::setTranslationOffset(u16 value)
{
	m_uiTranslationOffset = value;

}

void EzBSplineTransformInterpolator::setRotationOffset(u16 value)
{
	m_uiRotationOffset = value; 

}

void EzBSplineTransformInterpolator::setScaleOffset(u16 value)
{
	m_uiScaleOffset = value;

}


