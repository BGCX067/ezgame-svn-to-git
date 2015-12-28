#pragma once

#include "../EzCore/EzArray.h"
#include "../EzCore/EzRefObject.h"
#include "../EzAnimation3D/EzAnimationSequence.h"
#include "../EzAnimation3D/EzAnimatorManager.h"
#include "../EzMain3D/EzNode.h"

class EzActorManager : public EzRefObject
{
public:
	typedef EzMap<u32, EzAnimationSequence*>     EzAnimatorSequenceMap;

	EzActorManager(void);
	~EzActorManager(void);

	enum ETRANSITION_STATE
	{
		NO_TRANSITION,
		BLENDING,
		MORPHING,
		CROSSFADING
	};


	void setActorNode(EzNode* pActorNode);

	bool loadSequence(u32 uiSequenceID, const c8* filename);
	EzAnimationSequence* getSequence(u32 uiSequenceID);

	bool activate(u32 uiSequenceID, s32 iPriority = 0,
		bool bStartOver = false, float fWeight = 1.0f,
		f32 fEaseInTime = 0.0f,
		u32 uiTimeSyncSeqID = NULL);

	bool deactivate(u32 uiSequenceID, f32 fEaseOutTime = 0.0f);


protected:

	EzAnimatorManagerPtr				m_spAnimatorManager;
	EzNodePtr							m_ActorNodetPtr;
	EzAnimatorSequenceMap				m_mapSequences;

	f32 m_fTime;




};
