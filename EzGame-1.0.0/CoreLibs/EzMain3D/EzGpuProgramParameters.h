#pragma once
#include "../EzCore/EzString.h"
#include "../EzCore/EzMap.h"
#include "../EzCore/EzArray.h"
#include "../EzCore/EzRefObject.h"


struct EzGpuLogicalIndexUse
{
	/// Physical buffer index
	unsigned int uiPhysicalIndex;
	/// Current physical size allocation
	unsigned int uiCurrentSize;

	EzGpuLogicalIndexUse(unsigned int bufIdx, unsigned int curSz) 
		: uiPhysicalIndex(bufIdx), uiCurrentSize(curSz) {}
};
	
typedef EzMap<unsigned int, EzGpuLogicalIndexUse>			EzGpuLogicalIndexUseMap;



class EzGpuProgramParameters : public EzRefObject
{
public:

	typedef EzArray<float>							FloatConstantStorage;


	EzGpuProgramParameters(void);
	~EzGpuProgramParameters(void);


	unsigned int setShaderConstant(unsigned int uiIndex, const float *val, unsigned int uiCount);

	EzGpuLogicalIndexUseMap* getFloatLogicalToPhysical();
	EzGpuLogicalIndexUseMap* getIntLogicalToPhysical();

	const float* getFloatPointer(unsigned int uiPos) const;


protected:
	unsigned int _getFloatConstantPhysicalIndex(unsigned int uiLogicalIndex, unsigned int uiRequestedSize);



	FloatConstantStorage		m_FloatConstants;

	unsigned int							m_uiPhysicalIndex;

	EzGpuLogicalIndexUseMap		m_FloatLogicalToPhysical;
	EzGpuLogicalIndexUseMap		m_IntLogicalToPhysical;
};

inline const float* EzGpuProgramParameters::getFloatPointer(unsigned int uiPos) const 
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
