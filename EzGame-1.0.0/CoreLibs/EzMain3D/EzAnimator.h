#pragma once

#include "EzObject.h"
#include "../EzCore/EzCore.h"
#include "../EzCore/EzCriticalSection.h"

class EzObjectNET;

EzSmartPointer(EzAnimator);

class EzAnimator : public EzObject
{

	EzDeclareRTTI;
	EzDeclareFlags(unsigned int);
	
public:
	EzAnimator(void);
	~EzAnimator(void);

	enum ECYCLE_TYPE
	{
		CYCLE_LOOP,
		CYCLE_REVERSE,
		CYCLE_CLAMP,
		MAX_CYCLE_TYPES
	};

	void setActive(bool bActive);
	bool getActive() const;

	virtual void update(float fTime) = 0;

	bool dontDoUpdate(float fTime);

	float getLastTime();

	EzAnimator* getNextAnimator();
	void setNextAnimator( EzAnimator* obj );
	void setTarget( EzObjectNET * new_target );
	EzObjectNET* getTarget();
	float getFrequency() const;
	void setFrequency( float n );
	float getPhase() const;
	void setPhase( float n );
	virtual void normalizeKeys();
	float getStartTime() const;
	void setStartTime( float n );
	float getStopTime() const;
	void setStopTime( float n );

	void setCycleType(ECYCLE_TYPE eType);
	ECYCLE_TYPE getCycleType() const;
	void setPlayBackwards(bool bPlayBackwards);
	bool getPlayBackwards() const;

	inline void setManagerControlled(bool bManagerControlled);
	inline bool getManagerControlled() const;

	virtual bool isStreamable() const;


	
protected:
	// flags
	enum
	{
		ANIMTYPE_MASK           = 0x0001,
		ANIMTYPE_POS            = 0,
		CYCLETYPE_MASK          = 0x0006,
		CYCLETYPE_POS           = 1,
		ACTIVE_MASK             = 0x0008,
		DIRECTION_MASK          = 0x0010,
		MANAGERCONTROLLED_MASK  = 0x0020,
		COMPUTESCALEDTIME_MASK  = 0x0040,
		FORCEUDPATE_MASK        = 0x0080
	};

	void setComputeScaledTime(bool bComputeScaledTime);
	bool getComputeScaledTime() const;

	float computeScaledTime(float fTime);

	/*! Index of the next controller. */
	EzAnimatorPtr m_spNextAnimator;
	/*!
	 * Controller flags (usually 0x000C). Probably controls loops.
	 *              Bit 0 : Anim type, 0=APP_TIME 1=APP_INIT
	 *              Bit 1-2 : Cycle type  00=Loop 01=Reverse 10=Loop
	 *              Bit 3 : Active
	 *              Bit 4 : Play backwards
	 */
	/*! Frequency (is usually 1.0). */
	float m_fFrequency;
	/*! Phase (usually 0.0). */
	float m_fPhase;
	/*! Controller start time. */
	float m_fKeyStartTime;
	/*! Controller stop time. */
	float m_fKeyStopTime;
	/*!
	 * Controller target (object index of the first controllable ancestor of this
	 * object).
	 */

	float							m_fWeightedLastTime;
	float							m_fLastTime;
	float							m_fScaledTime;
	float							m_fStartTime;


	EzObjectNET*				m_pTarget;

	static EzCriticalSection	m_sCachedValueLock;

};

inline bool EzAnimator::isStreamable() const
{
	return true;
}

inline void EzAnimator::setCycleType(ECYCLE_TYPE eType)
{
	setField(eType, CYCLETYPE_MASK, CYCLETYPE_POS); 
}

inline EzAnimator::ECYCLE_TYPE EzAnimator::getCycleType() const
{
	return (ECYCLE_TYPE)getField(CYCLETYPE_MASK, CYCLETYPE_POS); 
}

inline void EzAnimator::setPlayBackwards(bool bPlayBackwards)
{
	setBit(bPlayBackwards, DIRECTION_MASK);
}

inline bool EzAnimator::getPlayBackwards() const
{
	return getBit(DIRECTION_MASK);
}

inline void EzAnimator::setActive(bool bActive)
{
	setBit(bActive, ACTIVE_MASK);
}

inline bool EzAnimator::getActive() const
{
	return getBit(ACTIVE_MASK);
}

inline void EzAnimator::setComputeScaledTime(bool bComputeScaledTime)
{
	setBit(bComputeScaledTime, COMPUTESCALEDTIME_MASK);
}

inline bool EzAnimator::getComputeScaledTime() const
{
	return getBit(COMPUTESCALEDTIME_MASK);
}

inline void EzAnimator::setManagerControlled(bool bManagerControlled)
{
	setBit(bManagerControlled, MANAGERCONTROLLED_MASK);
}

inline bool EzAnimator::getManagerControlled() const
{
	return getBit(MANAGERCONTROLLED_MASK);
}


