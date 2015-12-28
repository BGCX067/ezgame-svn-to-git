#pragma once

#include "../EzCore/EzCore.h"
#include "EzObject.h"
#include "EzAnimator.h"
#include "EzExtraData.h"

EzSmartPointer(EzObjectNET);

class EzInterpAnimator;


class EzObjectNET : public EzObject
{
	EzDeclareAbstractStream;
	EzDeclareRTTI;
	EzDeclareCopyMember(EzObjectNET);

public:
	void setName(const char* name) { m_Name = name; }
	const char* getName() { return m_Name.c_str(); }

	void addExtraData( EzExtraData * obj);
	void removeExtraData( EzExtraData * obj );
	void clearExtraData();
	unsigned int getExtraDataSize();
	const EzArray<EzExtraData*>& getExtraData() const;

	void addAnimator( EzAnimator* pAnimator );
	void removeAnimator( EzAnimator* pAnimator );
	void clearAnimators();

	EzAnimator* getAnimator();
	EzAnimator* findAnimator(const EzRTTI* pkRTTI) const;
	
	bool isAnimated();

	virtual bool isEqual(EzObject* pObject);

protected:
	EzObjectNET(void);
	virtual ~EzObjectNET(void);

	typedef EzArray<EzExtraData*>	EzExtraDataArray;

	EzExtraDataArray				m_aExtraDataList;
	EzAnimatorPtr					m_spAnimator;

private:
	EzStringc						m_Name;
};

inline unsigned int EzObjectNET::getExtraDataSize() 
{ 
	return m_aExtraDataList.size(); 
}

inline EzAnimator* EzObjectNET::findAnimator(const EzRTTI* pkRTTI) const
{
	EzAnimator* pAnimator;

	for (pAnimator = m_spAnimator; pAnimator != NULL; pAnimator = pAnimator->getNextAnimator())
	{
		if (pAnimator->isExactKindOf(pkRTTI))
		{
			return pAnimator;
		}
	}

	return NULL;
}

#define EZFindAnimatorController(classname, pkObjectNET)	((classname*) pkObjectNET->findAnimator(&classname::ms_RTTI))


