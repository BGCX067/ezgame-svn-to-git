#pragma once

#include "../EzMain3D/EzObject.h"
#include "SAniKeyFrame.h"


EzSmartPointer(EzKeyframeData);

class EzKeyframeData : public EzObject
{
	EzDeclareRTTI;

public:
	EzKeyframeData(void);
	~EzKeyframeData(void);

	void setQuatRotateKeys(SAniKeyFrame<EzQuaternion>* pQuatRotateKeys, u32 uiKeyMaxNum, EANI_KEYFREAMTYPE eType);
	SAniKeyFrame<EzQuaternion>* getQuatRotateKeys(u32& uiKeyMaxNum, EANI_KEYFREAMTYPE& eType);
	
	inline const u32& getQuatRotatekeySize() const
	{ 
		return m_uiQuatRotateKeysSize; 
	}


	void setTranslateKeys(SAniKeyFrame<EzPoint3>* pTranslateKeys, u32 uiKeyMaxNum, EANI_KEYFREAMTYPE eType);
	SAniKeyFrame<EzPoint3>* getTranslateKeys(u32& uiKeyMaxNum, EANI_KEYFREAMTYPE& eType);
	
	inline const u32& getTranslateKeySize()  const
	{ 
		return m_uiTranslationsKeysSize; 
	}

	void setScaleKeys(SAniKeyFrame<f32>* pScalesKeys, u32 uiKeyMaxNum, EANI_KEYFREAMTYPE eType);
	SAniKeyFrame<f32>* getScaleKeys(u32& uiKeyMaxNum, EANI_KEYFREAMTYPE& eType);

	inline const u32& getScaleKeySize() const
	{ 
		return m_uiScaleKeysSize; 
	}


protected:
	
	SAniKeyFrame<EzQuaternion>*	m_pQuatRotateKeys;
	EANI_KEYFREAMTYPE			m_eQuatRotateKeysType;
	u32							m_uiQuatRotateKeysSize;		

	SAniKeyFrame<EzPoint3>*	m_pTranslationsKeys;
	EANI_KEYFREAMTYPE			m_eTranslationsKeysType;
	u32							m_uiTranslationsKeysSize;		

	SAniKeyFrame<f32>*			m_pScalesKeys;
	EANI_KEYFREAMTYPE			m_eScaleKeysType;
	u32							m_uiScaleKeysSize;		
};

