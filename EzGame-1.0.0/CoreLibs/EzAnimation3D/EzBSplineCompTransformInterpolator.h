#pragma once

#include "EzBSplineTransformInterpolator.h"



class EzBSplineCompTransformInterpolator : public EzBSplineTransformInterpolator
{
public:
	EzBSplineCompTransformInterpolator(void);
	~EzBSplineCompTransformInterpolator(void);

	virtual bool update(f32 fTime, EzTransform &Value);

	f32 getTranslateBias() const;
	void setTranslateBias( f32 value );

	f32 getTranslateMultiplier() const;

	void setTranslateMultiplier( f32 value );
	f32 getRotationBias() const;
	void setRotationBias( f32 value );

	f32 getRotationMultiplier() const;
	void setRotationMultiplier( f32 value );
	f32 getScaleBias() const;
	void setScaleBias( f32 value );
	f32 getScaleMultiplier() const;
	void setScaleMultiplier( f32 value );
	EzArray<EzQuaternion> getQuatRotateControlData() const;
	EzArray<EzPoint3> getTranslateControlData() const;
	EzArray<f32> getScaleControlData() const;
	EzArray< SAniKeyFrame<EzQuaternion> > sampleQuatRotateKeys(s32 npoints, s32 degree) const;
	EzArray< SAniKeyFrame<EzPoint3> > sampleTranslateKeys(s32 npoints, s32 degree) const;
	s32 getNumControlPoints() const;

	//--END CUSTOM CODE--//
protected:
	/*! Translation Bias */
	f32 m_fTranslationBias;
	/*! Translation Multiplier */
	f32 m_fTranslationMultiplier;
	/*! Rotation Bias */
	f32 m_fRotationBias;
	/*! Rotation Multiplier */
	f32 m_fRotationMultiplier;
	/*! Scale Bias */
	f32 m_fScaleBias;
	/*! Scale Multiplier */
	f32 m_fScaleMultiplier;
};
