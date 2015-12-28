#ifndef _EZAVOBJECT_H
#define _EZAVOBJECT_H


#include "EzObjectNET.h"
#include "EzBound.h"
#include "EzCullingProcess.h"
#include "EzDynamicEffectState.h"
#include "EzTexturingProperty.h"
#include "EzMaterialProperty.h"
#include "EzPropertyState.h"
#include "EzShaderLibrary.h"

class EzStream;
class EzVideoDriver;
class EzRenderer;
class EzNode;

EzSmartPointer(EzAVObject);

class EzAVObject : public EzObjectNET
{

	EzDeclareRTTI;
	EzDeclareAbstractStream;
	EzDeclareCopyMember(EzAVObject);
	EzDeclareFlags(unsigned short);

public:
	EzAVObject(void);
	virtual ~EzAVObject(void);


	void update(float fTime, bool bUpdateControllers = true);
	void updateEffects();
	virtual void updateWorldBound();
	virtual void updateWorldData();

	virtual void render(EzRenderer* pRenderer) {};

	virtual const EzBound& getWorldBound() { return m_WorldBound; }
	//! Updates the absolute position based on the relative and the parents position
	virtual void updateDownPass(float fTime, bool bUpdateControllers = true);

	virtual void updateEffectsDownPass(EzDynamicEffectState* pParentState);


	bool getSelectiveUpdateTransforms() const;
	void setSelectiveUpdateTransforms(bool bUpdate);
	bool isVisualObject();
	inline void  setAutomaticCulling(eCullingType type) { m_CullingType = type; }
	inline eCullingType getAutomaticCulling() { return m_CullingType; }

	void culled(EzCullingProcess& Culler);
	virtual void onVisible(EzCullingProcess& Culler) {};
	
	
	const float getScale();
	void setScale(const float fScale);
	const EzPoint3 getRotation();
	void setRotation(const EzPoint3& rotation);
	void setRotation(const EzMatrix3& mat);
	const EzPoint3& getPosition();
	void setPosition(const EzPoint3& newpos);
	void updateAnimators(float fTime, bool bProperties = true);
	EzNode * getParent() { return m_pParent; }
	void setParent(EzNode* parent) { m_pParent = parent; }

	void setVisibility(bool bFlag);
	bool getVisibility();


	// Property Member
	void addProperty( EzProperty* pProp );
	void removeProperty( EzProperty* pProp );
	EzPropertyState* getPropertyState();

	const EzPoint3& getWorldPosition();

	inline EzTransform& getWorldTransform();
	inline EzTransform& getLocalTransform();

	virtual EzAVObject* getObjectByName(const char* Name);

	
protected:
	typedef EzList<EzProperty*>			EzPropertyList;
    // flags
    enum
    {
        APP_CULLED_MASK                 = 0x0001,
		IS_VISIBILITY					= 0x0002,
        IS_NODE                         = 0x0004,
		SELECTIVE_XFORMS_MASK			= 0x0008
    };

	EzPropertyList								m_PropertyList;


	eCullingType								m_CullingType;
	EzBound										m_WorldBound;

	EzPropertyStatePtr							m_spPropertyState;

	EzNode*										m_pParent;


	EzTransform									m_Local;
	EzTransform									m_World;


};


inline void EzAVObject::setVisibility(bool bFlag)
{
	setBit(bFlag, IS_VISIBILITY);

}

inline bool EzAVObject::getVisibility()
{
	return getBit(IS_VISIBILITY);

}

inline bool EzAVObject::isVisualObject()
{ 
	return  (m_WorldBound.getSphereRadius() != 0.0f); 
}


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


inline const float EzAVObject::getScale()
{
	return m_Local.getScale();
}

inline void EzAVObject::setScale(const float fScale)
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

inline void EzAVObject::setSelectiveUpdateTransforms(bool bUpdate)
{
    setBit(bUpdate, SELECTIVE_XFORMS_MASK);
}





#endif 
