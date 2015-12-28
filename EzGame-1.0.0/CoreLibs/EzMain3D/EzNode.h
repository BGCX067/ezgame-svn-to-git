#pragma once


#include "EzSkinInstance.h"
#include "EzMacros.h"


class EzDynamicEffect;

EzSmartPointer(EzNode);

class EzNode : public EzAVObject
{
	EzDeclareStream;
	EzDeclareRTTI;

public:

	typedef EzList<EzAVObjectPtr>			EzNodeChildList;
	typedef EzList<EzSkinInstancePtr>		EzSkinsList;
	typedef	EzList<EzDynamicEffect*>		EzDynamicEffectList;

	EzNode(EzNode* parent);
	EzNode();
	~EzNode(void);


	virtual void updateDownPass(float fTime, bool bUpdateControllers = true);
	virtual void updateUpPass();
	virtual void updateWorldBound();

	EzDynamicEffectStatePtr updateEffectsUpPass();
	EzDynamicEffectStatePtr pushLocalEffects(EzDynamicEffectState* pParentState, bool bCopyOnChange);
	virtual void updateEffectsDownPass(EzDynamicEffectState* pParentState);
	

	virtual void onVisible(EzCullingProcess& Culler);

	virtual EzAVObject* getObjectByName(const char* Name);

	 	
	//! Adds a child to this scene node. If the scene node already
	//! has got a parent, it is removed from there as child.
	virtual void addChild(EzAVObject* child)
	{
		if (child)
		{

			if(child->getParent())
			{
				child->getParent()->removeChild(child);
				child->setParent(NULL);
			}

			m_aChildren.push_back(child);
			child->setParent(this);
		}
	}


	//! Removes a child from this scene node.
	//! \return Returns true if the child could be removed, and false if not.
	virtual bool removeChild(EzAVObject* child)
	{
		EzNodeChildList::Iterator it = m_aChildren.begin();
		for (; it != m_aChildren.end(); ++it)
			if ((*it) == child)
			{
				(*it)->setParent(NULL);
				m_aChildren.erase(it);
				return true;
			}

		return false;
	}


	//! Removes all children of this scene node
	virtual void removeAll()
	{
		EzNodeChildList::Iterator it = m_aChildren.begin();
		for (; it != m_aChildren.end(); ++it)
		{
			(*it)->setParent(NULL);
			(*it)->release();
		}

		m_aChildren.clear();
	}


	//! Removes this scene node from the scene, deleting it.
	virtual void remove()
	{
		if (m_pParent)
			m_pParent->removeChild(this);
	}


	const EzList<EzAVObjectPtr>& getChildren() const
	{
		return m_aChildren;
	}

	/*! 
	 * Should only be called by NiTriBasedGeom.  Adds a new SkinInstance to the specified mesh.  The bones must be below this node in the scene graph tree
	 */
	void addSkin( EzSkinInstance* skin_inst )
	{
		if (skin_inst)
		{
			m_Skins.push_back(skin_inst);
		}

	}

	/*! 
	 * Should only be called by NiTriBasedGeom.  Detaches the skin associated with a child mesh.
	 */
	bool removeSkin( EzSkinInstance* skin_inst )
	{
		EzList<EzSkinInstancePtr>::Iterator it = m_Skins.begin();
		//for (; it != Skins.end(); ++it)
		//	if ((*it) == skin_inst)
		//	{
		//		(*it)->setParent(NULL);
		//		(*it)->drop();
		//		Skins.erase(it);
		//		return true;
		//	}
		return false;

	}

	void attachEffect(EzDynamicEffect* pEffect);
	void detachEffect(EzDynamicEffect* pEffect);
	void detachAllEffects();

	
protected:
	EzSkinsList						m_Skins;
	EzNodeChildList					m_aChildren;
	EzDynamicEffectList				m_aEffectList;
};

