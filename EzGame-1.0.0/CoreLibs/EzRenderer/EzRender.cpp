#include "EzRender.h"
#include "EzVideoDriver.h"
#include "../EzMain3D/EzNode.h"


EzImplementRootRTTI(EzRender);

EzRender::EzRender(EzVideoDriver* pDriver)
: m_pDriver(pDriver), m_pActiveCamera(0), m_AmbientLight(1.0f, 1.0f, 1.0f, 1.0f)
{
}

EzRender::~EzRender(void)
{
}




void EzRender::setCamera(EzCamera* pCamera)
{
	m_pActiveCamera = pCamera;
}

void EzRender::begin()
{
	m_pDriver->beginScene(true, true, SColor(255,100,101,140));


	m_pDriver->setTransform ( ETS_PROJECTION, IdentityMatrix );
	m_pDriver->setTransform ( ETS_VIEW, IdentityMatrix );
	m_pDriver->setTransform ( ETS_WORLD, IdentityMatrix );
	m_pDriver->setTransform ( ETS_TEXTURE_0, IdentityMatrix );
	m_pDriver->setTransform ( ETS_TEXTURE_1, IdentityMatrix );
	m_pDriver->setTransform ( ETS_TEXTURE_2, IdentityMatrix );
	m_pDriver->setTransform ( ETS_TEXTURE_3, IdentityMatrix );

	if(m_pActiveCamera)
	{
		m_pActiveCamera->update(0);

		m_pActiveCamera->render(m_pDriver);

		m_vCamWorldPos = m_pActiveCamera->getWorldPosition();
	}

}


void EzRender::end()
{
	m_pDriver->endScene();
}

