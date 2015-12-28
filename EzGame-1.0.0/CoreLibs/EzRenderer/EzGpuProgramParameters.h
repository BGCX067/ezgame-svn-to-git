#pragma once
#include "../EzCore/EzString.h"
#include "../EzCore/EzMap.h"
#include "../EzCore/EzArray.h"


struct EzGpuLogicalIndexUse
{
	/// Physical buffer index
	u32 uiPhysicalIndex;
	/// Current physical size allocation
	u32 uiCurrentSize;

	EzGpuLogicalIndexUse(u32 bufIdx, u32 curSz) 
		: uiPhysicalIndex(bufIdx), uiCurrentSize(curSz) {}
};
	
typedef EzMap<u32, EzGpuLogicalIndexUse>			EzGpuLogicalIndexUseMap;



class EzGpuProgramParameters
{
public:

	typedef EzArray<float>							FloatConstantStorage;


	EzGpuProgramParameters(void);
	~EzGpuProgramParameters(void);


	u32 setShaderConstant(u32 uiIndex, const f32 *val, u32 uiCount);

	EzGpuLogicalIndexUseMap* getFloatLogicalToPhysical();
	EzGpuLogicalIndexUseMap* getIntLogicalToPhysical();

	const f32* getFloatPointer(u32 uiPos) const;


protected:
	u32 _getFloatConstantPhysicalIndex(u32 uiLogicalIndex, u32 uiRequestedSize);



	FloatConstantStorage		m_FloatConstants;

	u32							m_uiPhysicalIndex;

	EzGpuLogicalIndexUseMap		m_FloatLogicalToPhysical;
	EzGpuLogicalIndexUseMap		m_IntLogicalToPhysical;
};

inline const f32* EzGpuProgramParameters::getFloatPointer(u32 uiPos) const 
{ 
	EZASSERT(uiPos <= m_FloatConstants.size())
	return &m_FloatConstants[uiPos]; 
}

inline EzGpuLogicalIndexUseMap* EzGpuProgramParameters::getFloatLogicalToPhysical() 
{
	return &m_FloatLogicalToPhysical;

}

inline EzGpuLogicalIndexUseMap* EzGpuProgramParameters::getIntLogicalToPhysical()
{
	return &m_IntLogicalToPhysical;
}
