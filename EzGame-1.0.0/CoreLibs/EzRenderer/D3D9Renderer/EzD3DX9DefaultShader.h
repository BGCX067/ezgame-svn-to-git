#pragma once

#include "EzD3DX9Shader.h"
#include "../../EzMain3d/EzMacros.h"

class EzD3DX9DefaultShader : public EzShader
{
	EzDeclareShader;

public:
	EzD3DX9DefaultShader(void);
	~EzD3DX9DefaultShader(void);

	virtual u32 beginPipeline(EzGeometry* pGeometry);
	virtual u32 updatePipeline(EzGeometry* pGeometry, EzPropertyState* pPropState); 
    virtual u32 endPipeline(EzGeometry* pGeometry); 

	virtual EzStringc getName();

protected:
	u32 constructPipeline(EzGeometry* pGeometry, EzPropertyState* pPropState);


};

inline EzStringc EzD3DX9DefaultShader::getName()
{
	return "Default Shader";
}
