#pragma once

#include "EzAVObject.h"

EzSmartPointer(EzAVObjectPalette);

class EzAVObjectPalette : public EzRefObject
{
public:
	EzAVObjectPalette(EzAVObject* pRoot = NULL);
	~EzAVObjectPalette(void);

	void setAVObject(const char* pcName, EzAVObject* pObj);
	EzAVObject* getRoot();
	void setRoot(EzAVObject* pRoot);
	EzAVObject* getAVObject(const char* pName);


protected:
	typedef EzMap<EzStringc, EzAVObject*>				EzAVObjectMap;

	EzAVObjectMap		m_Hash;

	EzAVObject*			m_pRoot;
};


inline void EzAVObjectPalette::setAVObject(const char* Name, EzAVObject* pObj)
{

	EzAVObjectMap::Node* pFindNode;

	pFindNode = m_Hash.find(Name);

	if (pObj == NULL)
	{
		m_Hash.remove(EzStringc(Name));
	}
	else
	{
		m_Hash.insert(EzStringc(Name), pObj);
	}
}

inline void EzAVObjectPalette::setRoot(EzAVObject* pRoot)
{
	m_pRoot = pRoot;
}

inline EzAVObject* EzAVObjectPalette::getRoot()
{
	return m_pRoot;
}


inline EzAVObject* EzAVObjectPalette::getAVObject(const char* pName)
{
	EzAVObjectMap::Node* pFindNode;

	pFindNode = m_Hash.find(pName);

	if(pFindNode)
	{
		return pFindNode->getValue();
	}
	else if (m_pRoot)
	{
		EzAVObject*  pObject; 
		pObject = m_pRoot->getObjectByName(pName);
		if (pObject)
		{
			setAVObject(pName, pObject);
		}

		return pObject;
	}

	return NULL;
}
