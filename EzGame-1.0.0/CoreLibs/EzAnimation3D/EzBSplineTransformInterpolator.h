#pragma once

#include "EzBSplineInterpolator.h"
#include "SAniKeyFrame.h"


class EzBSplineTransformInterpolator : public EzBSplineInterpolator
{
public:
	EzBSplineTransformInterpolator(void);
	~EzBSplineTransformInterpolator(void);

	const EzPoint3& getTranslation()
	{
		return m_TransformValue.getTranslation();
	}

	void setTranslation( EzPoint3 Translate)
	{
		m_TransformValue.setTranslation(Translate);
	}

	EzQuaternion getRotation() 
	{
		return m_TransformValue.getToMatrixRotation().toQuat();

	}
	void setRotation( EzQuaternion value )
	{
		m_TransformValue.setRotation(value);
	}
	const f32 getScale()
	{
		return m_TransformValue.getScale();
	}
	void setScale( f32 fValue )
	{
		m_TransformValue.setScale(fValue);

	}

	void setTranslationOffset(u16 value);
	void setRotationOffset(u16 value);
	void setScaleOffset(u16 value);

	s32 getNumControlPoints() const;


	
protected:
	EzTransform		    m_TransformValue;

	/*! Starting offset for the translation data. (USHRT_MAX for no data.) */
	u16 m_uiTranslationOffset;
	/*! Starting offset for the rotation data. (USHRT_MAX for no data.) */
	u16 m_uiRotationOffset;
	/*! Starting offset for the scale data. (USHRT_MAX for no data.) */
	u16 m_uiScaleOffset;


};

