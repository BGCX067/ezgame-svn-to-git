#include "EzActorManager.h"
#include "../EzMain3D/EzStream.h"
#include "../EzAnimation3D/EzInterpAnimator.h"
#include "../EzApplication/EzDevice.h"
#include "../EzApplication/EzLogReceiver.h"


EzActorManager::EzActorManager(void)
: m_ActorNodetPtr(0), m_fTime(-FLT_MAX)
{
	
}

EzActorManager::~EzActorManager(void)
{
}

void EzActorManager::setActorNode(EzNode* pActorNode)
{
	m_ActorNodetPtr = pActorNode;
}

bool EzActorManager::loadSequence(u32 uiSequenceID, const c8* filename)
{

	//EZASSERT(m_ActorNodetPtr);
	//EzStream stream;
	//
	//if(m_mapSequences.find(uiSequenceID))
	//{
	//	log(EZ_ERROR_ALREADY_HAVE_SEQUENCE_NUMBER,"There is already %d Sequence Number in the ActorManager.", uiSequenceID);
	//	return false;
	//}


	//EzAnimationSequence* pSequence = stream.loadKF(filename, m_ActorNodetPtr);

	//if(pSequence)
	//{
	//	EzObjectNET* pBoneRoot = EzDynamicCast(EzObjectNET, m_ActorNodetPtr->getObjectByName(pSequence->getTargetName()));
	//	EzAnimatorManager* pAnimatorManager;

	//	if(!pBoneRoot)
	//	{
	//		EzDelete pSequence;
	//		log(EZ_WARN_NOT_FIND_BONE_NODE,"There is not %s Bone in node.", pSequence->getTargetName());
	//		return false;
	//	}

	//	pAnimatorManager = EZFindAnimatorController(EzAnimatorManager, pBoneRoot);

	//	if(!pAnimatorManager)
	//	{
	//		pAnimatorManager = EzNew EzAnimatorManager(EzDynamicCast(EzAVObject, pBoneRoot));

	//		pAnimatorManager->setTarget(pBoneRoot);
	//		pBoneRoot->addAnimator(pAnimatorManager);
	//	}

	//	pAnimatorManager->addAnimationSequence(pSequence);

	//	m_mapSequences.insert(uiSequenceID, pSequence);

	//	return true;
	//}

	return false;
}


EzAnimationSequence* EzActorManager::getSequence(u32 uiSequenceID)
{
	EzMap<u32, EzAnimationSequence*>::Node* findSequence;

	findSequence = m_mapSequences.find(uiSequenceID);

	if(findSequence)
	{
		return (EzAnimationSequence*) findSequence->getValue();
	}

	return NULL;

}

bool EzActorManager::deactivate(u32 uiSequenceID, f32 fEaseOutTime)
{
	EzAnimationSequence* pkSequence = getSequence(uiSequenceID);
	if (!pkSequence)
	{
		return false;
	}

	if (pkSequence->getState() == EzAnimationSequence::EASEOUT ||
		pkSequence->getState() == EzAnimationSequence::EASEIN)
	{
		return false;
	}

	// Update the sequence without updating interpolators to give it a chance
	// to finish easing in, if necessary. This will prevent failure to
	// deactivate due to the fact that the sequence will not actually be fully
	// active until the next Update.
	if (m_fTime != -FLT_MAX)
	{
		pkSequence->update(m_fTime, false);
	}

	bool bSuccess = m_spAnimatorManager->deactivateSequence(pkSequence, fEaseOutTime);
		
	if (bSuccess)
	{
		//m_bBuildTimeline = true;

		//// Update the sequence here to deactivate it immediately. This is
		//// necessary in order to catch events because the timeline is built
		//// before the sequence will be next updated.
		//if (m_fTime != INVALID_TIME)
		//{
		//	pkSequence->Update(m_fTime, false);
		//}

		//if (fEaseOutTime == 0.0f)
		//{
		//	// If the ease-out time is 0.0, we can immediately remove the
		//	// sequence from the extra sequences set.
		//	int iIndex = m_kExtraSequences.Find(eSequenceID);
		//	NIASSERT(iIndex != -1);
		//	if (iIndex != -1)
		//	{
		//		m_kExtraSequences.RemoveAt(iIndex);
		//	}

		//	// If the ease-out time is 0.0, we need to immediately raise any
		//	// AnimDeactivated events for the sequence.
		//	RaiseAnimDeactivatedEvents(eSequenceID);
		//}
	}

	return bSuccess;

}

bool EzActorManager::activate(u32 uiSequenceID, s32 iPriority, bool bStartOver, float fWeight, float fEaseInTime, u32 uiTimeSyncSeqID)
{
	EzAnimationSequence* pkSequence = getSequence(uiSequenceID);
	if (!pkSequence)
	{
		return false;
	}

	EzAnimationSequence* pkTimeSyncSeq = NULL;
	//if (eTimeSyncSeqID != NiKFMTool::SYNC_SEQUENCE_ID_NONE)
	//{
	//	pkTimeSyncSeq = GetSequence(eTimeSyncSeqID);
	//	if (!pkTimeSyncSeq)
	//	{
	//		return false;
	//	}
	//}

	// Update the sequence without updating interpolators to give it a chance
	// to finish easing out, if necessary. This will prevent failure to
	// activate due to the fact that the sequence will not actually be fully
	// inactive until the next Update.
	if (m_fTime != -FLT_MAX)
	{
		pkSequence->update(m_fTime, false);
	}

	bool bSuccess = m_spAnimatorManager->activateSequence(pkSequence, iPriority, 
		bStartOver, fWeight, fEaseInTime, pkTimeSyncSeq);
	if (bSuccess)
	{
		//m_bBuildTimeline = true;

		// Update the sequence here to activate it immediately. This is
		// necessary in order to catch events because the timeline is built
		// before the sequence will be next updated.
		if (m_fTime != -FLT_MAX)
		{
			pkSequence->update(m_fTime, false);
		}

		//// We need to use AddUnique here instead of Add because it is possible
		//// that the sequence ID already exists in the set (if the sequence
		//// was deactivated but not yet removed from the set).
		//m_kExtraSequences.AddUnique(eSequenceID);

		if (fEaseInTime == 0.0f)
		{
			//// If the ease-in time is 0.0, we need to immediately raise any
			//// AnimActivated events for the sequence.
			//RaiseAnimActivatedEvents(eSequenceID);
		}
	}

	return bSuccess;
}
