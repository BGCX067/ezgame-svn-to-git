#include "EzD3DX9Mappings.h"


EzD3DX9Mappings::EzD3DX9Mappings(void)
{
}

EzD3DX9Mappings::~EzD3DX9Mappings(void)
{
}

D3DFORMAT EzD3DX9Mappings::get(EzIndexBuffer::EIndexType eIndexType)
{
	if (eIndexType == EzIndexBuffer::EIT_32BIT)
	{
		return D3DFMT_INDEX32;
	}
	else
	{
		return D3DFMT_INDEX16;
	}
}

D3DDECLTYPE EzD3DX9Mappings::get(EzVertexElement::EVertexElementType eType)
{
	switch (eType)
	{
	case EzVertexElement::EVET_COLOUR:
	case EzVertexElement::EVET_COLOUR_ABGR:
	case EzVertexElement::EVET_COLOUR_ARGB:
		return D3DDECLTYPE_D3DCOLOR;

	case EzVertexElement::EVET_FLOAT1:
		return D3DDECLTYPE_FLOAT1;

	case EzVertexElement::EVET_FLOAT2:
		return D3DDECLTYPE_FLOAT2;

	case EzVertexElement::EVET_FLOAT3:
		return D3DDECLTYPE_FLOAT3;

	case EzVertexElement::EVET_FLOAT4:
		return D3DDECLTYPE_FLOAT4;

	case EzVertexElement::EVET_SHORT2:
		return D3DDECLTYPE_SHORT2;

	case EzVertexElement::EVET_SHORT4:
		return D3DDECLTYPE_SHORT4;

	case EzVertexElement::EVET_UBYTE4:
		return D3DDECLTYPE_UBYTE4;

	}
	// to keep compiler happy
	return D3DDECLTYPE_FLOAT3;

}

D3DDECLUSAGE EzD3DX9Mappings::get(EzVertexElement::EVertexElementSemantic eSemantic)
{
	switch (eSemantic)
	{
	case EzVertexElement::EVES_BLEND_INDICES:
		return D3DDECLUSAGE_BLENDINDICES;

	case EzVertexElement::EVES_BLEND_WEIGHTS:
		return D3DDECLUSAGE_BLENDWEIGHT;

	case EzVertexElement::EVES_DIFFUSE:
		return D3DDECLUSAGE_COLOR; // NB index will differentiate

	case EzVertexElement::EVES_SPECULAR:
		return D3DDECLUSAGE_COLOR; // NB index will differentiate

	case EzVertexElement::EVES_NORMAL:
		return D3DDECLUSAGE_NORMAL;

	case EzVertexElement::EVES_POSITION:
		return D3DDECLUSAGE_POSITION;

	case EzVertexElement::EVES_TEXTURE_COORDINATES:
		return D3DDECLUSAGE_TEXCOORD;

	case EzVertexElement::EVES_BINORMAL:
		return D3DDECLUSAGE_BINORMAL;

	case EzVertexElement::EVES_TANGENT:
		return D3DDECLUSAGE_TANGENT;

	}
	// to keep compiler happy
	return D3DDECLUSAGE_POSITION;

}

UINT EzD3DX9Mappings::get(Ez3DBuffer::EUsage eUsage)
{
	UINT ret = 0;
	if (eUsage & Ez3DBuffer::EDYNAMIC)
	{
		ret |= D3DUSAGE_DYNAMIC;
	}
	if (eUsage & Ez3DBuffer::EWRITE_ONLY)
	{
		ret |= D3DUSAGE_WRITEONLY;
	}
	return ret;
}

UINT EzD3DX9Mappings::get(Ez3DBuffer::ELockOptions eOptions, Ez3DBuffer::EUsage eUsage)
{
	UINT ret = 0;
	if (eOptions == Ez3DBuffer::EDISCARD)
	{
		EZASSERT(eUsage & Ez3DBuffer::EDYNAMIC);
		ret |= D3DLOCK_DISCARD;
	}
	if (eOptions == Ez3DBuffer::EREAD_ONLY)
	{
		EZASSERT(!(eUsage & Ez3DBuffer::EWRITE_ONLY));
		ret |= D3DLOCK_READONLY;

	}
	if (eOptions == Ez3DBuffer::ENO_OVERWRITE)
	{
		EZASSERT(!(eUsage & Ez3DBuffer::EDYNAMIC));
		ret |= D3DLOCK_NOOVERWRITE;
	}

	return ret;
}

D3DMULTISAMPLE_TYPE EzD3DX9Mappings::get(EMULTISAMPLE_TYPE eType)
{
	switch(eType)
	{
	case EMULTISAMPLE_NONE:
		return D3DMULTISAMPLE_NONE; 
	case EMULTISAMPLE_NONMASKABLE:
		return D3DMULTISAMPLE_NONMASKABLE;
	case EMULTISAMPLE_2_SAMPLES:
		return D3DMULTISAMPLE_2_SAMPLES;
	case EMULTISAMPLE_3_SAMPLES:
		return D3DMULTISAMPLE_3_SAMPLES;
	case EMULTISAMPLE_4_SAMPLES:
		return D3DMULTISAMPLE_4_SAMPLES;
	case EMULTISAMPLE_5_SAMPLES:
		return D3DMULTISAMPLE_5_SAMPLES;
	case EMULTISAMPLE_6_SAMPLES:
		return D3DMULTISAMPLE_6_SAMPLES;
	case EMULTISAMPLE_7_SAMPLES:
		return D3DMULTISAMPLE_6_SAMPLES;
	case EMULTISAMPLE_8_SAMPLES:
		return D3DMULTISAMPLE_8_SAMPLES;
	case EMULTISAMPLE_9_SAMPLES:
		return D3DMULTISAMPLE_9_SAMPLES;
	case EMULTISAMPLE_10_SAMPLES:
		return D3DMULTISAMPLE_10_SAMPLES;
	case EMULTISAMPLE_11_SAMPLES:
		return D3DMULTISAMPLE_11_SAMPLES;
	case EMULTISAMPLE_12_SAMPLES:
		return D3DMULTISAMPLE_12_SAMPLES;
	case EMULTISAMPLE_13_SAMPLES:
		return D3DMULTISAMPLE_13_SAMPLES;
	case EMULTISAMPLE_14_SAMPLES:
		return D3DMULTISAMPLE_14_SAMPLES;
	case EMULTISAMPLE_15_SAMPLES:
		return D3DMULTISAMPLE_15_SAMPLES;
	case EMULTISAMPLE_16_SAMPLES:
		return D3DMULTISAMPLE_16_SAMPLES;
	default:
		return D3DMULTISAMPLE_NONE;
	}
}



D3DFORMAT EzD3DX9Mappings::get(ECOLOR_FORMAT format)
{
	switch( format )
	{
	case ECF_A1R5G5B5:
		return D3DFMT_A1R5G5B5;
	case ECF_R5G6B5:
		return D3DFMT_R5G6B5;
	case ECF_A4R4G4B4:
		return D3DFMT_A4R4G4B4;
	case ECF_R8G8B8:
		return D3DFMT_R8G8B8;
	case ECF_A8R8G8B8:
		return D3DFMT_A8R8G8B8;
	case ECF_L8:
		return D3DFMT_L8;
	case ECF_DXT1:
		return D3DFMT_DXT1;
	case ECF_DXT2:
		return D3DFMT_DXT2;
	case ECF_DXT3:
		return D3DFMT_DXT3;
	case ECF_DXT4:
		return D3DFMT_DXT4;
	case ECF_DXT5:
		return D3DFMT_DXT5;
	case ECF_A16B16G16R16:
		return D3DFMT_A16B16G16R16;
	case ECF_G16R16F:
		return D3DFMT_G16R16F;
	case ECF_A16B16G16R16F:
		return D3DFMT_A16B16G16R16F;
	case ECF_R32F:
		return D3DFMT_R32F;
	case ECF_G32R32F:
		return D3DFMT_G32R32F;
	case ECF_A32B32G32R32F:
		return D3DFMT_A32B32G32R32F;
	default:
		return D3DFMT_UNKNOWN;
	}

	return D3DFMT_UNKNOWN;
}



ECOLOR_FORMAT EzD3DX9Mappings::get(D3DFORMAT format)
{
	switch( format )
	{
	case D3DFMT_A1R5G5B5:
		return ECF_A1R5G5B5;
	case D3DFMT_R5G6B5:
		return ECF_R5G6B5;
	case D3DFMT_A4R4G4B4:
		return ECF_A4R4G4B4;
	case D3DFMT_R8G8B8:
		return ECF_R8G8B8;
	case D3DFMT_A8R8G8B8:
		return ECF_A8R8G8B8;
	case D3DFMT_L8:
		return ECF_L8;
	case D3DFMT_DXT1:
		return ECF_DXT1;
	case D3DFMT_DXT2:
		return ECF_DXT2;
	case D3DFMT_DXT3:
		return ECF_DXT3;
	case D3DFMT_DXT4:
		return ECF_DXT4;
	case D3DFMT_DXT5:
		return ECF_DXT5;
	case D3DFMT_A16B16G16R16:
		return ECF_A16B16G16R16;
	case D3DFMT_G16R16F:
		return ECF_G16R16F;
	case D3DFMT_A16B16G16R16F:
		return ECF_A16B16G16R16F;
	case D3DFMT_R32F:
		return ECF_R32F;
	case D3DFMT_G32R32F:
		return ECF_G32R32F;
	case D3DFMT_A32B32G32R32F:
		return ECF_A32B32G32R32F;
	}

	return ECF_UNKNOWN;
}

D3DTEXTUREOP EzD3DX9Mappings::get(ETEXTURE_APPLY_MODE eMode)
{
	switch( eMode )
	{
	case APPLY_MODULATE:
		return D3DTOP_MODULATE;
	}

	return D3DTOP_MODULATE;
}

D3DTEXTURESTAGESTATETYPE EzD3DX9Mappings::get(ETEXTURESTAGESTATETYPE eStage)
{
	switch(eStage)
	{
	case ETS_COLOROP:
		return D3DTSS_COLOROP;
	case ETS_COLORARG1:
		return D3DTSS_COLORARG1;
	case ETS_COLORARG2:
		return D3DTSS_COLORARG2;
	case ETS_ALPHAOP:
		return D3DTSS_ALPHAOP;
	case ETS_ALPHAARG1:
		return D3DTSS_ALPHAARG1;
	case ETS_ALPHAARG2:
		return D3DTSS_ALPHAARG2;
	case ETS_BUMPENVMAT00:
		return D3DTSS_BUMPENVMAT00;
	case ETS_BUMPENVMAT01:
		return D3DTSS_BUMPENVMAT01;
	case ETS_BUMPENVMAT10:
		return D3DTSS_BUMPENVMAT10;
	case ETS_BUMPENVMAT11:
		return D3DTSS_BUMPENVMAT11;
	case ETS_TEXCOORDINDEX:
		return D3DTSS_TEXCOORDINDEX;
	case ETS_BUMPENVLSCALE:
		return D3DTSS_BUMPENVLSCALE;
	case ETS_BUMPENVLOFFSET:
		return D3DTSS_BUMPENVLOFFSET;

	case ETS_TEXTURETRANSFORMFLAGS:
		return D3DTSS_TEXTURETRANSFORMFLAGS;
	case ETS_COLORARG0:
		return D3DTSS_COLORARG0;
	case ETS_ALPHAARG0:
		return D3DTSS_ALPHAARG0;
	case ETS_RESULTARG:
		return D3DTSS_RESULTARG;
	case ETS_CONSTANT:
		return D3DTSS_CONSTANT;

	}

	return D3DTSS_FORCE_DWORD;

}

D3DTEXTUREOP EzD3DX9Mappings::get(ETEXTUREOP eStage)
{
	return (D3DTEXTUREOP) eStage;

}


D3DTEXTURETRANSFORMFLAGS EzD3DX9Mappings::get(ETEXTURETRANSFORMFLAGS eStage)
{
	return (D3DTEXTURETRANSFORMFLAGS) eStage;
}