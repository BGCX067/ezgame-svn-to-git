#pragma once

#include "../EzCore/EzCore.h"
#include "../EzMain3D/EzObject.h"


class EzBSplineData : public EzObject
{
	EzDeclareRootRTTI(EzBSplineData);

public:
	EzBSplineData(void);
	~EzBSplineData(void);

	const EzArray<f32>& getFloatControlPoints() const;
	EzArray<f32> getFloatControlPointRange(u32 offset, u32 count) const;
	const EzArray<s16>& getShortControlPoints() const;
	EzArray<s16> getShortControlPointRange(u32 offset, u32 count) const;

	void getCompactedValueDegree3(f32 fTime, f32* afPos, u16 uiDimension, u32 offset, u32 NumControlPoints, f32 mult, f32 bias);

	void setFloatControlPoints(const EzArray<f32> fPoints);
	void setShortControlPoints(const EzArray<s16> iPoints);

protected:
	EzArray<f32> m_FloatControlPoints;
	EzArray<s16> m_ShortControlPoints;
};

