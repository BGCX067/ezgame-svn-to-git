#pragma once

#include "../core/EzArray.h"
#include "../core/EzRefObject.h"
#include "../EzAnimation3D/EzControllerSequence.h"
#include "../EzAnimation3D/EzControllerManager.h"
#include "EzNode.h"

class EzActorManager : public EzRefObject
{
public:
	EzActorManager(void);
	~EzActorManager(void);


	void setActorNode(EzNode* pActorNode);

	bool loadSequence(const char* name, const char* filename);


	void update(float fTime) { m_pControllerManager->update(fTime); }


protected:
	EzControllerManagerPtr				m_pControllerManager;
	EzNodePtr							m_ActorNodetPtr;

};

