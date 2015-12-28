#pragma once

#include "../EzCore/EzRefObject.h"
#include "../EzCore/EzRTTI.h"
#include "../EzRenderer/EzRender.h"
#include "../EzMain3D/EzCullingProcess.h"
#include "../EzMain3D/EzNode.h"

#include "SEvent.h"
#include "EzApplication.h"
#include "EzDevice.h"
#include "SCreationParameters3D.h"


#define  MAX_MOUSE_BUTTON			3

class EzApplication3D : public EzApplication
{
	EzDeclareRTTI;

public:
	~EzApplication3D(void);

	bool initialize();

	virtual void createScene() = 0;
	virtual void update(float time) = 0;
	void excute();
	

	EzNode* getScene() { return m_pScene; }

	virtual	void OnCameraMoveWorking(EzCamera *pCam);

	EzRenderer* getRender() 
	{ 
		return (EzRenderer*) getDevice()->getRender();
	} 
	EzVideoDriver* getVideoDriver() 
	{ 
		return (EzVideoDriver*) getDevice()->getVideoDriver(); 
	}

	bool isMouseKeyDown(u16 uiIdx) 
	{ 
		return m_bMouseDown[uiIdx]; 
	}

	void setMaxFrameRate(f32 fMax);

	inline f32 getFrameTime() 
	{ 
		return m_fFrameTime; 
	}



protected:
	void OnKeyEvent(const SEvent::SKeyInput &KeyEvent);
	void OnUserEvent(const SEvent::SUserEvent &UserEvent);
	void OnLogEvent(const SEvent::SLogEvent &LogEvent);
	void OnMouseEvent(const SEvent::SMouseInput &MouseEvent);


	EzApplication3D();

	void setCreateOption(const SCreationParameters3D& param)
	{
		m_param = param;

	}

	virtual	void OnRenderStart(EzRenderer* pRender);
	virtual	void OnRenderEnd(EzRenderer* pRender);
	virtual	void OnRender(EzRenderer* pRender);


	virtual	void OnKeyDown(EKEY_CODE eKey, wchar_t Char, bool bShift, bool bControl);
	virtual	void OnKeyUp(EKEY_CODE eKey, wchar_t Char, bool bShift, bool bControl);

	virtual	void OnMouseMove(s32 x, s32 y);
	virtual	void OnMouseLDown(s32 x, s32 y);
	virtual	void OnMouseMDown(s32 x, s32 y);
	virtual	void OnMouseRDown(s32 x, s32 y);
	virtual	void OnMouseLUp(s32 x, s32 y);
	virtual	void OnMouseMUp(s32 x, s32 y);
	virtual	void OnMouseRUp(s32 x, s32 y);
	virtual	void OnMouseWheel(s32 x, s32 y, f32 fWheel);
	virtual	void OnMouseDownMove(s32 x, s32 y);


	f32							m_fFrameRate;
	f32							m_fAccumTime; // Time elapsed since application start
	f32							m_fLastTime;
	f32							m_fFrameTime;
	f32							m_fLastFrame;
	f32							m_fCurrentTime;
	f32							m_fFixedTimeIncrement;
	f32							m_fMinFramePeriod;
	bool						m_bUseFixedTime;
	s32							m_uiFramesCounted;

protected:

	bool measureTime();


	bool m_bMouseDown[MAX_MOUSE_BUTTON];



	EzPoint3	m_vOldTarget;
	EzPoint2		m_vMousePos;
	EzPoint2		m_vMousePressPos;


	f32			m_fMouseWheel;
	f32			m_fCurrentZoom;

	bool		m_bZooming;
	bool		m_bTranslating;

	s32			m_fZoomStartX;
	s32			m_fZoomStartY;
	f32			m_fZoomSpeed;
	
	f32			m_fTranslateSpeed;
	s32			m_nTranslateStartX;
	s32			m_nTranslateStartY;

	bool		m_bRotating;
	f32			m_fRotateSpeed;
	s32			m_nRotateStartX;
	s32			m_nRotateStartY;
	f32			m_fRotX;
	f32			m_fRotY;




	HWND createWindows(bool bFullScreen);

	EzCullingProcessPtr			m_pCulling;
	Ez3DVisibleArray			m_Visible;

	EzNodePtr					m_pScene;

	SCreationParameters3D		m_param;






};



