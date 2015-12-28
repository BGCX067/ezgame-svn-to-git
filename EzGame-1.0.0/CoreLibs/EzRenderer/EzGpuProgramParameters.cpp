#include "EzGpuProgramParameters.h"
#include "../EzCore/EzSystem.h"


EzGpuProgramParameters::EzGpuProgramParameters(void)
: m_uiPhysicalIndex(0)
{
}

EzGpuProgramParameters::~EzGpuProgramParameters(void)
{
}

u32 EzGpuProgramParameters::setShaderConstant(u32 uiIndex, const f32 *val, u32 uiCount) 
{
	// Raw buffer size is 4x count
	u32 uiRequestedSize = uiCount * 4;
	u32 uiPhysicalIndex = _getFloatConstantPhysicalIndex(uiIndex, uiRequestedSize);

	EZASSERT(uiPhysicalIndex + uiCount <= m_FloatConstants.size());
	memcpy(&m_FloatConstants[uiPhysicalIndex], val, uiRequestedSize * sizeof(f32));
	return uiPhysicalIndex;
	
}

u32 EzGpuProgramParameters::_getFloatConstantPhysicalIndex(u32 uiLogicalIndex, u32 uiRequestedSize) 
{

	u32 uiPhysicalIndex;
	EzGpuLogicalIndexUseMap::Node *pNode = m_FloatLogicalToPhysical.find(uiLogicalIndex);
	//
	if (!pNode)
	{
		if (uiRequestedSize)
		{
			uiPhysicalIndex = m_FloatConstants.size();

			for (u32 logicalNum = 0; logicalNum < uiRequestedSize; ++logicalNum)
			{
				m_FloatConstants.push_back(0.0f);
			}

			m_FloatLogicalToPhysical.insert(uiLogicalIndex, EzGpuLogicalIndexUse(uiPhysicalIndex, uiRequestedSize));
		}
	}
	else
	{
		EZASSERT(pNode->getValue().uiCurrentSize <= uiRequestedSize)
		uiPhysicalIndex = pNode->getValue().uiPhysicalIndex;
	}

	return uiPhysicalIndex;
}

