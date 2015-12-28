#pragma once

#include "EzMaxShader.h"

#define SHADER_NCOLBOX 4


class EzMaxShaderDialog : public ShaderParamDlg {
public:



	EzMaxShader*    m_pShader;
//	NiMAXPerTextureDialog* m_pTextureExportDlg;
	StdMat2*        m_pMtl;
	HPALETTE        m_hOldPal;
	HWND            m_hwmEdit;
	IMtlParams*     m_pMtlPar;
	HWND            m_hRollup;
	TimeValue       m_curTime;
	BOOL            m_valid;
	BOOL            m_isActive;
	TexDADMgr       m_dadMgr;

	IColorSwatch*	m_colorSwatch[SHADER_NCOLBOX];
	ISpinnerControl *m_pShininessSpinner;
	ISpinnerControl *m_pAlphaSpinner;
	ISpinnerControl *m_pTestRefSpinner2;
	ICustButton* m_texMBut[NMBUTS];


	EzMaxShaderDialog( HWND hwMtlEdit, IMtlParams *pParams);
	~EzMaxShaderDialog(void);


	// required for correctly operating map buttons
	int FindSubTexFromHWND(HWND hw);

	inline void SetThing(ReferenceTarget *m);
	inline void SetThings( StdMat2* theMtl, Shader* theShader );

	inline Class_ID ClassID();
	inline ReferenceTarget* GetThing();
	inline Shader* GetShader();

	inline void SetTime(TimeValue t);

	inline BOOL KeyAtCurTime(int id);
	inline void DeleteThis();
	inline void ActivateDlg( BOOL dlgOn );
	inline HWND GetHWnd();
	inline void NotifyChanged();
	inline void ReloadDialog();
	inline void UpdateDialog( ParamID paramId );
	inline void UpdateMtlDisplay();
	inline int ColorIDCToIndex(int idc);
	void SetMtlColor(int i, Color c, EzMaxShader* pShader, IColorSwatch** cs, TimeValue t);
	inline Color GetMtlColor( int i, EzMaxShader* pShader );
	inline TCHAR *GetColorName( int i );

	// Methods
	BOOL PanelProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam ); 

	void LoadDialog(BOOL draw);
	void UpdateColSwatches();
	void UpdateMapButtons();
	void UpdateOpacity();

	void SelectEditColor(int i) 
	{ 
		m_colorSwatch[ i ]->EditThis(FALSE); 
	}

protected:
	void FillShaderDropdown(HWND hWndParent);

private:
	static const char *ms_aApplyModes[ETEXTURE_APPLY_MODE::APPLY_MAX_MODES];
};


BOOL CALLBACK EzShaderDlgProc(HWND hwndDlg, 
									UINT msg, 
									WPARAM wParam, 
									LPARAM lParam);


inline void EzMaxShaderDialog::SetThing(ReferenceTarget *m)
{ 
	m_pMtl = (StdMat2*)m; 
}

inline void EzMaxShaderDialog::SetThings( StdMat2* theMtl, Shader* theShader )
{	
	if (m_pShader) 
		m_pShader->SetParamDlg(NULL);   

	m_pShader = (EzMaxShader*)theShader; 

	if (m_pShader)
		m_pShader->SetParamDlg(this); 

	m_pMtl = theMtl; 
}


inline Class_ID EzMaxShaderDialog::ClassID()
{ 
	return EzShaderClassID; 
}

inline ReferenceTarget* EzMaxShaderDialog::GetThing()
{ 
	return m_pMtl; 
} // mtl is the thing! (for DAD!)

inline Shader* EzMaxShaderDialog::GetShader()
{ 
	return m_pShader; 
}


inline void EzMaxShaderDialog::SetTime(TimeValue t) 
{
	//DS 2/26/99: added interval test to prevent redrawing when not necessary
	m_curTime = t; 
	if (!m_pShader->m_validInterval.InInterval(t)) 
	{
		Interval v;
		m_pShader->Update(t,v);
		LoadDialog(TRUE); 
	}
}		


inline BOOL EzMaxShaderDialog::KeyAtCurTime(int id) 
{ 
	return m_pShader->KeyAtTime(id,m_curTime); 
} 

inline void EzMaxShaderDialog::DeleteThis() 
{ 
	delete this; 
}

inline void EzMaxShaderDialog::ActivateDlg( BOOL dlgOn )
{ 
	m_isActive = dlgOn; 
}
inline HWND EzMaxShaderDialog::GetHWnd()
{ 
	return m_hRollup; 
}

inline void EzMaxShaderDialog::NotifyChanged()
{ 
	m_pShader->NotifyChanged(); 
}

inline void EzMaxShaderDialog::ReloadDialog()
{ 
	Interval v; 
	m_pShader->Update(m_pMtlPar->GetTime(), v); 
	LoadDialog(FALSE);
}

inline void EzMaxShaderDialog::UpdateDialog( ParamID paramId )
{ 
	ReloadDialog(); 
}

// redraw viewports
inline void EzMaxShaderDialog::UpdateMtlDisplay()
{ 
	m_pMtlPar->MtlChanged(); 
} 

inline int EzMaxShaderDialog::ColorIDCToIndex(int idc) 
{
	switch (idc) 
	{
		case IDC_CLR_AMBIENT: return 0;
		case IDC_CLR_DIFFUSE: return 1;
		case IDC_CLR_SPECULAR: return 2;
		case IDC_CLR_EMITTANCE: return 3;
		default: return 0;
	}
}

inline Color EzMaxShaderDialog::GetMtlColor( int i, EzMaxShader* pShader ) 
{
	switch(i) 
	{
	case 0:  return pShader->GetAmbientClr(0,FALSE); 
	case 1:  return pShader->GetDiffuseClr(0,FALSE);
	case 2:  return pShader->GetSpecularClr(0,FALSE);
	case 3:  return pShader->GetEmittance(0,FALSE);
	default: return Color(0,0,0);
	}
}


inline TCHAR *EzMaxShaderDialog::GetColorName( int i )
{
	switch(i) {
		case 0:  return GetString(IDS_MAT_AMBIENT);	 
		case 1:  return GetString(IDS_MAT_DIFFUSE);	 
		case 2:  return GetString(IDS_MAT_SPECULAR);	 
		case 3:  return GetString(IDS_MAT_EMITTANCE);	 
		default: return GetString(IDS_MAT_DIFFUSE);	 
	}
}


