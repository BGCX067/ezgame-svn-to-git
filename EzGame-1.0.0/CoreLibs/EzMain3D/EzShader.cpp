#include "EzShader.h"
#include "EzRenderer.h"

EzImplementRootRTTI(EzShader);

EzShader::EzShader(void)
: m_pRenderer(NULL), m_pDriver(NULL)
{
}

EzShader::~EzShader(void)
{
}

void EzShader::setRenderer(EzRenderer* pRenderer)
{
	m_pRenderer = pRenderer;

	if(pRenderer)
	{
		m_pDriver = pRenderer->getVideoDriver();
	}
	else
	{
		m_pDriver = NULL;
	}
}

unsigned int EzShader::beginPipeline(EzGeometry* pGeometry)
{
	return 0;
}

unsigned int EzShader::updatePipeline(EzGeometry* pGeometry, EzPropertyState* pPropState)
{
	return 0;
}
unsigned int EzShader::endPipeline(EzGeometry* pGeometry)
{
	return 0;
}