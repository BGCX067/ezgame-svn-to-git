#include "EzDynamicEffect.h"
#include "EzNode.h"


EzImplementRTTI(EzDynamicEffect, EzAVObject);


unsigned int EzDynamicEffect::ms_uiNextIndex = 1;

EzDynamicEffect::EzDynamicEffect(void)
{
    m_iIndex = ms_uiNextIndex++;
}

EzDynamicEffect::~EzDynamicEffect(void)
{
}



void EzDynamicEffect::attachAffectedNode(EzNode* pAffectedNode)
{

	EzList<EzNode*>::Iterator   it;

	it = m_aAffectedNodeList.find(pAffectedNode);

	if(it != m_aAffectedNodeList.end())
		return;


	m_aAffectedNodeList.push_front(pAffectedNode);
}

void EzDynamicEffect::detachAffectedNode(EzNode* pAffectedNode)
{

	EzList<EzNode*>::Iterator   it;

	it = m_aAffectedNodeList.find(pAffectedNode);

	if(it == m_aAffectedNodeList.end())
		return;


	m_aAffectedNodeList.erase(it);

	pAffectedNode->detachEffect(this);
}

bool EzDynamicEffect::isUnaffectedNode(EzNode* pPossibleNode)
{
	EzList<EzNode*>::Iterator  it;

	it = m_aUnaffectedNodeList.find(pPossibleNode);

	if(it != m_aUnaffectedNodeList.end())
	{
		return true;
	}

	return false;
}

void EzDynamicEffect::attachUnaffectedNode(EzNode* pUnaffectedNode)
{

	EzList<EzNode*>::Iterator  it;

	it = m_aUnaffectedNodeList.find(pUnaffectedNode);

	if(it != m_aUnaffectedNodeList.end())
		return;

	m_aUnaffectedNodeList.push_front(pUnaffectedNode);
}

void EzDynamicEffect::detachUnaffectedNode(EzNode* pUnaffectedNode)
{
	EzList<EzNode*>::Iterator  it;

	it = m_aUnaffectedNodeList.find(pUnaffectedNode);

	if(it == m_aUnaffectedNodeList.end())
		return;

	m_aUnaffectedNodeList.erase(it);
}
