#pragma once
#include "../EzCore/EzArray.h"



class EzTriBasedGeomData;
class EzGeometryData;
class EzSkinData;


class Interaction
{
public:
	u32		m_uiBone;
	f32		m_fWeight;

	bool operator < (const Interaction& other) const
	{
		return (m_fWeight > other.m_fWeight);
	}
};

class VertexInteractions : public EzArray<Interaction>
{
public:
	u32 getBone(u32 uiIdx) const;
	f32 getWeight(u32 uiIdx) const;
	f32 findWeight(u32 uiBone) const;
	bool removeBone(u32 uiBone);
	void normalize();
	bool operator==(const VertexInteractions& other);
};



inline u32 VertexInteractions::getBone(u32 uiIdx) const
{
	EZASSERT(uiIdx < size())
	return data[uiIdx].m_uiBone;
}

inline f32 VertexInteractions::getWeight(u32 uiIdx) const
{
	EZASSERT(uiIdx < size())
	return data[uiIdx].m_fWeight;
}

inline f32 VertexInteractions::findWeight(u32 uiBone) const
{
	for (u32 uiIdx = 0; uiIdx < size(); uiIdx++)
	{
		if (data[uiIdx].m_uiBone == uiBone)
			return data[uiIdx].m_fWeight;
	}

	return 0.0f;
}

inline void VertexInteractions::normalize()
{
	u32 uiIdx;
	f32 fWeightSum = 0.0f;

	for (uiIdx = 0; uiIdx < size(); uiIdx++)
	{
		fWeightSum += data[uiIdx].m_fWeight;
	}

	EZASSERT(fWeightSum != 0.0f);

	f32 fScale = 1.0f / fWeightSum;

	for (uiIdx = 0; uiIdx < size(); uiIdx++)
	{
		data[uiIdx].m_fWeight *= fScale;
	}
}

inline bool VertexInteractions::removeBone(u32 uiBone)
{
	u32 uiIdx;

	for (uiIdx = 0; uiIdx < size(); uiIdx++)
	{
		if (data[uiIdx].m_uiBone == uiBone)
		{
			erase(uiIdx);
			return true;
		}
	}

	return false;
}

inline bool VertexInteractions::operator==(const VertexInteractions& other)
{
	if (size() != other.size())
		return false;

	for (u32 uiIdx = 0; uiIdx < size(); uiIdx++)
	{
		if (data[uiIdx].m_uiBone != other[uiIdx].m_uiBone)
			return false;
	}

	return true;
}


bool makePartitions(EzTriBasedGeomData* pTriData,	const EzSkinData* pSkinData, u8 ucBonesPerPartition,
		u8 ucBonesPerVertex, bool bForcePalette);
