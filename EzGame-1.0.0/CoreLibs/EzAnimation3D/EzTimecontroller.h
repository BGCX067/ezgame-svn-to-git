#pragma once

#include "../EzMain3D/EzObject.h"
#include "../core/EzCore.h"
#include "../core/EzCriticalSection.h"

//#include "../core/EzSmartPointer.h"

namespace GameEasy
{

class EzObjectNET;

EzSmartPointer(EzTimeController);

class EzTimeController : public EzObject
{

	EzDeclareRTTI;
	EzDeclareFlags(u32);
	
public:
	EzTimeController(void);
	~EzTimeController(void);

	enum ECYCLE_TYPE
	{
		CYCLE_LOOP,
		CYCLE_REVERSE,
		CYCLE_CLAMP,
		MAX_CYCLE_TYPES
	};

	void setActive(bool bActive);
	bool getActive() const;

	virtual void update(f32 fTime) = 0;

	bool dontDoUpdate(f32 fTime);

	f32 getLastTime();

	EzTimeController* getNextController();
	void setNextController( EzTimeController* obj );
	void setTarget( EzObjectNET * new_target );
	EzObjectNET* getTarget();
	f32 getFrequency() const;
	void setFrequency( f32 n );
	f32 getPhase() const;
	void setPhase( f32 n );
	virtual void normalizeKeys();
	f32 getStartTime() const;
	void setStartTime( f32 n );
	f32 getStopTime() const;
	void setStopTime( f32 n );

	void setCycleType(ECYCLE_TYPE eType);
	ECYCLE_TYPE getCycleType() const;
	void setPlayBackwards(bool bPlayBackwards);
	bool getPlayBackwards() const;

	inline void setManagerControlled(bool bManagerControlled);
	inline bool getManagerControlled() const;


	
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

	f32 computeScaledTime(f32 fTime);

	/*! Index of the next controller. */
	EzTimeControllerPtr m_spNextController;
	/*!
	 * Controller flags (usually 0x000C). Probably controls loops.
	 *              Bit 0 : Anim type, 0=APP_TIME 1=APP_INIT
	 *              Bit 1-2 : Cycle type  00=Loop 01=Reverse 10=Loop
	 *              Bit 3 : Active
	 *              Bit 4 : Play backwards
	 */
	/*! Frequency (is usually 1.0). */
	f32 m_fFrequency;
	/*! Phase (usually 0.0). */
	f32 m_fPhase;
	/*! Controller start time. */
	f32 m_fKeyStartTime;
	/*! Controller stop time. */
	f32 m_fKeyStopTime;
	/*!
	 * Controller target (object index of the first controllable ancestor of this
	 * object).
	 */

	f32							m_fWeightedLastTime;
	f32							m_fLastTime;
	f32							m_fScaledTime;
	f32							m_fStartTime;


	EzObjectNET*				m_pTarget;

	static EzCriticalSection	m_sCachedValueLock;

};

inline void EzTimeController::setCycleType(ECYCLE_TYPE eType)
{
	setField(eType, CYCLETYPE_MASK, CYCLETYPE_POS); 
}

inline EzTimeController::ECYCLE_TYPE EzTimeController::getCycleType() const
{
	return (ECYCLE_TYPE)getField(CYCLETYPE_MASK, CYCLETYPE_POS); 
}

inline void EzTimeController::setPlayBackwards(bool bPlayBackwards)
{
	setBit(bPlayBackwards, DIRECTION_MASK);
}

inline bool EzTimeController::getPlayBackwards() const
{
	return getBit(DIRECTION_MASK);
}

inline void EzTimeController::setActive(bool bActive)
{
	setBit(bActive, ACTIVE_MASK);
}

inline bool EzTimeController::getActive() const
{
	return getBit(ACTIVE_MASK);
}

inline void EzTimeController::setComputeScaledTime(bool bComputeScaledTime)
{
	setBit(bComputeScaledTime, COMPUTESCALEDTIME_MASK);
}

inline bool EzTimeController::getComputeScaledTime() const
{
	return getBit(COMPUTESCALEDTIME_MASK);
}

inline void EzTimeController::setManagerControlled(bool bManagerControlled)
{
	setBit(bManagerControlled, MANAGERCONTROLLED_MASK);
}

inline bool EzTimeController::getManagerControlled() const
{
	return getBit(MANAGERCONTROLLED_MASK);
}





}
