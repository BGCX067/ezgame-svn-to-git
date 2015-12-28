#include "EzCullingProcess.h"
#include "EzAVObject.h"
#include "EzCamera.h"
#include "../EzCore/EzMeasurePerformance.h"


EzImplementRootRTTI(EzCullingProcess);



EzCullingProcess::EzCullingProcess(void)
: m_pCamera(0)
{
}

EzCullingProcess::~EzCullingProcess(void)
{
}


void EzCullingProcess::process(const EzCamera* pCamera, EzAVObject* pScene, Ez3DVisibleArray* pVisibleSet)
{
	EZMEASURE_PERFORMANCE_SCOPETIMER(CULLING_TIME);

	Ez3DVisibleArray* pSaveVisibleSet = NULL;
	m_pCamera = pCamera;
	
	if (pVisibleSet)
	{
		pVisibleSet->set_used(0);

		pSaveVisibleSet = m_pVisibleSet;
		m_pVisibleSet = pVisibleSet;
	}

	if(pVisibleSet && m_pCamera)
		pScene->culled(*this);

	if (pVisibleSet)
		m_pVisibleSet = pSaveVisibleSet;

	m_pCamera = 0;

	
}

//! returns if AVObject is culled
void EzCullingProcess::process(EzAVObject* pAVObject)
{
	switch ( pAVObject->getAutomaticCulling() ) 
	{
	// can be seen by a bounding box ?
	case EAC_BOX:
		{
			EzBound Bound = pAVObject->getWorldBound(); 
			EzAabbox3d tbox = Bound.getBoundBox();
			
			if(tbox.intersectsWithBox(m_pCamera->getViewFrustum().getBoundingBox() ))
			{
				pAVObject->onVisible(*this);

			}
		}
		break;

	// can be seen by a bounding sphere
	case EAC_FRUSTUM_SPHERE:
		{ // requires bbox diameter

			int i;
			int iSide;
			EzBound Bound = pAVObject->getWorldBound(); 
			SViewFrustum frust = m_pCamera->getViewFrustum();

			for (i=0; i<SViewFrustum::VF_PLANE_COUNT; ++i)
			{
				if(Bound.classifyPlaneRelation(frust.planes[i]) != ISREL3D_FRONT)
				{
					continue;
				}

				break;
			}

			if(i == SViewFrustum::VF_PLANE_COUNT)
			{
				pAVObject->onVisible(*this);
			}

		}
		break;

		// can be seen by cam pyramid planes ?
	case EAC_FRUSTUM_BOX:
		{

			EzBound Bound = pAVObject->getWorldBound(); 
			EzAabbox3d tbox = Bound.getBoundBox();
				
			SViewFrustum frust = m_pCamera->getViewFrustum();

			int i;

			for (i=0; i<SViewFrustum::VF_PLANE_COUNT; ++i)
			{
				if(tbox.classifyPlaneRelation( frust.planes[i] ) != ISREL3D_FRONT )
				{
					continue;
				}

				break;
			}

			if(i == SViewFrustum::VF_PLANE_COUNT)
			{
				pAVObject->onVisible(*this);
			}
		}
		break;

	case EAC_OFF:
		break;
	}

}
