#pragma once

#include "../EzCore/EzRefObject.h"
#include "../EzCore/EzSmartPointer.h"
#include "EzProperty.h"


class EzMaterialProperty;
class EzTexturingProperty;



class EzPropertyState : public EzRefObject
{
	EzDeclareRootRTTI(EzPropertyState);

public:
	EzPropertyState(void);
	~EzPropertyState(void);

    EzMaterialProperty* getMaterial() const;
    EzTexturingProperty* getTexturing() const;

    EzPropertyState& operator=(const EzPropertyState& State);
    EzPropertyPtr& operator[](unsigned int iType);

	void setProperty(EzProperty* pProp);
	void removeProperty(EzProperty* pProp);

protected:
    EzPropertyPtr	m_aspProps[EzProperty::MAX_TYPES];
};

EzSmartPointer(EzPropertyState);

inline void EzPropertyState::removeProperty(EzProperty* pProp)
{
	m_aspProps[pProp->type()] = NULL;
}


inline void EzPropertyState::setProperty(EzProperty* pkProp)
{
    if(!pkProp)
        return;

    m_aspProps[pkProp->type()] = pkProp;
}

inline EzPropertyPtr &EzPropertyState::operator[](unsigned int iType)
{
    return m_aspProps[iType];
}


// used to simplify the code below
#define EzPropStateGetProp(e) (EzProperty*)m_aspProps[e]

inline EzMaterialProperty* EzPropertyState::getMaterial() const
{
	return (EzMaterialProperty*) EzPropStateGetProp(EzProperty::MATERIAL);

}

inline EzTexturingProperty* EzPropertyState::getTexturing() const
{
	return (EzTexturingProperty*) EzPropStateGetProp(EzProperty::TEXTURE);
}