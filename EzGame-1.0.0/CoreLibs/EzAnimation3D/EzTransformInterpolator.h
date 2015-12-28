#pragma once


#include "EzKeyBasedInterpolator.h"
#include "EzTransformData.h"


EzSmartPointer(EzTransformInterpolator);

class EzTransformInterpolator : public EzKeyBasedInterpolator
{
public:
	EzTransformInterpolator(void);
	~EzTransformInterpolator(void);

	inline const EzPoint3& getTranslation()
	{
		return m_TransformValue.getTranslation();
	}

	inline void setTranslation( EzPoint3 value)
	{
		return m_TransformValue.setTranslation(value);
	}

	inline const EzQuaternion getRotation()
	{
		return m_TransformValue.getToMatrixRotation().toQuat();
	}

	inline void setRotation( EzQuaternion value )
	{
		m_TransformValue.setRotation(value);

	}
	inline const f32 getScale()
	{
		return m_TransformValue.getScale();

	}
	inline void setScale( f32 fValue )
	{
		m_TransformValue.setScale(fValue);

	}
	
	EzTransformData* getData() const;
	void setData( EzTransformData * value );

	virtual void normalizeKeys( f32 phase, f32 frequency );
	bool update(f32 fTime, EzTransform &Value);



protected:

	EzTransform		    m_TransformValue;

	EzTransformDataPtr m_spData;

	u32 m_uiLastScaleIdx;
	u32 m_uiLastTransIdx;
	u32 m_uiLastRotIdx;
};
