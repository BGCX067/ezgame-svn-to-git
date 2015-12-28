#include "EzBSplineBasisData.h"


EzImplementRTTI(EzBSplineBasisData, EzObject);

EzBSplineBasisData::EzBSplineBasisData(void)
{
}

EzBSplineBasisData::~EzBSplineBasisData(void)
{
}

u32 EzBSplineBasisData::getNumControlPoints() const
{
	return numControlPoints;

}

void EzBSplineBasisData::setNumControlPoints( u32 value )
{
	numControlPoints = value;

}

