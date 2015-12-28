#pragma once

#include "../EzCore/EzRefObject.h"
#include "../EzCore/EzSmartPointer.h"

EzSmartPointer(EzDynamicEffectState);

class EzDynamicEffect;
class EzLight;


typedef void* EzDynEffectStateIter;

class EzDynamicEffectState : public EzRefObject
{
public:
	EzDynamicEffectState(void);
	~EzDynamicEffectState(void);

	void addEffect(EzDynamicEffect* pEffect);
	void removeEffect(EzDynamicEffect* pEffect);
	EzDynamicEffectState* copy() const;


	inline EzLight* getNextLight(EzDynEffectStateIter& iter) const
	{
		if(!iter)
			return NULL;

		EzDynamicEffect* pItem = ((ListItem*)iter)->m_pItem;

		iter = (EzDynEffectStateIter)(((ListItem*)iter)->m_pNext);

		return (EzLight*)pItem;
	}

	inline EzDynEffectStateIter getLightHeadPos() const
	{
		return (EzDynEffectStateIter)m_pLightList;
	}




protected:
	class ListItem 
	{
	public:
		ListItem*				m_pNext;
		EzDynamicEffect*	m_pItem;
	};


	static void insertListSorted(ListItem* &pL,EzDynamicEffect* pItem);
	static void removeList(ListItem* &pL, EzDynamicEffect* pItem);

	static inline void deleteList(ListItem* &pL)
	{
		while(pL)
		{
			ListItem* pKill = pL;
			pL = pL->m_pNext;
			delete  pKill;
		}
	}

	static inline void copyList(const ListItem* pSrc, ListItem* &pDest)
	{
		if(pSrc)
		{
			pDest = new ListItem;
			pDest->m_pItem = pSrc->m_pItem;

			const ListItem* pSrcItem = pSrc;
			ListItem* pDestItem = pDest;
			while(pSrcItem->m_pNext)
			{
				pDestItem->m_pNext = new ListItem;

				pDestItem = pDestItem->m_pNext;
				pSrcItem = pSrcItem->m_pNext;

				pDestItem->m_pItem = pSrcItem->m_pItem;
			}

			pDestItem->m_pNext = NULL;
		}

	}


    
	ListItem*	m_pLightList;


};

