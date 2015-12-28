#include "EzVertexElement.h"


EzVertexElement::EzVertexElement(u16 usSource, u32 uiOffset, EVertexElementType eType, EVertexElementSemantic eSemantic, u8 uIndex)
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

u32 EzVertexElement::getTypeSize(EVertexElementType eType)
{
	switch(eType)
	{
	case EVET_COLOUR:
	case EVET_COLOUR_ABGR:
	case EVET_COLOUR_ARGB:
		return sizeof(u32);
	case EVET_FLOAT1:
		return sizeof(f32);
	case EVET_FLOAT2:
		return sizeof(f32)*2;
	case EVET_FLOAT3:
		return sizeof(f32)*3;
	case EVET_FLOAT4:
		return sizeof(f32)*4;
	case EVET_SHORT1:
		return sizeof(s16);
	case EVET_SHORT2:
		return sizeof(s16)*2;
	case EVET_SHORT3:
		return sizeof(s16)*3;
	case EVET_SHORT4:
		return sizeof(s16)*4;
	case EVET_UBYTE4:
		return sizeof(c8)*4;
	}
	return 0;
}

