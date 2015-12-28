#pragma once

#include "../EzCore/EzCore.h"
#include "../EzMain3D/EzAnimator.h"
#include "EzAnimationSequence.h"
#include "../Ezmain3D/EzAVObjectPalette.h"



EzSmartPointer(EzAnimatorManager);

class EzAnimatorManager : public EzAnimator
{
	EzDeclareRTTI;

public:
	EzAnimatorManager(EzAVObject* pTarget);
	~EzAnimatorManager(void);

	u32 getControllerSequenceCount();
	void addAnimationSequence(EzAnimationSequence* pControllerSequence);

	bool activateSequence(EzAnimationSequence* pSequence, s32 iPriority = 0, bool bStartOver = false, 
		f32 fWeight = 1.0f, f32 fEaseInTime = 0.0f, EzAnimationSequence* TimeSyncSeq = NULL);

	bool deactivateSequence(EzAnimationSequence* pSequence, f32 fEaseOutTime);


	void update(f32 fTime);

	EzAVObjectPalette* getObjectPalette() const;



protected:
	EzArray<EzAnimationSequence*> m_aControllerSequences;

	EzAVObjectPalettePtr m_spObjectPalette;
};


inline EzAVObjectPalette* EzAnimatorManager::getObjectPalette() const
{
	return m_spObjectPalette;

}

