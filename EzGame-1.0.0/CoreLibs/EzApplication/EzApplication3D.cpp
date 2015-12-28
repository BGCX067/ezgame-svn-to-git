#include "EzApplication3D.h"
#include "EzDevice3DWin32.h"
#include "../EzMain3D/EzLight.h"
#include "../EzCore/EzDebugConsole.h"
#include "../EzCore/EzMeasurePerformance.h"
#include "../EzRenderer/EzRenderer.h"

#define AFX_DLL_IMPORT


EzImplementRTTI(EzApplication3D, EzApplication);

EzApplication3D::EzApplication3D()
: m_bZooming(false), m_bRotating(false), m_bTranslating(false),
	m_fZoomSpeed(5.0f), m_fRotateSpeed(-5.0f), m_fTranslateSpeed(5.0f), m_fMouseWheel(0),
	m_nRotateStartX(0), m_nRotateStartY(0), m_fZoomStartX(0), m_fZoomStartY(0),
	m_nTranslateStartX(0), m_nTranslateStartY(0), m_fCurrentZoom(100.0f), m_fRotX(0.0f), m_fRotY(0.0f)
	, m_pScene(0), m_bUseFixedTime(false), m_fLastTime(-1.0f), m_uiFramesCounted(0), m_fLastFrame(0)
{

	m_fMinFramePeriod = 1.0f / 10000.0f;

	for(u16 n = 0; n < MAX_MOUSE_BUTTON; ++n)
	{
		m_bMouseDown[n] = false;
	}
}

EzApplication3D::~EzApplication3D(void)
{
}

void EzApplication3D::setMaxFrameRate(f32 fMax)
{
    // convert from rate to period - if a bogus max framerate is passed in,
    // disable framerate limitation
    if(fMax < 1e-5f)
        m_fMinFramePeriod = 1e-5f;    
    else
        m_fMinFramePeriod = 1.0f / fMax;    
}

bool EzApplication3D::measureTime()
{
    if (!m_bUseFixedTime)
    {
        // start performance measurements
        if (m_fLastTime == -1.0f)
        {
            m_fLastTime = getTimeSec();
            m_fAccumTime = 0.0f;
            m_uiFramesCounted = 0;
        }

        // measure time
        m_fCurrentTime = getTimeSec();
        float fDeltaTime = m_fCurrentTime - m_fLastTime;

//#if defined(WIN32)
//        // NVPerfHUD support!
//        if (m_bNVPerfHUD && fDeltaTime == 0.0f)
//            return true;
//#endif

        if (fDeltaTime < 0.0f)
            fDeltaTime = 0.0f;
        m_fLastTime = m_fCurrentTime;
        m_fAccumTime += fDeltaTime;

        // frame rate limiter
        if (m_fAccumTime < (m_fLastFrame + m_fMinFramePeriod))
            return false;

        m_fFrameTime = m_fAccumTime - m_fLastFrame;
        m_fLastFrame = m_fAccumTime;

        return true;
    }
    else
    {
        m_fCurrentTime += m_fFixedTimeIncrement;
        m_fAccumTime = ((float)m_uiFramesCounted) * m_fFixedTimeIncrement;      
        m_fFrameTime = m_fAccumTime - m_fLastFrame;
        m_fLastFrame = m_fAccumTime;

        return true;
    }
}


bool EzApplication3D::initialize()
{

	if(!preinitialize())
	{
		return false;
	}

	m_pDevice = create3DDeviceEx(m_param);

	if(!m_pDevice)
	{
		return false;
	}
	
	EzCamera* pCamera = new EzCamera;
	pCamera->setName("Default Camera");
	pCamera->setPosition(EzPoint3(0, 0, 0));
	pCamera->setTarget(EzPoint3(0, 0, 0));
	pCamera->setAspectRatio((f32)(800 / 600));
	pCamera->update(0.0f);

	getRender()->setCamera(pCamera);

	m_pCulling = EzNew EzCullingProcess;

	m_pScene = EzNew EzNode(0);
	
	return true;
}

void EzApplication3D::OnRenderStart(EzRenderer* pRender)
{
	m_pCulling->process(getRender()->getActiveCamera(), m_pScene, &m_Visible);
}

void EzApplication3D::OnRenderEnd(EzRenderer* pRender)
{
	char fps[255];

	sprintf_s(fps, 255, "FPS = %5.2f PrimitiveCount = %f Culling Time = %f skinning time = %f Draw Visible = %d"
						, m_fFrameRate, 
						EZMEASURE_PERFORMANCE_VALUE(PRIMITIVE_COUNT), 
						EZMEASURE_PERFORMANCE_VALUE(CULLING_TIME), 
						EZMEASURE_PERFORMANCE_VALUE(SKINNINGTIME), 
						m_Visible.size());

	m_pDevice->setWindowCaption(fps);
}


void EzApplication3D::OnCameraMoveWorking(EzCamera *pCam)
{
	if(pCam)
	{
		const SViewFrustum va = pCam->getViewFrustum();

		f32 fRotX = m_fRotX;
		f32 fRotY = m_fRotY;
		f32 fZoom = m_fCurrentZoom;

		EzPoint3	vUpVector;
		EzPoint3	vCamPos;
		EzPoint3	vCamTarget;

		vCamPos = pCam->getPosition();
		vCamTarget = pCam->getTarget();
		vUpVector = pCam->getUpVector();

		if ( m_fMouseWheel )
		{
			m_fCurrentZoom += m_fMouseWheel * (m_fZoomSpeed);

			f32 targetMinDistance = 0.1f;

			if (m_fCurrentZoom < targetMinDistance) // jox: fixed bug: bounce back when zooming to close
				m_fCurrentZoom = targetMinDistance;

			m_fMouseWheel = 0;
		}


		if (  isMouseKeyDown(1) )
		{
			if (!m_bZooming)
			{
				m_fZoomStartX = m_vMousePressPos.X;
				m_fZoomStartY = m_vMousePressPos.Y;
				m_bZooming = true;
				fZoom = m_fCurrentZoom;
			}
			else
			{
				f32 old = fZoom;
				fZoom += (m_fZoomStartX - m_vMousePressPos.X) * m_fZoomSpeed;

				f32 targetMinDistance = 0.1f;
				if (fZoom < targetMinDistance) // jox: fixed bug: bounce back when zooming to close
					fZoom = targetMinDistance;

				if (fZoom < 0)
					fZoom = old;
			}
		}
		else
		{
			if (m_bZooming)
			{
				f32 old = m_fCurrentZoom;
				m_fCurrentZoom = m_fCurrentZoom + (m_fZoomStartX - m_vMousePressPos.X ) * m_fZoomSpeed;
				fZoom = m_fCurrentZoom;

				if (fZoom < 0)
					fZoom = m_fCurrentZoom = old;
			}

			m_bZooming = false;
		}

		// Translation ---------------------------------
		EzPoint3 vTranslate(m_vOldTarget);

		EzPoint3 tvectX = vCamPos - vCamTarget;
	
		tvectX = tvectX.crossProduct(vUpVector);
		tvectX.normalize();

		EzPoint3 tvectY = (va.getFarLeftDown() - va.getFarRightDown());
		tvectY = tvectY.crossProduct(vUpVector.Y > 0 ? vCamPos - vCamTarget : vCamTarget - vCamPos);
		tvectY.normalize();

		if (isMouseKeyDown(2) && !m_bZooming)
		{
			if (!m_bTranslating)
			{
				m_nTranslateStartX = m_vMousePressPos.X;
				m_nTranslateStartY = m_vMousePressPos.Y;
				m_bTranslating = true;
			}
			else
			{

				vTranslate +=	tvectX * (f32)((m_vMousePressPos.X - m_nTranslateStartX ))*m_fTranslateSpeed + 
									tvectY * (f32)((m_vMousePressPos.Y - m_nTranslateStartY))*m_fTranslateSpeed;
			}
		}
		else
		{
			if (m_bTranslating)
			{
				vTranslate +=	tvectX * (f32)((m_vMousePressPos.X - m_nTranslateStartX))*m_fTranslateSpeed + 
								tvectY * (f32)((m_vMousePressPos.Y - m_nTranslateStartY))*m_fTranslateSpeed;
				m_vOldTarget = vTranslate;
			}

			m_bTranslating = false;
		}

		// Rotation ------------------------------------
		if (isMouseKeyDown(0) && !m_bZooming)
		{
			if (!m_bRotating)
			{
				m_nRotateStartX = m_vMousePressPos.X;
				m_nRotateStartY = m_vMousePressPos.Y;
				fRotX = m_fRotX;
				fRotY = m_fRotY;
				m_bRotating = true;
			}
			else
			{
				fRotX += (m_nRotateStartX - m_vMousePressPos.X) * m_fRotateSpeed;
				fRotY += (m_nRotateStartY - m_vMousePressPos.Y) * m_fRotateSpeed;
			}
		}
		else
		{
			if (m_bRotating)
			{
				m_fRotX = m_fRotX + (m_nRotateStartX - m_vMousePressPos.X) * m_fRotateSpeed;
				m_fRotY = m_fRotY + (m_nRotateStartY - m_vMousePressPos.Y) * m_fRotateSpeed;
				fRotX = m_fRotX;
				fRotY = m_fRotY;
			}

			m_bRotating = false;
		}

	// Set Pos ------------------------------------

		vCamTarget = vTranslate;

		vCamPos.X = fZoom + vCamTarget.X;
		vCamPos.Y = vCamTarget.Y;
		vCamPos.Z = vCamTarget.Z;

		vCamPos.rotateXYBy(fRotY, vCamTarget);
		vCamPos.rotateXZBy(-fRotX, vCamTarget);

		// jox: fixed bug: jitter when rotating to the top and bottom of y
		vUpVector.set(0,1,0);
		vUpVector.rotateXYBy(-fRotY, EzPoint3(0,0,0));
		vUpVector.rotateXZBy(-fRotX+180.f, EzPoint3(0,0,0));

		pCam->setUpVector(vUpVector);
		pCam->setPosition(vCamPos);
		pCam->setTarget(vCamTarget);

	}

}


void EzApplication3D::excute()
{
	if(m_pDevice)
	{
		createScene();

		while(m_pDevice->run())
		{
			if (!measureTime())
			{
				m_pDevice->yield();
				continue;
			}


			OnCameraMoveWorking(getRender()->getActiveCamera());

			update(m_fAccumTime);

			OnRenderStart(getRender());

			OnRender(getRender());

			OnRenderEnd(getRender());

			m_uiFramesCounted++;

			m_fFrameRate = 1.0f / getFrameTime();

			EZMEASURE_PERFORMANCE_RECORDMETRICS();

		}

	}

}



void EzApplication3D::OnRender(EzRenderer* pRender)
{

	getRender()->begin();

	for(u32 i = 0; i < m_Visible.size(); i++)
	{
		m_Visible[i]->render(pRender);

	}

	getRender()->end();

}



void EzApplication3D::OnKeyDown(EKEY_CODE eKey, wchar_t Char, bool bShift, bool bControl)
{

}

void EzApplication3D::OnKeyUp(EKEY_CODE eKey, wchar_t Char, bool bShift, bool bControl)
{

}

void EzApplication3D::OnUserEvent(const SEvent::SUserEvent &UserEvent)
{

}

void EzApplication3D::OnLogEvent(const SEvent::SLogEvent &LogEvent)
{

	CSP(LogEvent.Text);
	CSP("\r\n");

//	sendMonitor(LogEvent.Text, strlen(LogEvent.Text));

}

void EzApplication3D::OnKeyEvent(const SEvent::SKeyInput &KeyEvent)
{
	if(KeyEvent.bPressedDown)
	{
		OnKeyDown(KeyEvent.eKey, KeyEvent.Char, KeyEvent.bShift, KeyEvent.bControl);
	}
	else
	{
		OnKeyUp(KeyEvent.eKey, KeyEvent.Char, KeyEvent.bShift, KeyEvent.bControl);
	}
}
void EzApplication3D::OnMouseEvent(const SEvent::SMouseInput &MouseEvent)
{
	switch(MouseEvent.Event)
	{
	case EMIE_LMOUSE_PRESSED_DOWN:
		OnMouseLDown(MouseEvent.X, MouseEvent.Y);
		break;
	case EMIE_RMOUSE_PRESSED_DOWN:
		OnMouseRDown(MouseEvent.X, MouseEvent.Y);
		break;
	case EMIE_MMOUSE_PRESSED_DOWN:
		OnMouseMDown(MouseEvent.X, MouseEvent.Y);
		break;
	case EMIE_LMOUSE_LEFT_UP:
		OnMouseLUp(MouseEvent.X, MouseEvent.Y);
		break;
	case EMIE_RMOUSE_LEFT_UP:
		OnMouseRUp(MouseEvent.X, MouseEvent.Y);
		break;
	case EMIE_MMOUSE_LEFT_UP:
		OnMouseMUp(MouseEvent.X, MouseEvent.Y);
		break;
	case EMIE_MOUSE_DOWN_MOVED:
		OnMouseDownMove(MouseEvent.X, MouseEvent.Y);
		break;
	case EMIE_MOUSE_MOVED:
		OnMouseMove(MouseEvent.X, MouseEvent.Y);
		break;
	case EMIE_MOUSE_WHEEL:
		OnMouseWheel(MouseEvent.X, MouseEvent.Y, MouseEvent.fWheel);
		break;
	}
}



void EzApplication3D::OnMouseDownMove(s32 x, s32 y)
{
	m_vMousePressPos.set(x, y);
}

void EzApplication3D::OnMouseMove(s32 x, s32 y)
{
	m_vMousePos.set(x, y);
}

void EzApplication3D::OnMouseLDown(s32 x, s32 y)
{
	m_vMousePos.set(x, y);
	m_vMousePressPos.set(x, y);
	m_bMouseDown[0] = true;
}

void EzApplication3D::OnMouseMDown(s32 x, s32 y)
{
	m_vMousePos.set(x, y);
	m_vMousePressPos.set(x, y);
	m_bMouseDown[1] = true;
}

void EzApplication3D::OnMouseRDown(s32 x, s32 y)
{
	m_vMousePos.set(x, y);
	m_vMousePressPos.set(x, y);
	m_bMouseDown[2] = true;
}

void EzApplication3D::OnMouseLUp(s32 x, s32 y)
{
	m_vMousePos.set(x, y);
	m_vMousePressPos.set(x, y);
	m_bMouseDown[0] = false;
}

void EzApplication3D::OnMouseMUp(s32 x, s32 y)
{
	m_vMousePos.set(x, y);
	m_vMousePressPos.set(x, y);
	m_bMouseDown[1] = false;
}

void EzApplication3D::OnMouseRUp(s32 x, s32 y)
{
	m_vMousePos.set(x, y);
	m_vMousePressPos.set(x, y);
	m_bMouseDown[2] = false;
}

void EzApplication3D::OnMouseWheel(s32 x, s32 y, f32 fWheel)
{
	m_vMousePos.set(x, y);
	m_vMousePressPos.set(x, y);
	m_fMouseWheel = fWheel;
}

