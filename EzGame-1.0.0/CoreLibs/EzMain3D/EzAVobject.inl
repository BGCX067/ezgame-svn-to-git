

inline void EzAVObject::updateWorldBound()
{
	// NOTE: This virtual function is intentionally inlined.
}

inline EzTransform& EzAVObject::getWorldTransform()
{
	return m_World;
}



inline EzTransform& EzAVObject::getLocalTransform()
{
	return m_Local;
}


inline const f32 EzAVObject::getScale()
{
	return m_Local.getScale();
}

inline void EzAVObject::setScale(const f32 fScale)
{
	m_Local.setScale(fScale);

}

inline const EzPoint3 EzAVObject::getRotation()
{
	return m_Local.getRotation();
}

inline void EzAVObject::setRotation(const EzPoint3& rotation)
{
	m_Local.setRotation(rotation);
}

inline void EzAVObject::setRotation(const EzMatrix3& mat)
{
	m_Local.setRotation(mat);
}

inline void  EzAVObject::culled(EzCullingProcess& Culler)
{ 
	Culler.process(this); 
}

inline void EzAVObject::updateEffectsDownPass(EzDynamicEffectState* pParentState)
{
	/* */
}


  
inline const EzPoint3& EzAVObject::getPosition()
{
	return m_Local.getTranslation();
}

inline void EzAVObject::setPosition(const EzPoint3& newpos)
{
	m_Local.setTranslation(newpos);
}

inline const EzPoint3& EzAVObject::getWorldPosition()
{
	return m_World.getTranslation();
}

inline bool EzAVObject::getSelectiveUpdateTransforms() const
{
    return getBit(SELECTIVE_XFORMS_MASK);
}
//---------------------------------------------------------------------------
inline void EzAVObject::setSelectiveUpdateTransforms(bool bUpdate)
{
    setBit(bUpdate, SELECTIVE_XFORMS_MASK);
}


