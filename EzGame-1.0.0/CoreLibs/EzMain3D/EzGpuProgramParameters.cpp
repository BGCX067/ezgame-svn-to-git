#include "EzGpuProgramParameters.h"
#include "../EzCore/EzSystem.h"


EzGpuProgramParameters::EzGpuProgramParameters(void)
: m_uiPhysicalIndex(0)
{
}

EzGpuProgramParameters::~EzGpuProgramParameters(void)
{
}

unsigned int EzGpuProgramParameters::setShaderConstant(unsigned int uiIndex, const float *val, unsigned int uiCount) 
{
	// Raw buffer size is 4x count
	unsigned int uiRequestedSize = uiCount * 4;
	unsigned int uiPhysicalIndex = _getFloatConstantPhysicalIndex(uiIndex, uiRequestedSize);

	EZASSERT(uiPhysicalIndex + uiCount <= m_FloatConstants.size());
	memcpy(&m_FloatConstants[uiPhysicalIndex], val, uiRequestedSize * sizeof(float));
	return uiPhysicalIndex;
	
}

unsigned int EzGpuProgramParameters::_getFloatConstantPhysicalIndex(unsigned int uiLogicalIndex, unsigned int uiRequestedSize) 
{

	unsigned int uiPhysicalIndex;
	EzGpuLogicalIndexUseMap::Node *pNode = m_FloatLogicalToPhysical.find(uiLogicalIndex);
	//
	if (!pNode)
	{
		if (uiRequestedSize)
		{
			uiPhysicalIndex = m_FloatConstants.size();

			for (unsigned int logicalNum = 0; logicalNum < uiRequestedSize; ++logicalNum)
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

