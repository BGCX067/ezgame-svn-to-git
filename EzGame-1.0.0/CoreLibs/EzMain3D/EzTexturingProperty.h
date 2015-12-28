#pragma once

#include "../EzCore/EzTransForm.h"
#include "EzProperty.h"
#include "EzTexture.h"

class EzTexturingProperty : public EzProperty 
{
	EzDeclareStream;
	EzDeclareRTTI;

public:
	EzTexturingProperty(void);
	~EzTexturingProperty(void);

	enum EMAP
	{
		BASE_MAP = 0,
		DARK_MAP,
		DETAIL_INDEX,
		GLOSS_INDEX,
		GLOW_INDEX,
		BUMP_INDEX,
		NORMAL_INDEX,
		PARALLAX_INDEX,
		DECAL_BASE,
		SHADER_BASE,
		INDEX_MAX
	};

	class Map :  public EzRefObject
	{
	public:
		Map()
		: uvSet(0), hasTextureTransform(false), tiling(EzPoint2(1.0f, 1.0f))
		{};
		~Map() {};

		void saveBinary(EzStream& Stream);
		void loadBinary(EzStream& Stream);

		Map& operator=(const Map& other) 
		{ 
			m_spTexture = other.m_spTexture;
			m_pTextureTransform = other.m_pTextureTransform;
			eClampMode = other.eClampMode;
			eFilterMode = other.eFilterMode;
			eApplymode = other.eApplymode;
			hasTextureTransform = other.hasTextureTransform;
			translation = other.translation;
			tiling = other.tiling;
			usRotation = other.usRotation;
			usTransformType = other.usTransformType;
			centerOffset = other.centerOffset;
			uvSet = other.uvSet;
		
			return *this;
		};

		bool operator==(const Map& kObj) const;
        bool operator!=(const Map& kObj) const;


	protected:
		EzTexturePtr				m_spTexture;
		ETEXTURE_CLAMP				eClampMode;
		ETEXTURE_FILTERMODE			eFilterMode;
		ETEXTURE_APPLY_MODE			eApplymode;

		EzTransform					m_pTextureTransform;

		bool						hasTextureTransform;
		EzPoint2						translation;
		EzPoint2						tiling;
		unsigned short				usRotation;
		unsigned short				usTransformType;
		EzPoint2						centerOffset;
		short						uvSet;

	};

	inline ETYPE type() { return TEXTURE; }

	unsigned short getTextureCount();
	void setTextureCount(unsigned short uiTexCount) { m_uiTextureCount = uiTexCount; }

	void setBaseMap(Map *pMap);
	Map* getBaseMap();
	const Map* getBaseMap() const;

protected:
	unsigned short								m_uiTextureCount;
	Map*							m_pRanderTexture[INDEX_MAX];
};

EzSmartPointer(EzTexturingProperty);


inline bool EzTexturingProperty::Map::operator!=(const Map& Obj) const
{
    return !(*this == Obj);
}


inline void EzTexturingProperty::setBaseMap(Map *pMap)
{
	Map *pOldMap = m_pRanderTexture[BASE_MAP];

	if(pOldMap != pMap)
	{
		EzDelete pOldMap;
		m_pRanderTexture[BASE_MAP] = pMap;
	}
}

inline EzTexturingProperty::Map* EzTexturingProperty::getBaseMap()
{
	return m_pRanderTexture[BASE_MAP];
}

inline const EzTexturingProperty::Map* EzTexturingProperty::getBaseMap() const
{
	return m_pRanderTexture[BASE_MAP];
}
