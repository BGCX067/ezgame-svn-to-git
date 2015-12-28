#include <float.h>
#include "EzTimeController.h"


namespace GameEasy
{

EzCriticalSection	EzTimeController::m_sCachedValueLock;

EzImplementRootRTTI(EzTimeController);


EzTimeController::EzTimeController(void)
:  m_spNextController(NULL), m_fFrequency(0.0f), m_fPhase(0.0f), m_fKeyStartTime(FLT_MAX), m_fKeyStopTime(-FLT_MAX),   
	m_fStartTime(-FLT_MAX), m_fLastTime(-FLT_MAX), m_fWeightedLastTime(0.0f), m_fScaledTime(0.0f),  m_pTarget(NULL)
{
}

EzTimeController::~EzTimeController(void)
{
}

EzTimeController* EzTimeController::getNextController()
{
	return m_spNextController;

}

void EzTimeController::setNextController( EzTimeController* obj )
{
	m_spNextController = obj;
}

void EzTimeController::setTarget( EzObjectNET * new_target )
{
	m_pTarget = new_target;

}

EzObjectNET* EzTimeController::getTarget()
{
	return m_pTarget;

}

f32 EzTimeController::getFrequency() const
{
	return m_fFrequency;
}

void EzTimeController::setFrequency( f32 n )
{
	m_fFrequency = n;
}

f32 EzTimeController::getPhase() const
{
	return m_fPhase;

}

void EzTimeController::setPhase( f32 n )
{
	m_fPhase = n;
}

void EzTimeController::normalizeKeys()
{
	//Normalize the start and stop times
	m_fKeyStartTime = m_fFrequency * m_fKeyStartTime + m_fPhase;
	m_fKeyStopTime = m_fFrequency * m_fKeyStopTime + m_fPhase;

	//Set phase to 0 and frequency to 1
	m_fPhase = 0.0f;
	m_fFrequency = 1.0f;

}
f32 EzTimeController::getStartTime() const
{
	return m_fKeyStartTime;
}

void EzTimeController::setStartTime( f32 n )
{
	m_fKeyStartTime = n;
}


f32 EzTimeController::getStopTime() const
{
	return m_fKeyStopTime;
}
void EzTimeController::setStopTime( f32 n )
{
	m_fKeyStopTime = n;
}


f32 EzTimeController::getLastTime()
{
	return m_fLastTime;

}


bool EzTimeController::dontDoUpdate(f32 fTime)
{
    if (!getActive() || (m_fLastTime == fTime))
        return true;

    if (getComputeScaledTime())
    {
        f32 fScaledTime = computeScaledTime(fTime);

		if (m_fScaledTime == fScaledTime)
			return true;

        m_fScaledTime = fScaledTime;
    }

    return false;
}


f32 EzTimeController::computeScaledTime(f32 fTime)
{
    static ECYCLE_TYPE seLastCycle = MAX_CYCLE_TYPES;
    static f32 sfLastHi;
    static f32 sfLastLo;
    static f32 sfLastTime;
    static f32 sfLastScaled;

    f32 fScaledTime = 0.0f;

    // If this is the first time Update is being called then we start
    // the animation at m_fPhase.  If the animation is APP_INIT, reset
    // the delta time to 0.
    if (m_fStartTime == -FLT_MAX)
    {
        m_fStartTime = fTime;
    }

    f32 fDeltaTime;
    if (m_fLastTime == -FLT_MAX)
    {
        m_fWeightedLastTime = 0.0f;
        fDeltaTime = 0.0f;
    }
    else
    {
        // Calculate time since last update
        fDeltaTime = fTime - m_fLastTime;
    }


    // Scale time linearly by frequency and phase values 
    // specified by the application.
    //
    // time' = freq * time + phase
    //
    m_fWeightedLastTime += fDeltaTime * m_fFrequency;
    fScaledTime = m_fWeightedLastTime + m_fPhase;

    m_fLastTime = fTime;
    ECYCLE_TYPE eCycle = getCycleType();

    m_sCachedValueLock.lock();
    if (sfLastHi == m_fKeyStopTime && sfLastLo == m_fKeyStartTime && 
        sfLastTime == fScaledTime && seLastCycle == eCycle)
    {
        f32 fReturn = sfLastScaled;
         m_sCachedValueLock.unlock();
        return fReturn;
    }

    sfLastHi = m_fKeyStopTime;
    sfLastLo = m_fKeyStartTime;
    sfLastTime = fScaledTime;
    seLastCycle = eCycle;

    if (m_fKeyStopTime == -FLT_MAX || m_fKeyStartTime == FLT_MAX)
    {
        // Avoiding overflow from span computation -
        // do nothing, conditions below will handle clamping.
    }
    else switch (eCycle) // Switch on m_CycleType
    {
        case CYCLE_LOOP:
        {
            //
            // Loop -- only need fractional time
            //
            f32 fSpan = m_fKeyStopTime - m_fKeyStartTime;

            if (fSpan != 0.0f)
            {
                fScaledTime = fmod(fScaledTime - m_fKeyStartTime, fSpan) +
                    m_fKeyStartTime;

                if (fScaledTime < m_fKeyStartTime)
                {
                    fScaledTime += fSpan;
                }
            }
            else
            {
                fScaledTime = m_fKeyStartTime;
            }

            break;
        }
        case CYCLE_REVERSE:
        {
            //
            // Reverse -- If the integer portion of the scaled 
            // time divided by the delta time is odd, then the 
            // animation is on a reverse leg of the sequence.
            //
            // assert: m_fHiKeyTime != m_fLoKeyTime
            //
            f32 fSpan = m_fKeyStopTime - m_fKeyStartTime;
            f32 fTimeFract;

            if (fSpan != 0.0f)
            {
                f32 fDoubleSpan = 2.0f * fSpan;
                fTimeFract = fmod(fScaledTime, fDoubleSpan);
                if (fTimeFract < 0.0f)
                {
                    fTimeFract += fDoubleSpan;
                }

                if (fTimeFract > fSpan)
                    fScaledTime = fDoubleSpan - fTimeFract + m_fKeyStartTime;
                else
                    fScaledTime = fTimeFract + m_fKeyStartTime;
            }
            else
                fScaledTime = m_fKeyStartTime;
            
            break;
        }
        case CYCLE_CLAMP:
        {
            // Clamp - do nothing, conditions below will handle clamping.
            break;
        }
        default:
            // perhaps some better error protection, like an NIASSERT() here?
            break;
    }

    if (fScaledTime > m_fKeyStopTime)
        fScaledTime = m_fKeyStopTime;
    else if (fScaledTime < m_fKeyStartTime)
        fScaledTime = m_fKeyStartTime;

    if (getPlayBackwards())
    {
        fScaledTime = m_fKeyStopTime - (fScaledTime - m_fKeyStartTime);
    }

    sfLastScaled = fScaledTime;
    m_sCachedValueLock.unlock();

    return fScaledTime;
}


}
