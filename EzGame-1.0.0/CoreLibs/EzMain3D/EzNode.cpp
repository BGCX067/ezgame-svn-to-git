#include "EzNode.h"
#include "EzCullingProcess.h"
#include "EzLight.h"
#include "EzStream.h"

EzImplementCreateObject(EzNode)
EzImplementRTTI(EzNode, EzAVObject);


EzNode::EzNode()
{
	m_pParent = NULL;
}


EzNode::EzNode(EzNode* parent)
{
	m_pParent = parent;
	
	if(m_pParent)
		m_pParent->addChild(this);
}

EzNode::~EzNode(void)
{
}

void EzNode::loadBinary(EzStream& Stream)
{
	EzAVObject::loadBinary(Stream);

}

void EzNode::saveBinary(EzStream& Stream)
{
	EzAVObject::saveBinary(Stream);
}


void EzNode::onVisible(EzCullingProcess& Culler)
{

	if(isVisualObject())
	{
		EzList<EzAVObjectPtr>::Iterator it = m_aChildren.begin();

		for (; it != m_aChildren.end(); ++it)
		{
			(*it)->culled(Culler);
		}
	}
}

EzAVObject* EzNode::getObjectByName(const char* Name)
{
	EzAVObject* pObject = EzAVObject::getObjectByName(Name);

	if(pObject)
	{
		return pObject;
	}

	EzNodeChildList::Iterator it = m_aChildren.begin();
	for (; it != m_aChildren.end(); ++it)
	{
		EzAVObject* pChildObject = *it;

		if(pChildObject)
		{
			pObject = pChildObject->getObjectByName(Name);

			if(pObject)
			{
				return pObject;
			}
		}
	}


	return NULL;

}


void EzNode::updateWorldBound()
{

	EzList<EzAVObjectPtr>::Iterator it = m_aChildren.begin();

	m_WorldBound.setZero();

	for (; it != m_aChildren.end(); ++it)
	{
		if(m_WorldBound.getSphereRadius() == 0.0f)
		{
			m_WorldBound = (*it)->getWorldBound();

		}
		else
		{
			m_WorldBound.merg((*it)->getWorldBound());
		}
	}
}


void EzNode::updateUpPass()
{
	EzNode* pParent = getParent();
	updateWorldBound();
	
	if (pParent)
		pParent->updateUpPass();
}

void EzNode::updateDownPass(float fTime, bool bUpdateControllers)
{
	if (bUpdateControllers)
		updateAnimators(fTime);
	
	updateWorldData();
	
	EzList<EzAVObjectPtr>::Iterator it = m_aChildren.begin();

	m_WorldBound.setZero();

	for (; it != m_aChildren.end(); ++it)
	{
		(*it)->updateDownPass(fTime, bUpdateControllers);

		if(m_WorldBound.getSphereRadius() == 0.0f)
		{
			m_WorldBound = (*it)->getWorldBound();

		}
		else
		{
			m_WorldBound.merg((*it)->getWorldBound());
		}

	}
}

EzDynamicEffectStatePtr EzNode::updateEffectsUpPass()
{
	EzDynamicEffectStatePtr pParentState = NULL;
	
	if (m_pParent) 
		pParentState = m_pParent->updateEffectsUpPass();

	return pushLocalEffects(pParentState, false);

}

EzDynamicEffectStatePtr EzNode::pushLocalEffects(EzDynamicEffectState* pParentState, bool bCopyOnChange)
{
	EzDynamicEffectStatePtr pState = pParentState;

	bool bCopiedAlready = false;

	if (pParentState)
	{
		EzDynEffectStateIter Pos = pParentState->getLightHeadPos();
		
		while (Pos)
		{
			EzLight* pLight = pParentState->getNextLight(Pos);
			if (pLight)
			{
				if (pLight->isUnaffectedNode(this))
				{
					if(!bCopiedAlready)
					{
						pState = pParentState->copy();
						bCopiedAlready = true;
					}
					
					pState->removeEffect(pLight);
				}
			}

		}
	}


	if(m_aEffectList.getSize() == 0)
	{
		return pState;
	}
	else
	{
		if (!pParentState)
		{
			pState = EzNew EzDynamicEffectState;
		}
		else
		{
			if(bCopyOnChange && !bCopiedAlready)
				pState = pParentState->copy();
			else if (!bCopyOnChange && !bCopiedAlready)
				pState = pParentState;
		}


		EzList<EzDynamicEffect*>::Iterator   it;

		it = m_aEffectList.begin();

		while (it != m_aEffectList.end())
		{
			pState->addEffect(*it);
			it++;
		}

		return pState;
	}   
}



void EzNode::updateEffectsDownPass(EzDynamicEffectState* pParentState)
{
	EzDynamicEffectStatePtr pState = pushLocalEffects(pParentState, true);

	EzList<EzAVObjectPtr>::Iterator it = m_aChildren.begin();
	for (; it != m_aChildren.end(); ++it)
	{
		(*it)->updateEffectsDownPass(pState);
	}
}

void EzNode::attachEffect(EzDynamicEffect* pEffect)
{

	EzList<EzDynamicEffect*>::Iterator   it;

	it = m_aEffectList.find(pEffect);

	if (it != m_aEffectList.end())
		return;

	m_aEffectList.push_front(pEffect);
	
	pEffect->attachAffectedNode(this);
}
//---------------------------------------------------------------------------
void EzNode::detachEffect(EzDynamicEffect* pEffect)
{

	EzList<EzDynamicEffect*>::Iterator   it;

	it = m_aEffectList.find(pEffect);

	if (it == m_aEffectList.end())
		return;

	m_aEffectList.erase(it);

	pEffect->detachAffectedNode(this);
}
//---------------------------------------------------------------------------
void EzNode::detachAllEffects()
{

	EzList<EzDynamicEffect*>::Iterator   it;

	it = m_aEffectList.begin();

	while (it != m_aEffectList.end())
	{
		(*it)->detachAffectedNode(this);
		it = m_aEffectList.erase(it);
	}
}
