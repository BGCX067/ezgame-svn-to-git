#include "EzAVObject.h"
#include "EzNode.h"
#include "EzProperty.h"
#include "EzCullingProcess.h"
#include "EzStream.h"



EzImplementRTTI(EzAVObject, EzObjectNET);

EzAVObject::EzAVObject(void)
: m_pParent(0), m_CullingType(EAC_FRUSTUM_BOX), m_spPropertyState(NULL)
{
	m_Local.makeIdentity();
	m_World.makeIdentity();
	setSelectiveUpdateTransforms(false);

}

EzAVObject::~EzAVObject(void)
{
	m_spPropertyState = NULL;
}	


void EzAVObject::loadBinary(EzStream& Stream)
{
	EzStringc ShaderLibName;
	EzObjectNET::loadBinary(Stream);

	EzStreamLoadTransform(Stream, m_Local);

	unsigned int uiPropertyCount;

	EzStreamLoad<unsigned int>(Stream, uiPropertyCount);

	EzProperty* pProperty;

	for(unsigned int n = 0; n < uiPropertyCount; ++n)
	{
		pProperty = EzDynamicCast(EzProperty, EzStreamLoadObject(Stream));
		addProperty(pProperty);
	}
}

void EzAVObject::saveBinary(EzStream& Stream)
{
	EzObjectNET::saveBinary(Stream);

	EzStreamWriteTransform(Stream, m_Local);

	EzStreamWrite<unsigned int>(Stream, m_PropertyList.getSize());

	EzPropertyList::Iterator    it;

	for(it = m_PropertyList.begin(); it != m_PropertyList.end(); it++)
	{
		EzStreamWriteObject(Stream, EzStaticCast(EzObject, *it));
	}



}

void EzAVObject::copyMember(EzAVObject* pDest)
{
	EzCopyMember(EzObjectNET);


	//pDest->m_WorldBound = getWorldBound();
	//pDest->m_World = m_World;
	//pDest->m_Transform = m_Transform;

	//for(unsigned int n = 0; n < m_aProperties.size(); ++n)
	//{
	//	pDest->m_aProperties.push_back(m_aProperties[n]);
	//	m_aProperties[n]->addRef();
	//}
}

EzAVObject* EzAVObject::getObjectByName(const char* Name)
{
	if(strcmp(getName(), Name) == 0)
	{
		return this;
	}

	return NULL;

}

void EzAVObject::updateWorldData()
{
	const char* name = getName();

    if (m_pParent)
        m_World = m_pParent->m_World * m_Local;
    else
        m_World = m_Local;

    //if (m_spCollisionObject)
    //    m_spCollisionObject->UpdateWorldData();
}


void EzAVObject::updateDownPass(float fTime, bool bUpdateControllers)
{
    if (bUpdateControllers)
        updateAnimators(fTime);

	updateWorldData();
	updateWorldBound();
}

void EzAVObject::update(float fTime, bool bUpdateControllers)
{
	updateDownPass(fTime, bUpdateControllers);

	if (m_pParent)
		m_pParent->updateUpPass();

}



void EzAVObject::updateEffects()
{
	// get the effect state incident upon the object's parent, or NULL (the
	// empty state) if the object is a root
	EzDynamicEffectStatePtr pParentState = NULL;
	if (m_pParent)
		pParentState = m_pParent->updateEffectsUpPass();
	
	updateEffectsDownPass(pParentState);
}



void EzAVObject::addProperty( EzProperty* pProp )
{

	if(m_spPropertyState == NULL)
	{
		m_spPropertyState = EzNew EzPropertyState;
	}

	m_spPropertyState->setProperty(pProp);

	m_PropertyList.push_front(pProp);
}

void EzAVObject::removeProperty(EzProperty* pProp)
{
	m_spPropertyState->removeProperty(pProp);

	EzPropertyList::Iterator    it;

	it = m_PropertyList.find(pProp);

	if(it != m_PropertyList.end())
	{
		m_PropertyList.erase(it);
	}
}

EzPropertyState* EzAVObject::getPropertyState()
{
	return m_spPropertyState;
}

void EzAVObject::updateAnimators(float fTime, bool bProperties)
{
    //if (bProperties)
    //{
    //    NiTListIterator kPos = m_kPropertyList.GetHeadPos();
    //    while (kPos)
    //    {
    //        NiProperty* pkProperty = m_kPropertyList.GetNext(kPos);
    //        if (pkProperty && pkProperty->GetControllers())
    //            pkProperty->Update(fTime);
    //    }
    //}

    EzAnimator* pAnimator = getAnimator();

	for (/**/; pAnimator; pAnimator = pAnimator->getNextAnimator())
        pAnimator->update(fTime);
}



