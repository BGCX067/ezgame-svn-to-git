#pragma once



#include "EzAppMain.h"

class CSampleAPP3D : public EzApplication3D
{
public:

	virtual	bool preinitialize();
	virtual void createScene();
	virtual void update(float time);

	EzCostum* m_pCostum;
	EzActorManager m_act;

protected:
	void OnKeyUp(EKEY_CODE eKey, wchar_t Char, bool bShift, bool bControl);

};
