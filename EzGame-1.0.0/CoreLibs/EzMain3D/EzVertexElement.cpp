#include "EzVertexElement.h"


EzVertexElement::EzVertexElement(unsigned short usSource, unsigned int uiOffset, EVertexElementType eType, EVertexElementSemantic eSemantic, u8 uIndex)
: m_eType(eType), m_eSemantic(eSemantic), m_uiOffset(uiOffset), m_usSource(usSource), m_uIndex(uIndex)
{
}

EzVertexElement::~EzVertexElement(void)
{
}

EzVertexElement::EVertexElementType EzVertexElement::getBestColourVertexElementType()
{
	return EVET_COLOUR_ARGB; // prefer D3D format on windows
}

unsigned int EzVertexElement::getTypeSize(EVertexElementType eType)
{
	switch(eType)
	{
	case EVET_COLOUR:
	case EVET_COLOUR_ABGR:
	case EVET_COLOUR_ARGB:
		return sizeof(unsigned int);
	case EVET_FLOAT1:
		return sizeof(float);
	case EVET_FLOAT2:
		return sizeof(float)*2;
	case EVET_FLOAT3:
		return sizeof(float)*3;
	case EVET_FLOAT4:
		return sizeof(float)*4;
	case EVET_SHORT1:
		return sizeof(short);
	case EVET_SHORT2:
		return sizeof(short)*2;
	case EVET_SHORT3:
		return sizeof(short)*3;
	case EVET_SHORT4:
		return sizeof(short)*4;
	case EVET_UBYTE4:
		return sizeof(c8)*4;
	}
	return 0;
}

