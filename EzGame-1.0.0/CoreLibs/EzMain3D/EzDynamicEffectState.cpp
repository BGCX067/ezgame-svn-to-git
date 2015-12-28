#include "EzDynamicEffectState.h"
#include "EzDynamicEffect.h"
#include "EzLight.h"


EzDynamicEffectState::EzDynamicEffectState(void)
: m_pLightList(0)
{
}

EzDynamicEffectState::~EzDynamicEffectState(void)
{
	// remove all items
	deleteList(m_pLightList);

}

EzDynamicEffectState* EzDynamicEffectState::copy() const
{
	EzDynamicEffectState* pNew = EzNew EzDynamicEffectState;

	copyList(m_pLightList, pNew->m_pLightList);


	return pNew;
}

void EzDynamicEffectState::addEffect(EzDynamicEffect* pEffect)
{
	if(!pEffect)
		return;

	if(pEffect->getEffectType() <= EzDynamicEffect::MAX_LIGHT_TYPE)
		insertListSorted(m_pLightList, pEffect);

}
void EzDynamicEffectState::removeEffect(EzDynamicEffect* pEffect)
{
	if(!pEffect)
		return;

	if(pEffect->getEffectType() <= EzDynamicEffect::MAX_LIGHT_TYPE)
		removeList(m_pLightList, pEffect);

}

void EzDynamicEffectState::insertListSorted(ListItem* &pL, EzDynamicEffect* pItem)
{
	unsigned int uiIndex = pItem->getIndex();

	if(!pL || (uiIndex < pL->m_pItem->getIndex()))
	{
		ListItem* pNew = new ListItem;
		pNew->m_pItem = pItem;
		pNew->m_pNext = pL;
		pL = pNew;
	}
	else if(uiIndex == pL->m_pItem->getIndex())
		return;
	else
	{
		ListItem* pIter = pL;
		ListItem* pNext = pL->m_pNext;

		EZASSERT(pL->m_pItem && (uiIndex >= pL->m_pItem->getIndex()));

		while(pNext && (uiIndex > pNext->m_pItem->getIndex()))
		{
			pIter = pNext;
			pNext = pIter->m_pNext;
		}

		if(pNext && (uiIndex == pNext->m_pItem->getIndex()))
			return;

		ListItem* pNew = new ListItem;
		pNew->m_pItem = pItem;
		pNew->m_pNext = pNext;
		pIter->m_pNext = pNew;
	}
}
//---------------------------------------------------------------------------
void EzDynamicEffectState::removeList(ListItem* &pL, EzDynamicEffect* pItem)
{
	unsigned int uiIndex = pItem->getIndex();

	if(!pL || (uiIndex < pL->m_pItem->getIndex()))
		return;
	else if(uiIndex == pL->m_pItem->getIndex())
	{
		ListItem* pKill = pL;
		pL = pL->m_pNext;
		delete  pKill;
	}
	else
	{
		ListItem* pIter = pL;
		ListItem* pNext = pL->m_pNext;

		EZASSERT(pL->m_pItem && (uiIndex >= pL->m_pItem->getIndex()));

		// While the current node has a lower or equal index item
		while(pNext && (uiIndex > pNext->m_pItem->getIndex()))
		{
			pIter = pNext;
			pNext = pIter->m_pNext;
		}

		// item is in EzList - return
		if(pNext && (uiIndex == pNext->m_pItem->getIndex()))
		{
			pIter->m_pNext = pNext->m_pNext;
			delete  pNext;
		}
	}
}
