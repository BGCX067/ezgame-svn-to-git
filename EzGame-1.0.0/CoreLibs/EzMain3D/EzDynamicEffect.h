#pragma once

#include "EzAVObject.h"

class EzNode;

EzSmartPointer(EzDynamicEffect);


class EzDynamicEffect : public EzAVObject
{
	EzDeclareRTTI;

public:

	enum EEFFECT_TYPE
	{
		AMBIENT_LIGHT = 0,
		POINT_LIGHT = 2,
		DIR_LIGHT = 3,
		SPOT_LIGHT = 4,

		SHADOWPOINT_LIGHT = 5,
		SHADOWDIR_LIGHT = 6,
		SHADOWSPOT_LIGHT = 7,

		TEXTURE_EFFECT = 8,
		MAX_TYPES = TEXTURE_EFFECT + 1,

		// Note - all vertex-based lights types _must_ come before
		// the texture effects.  This allows the enum below to be
		// used for fast type checking in the low-level renderers, i.e.
		// (pE->GetEffectType() <= MAX_LIGHT_TYPE) <---> EzIsKindOf(NiLight, pS)
		MAX_LIGHT_TYPE = SHADOWSPOT_LIGHT,

		// Light index in provided so a NiDynamicEffect object and quickly 
		// update it's effect type when a NiShadowGenerator object is applied.
		LIGHT_INDEX = POINT_LIGHT,
		SHADOWLIGHT_INDEX = SHADOWPOINT_LIGHT,
		SHADOW_INDEX_DIFF = SHADOWPOINT_LIGHT - POINT_LIGHT

	};

	EzDynamicEffect(void);
	~EzDynamicEffect(void);

	void attachAffectedNode(EzNode* pAffectedNode);
	void detachAffectedNode(EzNode* pAffectedNode);

	void attachUnaffectedNode(EzNode* pUnaffectedNode);
	void detachUnaffectedNode(EzNode* pUnaffectedNode);

	bool isUnaffectedNode(EzNode* pPossibleNode);

	inline unsigned int getIndex() const
	{
		return m_iIndex;
	}

	inline EEFFECT_TYPE getEffectType() { return (EEFFECT_TYPE ) m_eEffectType; }
	inline void setEffectType(EEFFECT_TYPE eEffectType) { 	m_eEffectType = eEffectType; }


protected:
	EzList<EzNode*>			m_aAffectedNodeList;
	EzList<EzNode*>			m_aUnaffectedNodeList;


	unsigned int				m_iIndex;
	EEFFECT_TYPE	m_eEffectType;

	static unsigned int		ms_uiNextIndex;



};
