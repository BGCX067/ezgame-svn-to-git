#pragma once

#include "../EzCore/EzString.h"
#include "../EzCore/EzRefObject.h"


class EzGeometry;
class EzRenderer;
class EzVideoDriver;
class EzPropertyState;

class EzShader : public EzRefObject
{
	EzDeclareRootRTTI(EzShader);

public:
	EzShader(void);
	~EzShader(void);

	virtual EzStringc getName() = 0;

	virtual unsigned int beginPipeline(EzGeometry* pGeometry);
	virtual unsigned int updatePipeline(EzGeometry* pGeometry, EzPropertyState* pPropState); 
    virtual unsigned int endPipeline(EzGeometry* pGeometry); 


	void setRenderer(EzRenderer* pRenderer);
	void setDriver(EzVideoDriver* pDriver);


protected:
	EzRenderer*		m_pRenderer;
	EzVideoDriver*	m_pDriver;

};

inline void EzShader::setDriver(EzVideoDriver* pDriver)
{
	m_pDriver = pDriver;
}



