#pragma once

#include "../EzCore/EzCore.h"
#include "EzAVObject.h"


EzSmartPointer(EzCamera);

class EzCamera : public EzAVObject
{
	EzDeclareRTTI;

public:
	EzCamera(void);
	virtual ~EzCamera(void);

	virtual void update(float fTime, bool bUpdateControllers = true);

	void setProjectionMatrix(const EzMatrix4& projection, bool bOrthogonal = false);
	const EzMatrix4& getProjectionMatrix() const;
	const EzMatrix4& getViewMatrix() const;

	void setTarget(const EzPoint3& pos);
	void setRotation(const EzPoint3& rotation);

	const EzPoint3& getPosition();
	//void setPosition(const EzPoint3& newpos);

	const EzPoint3& getTarget() const;
	void setUpVector(const EzPoint3& pos);
	const EzPoint3& getUpVector() const;

	float getNearValue() const;
	float getFarValue() const;
	float getAspectRatio() const;
	float getFOV() const;
	void setNearValue(float zn);
	void setFarValue(float zf);
	void setAspectRatio(float aspect);
	void setFOV(float fovy);

	//! Binds the camera scene node's rotation to its target position and vice vera, or unbinds them.
	void bindTargetAndRotation(bool bBound);

	//! Queries if the camera scene node's rotation and its target position are bound together.
	bool getTargetAndRotationBinding(void) const;

	const SViewFrustum& getViewFrustum() const;

	
protected:
	void recalculateViewArea();
	void recalculateProjectionMatrix();


	EzPoint3 m_vTarget;
	EzPoint3 m_vUpVector;

	float m_fFovy;	// Field of view, in radians. 
	float m_fAspect;	// Aspect ratio. 
	float m_fZNear;	// value of the near view-plane. 
	float m_fZFar;	// Z-value of the far view-plane.

	SViewFrustum m_ViewArea;

	bool m_bOrthogonal;
	bool m_bTargetAndRotationAreBound;
};



inline void EzCamera::setProjectionMatrix(const EzMatrix4& projection, bool bOrthogonal)
{
	m_bOrthogonal = bOrthogonal;
	m_ViewArea.Matrices [ ETS_PROJECTION ] = projection;
	m_ViewArea.setTransformState ( ETS_PROJECTION );
}


inline const EzMatrix4& EzCamera::getProjectionMatrix() const
{
	return m_ViewArea.Matrices [ ETS_PROJECTION ];
}


inline const EzMatrix4& EzCamera::getViewMatrix() const
{
	return m_ViewArea.Matrices [ ETS_VIEW ];
}




inline void EzCamera::setTarget(const EzPoint3& pos)
{
	m_vTarget = pos;

	if(m_bTargetAndRotationAreBound)
	{
		const EzPoint3 vToTarget = m_vTarget - m_World.getTranslation();
		m_Local.setRotation(vToTarget.getHorizontalAngle());
	}
}

inline void EzCamera::setRotation(const EzPoint3& rotation)
{
	if(m_bTargetAndRotationAreBound)
		m_vTarget = m_World.getTranslation() + rotation.rotationToDirection();

	m_Local.setRotation(rotation);
}

inline const EzPoint3& EzCamera::getPosition()
{
	return m_Local.getTranslation();

}


inline const EzPoint3& EzCamera::getTarget() const
{
	return m_vTarget;
}

inline void EzCamera::setUpVector(const EzPoint3& pos)
{
	m_vUpVector = pos;
}

inline const EzPoint3& EzCamera::getUpVector() const
{
	return m_vUpVector;
}

inline float EzCamera::getNearValue() const 
{
	return m_fZNear;
}


inline float EzCamera::getFarValue() const 
{
	return m_fZFar;
}


inline float EzCamera::getAspectRatio() const 
{
	return m_fAspect;
}


inline float EzCamera::getFOV() const 
{
	return m_fFovy;
}


inline void EzCamera::setNearValue(float f)
{
	m_fZNear = f;
	recalculateProjectionMatrix();
}


inline void EzCamera::setFarValue(float f)
{
	m_fZFar = f;
	recalculateProjectionMatrix();
}


inline void EzCamera::setAspectRatio(float f)
{
	m_fAspect = f;
	recalculateProjectionMatrix();
}


inline void EzCamera::setFOV(float f)
{
	m_fFovy = f;
	recalculateProjectionMatrix();
}


inline void EzCamera::recalculateViewArea()
{
	m_ViewArea.cameraPosition = m_World.getTranslation();
	m_ViewArea.setFrom(m_ViewArea.Matrices[SViewFrustum::ETS_VIEW_PROJECTION_3]);
}

inline void EzCamera::recalculateProjectionMatrix()
{
	m_ViewArea.Matrices [ ETS_PROJECTION ].buildProjectionMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fZNear, m_fZFar);
	m_ViewArea.setTransformState ( ETS_PROJECTION );
}

inline void EzCamera::bindTargetAndRotation(bool bBound)
{
	m_bTargetAndRotationAreBound = bBound;
}


inline bool EzCamera::getTargetAndRotationBinding(void) const
{
	return m_bTargetAndRotationAreBound;
}

inline const SViewFrustum& EzCamera::getViewFrustum() const
{
	return m_ViewArea;
}

