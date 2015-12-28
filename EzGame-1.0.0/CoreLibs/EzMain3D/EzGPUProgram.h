#pragma once

#include "../EzCore/EzString.h"
#include "EzObject.h"
#include "EzGpuProgramParameters.h"
#include "limits.h"

EzSmartPointer(EzGPUProgram);

enum EGPUProgramType
{
	EGP_VERTEX_PROGRAM,
	EGP_FRAGMENT_PROGRAM,
	EGP_GEOMETRY_PROGRAM
};

enum EGPUConstantType
{
	EGP_FLOAT1 = 1,
	EGP_FLOAT2 = 2,
	EGP_FLOAT3 = 3,
	EGP_FLOAT4 = 4,
	EGP_SAMPLER1D = 5,
	EGP_SAMPLER2D = 6,
	EGP_SAMPLER3D = 7,
	EGP_SAMPLERCUBE = 8,
	EGP_SAMPLER1DSHADOW = 9,
	EGP_SAMPLER2DSHADOW = 10,
	EGP_MATRIX_2X2 = 11,
	EGP_MATRIX_2X3 = 12,
	EGP_MATRIX_2X4 = 13,
	EGP_MATRIX_3X2 = 14,
	EGP_MATRIX_3X3 = 15,
	EGP_MATRIX_3X4 = 16,
	EGP_MATRIX_4X2 = 17,
	EGP_MATRIX_4X3 = 18,
	EGP_MATRIX_4X4 = 19,
	EGP_INT1 = 20,
	EGP_INT2 = 21,
	EGP_INT3 = 22,
	EGP_INT4 = 23,
	EGP_UNKNOWN = 99
};

struct EzGpuConstantDefinition
{
	/// Data type
	EGPUConstantType constType;
	unsigned int uiPhysicalIndex;
	unsigned int uiLogicalIndex;
	unsigned int uiElementSize;
	unsigned int uiArraySize;

	bool isFloat() const
	{
		switch(constType)
		{
		case EGP_INT1:
		case EGP_INT2:
		case EGP_INT3:
		case EGP_INT4:
		case EGP_SAMPLER1D:
		case EGP_SAMPLER2D:
		case EGP_SAMPLER3D:
		case EGP_SAMPLERCUBE:
		case EGP_SAMPLER1DSHADOW:
		case EGP_SAMPLER2DSHADOW:
			return false;
		default:
			return true;
		};

	}

	bool isSampler() const
	{
		switch(constType)
		{
		case EGP_SAMPLER1D:
		case EGP_SAMPLER2D:
		case EGP_SAMPLER3D:
		case EGP_SAMPLERCUBE:
		case EGP_SAMPLER1DSHADOW:
		case EGP_SAMPLER2DSHADOW:
			return true;
		default:
			return false;
		};

	}

	EzGpuConstantDefinition()
		: constType(EGP_UNKNOWN)
		, uiPhysicalIndex(UINT_MAX)
		, uiElementSize(0)
		, uiArraySize(1) {}
};


class EzGPUProgram : public EzObject
{
	EzDeclareRTTI

public:
	virtual EzGpuProgramParameters* createParameters(void);
	virtual void release() = 0;

    EGPUProgramType getType(void) const  
	{
		return m_eType;
	}

	void setType(EGPUProgramType eType) 
	{
		m_eType = eType;
	}




protected:
	EzGPUProgram(void);
	virtual ~EzGPUProgram(void);

	/** @copydoc Resource::loadImpl */
	virtual void loadImpl()
	{

	}
	/** Overridden from GpuProgram */
	virtual void loadFromSource()
	{

	}

	virtual void loadFromLowLevel()
	{
	}
		


protected:
	EzStringc					m_Source;
	EzStringc					m_SourceFileName;
	bool						m_bLoadFromFile;
	EGPUProgramType				m_eType;

	EzGpuLogicalIndexUseMap		m_FloatLogicalToPhysical;
	EzGpuLogicalIndexUseMap		m_IntLogicalToPhysical;



};

