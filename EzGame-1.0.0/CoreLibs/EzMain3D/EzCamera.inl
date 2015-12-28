

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

inline f32 EzCamera::getNearValue() const 
{
	return m_fZNear;
}


inline f32 EzCamera::getFarValue() const 
{
	return m_fZFar;
}


inline f32 EzCamera::getAspectRatio() const 
{
	return m_fAspect;
}


inline f32 EzCamera::getFOV() const 
{
	return m_fFovy;
}


inline void EzCamera::setNearValue(f32 f)
{
	m_fZNear = f;
	recalculateProjectionMatrix();
}


inline void EzCamera::setFarValue(f32 f)
{
	m_fZFar = f;
	recalculateProjectionMatrix();
}


inline void EzCamera::setAspectRatio(f32 f)
{
	m_fAspect = f;
	recalculateProjectionMatrix();
}


inline void EzCamera::setFOV(f32 f)
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
