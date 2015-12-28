#include "EzD3DX9TextureStage.h"
#include "../../EzMain3D/EzTexturingProperty.h"
#include "EzD3DX9Mappings.h"
#include "EzD3DX9Texture.h"
#include "../EzTextureStage.h"

EzD3DX9TextureStage::FilterMappingModes			EzD3DX9TextureStage::ms_akD3DFilterMapping[FILTER_MAX_MODES];
EzD3DX9TextureStage::TextureAddressSettings		EzD3DX9TextureStage::ms_akD3DAddressMapping[ETC_CLAMP_MAX_MODES];

EzD3DX9TextureStage::EzD3DX9TextureStage(IDirect3DDevice9* pID3DDevice, u32 uiMaxTextureBlendStages)
: m_pID3DDevice(pID3DDevice), m_eLastTextureFilterMode(FILTER_NEAREST), m_eLastTextureClampMode(ETC_CLAMP_U_CLAMP_V), 
m_eLastTextureApplyMode(APPLY_MODULATE), m_uiLastMaxTextureBlendStages(uiMaxTextureBlendStages)
{

	disableStage(0);

	// Address mappings
	ms_akD3DAddressMapping[ETC_WRAP_U_WRAP_V].m_eU = D3DTADDRESS_WRAP;
	ms_akD3DAddressMapping[ETC_WRAP_U_WRAP_V].m_eV = D3DTADDRESS_WRAP;

	ms_akD3DAddressMapping[ETC_WRAP_U_CLAMP_V].m_eU = D3DTADDRESS_WRAP;
	ms_akD3DAddressMapping[ETC_WRAP_U_CLAMP_V].m_eV = D3DTADDRESS_CLAMP;

	ms_akD3DAddressMapping[ETC_CLAMP_U_WRAP_V].m_eU = D3DTADDRESS_CLAMP;
	ms_akD3DAddressMapping[ETC_CLAMP_U_WRAP_V].m_eV = D3DTADDRESS_WRAP;

	ms_akD3DAddressMapping[ETC_CLAMP_U_CLAMP_V].m_eU = D3DTADDRESS_CLAMP;
	ms_akD3DAddressMapping[ETC_CLAMP_U_CLAMP_V].m_eV = 	D3DTADDRESS_CLAMP;



	// FILTER_NEAREST
	ms_akD3DFilterMapping[FILTER_NEAREST].m_eMag = D3DTEXF_POINT;
	ms_akD3DFilterMapping[FILTER_NEAREST].m_eMin = D3DTEXF_POINT;
	ms_akD3DFilterMapping[FILTER_NEAREST].m_eMip = D3DTEXF_NONE;
	//  FILTER_BILERP
	ms_akD3DFilterMapping[FILTER_BILERP].m_eMag = D3DTEXF_LINEAR;
	ms_akD3DFilterMapping[FILTER_BILERP].m_eMin = D3DTEXF_LINEAR;
	ms_akD3DFilterMapping[FILTER_BILERP].m_eMip = D3DTEXF_NONE;
	//  FILTER_TRILERP
	ms_akD3DFilterMapping[FILTER_TRILERP].m_eMag = D3DTEXF_LINEAR;
	ms_akD3DFilterMapping[FILTER_TRILERP].m_eMin = D3DTEXF_LINEAR;
	ms_akD3DFilterMapping[FILTER_TRILERP].m_eMip = D3DTEXF_LINEAR;
	//  FILTER_NEAREST_MIPNEAREST
	ms_akD3DFilterMapping[FILTER_NEAREST_MIPNEAREST].m_eMag = D3DTEXF_POINT;
	ms_akD3DFilterMapping[FILTER_NEAREST_MIPNEAREST].m_eMin = D3DTEXF_POINT;
	ms_akD3DFilterMapping[FILTER_NEAREST_MIPNEAREST].m_eMip = D3DTEXF_POINT;
	//  FILTER_NEAREST_MIPLERP
	ms_akD3DFilterMapping[FILTER_NEAREST_MIPLERP].m_eMag = D3DTEXF_POINT;
	ms_akD3DFilterMapping[FILTER_NEAREST_MIPLERP].m_eMin = D3DTEXF_POINT;
	ms_akD3DFilterMapping[FILTER_NEAREST_MIPLERP].m_eMip = D3DTEXF_LINEAR;
	//  FILTER_BILERP_MIPNEAREST
	ms_akD3DFilterMapping[FILTER_BILERP_MIPNEAREST].m_eMag = D3DTEXF_LINEAR;
	ms_akD3DFilterMapping[FILTER_BILERP_MIPNEAREST].m_eMin = D3DTEXF_LINEAR;
	ms_akD3DFilterMapping[FILTER_BILERP_MIPNEAREST].m_eMip = D3DTEXF_POINT;

}

EzD3DX9TextureStage::~EzD3DX9TextureStage(void)
{
}

void EzD3DX9TextureStage::setTextureBlendStageCount(u32 uiStage)
{
	m_uiLastMaxTextureBlendStages = uiStage;
}


void EzD3DX9TextureStage::setTransform(u32 state, const EzMatrix4& mat)
{
	m_pID3DDevice->SetTextureStageState(state, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	m_pID3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+ state),
			(D3DMATRIX*)((void*)mat.pointer()));
}


void EzD3DX9TextureStage::disableStage(u32 uiStage)
{
	while(uiStage < m_uiLastMaxTextureBlendStages)
	{
		m_pID3DDevice->SetTexture(uiStage, 0);
		m_pID3DDevice->SetTextureStageState(uiStage, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_pID3DDevice->SetTextureStageState(uiStage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		m_pID3DDevice->SetTextureStageState(uiStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		uiStage++;
	}
}

void EzD3DX9TextureStage::texturePipeline(EzTextureStage* pTexStage)
{
	u32 uiState;
	u32 uiValue;
	bool bResult;

	EzTexture* source = pTexStage->getTexture();
	u32 uiStage = pTexStage->getStage();
	EzTextureStageGroup* pTextureStageGroup = pTexStage->getTextureStageGroup();

	m_pID3DDevice->SetTexture(uiStage, ((const EzD3DX9Texture*) source)->getDX9Texture());


	bResult = pTextureStageGroup->getFirstStageState(uiState, uiValue);

	while(bResult)
	{
		if(EzD3DX9Mappings::get((ETEXTURESTAGESTATETYPE) uiState) == D3DTSS_TEXCOORDINDEX)
		{
			m_pID3DDevice->SetTextureStageState(uiStage, EzD3DX9Mappings::get((ETEXTURESTAGESTATETYPE) uiState), uiValue);
		}
		else if(EzD3DX9Mappings::get((ETEXTURESTAGESTATETYPE) uiState) == D3DTSS_TEXTURETRANSFORMFLAGS)
		{
			m_pID3DDevice->SetTextureStageState(uiStage, D3DTSS_TEXTURETRANSFORMFLAGS, EzD3DX9Mappings::get((ETEXTURETRANSFORMFLAGS) uiValue) );
			m_pID3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+ uiStage),(D3DMATRIX*)((void*)pTexStage->getTextureTransformation().pointer()));
		}
		else
		{
			m_pID3DDevice->SetTextureStageState(uiStage, EzD3DX9Mappings::get((ETEXTURESTAGESTATETYPE) uiState), EzD3DX9Mappings::get((ETEXTUREOP) uiValue));
		}

		bResult = pTextureStageGroup->getNextStageState(uiState, uiValue);
	}

	ETEXTURE_CLAMP				eClampMode = pTexStage->getTextureClampMode();
	ETEXTURE_FILTERMODE			eFilterMode = pTexStage->getTextureFilterMode();

	if(m_eLastTextureClampMode != eClampMode)
	{
		m_pID3DDevice->SetSamplerState(uiStage, D3DSAMP_ADDRESSU, ms_akD3DAddressMapping[eClampMode].m_eU );
		m_pID3DDevice->SetSamplerState(uiStage, D3DSAMP_ADDRESSV, ms_akD3DAddressMapping[eClampMode].m_eV );

		m_eLastTextureClampMode = eClampMode;
	}

	if(m_eLastTextureFilterMode != eFilterMode)
	{
		m_pID3DDevice->SetSamplerState(uiStage, D3DSAMP_MAGFILTER, ms_akD3DFilterMapping[eFilterMode].m_eMag);
		m_pID3DDevice->SetSamplerState(uiStage, D3DSAMP_MINFILTER, ms_akD3DFilterMapping[eFilterMode].m_eMin);
		m_pID3DDevice->SetSamplerState(uiStage, D3DSAMP_MIPFILTER, ms_akD3DFilterMapping[eFilterMode].m_eMag);

		m_eLastTextureFilterMode = eFilterMode;
	}

//			EzMatrix4  mat;
//
//			if(pTexDesc->tiling.X == 0)
//			{
//				pTexDesc->tiling.X = 1.0f;
//			}
//
//			if(pTexDesc->tiling.Y == 0)
//			{
//				pTexDesc->tiling.Y = 1.0f;
//			}
//
//			mat.setScale(EzPoint3(pTexDesc->tiling.X, pTexDesc->tiling.Y, 1.0f));
//			setTransform(uiStage, mat);
//			oldHasTextureTransform[EzTexturingProperty::BASE_MAP] = true;


}

//void EzD3DX9TextureStage::texturePipeline(EzTextureStage* pTexStage)
//{
//	static bool oldHasTextureTransform[EzTexturingProperty::INDEX_MAX];
//	static EzMatrix4  IDENTITY_Matrix;
//
//	u32 uiStage = 0;
//	EzTextureDesc* pTexDesc;
//
//	if(pTexProperty->hasTextureMap(EzTexturingProperty::BASE_MAP))
//	{
//		pTexDesc = pTexProperty->getTextureDesc(EzTexturingProperty::BASE_MAP);
//		EzTexture*  source = pTexDesc->source;
//
//		if(pTexDesc->hasTextureTransform)
//		{
//			EzMatrix4  mat;
//
//			if(pTexDesc->tiling.X == 0)
//			{
//				pTexDesc->tiling.X = 1.0f;
//			}
//
//			if(pTexDesc->tiling.Y == 0)
//			{
//				pTexDesc->tiling.Y = 1.0f;
//			}
//
//			mat.setScale(EzPoint3(pTexDesc->tiling.X, pTexDesc->tiling.Y, 1.0f));
//			setTransform(uiStage, mat);
//			oldHasTextureTransform[EzTexturingProperty::BASE_MAP] = true;
//		}
//		else
//		{
//
//
//			if(oldHasTextureTransform[EzTexturingProperty::BASE_MAP])
//				setTransform(uiStage, IDENTITY_Matrix);
//			
//			oldHasTextureTransform[EzTexturingProperty::BASE_MAP] = false;
//		}
//
//
//		setTextureDesc(uiStage, pTexDesc);
//
//		m_pID3DDevice->SetTexture(uiStage, ((const EzD3DX9Texture*) source)->getDX9Texture());
//		m_pID3DDevice->SetTextureStageState(uiStage, D3DTSS_TEXCOORDINDEX, pTexDesc->uvSet);
//		m_pID3DDevice->SetTextureStageState(uiStage, D3DTSS_TEXCOORDINDEX, 0);
//		m_pID3DDevice->SetTextureStageState (uiStage, D3DTSS_COLOROP, D3DTOP_MODULATE);
//		m_pID3DDevice->SetTextureStageState (uiStage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//		m_pID3DDevice->SetTextureStageState (uiStage, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//		m_pID3DDevice->SetTextureStageState (uiStage, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);
//		uiStage++;
//
//	}
//
//	if(pTexProperty->hasTextureMap(EzTexturingProperty::DARK_MAP))
//	{
//		pTexDesc = pTexProperty->getTextureDesc(EzTexturingProperty::DARK_MAP);
//		EzTexture*  source = pTexDesc->source;
//
//
//		if(pTexDesc->hasTextureTransform)
//		{
//			EzMatrix4  mat;
//
//			mat.setScale(EzPoint3(pTexDesc->tiling.X, pTexDesc->tiling.Y, 1.0f));
//			setTransform(uiStage, mat);
//			oldHasTextureTransform[EzTexturingProperty::DARK_MAP] = true;
//		}
//		else
//		{
//			if(oldHasTextureTransform[EzTexturingProperty::DARK_MAP])
//				setTransform(uiStage, IDENTITY_Matrix);
//
//			oldHasTextureTransform[EzTexturingProperty::DARK_MAP] = false;
//		}
//
//		setTextureDesc(uiStage, pTexDesc);
//		m_pID3DDevice->SetTexture(uiStage, ((const EzD3DX9Texture*) source)->getDX9Texture());
//		m_pID3DDevice->SetTextureStageState(uiStage, D3DTSS_TEXCOORDINDEX, pTexDesc->uvSet);
//
//		m_pID3DDevice->SetTextureStageState(uiStage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//		m_pID3DDevice->SetTextureStageState(uiStage, D3DTSS_COLORARG2, D3DTA_CURRENT);
//		m_pID3DDevice->SetTextureStageState(uiStage, D3DTSS_COLOROP, D3DTOP_MODULATE);
//
//		uiStage++;
//	}
//
//	disableStage(uiStage);
//
//
//	m_uiMaxTextureBlendStages = uiStage;
//
//
//}

