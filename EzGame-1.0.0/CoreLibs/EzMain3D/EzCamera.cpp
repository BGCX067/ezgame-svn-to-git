#include "EzCamera.h"
#include <stdio.h>

//#include "../EzRenderer/EzRender.h"
//#include "../EzRenderer/EzVideoDriver.h"

EzImplementRTTI(EzCamera, EzAVObject);

EzCamera::EzCamera(void)
: m_vUpVector(0.0f, 1.0f, 0.0f), m_fZNear(1.0f), m_fZFar(3000.0f),
	 m_bTargetAndRotationAreBound(false)
{
		// set default projection
	m_fFovy = PI / 5.5f;	// Field of view, in radians. 
	m_fAspect = 4.0f / 3.0f;	// Aspect ratio. 

	recalculateProjectionMatrix();
	recalculateViewArea();
}

EzCamera::~EzCamera(void)
{
}


void EzCamera::update(float fTime, bool bUpdateControllers)
{
	updateWorldData();

	EzPoint3 pos = m_World.getTranslation();
	EzPoint3 tgtv = m_vTarget - pos;
	tgtv.normalize();

	// if upvector and vector to the target are the same, we have a
	// problem. so solve this problem:
	EzPoint3 up = m_vUpVector;
	up.normalize();

	float dp = tgtv.dotProduct(up);

	if ( equals(fabsf(dp), 1.f) )
	{
		up.X += 0.5f;
	}

	m_ViewArea.Matrices[ETS_VIEW].buildCameraLookAtMatrixLH(pos, m_vTarget, up);
	m_ViewArea.setTransformState(ETS_VIEW);
	recalculateViewArea();

}

