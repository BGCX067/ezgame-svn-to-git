#pragma once


#include "../../EzCore/EzCore.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9shader.h>


class EzTexturingProperty;
class EzTextureDesc;
class EzTextureStage;

class EzD3DX9TextureStage
{
public:
	class  TextureAddressSettings
	{
	public:
		D3DTEXTUREADDRESS m_eU;
		D3DTEXTUREADDRESS m_eV;
	};

	class FilterMappingModes
	{
	public:
		D3DTEXTUREFILTERTYPE		m_eMin;
		D3DTEXTUREFILTERTYPE		m_eMag;
		D3DTEXTUREFILTERTYPE		m_eMip;
	};

	EzD3DX9TextureStage(IDirect3DDevice9* pID3DDevice, unsigned int uiMaxTextureBlendStages );
	~EzD3DX9TextureStage(void);

	void texturePipeline(EzTextureStage* pTexStage);
	void disableStage(unsigned int uiStage);
	void setTextureBlendStageCount(unsigned int uiStage);
	void setTransform(unsigned int state, const EzMatrix4& mat);

protected:

	ETEXTURE_FILTERMODE				m_eLastTextureFilterMode;
	ETEXTURE_CLAMP					m_eLastTextureClampMode;
	ETEXTURE_APPLY_MODE				m_eLastTextureApplyMode;

	IDirect3DDevice9*	m_pID3DDevice;

	static FilterMappingModes			ms_akD3DFilterMapping[FILTER_MAX_MODES];
	static TextureAddressSettings		ms_akD3DAddressMapping[ETC_CLAMP_MAX_MODES];

	unsigned int								m_uiLastMaxTextureBlendStages;

};




