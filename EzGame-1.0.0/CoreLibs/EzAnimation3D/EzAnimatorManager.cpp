#include "EzAnimatorManager.h"
#include "../EzMain3D/EzAVObject.h"


EzImplementRTTI(EzAnimatorManager, EzAnimator);

EzAnimatorManager::EzAnimatorManager(EzAVObject* pTarget)
{
	m_spObjectPalette = EzNew EzAVObjectPalette(pTarget);
}

EzAnimatorManager::~EzAnimatorManager(void)
{
}

u32 EzAnimatorManager::getControllerSequenceCount()
{
	return m_aControllerSequences.size();

}

void EzAnimatorManager::addAnimationSequence(EzAnimationSequence* pSequence)
{
	pSequence->setOwner(this);

	pSequence->prepareTarget((EzNode*) m_pTarget);

	m_aControllerSequences.push_back(pSequence);
}



bool EzAnimatorManager::deactivateSequence(EzAnimationSequence* pSequence, f32 fEaseOutTime)
{

	return pSequence->deactivate(fEaseOutTime, false);
}



bool EzAnimatorManager::activateSequence(EzAnimationSequence* pSequence, s32 iPriority, bool bStartOver, 
		f32 fWeight, f32 fEaseInTime, EzAnimationSequence* TimeSyncSeq)
{

	return pSequence->activate(iPriority, bStartOver, fWeight, fEaseInTime, TimeSyncSeq, true);
}

void EzAnimatorManager::update(f32 fTime)
{

    for (u32 ui = 0; ui < m_aControllerSequences.size(); ++ui)
    {
		EzAnimationSequence* Sequence = m_aControllerSequences[ui];

		if (Sequence)
        {
            Sequence->update(fTime);
        }
    }
}


