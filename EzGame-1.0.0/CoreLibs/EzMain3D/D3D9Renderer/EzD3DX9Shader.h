#pragma once

#include "../../EzMain3D/EzShader.h"

class EzD3DX9Shader : public EzShader
{
public:
	EzD3DX9Shader(void);
	~EzD3DX9Shader(void);

	virtual unsigned int beginPipeline(EzGeometry* pGeometry);
	virtual unsigned int updatePipeline(EzGeometry* pGeometry); 
    virtual unsigned int endPipeline(EzGeometry* pGeometry); 

};
