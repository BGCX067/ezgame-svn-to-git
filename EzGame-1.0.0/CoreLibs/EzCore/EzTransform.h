#pragma once

#include "EzMatrix4.h"
#include "EzMatrix3.h"
#include <float.h>

class EZCORE_ENTRY EzTransform
{
public:
	EzTransform(void);
	~EzTransform(void);

	EzTransform operator*( const EzTransform & xform ) const
	{
		EzTransform res;

		res.m_fScale = m_fScale * xform.m_fScale;
		res.m_Rotation = m_Rotation * xform.m_Rotation;
		res.m_Translation = m_Translation + m_fScale * (m_Rotation * xform.m_Translation);

		return res;
	}

	bool operator==( const EzTransform & xform ) const
	{
		return (xform.m_Rotation == m_Rotation &&  xform.m_Translation == m_Translation && xform.m_fScale == m_fScale);
	}

	
	EzPoint3 operator*( const EzPoint3 & v ) const
	{
		return (((m_Rotation * v) * m_fScale) + m_Translation);
	}

	inline void makeIdentity()
	{
		m_Rotation.makeIdentity();
		m_Translation.X = 0.0f;
		m_Translation.Y = 0.0f;
		m_Translation.Z = 0.0f;
		m_fScale = 1.0f;
	}


	inline void setRotation(const EzPoint3 &rotation)
	{
		m_Rotation.setRotationDegrees(rotation);
	}

	inline EzPoint3 getRotation()
	{
		return m_Rotation.getRotationDegrees();
	}

	inline const EzMatrix3& getToMatrixRotation() const
	{
		return m_Rotation;
	}

	inline void setRotation(const EzMatrix3 &mat)
	{
		m_Rotation = mat;
	}

	inline void setRotation(const EzQuaternion &q)
	{
		m_Rotation.fromQuat(q);
	}

	inline void setScale(const float& fValue)
	{
		m_fScale = fValue;
	}

	inline const float& getScale() const
	{
		return m_fScale;
	}



	inline void setTranslation(const EzPoint3& Value)
	{
		m_Translation = Value;
	}

	inline const EzPoint3& getTranslation() const
	{
		return m_Translation;
	}

	inline EzMatrix4 EzTransform::getMatrix() const
	{
		EzMatrix4 m;

		m( 0, 0 ) = m_Rotation(0, 0) * m_fScale;
		m( 0, 1 ) = m_Rotation(0, 1) * m_fScale;
		m( 0, 2 ) = m_Rotation(0, 2) * m_fScale;
		m( 0, 3 ) = 0.0f;

		m( 1, 0 ) = m_Rotation(1, 0) * m_fScale;
		m( 1, 1 ) = m_Rotation(1, 1) * m_fScale;
		m( 1, 2 ) = m_Rotation(1, 2) * m_fScale;
		m( 1, 3 ) = 0.0f;

		m( 2, 0 ) = m_Rotation(2, 0) * m_fScale;
		m( 2, 1 ) = m_Rotation(2, 1) * m_fScale;
		m( 2, 2 ) = m_Rotation(2, 2) * m_fScale;
		m( 2, 3 ) = 0.0f;

		m( 3, 0 ) = m_Translation.X;
		m( 3, 1 ) = m_Translation.Y;
		m( 3, 2 ) = m_Translation.Z;
		m( 3, 3 ) = 1.0f;

		return m;
	}

	inline bool isTranslateValid() const
	{
		return m_Translation.X != -FLT_MAX;
	}
	
	inline bool isRotateValid() const
	{
		return !m_Rotation.isIdentity();
	}
	
	inline bool isScaleValid() const
	{
		return m_fScale != -FLT_MAX;
	}

	inline bool isTransformInvalid() const
	{
		return !(isScaleValid() || isRotateValid() || isTranslateValid());
	}

	inline void makeInvalid()
	{
		 m_Translation.X = -FLT_MAX;
		 m_fScale = -FLT_MAX;
		 m_Rotation.makeIdentity();
		 


	}

	void getInvert(EzTransform& Dest)
	{
		Dest.m_Rotation = m_Rotation.transpose();
		Dest.m_fScale = 1.0f / m_fScale;
		Dest.m_Translation = Dest.m_fScale * (Dest.m_Rotation * -m_Translation);
	}


private:
	EzMatrix3					m_Rotation;
	EzPoint3					m_Translation;
	float							m_fScale;

};

