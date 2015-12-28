#pragma once


#include "../EzCore/EzCore.h"
#include "../EzMain3D/EzCamera.h"
#include "../EzCore/EzRefObject.h"
#include "../EzCore/EzRTTI.h"


class EzAVObject;
class EzNode;
	

class EzVideoDriver;

class EzRender : public EzRefObject
{
	EzDeclareRootRTTI(EzRender);

public:
	EzRender(EzVideoDriver* pDriver);
	~EzRender(void);

	void setCamera(EzCamera* pCamera);
	EzCamera* getActiveCamera() { return m_pActiveCamera; }

	EzVideoDriver* getDriver() { return m_pDriver; }

	inline void setTargetWindow(void* WindowId)
	{
		m_pDriver->setTargetWindow(WindowId);
	}

	bool setResize(void* WindowId, s32 iWidth, s32 iHigth)
	{
		m_pDriver->setResize(WindowId, iWidth, iHigth);
	}


	void begin();
	void end();



protected:

		struct TransparentNodeEntry
		{
			// jay adds function
			TransparentNodeEntry() : pObject(0), fDistance(0.0f) {}

			TransparentNodeEntry(EzAVObject* o, const EzPoint3& vCameraPos)
				: pObject(o)
			{
				//fDistance = (f32)(pObject->getAbsoluteTransformation().getTranslation().getDistanceFromSQ(vCameraPos));
			}

			bool operator < (const TransparentNodeEntry& other) const
			{
				return (fDistance > other.fDistance);
			}

			EzAVObject* pObject;
			private:
			f32 fDistance;
		};


		//! sort on distance (sphere) to camera
		struct DistanceNodeEntry
		{
			DistanceNodeEntry() : pObject(0), fDistance(0.0f) {}

			DistanceNodeEntry(EzAVObject* o, f64 d)
				: pObject(o), fDistance(d) {}

			DistanceNodeEntry(EzAVObject* o, const EzPoint3& vCameraPos)
				: pObject(o)
			{
//				fDistance = pObject->getAbsoluteTransformation().getTranslation().getDistanceFromSQ(vCameraPos);
				//fDistance -= pObject->getBoundingBox().getExtent().getLengthSQ() * 0.5;
			}

			bool operator < (const DistanceNodeEntry& other) const
			{
				return fDistance < other.fDistance;
			}

			EzAVObject* pObject;
			
			private:
			f64 fDistance;
		};


	EzArray<DistanceNodeEntry>						m_aLightList;
	EzArray<TransparentNodeEntry>					m_aTransparentNodeList;

	EzArray<EzAVObject*>			m_ShadowNodeList;
	EzArray<EzAVObject*>			m_SkyBoxList;
	EzArray<EzAVObject*>			m_aSolidNodeList;
	


	EzList<EzAVObject*>					m_NodeList;




	EzColor											m_AmbientLight;

	EzVideoDriver*								m_pDriver;
	EzCamera*							m_pActiveCamera;
	
	EzPoint3										m_vCamWorldPos;



};

