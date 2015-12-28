#pragma once

#include "../../EzMain3D/EzShader.h"

class EzD3DX9Shader : public EzShader
{
public:
	EzD3DX9Shader(void);
	~EzD3DX9Shader(void);

	virtual u32 beginPipeline(EzGeometry* pGeometry);
	virtual u32 updatePipeline(EzGeometry* pGeometry); 
    virtual u32 endPipeline(EzGeometry* pGeometry); 

};
