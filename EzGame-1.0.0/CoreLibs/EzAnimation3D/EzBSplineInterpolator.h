#pragma once

#include "EzInterpolator.h"


class EzBSplineData;
class EzBSplineBasisData;


class EzBSplineInterpolator : public EzInterpolator
{
public:
	EzBSplineInterpolator(void);
	~EzBSplineInterpolator(void);

	f32 getStartTime() const;
	void setStartTime( f32 value );
	f32 getStopTime() const;
	void setStopTime( f32 value );

	EzBSplineData* getSplineData() const;
	void setSplineData( EzBSplineData * value );
	EzBSplineBasisData* getBasisData() const;
	void setBasisData( EzBSplineBasisData * value );

protected:
	// internal method for bspline calculation in child classes
	static void bspline(int n, int t, int l, f32 *control, f32 *output, int num_output);
	bool bsplineinterpolateQuat(EzQuaternion & value, s32 n, s32 t, s32 l, f32 *control, f32 mult, f32 bias );

protected:
	/*! Animation start time. */
	f32					m_fStartTime;
	/*! Animation stop time. */
	f32					m_fStopTime;
	/*! Refers to EzBSplineData. */
	EzBSplineData*	m_pBSpplineData;
	/*! Refers to CBSPlineBasisData. */
	EzBSplineBasisData* m_pBasisData;


};
