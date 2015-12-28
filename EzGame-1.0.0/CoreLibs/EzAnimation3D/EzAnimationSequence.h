#pragma once

#include "../EzMain3D/EzObject.h"
#include "EzInterpAnimator.h"
#include "EzInterpolator.h"
#include "EzBlendInterpolator.h"


	
class EzMuiltyTransformAnimator;
class EzAnimatorManager;
class EzAVObject;
class EzNode;

EzSmartPointer(EzAnimationSequence);

class EzAnimationSequence : public EzObject
{
public:
	EzAnimationSequence(void);
	~EzAnimationSequence(void);

	class ExtraData
	{
	public:
		ExtraData() {}

		EzStringc		m_TargetName;
		EzStringc		m_AnimatorType;
	};

    enum EANIMSTATE
    {
        INACTIVE,
        ANIMATING,
        EASEIN,
        EASEOUT,
        TRANSSOURCE,
        TRANSDEST,
        MORPHSOURCE
    };

    bool activate(u8 usPriority, bool bStartOver, f32 fWeight,  f32 fEaseInTime, EzAnimationSequence* pkTimeSyncSeq, bool bTransition);
 	bool deactivate(f32 fEaseOutTime, bool bTransition);
	void addInterpolator(EzInterpolator* pInterpolator, const ExtraData& data);

	f32 getFrequency() const;
	void setFrequency( f32 value );
	f32 getStartTime() const;
	void setStartTime( f32 value );
	f32 getStopTime() const;
	void setStopTime( f32 value );
	EzAnimator::ECYCLE_TYPE getCycleType() const;
	void setCycleType( EzAnimator::ECYCLE_TYPE n );
	u32 getNumControllers() const;
	u32 getControllerPriority( u32 controller ) const;
	void setControllerPriority( u32 controller, u32 priority );
	f32 getWeight() const;
	void setWeight( f32 value );
	const c8* getTargetName() const;
	void setTargetName( const c8* value );

	EANIMSTATE getState();

	f32 computeScaledTime(f32 fTime);

	void update(f32 fTime, bool bUpdateInterpolators = true);

	inline void setName(const c8* name) { m_Name = name; }
	inline const c8* getName() { return m_Name.c_str(); }

	void setOwner(EzAnimatorManager* pCtrlMgr);

	bool prepareTarget(EzNode* pTarget);


protected:
	class InterpolateItem
	{
	public:
		InterpolateItem() : m_spInterpAnimator(NULL), m_pBlendInterp(NULL), m_ucBlendIdx(UCHAR_MAX)
		{
		}
		
		inline void clear() 
		{
			m_spInterpAnimator = NULL;
			m_spInterpolator = NULL;
			m_pBlendInterp = NULL;
		}


		EzInterpAnimatorPtr			m_spInterpAnimator;
		EzInterpolatorPtr			m_spInterpolator;
		EzBlendInterpolator*		m_pBlendInterp;
		u8							m_ucBlendIdx;
	};

	EzMuiltyTransformAnimator* resolveTransformInterpolators(EzNode* pRoot, bool& bAddCtrl);
	void setInterpsWeightAndTime(f32 fWeight, f32 fEaseSpinner, f32 fTime);
	void attachInterpolators(c8 cPriority);
	void detachInterpolators();


	EzArray<InterpolateItem>		m_aInterp;
	EzArray<ExtraData>				m_aExtraData;


	EzAnimatorManager*			m_pOwner;


	EANIMSTATE		 m_eState;
	
	EzAnimator::ECYCLE_TYPE m_eAniCycleType;

	f32  m_fWeight;
	f32  m_fFrequency;
	f32  m_fStartKeyTime;
	f32  m_fStopKeyTime;



	// Variables used for time measurement.
	f32						m_fOffset;
	f32						m_fStartTime;
	f32						m_fEndTime;
	f32						m_fDestFrame;
	EzStringc				TargetName;
	EzStringc				m_Name;


	f32						m_fLastTime;
	f32						m_fWeightedLastTime;
	f32						m_fLastScaledTime;


};
