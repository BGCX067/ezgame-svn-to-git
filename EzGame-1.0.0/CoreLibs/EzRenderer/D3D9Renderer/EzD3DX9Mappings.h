#pragma once


#include "../EzVertexElement.h"
#include "../Ez3DBuffer.h"
#include "../EzIndexBuffer.h"
#include "../../EzCore/EzEnumTypes.h"
#include "../../EzCore/EzColor.h"
#include <d3d9.h>
#include <d3dx9.h>


class EzD3DX9Mappings
{
public:
	EzD3DX9Mappings(void);
	~EzD3DX9Mappings(void);

	static D3DFORMAT get(EzIndexBuffer::EIndexType eIndexType);
	static D3DDECLTYPE get(EzVertexElement::EVertexElementType eType);
	static D3DDECLUSAGE get(EzVertexElement::EVertexElementSemantic eSemantic);
	static UINT get(Ez3DBuffer::EUsage eUsage);
	static UINT get(Ez3DBuffer::ELockOptions eOptions, Ez3DBuffer::EUsage eUsage);
	static D3DMULTISAMPLE_TYPE get(EMULTISAMPLE_TYPE eType);
	static D3DFORMAT get(ECOLOR_FORMAT format);
	static ECOLOR_FORMAT get(D3DFORMAT format);
	static D3DCOLORVALUE get(EzColor colorf);
	static D3DTEXTUREOP get(ETEXTURE_APPLY_MODE eMode);
	static D3DTEXTURESTAGESTATETYPE get(ETEXTURESTAGESTATETYPE eStage);
	static D3DTEXTUREOP get(ETEXTUREOP eStage);
	static D3DTEXTURETRANSFORMFLAGS get(ETEXTURETRANSFORMFLAGS eStage);

	
	


};


inline D3DCOLORVALUE EzD3DX9Mappings::get(EzColor colorf)
{
	D3DCOLORVALUE D3dColor;

	D3dColor.r = colorf.r;
	D3dColor.g = colorf.g;
	D3dColor.b = colorf.b;
	D3dColor.a = colorf.a;

	return D3dColor;


}
