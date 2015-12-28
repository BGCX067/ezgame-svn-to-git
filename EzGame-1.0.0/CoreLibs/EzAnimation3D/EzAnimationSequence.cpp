#include "EzAnimationSequence.h"
#include "../EzMain3D/EzObjectNET.h"
#include "../EzMain3D/EzStringPalette.h"
#include "../EzMain3D/EzNode.h"
#include "EzInterpolator.h"
#include "../EzApplication/EzLogReceiver.h"
#include "EzAnimatorManager.h"
#include "EzMuiltyTransformAnimator.h"
#include "EzTransformAnimator.h"

EzAnimationSequence::EzAnimationSequence(void)
: m_eState(INACTIVE), m_fLastTime(-FLT_MAX), m_fWeightedLastTime(-FLT_MAX), m_fWeight(-FLT_MAX),
m_fFrequency(-FLT_MAX), m_fStartKeyTime(-FLT_MAX), m_fStopKeyTime(-FLT_MAX), m_fOffset(-FLT_MAX),
m_fStartTime(-FLT_MAX), m_fEndTime(-FLT_MAX), m_fDestFrame(-FLT_MAX)
{
}

EzAnimationSequence::~EzAnimationSequence(void)
{
	for(u32 ui = 0; ui < m_aInterp.size(); ++ui)
	{
		m_aInterp[ui].clear();

	}

	m_aExtraData.clear();
}

void EzAnimationSequence::setOwner(EzAnimatorManager* pCtrlMgr)
{
	m_pOwner = pCtrlMgr;
}


bool EzAnimationSequence::activate(u8 usPriority, bool bStartOver, f32 fWeight, f32 fEaseInTime, EzAnimationSequence* kTimeSyncSeq, bool bTransition)
{

    if (m_eState != INACTIVE)
    {
		log(EZ_WARN_ALREADY_ANIMATING_SEQUENCE, "This sequence is already animating!\n");
        return false;
    }

	// Attach the interpolators to their blend interpolators.
	attachInterpolators(usPriority);

	// Set parameters.
	m_fWeight = fWeight;

	if (fEaseInTime > 0.0f)
	{
		if (bTransition)
		{
			m_eState = TRANSDEST;
		}
		else
		{
			m_eState = EASEIN;
		}
		m_fStartTime = -FLT_MAX;
		m_fEndTime = fEaseInTime;
	}
	else
	{
		m_eState = ANIMATING;
	}

	if (bStartOver)
	{
		//ResetSequence();
	}

	m_fLastTime = -FLT_MAX;

	return true;
}


bool EzAnimationSequence::deactivate(f32 fEaseOutTime, bool bTransition)
{
	if (m_eState == INACTIVE)
	{
		return false;
	}

	if (fEaseOutTime > 0.0f)
	{
		if (bTransition)
		{
			m_eState = TRANSSOURCE;
		}
		else
		{
			m_eState = EASEOUT;
		}
		m_fStartTime = -FLT_MAX;
		m_fEndTime = fEaseOutTime;
	}
	else
	{
		// Store the new offset.
		if (m_fLastTime != -FLT_MAX)
		{
			m_fOffset += (m_fWeightedLastTime / m_fFrequency) - m_fLastTime;
		}

		m_eState = INACTIVE;

		//m_pkPartnerSequence = NULL;
		m_fDestFrame = -FLT_MAX;

		detachInterpolators();
	}

	//unsigned int uiCount = m_kActivationCallbacks.GetSize();
	//for (unsigned int ui = 0; ui < uiCount; ui++)
	//	m_kActivationCallbacks.GetAt(ui)->ActivationChanged(this, m_eState);

	return true;


}


void EzAnimationSequence::attachInterpolators(c8 cPriority)
{
	for (unsigned int ui = 0; ui < m_aInterp.size(); ui++)
	{
		InterpolateItem& Item = m_aInterp[ui];
		if (Item.m_spInterpolator != NULL)
		{
			if (Item.m_pBlendInterp != NULL)
			{
				Item.m_ucBlendIdx = Item.m_pBlendInterp->addInterpInfo(Item.m_spInterpolator, 0.0f, cPriority);
				
				EZASSERT(Item.m_ucBlendIdx != UCHAR_MAX);
			}
		}
	}
}

void EzAnimationSequence::detachInterpolators()
{
	for (unsigned int ui = 0; ui <  m_aInterp.size(); ui++)
	{
		InterpolateItem& Item = m_aInterp[ui];
		if (Item.m_pBlendInterp)
		{
			Item.m_pBlendInterp->removeInterpInfo(Item.m_ucBlendIdx);
		}
	}
}

void EzAnimationSequence::addInterpolator(EzInterpolator* pInterpolator, const ExtraData& data)
{
	InterpolateItem InterpItem;

	InterpItem.m_spInterpolator = pInterpolator;
	
	m_aInterp.push_back(InterpItem);
	m_aExtraData.push_back(data);
}

bool EzAnimationSequence::prepareTarget(EzNode* pTarget)
{
	bool bAddedCtrl;

	EzMuiltyTransformAnimator* pMuiltyTransformAnimator;

	EZASSERT(pTarget);
	EzAVObjectPalette* pObjectPalette = m_pOwner->getObjectPalette();
	EZASSERT(pObjectPalette);

	pMuiltyTransformAnimator = resolveTransformInterpolators((EzNode*) pTarget, bAddedCtrl);

	if(!pMuiltyTransformAnimator)
	{
		log(0, "Do....");
		return false;
	}

	for(u32 ui = 0 ; ui < m_aInterp.size(); ui++)
	{
		if(m_aInterp[ui].m_spInterpAnimator)
		{
			continue;
		}

		// Continue if we've already resolved this blend interpolator.
        if (m_aInterp[ui].m_pBlendInterp)
        {
            continue;
        }

		// Retrieve controller target.
		EzAVObject* pTarget = pObjectPalette->getAVObject(m_aExtraData[ui].m_TargetName.c_str());

		if(!pTarget)
		{
			log(EZ_WARN_NOT_FIND_BONE_NODE, "failed to find target with the following identifiers : %s", m_aExtraData[ui].m_TargetName);
			continue;
		}


		// Retrieve controller.

		u16 usIndex = pMuiltyTransformAnimator->getInterpolatorTargetIndex(pTarget);

		if (usIndex == USHRT_MAX)
		{
			usIndex = pMuiltyTransformAnimator->addInterpolatorTarget(pTarget);

			if(usIndex == USHRT_MAX)
			{
				continue;
			}
		}
		else
		{
			pMuiltyTransformAnimator->setInterpolatorTarget(usIndex, pTarget);
		}

		InterpolateItem* pItem = &m_aInterp[ui];


		m_aInterp[ui].m_spInterpAnimator = pMuiltyTransformAnimator;
		m_aInterp[ui].m_pBlendInterp = pMuiltyTransformAnimator->getBlendInterpolator(usIndex);
		m_aInterp[ui].m_pBlendInterp->setManagerControlled(true);
	}

	return true;


}

EzMuiltyTransformAnimator* EzAnimationSequence::resolveTransformInterpolators(EzNode* pRoot, bool& bAddCtrl)
{
	EzMuiltyTransformAnimator* pBlendCtlr = EZFindAnimatorController(EzMuiltyTransformAnimator, pRoot);
	
	if (pBlendCtlr == NULL)
	{
		u16 usTransformInterpCount = 0;

		for (unsigned int ui = 0; ui < m_aInterp.size(); ui++)
		{
			InterpolateItem* pItem = &m_aInterp[ui];

			if(m_aExtraData[ui].m_AnimatorType == "EzTransformAnimator")
			{
				usTransformInterpCount++;
			}
		}

		if (usTransformInterpCount == 0)
		{
			return NULL;
		}
		
		pBlendCtlr = EzNew EzMuiltyTransformAnimator(usTransformInterpCount);
		pBlendCtlr->setTarget(pRoot);
		pBlendCtlr->setManagerControlled(true);

		if (pRoot == m_pOwner->getTarget())
		{
			pBlendCtlr->setNextAnimator(m_pOwner->getNextAnimator());
			m_pOwner->setNextAnimator(pBlendCtlr);
		}
	}

	for (unsigned int ui = 0; ui < m_aInterp.size(); ui++)
	{
		InterpolateItem* pItem = &m_aInterp[ui];

		if(m_aExtraData[ui].m_AnimatorType == "EzTransformAnimator")
		{

			EzAVObject* pAVObject = pRoot->getObjectByName(m_aExtraData[ui].m_TargetName.c_str());

			EZASSERT(pAVObject);

			EzTransformAnimator* pTransCtlr = EZFindAnimatorController(EzTransformAnimator, pAVObject);

			if(!pTransCtlr)
			{
				continue;
			}

			u16 usIndex = pBlendCtlr->getInterpolatorTargetIndex(pAVObject);

			if (usIndex == USHRT_MAX)
			{
				usIndex = pBlendCtlr->addInterpolatorTarget(pAVObject);

				if(usIndex == USHRT_MAX)
				{
					continue;
				}
			}
			else
			{
				pBlendCtlr->setInterpolatorTarget(usIndex, pAVObject);
			}

			pAVObject->removeAnimator(pTransCtlr);

			pItem->m_spInterpAnimator = pBlendCtlr;
			pItem->m_pBlendInterp = pBlendCtlr->getBlendInterpolator(usIndex);
			pItem->m_pBlendInterp->setManagerControlled(true);
		}
	}

	return pBlendCtlr;

}

EzAnimationSequence::EANIMSTATE EzAnimationSequence::getState()
{
	return m_eState;
}



f32 EzAnimationSequence::getFrequency() const
{
	return m_fFrequency;

}

void EzAnimationSequence::setFrequency( f32 value )
{
	m_fFrequency = value;

}
f32 EzAnimationSequence::getStartTime() const
{
	return m_fStartKeyTime;

}
void EzAnimationSequence::setStartTime( f32 value )
{
	m_fStartKeyTime = value;

}

f32 EzAnimationSequence::getStopTime() const
{
	return m_fStopKeyTime;

}
void EzAnimationSequence::setStopTime( f32 value )
{
	m_fStopKeyTime = value;
	 

}
EzAnimator::ECYCLE_TYPE EzAnimationSequence::getCycleType() const
{
	return m_eAniCycleType;

}


void EzAnimationSequence::setCycleType( EzAnimator::ECYCLE_TYPE n )
{
	m_eAniCycleType = n;

}



u32 EzAnimationSequence::getControllerPriority( u32 controller ) const
{
	return 0;
}

void EzAnimationSequence::setControllerPriority( u32 controller, u32 priority )
{

}

f32 EzAnimationSequence::getWeight() const
{
	return m_fWeight;

}
void EzAnimationSequence::setWeight( f32 value )
{
	m_fWeight = value;
}

const c8* EzAnimationSequence::getTargetName() const
{
	return TargetName.c_str();

}
void EzAnimationSequence::setTargetName( const c8* value )
{
	TargetName = value;

}

void EzAnimationSequence::update(f32 fTime, bool bUpdateInterpolators)
{

	if (m_eState == INACTIVE)
	{
		return;
	}

	if (m_fOffset == -FLT_MAX)
	{
		m_fOffset = -fTime;
	}

	if (m_fStartTime == -FLT_MAX)
	{
		m_fStartTime = fTime;
		m_fEndTime = fTime + m_fEndTime;
	}

	f32 fEaseSpinner = 1.0f;
	f32 fTransSpinner = 1.0f;
	switch (m_eState)
	{
	case EASEIN:
		if (fTime < m_fEndTime)
		{
			EZASSERT(fTime >= m_fStartTime && m_fEndTime != m_fStartTime);
			fEaseSpinner = (fTime - m_fStartTime) / (m_fEndTime -
				m_fStartTime);
		}
		else
		{
			m_eState = ANIMATING;
		}
		break;
	case TRANSDEST:
		if (fTime < m_fEndTime)
		{
			EZASSERT(fTime >= m_fStartTime && m_fEndTime != m_fStartTime);
			fTransSpinner = (fTime - m_fStartTime) / (m_fEndTime -
				m_fStartTime);
		}
		else
		{
			if (m_fDestFrame != -FLT_MAX)
			{
				// This case is hit when we were blending in this
				// sequence. In this case, we need to reset the sequence
				// offset and clear the destination frame.
				m_fOffset = -fTime + m_fDestFrame;
				m_fDestFrame = -FLT_MAX;
			}
			m_eState = ANIMATING;
		}
		break;
	case EASEOUT:
		if (fTime < m_fEndTime)
		{
			EZASSERT(fTime >= m_fStartTime && m_fEndTime != m_fStartTime);
			fEaseSpinner = (m_fEndTime - fTime) / (m_fEndTime -
				m_fStartTime);
		}
		else
		{
			deactivate(0.0f, false);
			return;
		}
		break;
	//case MORPHSOURCE:
	//	{
	//		NIASSERT(m_pkPartnerSequence);

	//		// Compute initial offset for partner sequence, undoing phase
	//		// and frequency adjustments. This assumes the phase and
	//		// frequency will not change between now and the end time of
	//		// the morph.
	//		float fStartFrame = m_pkPartnerSequence
	//			->FindCorrespondingMorphFrame(this, m_fOffset + fTime);
	//		fStartFrame /= m_pkPartnerSequence->m_fFrequency;
	//		m_pkPartnerSequence->m_fOffset = fStartFrame - fTime;

	//		// Change sequence state appropriately.
	//		m_eState = TRANSSOURCE;

	//		// This case statement intentionally does not break. The code
	//		// for the TRANSSOURCE case should be subsequently run.
	//	}
	case TRANSSOURCE:
		if (fTime < m_fEndTime)
		{
			EZASSERT(fTime >= m_fStartTime && m_fEndTime != m_fStartTime);
			fTransSpinner = (m_fEndTime - fTime) / (m_fEndTime -
				m_fStartTime);
		}
		else
		{
			deactivate(0.0f, true);
			return;
		}
		break;
	default:
		break;
	}


	if (bUpdateInterpolators)
	{
		float fUpdateTime;
		if (m_fDestFrame != -FLT_MAX)
		{
			fUpdateTime = m_fDestFrame;
		}
		//else if (m_pkPartnerSequence)
		//{
		//	if (m_pkPartnerSequence->GetLastTime() !=
		//		m_pkPartnerSequence->m_fOffset + fTime)
		//	{
		//		m_pkPartnerSequence->Update(fTime, false);
		//	}

		//	fUpdateTime = FindCorrespondingMorphFrame(m_pkPartnerSequence,
		//		m_pkPartnerSequence->m_fOffset + fTime);
		//	fUpdateTime /= m_fFrequency;
		//}
		else
		{
			fUpdateTime = m_fOffset + fTime;
		}

		setInterpsWeightAndTime(m_fWeight * fTransSpinner, fEaseSpinner, computeScaledTime(fUpdateTime));
	}
}


void EzAnimationSequence::setInterpsWeightAndTime(f32 fWeight, f32 fEaseSpinner, f32 fTime)
{
	EZASSERT(m_eState != INACTIVE);

	for (unsigned int ui = 0; ui < m_aInterp.size(); ui++)
	{
		InterpolateItem& Item = m_aInterp[ui];
		if (Item.m_pBlendInterp)
		{
			Item.m_pBlendInterp->setWeight(fWeight, Item.m_ucBlendIdx);
			Item.m_pBlendInterp->setEaseSpinner(fEaseSpinner, Item.m_ucBlendIdx);
			Item.m_pBlendInterp->setTime(fTime, Item.m_ucBlendIdx);
		}
	}
}

f32 EzAnimationSequence::computeScaledTime(f32 fTime)
{

    f32 fScaledTime = 0.0f;

    f32 fWeightedLastTime = m_fWeightedLastTime;
    f32 fDeltaTime;
    if (m_fLastTime == -FLT_MAX)
    {
        fWeightedLastTime = 0.0f;
        fDeltaTime = fTime;
    }
    else
    {
        // Calculate time since last update
        fDeltaTime = fTime - m_fLastTime;
    }


    // Scale time linearly by frequency value 
    // specified by the application.
    //
    // time' = freq * time
    //
    fWeightedLastTime += fDeltaTime * m_fFrequency;
    fScaledTime = fWeightedLastTime;

    // Switch on m_CycleType
    switch (m_eAniCycleType)
    {
	case EzAnimator::CYCLE_LOOP:
        {
            // Loop -- only need fractional time
            f32 fSpan = m_fStopKeyTime - m_fStartTime;
            if (fSpan != 0.0f)
            {
                if (fScaledTime != m_fStopKeyTime)
                {
                    fScaledTime = 
                        fmod(fScaledTime - m_fStartTime, fSpan)
                        + m_fStartTime;
                }

                if (fScaledTime < m_fStartTime)
                {
                    fScaledTime += fSpan;
                }
            }
            else
            {
                fScaledTime = m_fStartTime;
            }
            break;
        }
	case EzAnimator::CYCLE_CLAMP:
            // Clamp - do nothing, conditions below will handle clamping.
            break;
    default:
            // Only LOOP and CLAMP cycle types are supported by sequences.
            break;
    }

    if (fScaledTime > m_fStopKeyTime)
    {
        fScaledTime = m_fStopKeyTime;
    }
    else if (fScaledTime < m_fStartTime)
    {
        fScaledTime = m_fStartTime;
    }

    m_fWeightedLastTime = fWeightedLastTime;
    m_fLastTime = fTime;
    m_fLastScaledTime = fScaledTime;

	return fScaledTime;

}

