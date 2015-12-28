#pragma once

#include "../EzCore/EzCore.h"
#include "EzTexture.h"
#include "EzTextureStageGroup.h"

EzSmartPointer(EzTextureStage);

class EzTextureStage :public EzRefObject
{
public:
	EzTextureStage(void);
	~EzTextureStage(void);

	static void initializePools();
	static void shutdownPools();
	static EzTextureStage* alloc();
	static void free(EzTextureStage* pGroup);

	void setTexture(EzTexture* pTexture);
	void setStage(u32 uiStage);
	u32 getStage();
	EzTexture* getTexture();

	inline void setTextureClampMode(ETEXTURE_CLAMP eMode);
	inline void setTextureFilterMode(ETEXTURE_FILTERMODE eFilterMode);

	inline ETEXTURE_CLAMP getTextureClampMode();
	inline ETEXTURE_FILTERMODE getTextureFilterMode();

	inline void setTextureTransformation(const EzMatrix4& Mat);
	inline const EzMatrix4& getTextureTransformation() const;
    inline void setTextureTransformation(bool bUseTransform);
    inline bool isTextureTransformation() const;


	void setStageState(u32 uiState, u32 uiValue);
	void setSamplerState(u32 uiState, u32 uiValue);
	void removeStageState(u32 uiState);
	void removeSamplerState(u32 uiState);

	EzTextureStageGroup* getTextureStageGroup();

	void update();

protected:

	ETEXTURE_CLAMP			m_eClampMode;
	ETEXTURE_FILTERMODE		m_eFilterMode;

	EzTextureStageGroup* m_pTextureStageGroup;

	bool				m_bTextureTransform;
	EzTexture*			m_pTexture;
	u32					m_uiStage;

	EzMatrix4			m_TextureTransformation;

	static EzMemPool<EzTextureStage>* ms_pTextureStagePool;


};


inline const EzMatrix4& EzTextureStage::getTextureTransformation() const
{
	return m_TextureTransformation;
}

inline bool EzTextureStage::isTextureTransformation() const
{
	return m_bTextureTransform;

}

inline void EzTextureStage::setTextureTransformation(bool bUseTransform)
{
	m_bTextureTransform = bUseTransform;

}

inline void EzTextureStage::setTextureTransformation(const EzMatrix4& Mat)
{
	m_TextureTransformation = Mat;
}

inline ETEXTURE_CLAMP EzTextureStage::getTextureClampMode()
{
	return m_eClampMode;

}
inline ETEXTURE_FILTERMODE EzTextureStage::getTextureFilterMode()
{
	return m_eFilterMode;
}


inline void EzTextureStage::setTextureClampMode(ETEXTURE_CLAMP eMode)
{
	m_eClampMode = eMode;
}

inline void EzTextureStage::setTextureFilterMode(ETEXTURE_FILTERMODE eFilterMode)
{
	m_eFilterMode = eFilterMode;
}


inline EzTextureStageGroup* EzTextureStage::getTextureStageGroup()
{
	return m_pTextureStageGroup;
}

inline EzTexture* EzTextureStage::getTexture()
{
	return m_pTexture;

}

inline void EzTextureStage::setTexture(EzTexture* pTexture)
{
	m_pTexture = pTexture;
}

inline void EzTextureStage::setStage(u32 uiStage)
{
	m_uiStage = uiStage;
}

inline u32 EzTextureStage::getStage()
{
	return m_uiStage;
}



inline void EzTextureStage::setStageState(u32 uiState, u32 uiValue)
{
    //if (uiState == D3DTSS_TEXTURETRANSFORMFLAGS)
    //{
    //    if (uiValue != D3DTTFF_DISABLE && uiValue != D3DTTFF_PROJECTED)
    //        m_bTextureTransform = true;
    //    else
    //        m_bTextureTransform = false;
    //}
    m_pTextureStageGroup->setStageState(uiState, uiValue);
}

inline void EzTextureStage::setSamplerState(u32 uiState, u32 uiValue)
{
    m_pTextureStageGroup->setSamplerState(uiState, uiValue);
}

inline void EzTextureStage::removeStageState(u32 uiState)
{
    m_pTextureStageGroup->removeStageState(uiState);
}

inline void EzTextureStage::removeSamplerState(u32 uiState)
{
    m_pTextureStageGroup->removeSamplerState(uiState);
}