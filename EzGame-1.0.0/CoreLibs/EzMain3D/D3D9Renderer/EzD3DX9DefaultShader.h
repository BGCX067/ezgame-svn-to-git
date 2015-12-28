#pragma once

#include "EzD3DX9Shader.h"
#include "../../EzMain3d/EzMacros.h"

class EzD3DX9DefaultShader : public EzShader
{
	EzDeclareShader;

public:
	EzD3DX9DefaultShader(void);
	~EzD3DX9DefaultShader(void);

	virtual unsigned int beginPipeline(EzGeometry* pGeometry);
	virtual unsigned int updatePipeline(EzGeometry* pGeometry, EzPropertyState* pPropState); 
    virtual unsigned int endPipeline(EzGeometry* pGeometry); 

	virtual EzStringc getName();

protected:
	unsigned int constructPipeline(EzGeometry* pGeometry, EzPropertyState* pPropState);


};

inline EzStringc EzD3DX9DefaultShader::getName()
{
	return "Default Shader";
}
