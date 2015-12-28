#include "../EzMaxPluginPch.h"
#include "../EzMax.h"
#include "EzMaxShaderDialog.h"




// tex channel number to button IDC
static int texMButtonsIDC[] = 
{
	IDC_MAP_DIFFUSE, 
};

const char *
EzMaxShaderDialog::ms_aApplyModes[ETEXTURE_APPLY_MODE::APPLY_MAX_MODES] =
{ 
	"REPLACE",
	"DECAL",
	"MODULATE",
};



// This array gives the texture map number for given MButton number								
static int texmapFromMBut[] = { 0 };

static int colID[SHADER_NCOLBOX] = { IDC_CLR_AMBIENT, IDC_CLR_DIFFUSE, IDC_CLR_SPECULAR, IDC_CLR_EMITTANCE  };

BOOL CALLBACK EzShaderDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	EzMaxShaderDialog *theDlg;

	if (msg == WM_INITDIALOG) 
	{
		theDlg = (EzMaxShaderDialog *) lParam;
		SetWindowLong(hwndDlg, GWL_USERDATA, lParam);
	}
	else if (msg == WM_CLOSE || msg == WM_DESTROY) 
	{
		SetWindowLong(hwndDlg, GWL_USERDATA, NULL);
		return FALSE;
	}
	else 
	{
		theDlg = (EzMaxShaderDialog *) GetWindowLong(hwndDlg, GWL_USERDATA);

		if (theDlg == NULL)
			return FALSE; 
	}

	theDlg->m_isActive = 1;
	BOOL res = theDlg->PanelProc(hwndDlg, msg, wParam, lParam);

	// we need to make sure we didn't accidentally cause the shader
	// dialog to die here
	theDlg = (EzMaxShaderDialog *) GetWindowLong(hwndDlg, GWL_USERDATA);
	if (theDlg == NULL)
		return res; 
	theDlg->m_isActive = 0;
	return res;
}


EzMaxShaderDialog::EzMaxShaderDialog( HWND hwMtlEdit, IMtlParams *pParams)
{

	m_pMtl = NULL;
	m_pShader = NULL;
	m_hwmEdit = hwMtlEdit;
	m_pMtlPar = pParams;
	m_dadMgr.Init(this);
	m_hRollup = NULL;
	m_curTime = 0;
	m_isActive = m_valid = FALSE;

	m_pShininessSpinner = NULL;
	m_pAlphaSpinner = NULL;
	m_pTestRefSpinner2 = NULL;


	long i = 0;
	for( ; i < SHADER_NCOLBOX; ++i )
		m_colorSwatch[ i ] = NULL;

	for( i = 0; i < NMBUTS; ++i )
		m_texMBut[ i ] = NULL;

}

EzMaxShaderDialog::~EzMaxShaderDialog(void)
{
	HDC hdc = GetDC(m_hRollup);
	GetGPort()->RestorePalette(hdc, m_hOldPal);
	ReleaseDC(m_hRollup, hdc);

	if ( m_pShader ) 
		m_pShader->SetParamDlg(NULL);

	for (long i=0; i < NMBUTS; i++ )
	{
		ReleaseICustButton( m_texMBut[i] );
		m_texMBut[i] = NULL; 
	}

	ReleaseISpinner(m_pShininessSpinner);
	ReleaseISpinner(m_pAlphaSpinner);
	ReleaseISpinner(m_pTestRefSpinner2);

	SetWindowLong(m_hRollup, GWL_USERDATA, NULL);
	m_hRollup = NULL;
}


void  EzMaxShaderDialog::LoadDialog(BOOL draw) 
{
	if (m_pShader && m_hRollup) 
	{
		m_pShininessSpinner->SetValue(m_pShader->GetShininess(0, 0),FALSE);
		m_pShininessSpinner->SetKeyBrackets(KeyAtCurTime(sk_shininess));

		m_pAlphaSpinner->SetValue(m_pShader->GetAlpha(0, FALSE), FALSE);
		m_pAlphaSpinner->SetKeyBrackets(KeyAtCurTime(sk_alpha));

		HWND hwndItem;


		hwndItem = GetDlgItem(m_hRollup, IDC_CBO_APPLY_MODE);
		SendMessage(hwndItem, CB_SETCURSEL, 
			m_pShader->GetTextureApplyMode(0, FALSE), 0);

		FillShaderDropdown(m_hRollup);


		UpdateColSwatches();
	}
}


static TCHAR* mapStates[] = { _T(" "), _T("m"),  _T("M") };

void EzMaxShaderDialog::UpdateMapButtons() 
{

	for ( long i = 0; i < NMBUTS; ++i ) {
		int nMap = texmapFromMBut[ i ];
		int state = m_pMtl->GetMapState( nMap );
		m_texMBut[i]->SetText( mapStates[ state ] );

		TSTR nm = m_pMtl->GetMapName( nMap );
		m_texMBut[i]->SetTooltip(TRUE,nm);
	}
}

// Necessary - but unused
void EzMaxShaderDialog::UpdateOpacity() 
{
}

void EzMaxShaderDialog::UpdateColSwatches() 
{

	m_colorSwatch[0]->SetKeyBrackets( m_pShader->KeyAtTime(sk_amb, m_curTime) );

	m_colorSwatch[0]->SetColor( m_pShader->GetAmbientClr(0, FALSE) );


	m_colorSwatch[1]->SetKeyBrackets( m_pShader->KeyAtTime( sk_diff, m_curTime) );
	m_colorSwatch[1]->SetColor( m_pShader->GetDiffuseClr(0, FALSE) );

	m_colorSwatch[2]->SetKeyBrackets( m_pShader->KeyAtTime( sk_spec, m_curTime) );
	m_colorSwatch[2]->SetColor( m_pShader->GetSpecularClr(0, FALSE) );

	m_colorSwatch[3]->SetKeyBrackets( m_pShader->KeyAtTime( sk_emit, m_curTime) );
	m_colorSwatch[3]->SetColor( m_pShader->GetEmittance(0, FALSE));

}


int EzMaxShaderDialog::FindSubTexFromHWND(HWND hw) 
{
	for (long i=0; i<NMBUTS; i++) 
	{
		if (hw == m_texMBut[i]->GetHwnd()) 
			return texmapFromMBut[i];
	}	
	return -1;
}


void EzMaxShaderDialog::SetMtlColor(int i, Color c, EzMaxShader* pShader, IColorSwatch** cs, TimeValue t)
{
	switch(i) {
case 0: //ambient
	pShader->SetAmbientClr(c,t); 
	break;
case 1: //diffuse
	pShader->SetDiffuseClr(c,t); 
	break;
case 2: // specular
	pShader->SetSpecularClr(c,t); 
	break;
case 3: 
	pShader->SetEmittance(c,t); 
	break;
	}
}


BOOL EzMaxShaderDialog::PanelProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam ) 
{
	int id = LOWORD(wParam);
	int code = HIWORD(wParam);
	int j;
	switch (msg) {
		case WM_INITDIALOG:
			{

				HDC theHDC = GetDC(hwndDlg);
				m_hOldPal = GetGPort()->PlugPalette(theHDC);
				ReleaseDC(hwndDlg,theHDC);

				for(int n = 0; n < SHADER_NCOLBOX; ++n)
				{
					m_colorSwatch[n] = GetIColorSwatch( GetDlgItem(hwndDlg, colID[n]),
						GetMtlColor(n, m_pShader), GetColorName(n));
				}


				m_pShininessSpinner = SetupIntSpinner(hwndDlg, IDC_SPN_SHININESS, IDC_EDT_SHININESS, 0,100, 0);
				m_pAlphaSpinner = SetupIntSpinner(hwndDlg, IDC_SPN_ALPHA, IDC_EDT_ALPHA, 0,100, 0);

				for (j=0; j<NMBUTS; j++) 
				{
					m_texMBut[j] = GetICustButton(GetDlgItem(hwndDlg,texMButtonsIDC[j]));
					assert( m_texMBut[j] );
					m_texMBut[j]->SetRightClickNotify(TRUE);
					m_texMBut[j]->SetDADMgr(&m_dadMgr);
				}

				//--
				hwndDlg = GetDlgItem(hwndDlg, IDC_CBO_APPLY_MODE);

				SendMessage(hwndDlg, CB_RESETCONTENT, 0L, 0L);
				for (j = 0; j < ETEXTURE_APPLY_MODE::APPLY_MAX_MODES; j++) 
				{
					SendMessage(hwndDlg, CB_ADDSTRING, 0L, (LPARAM)(ms_aApplyModes[j]) );
				}

				LoadDialog(TRUE);
			}
			break;

		case WM_COMMAND: 

			int iSel;

			for ( int i=0; i<NMBUTS; i++) 
			{
				if (id == texMButtonsIDC[i]) 
				{
					PostMessage(m_hwmEdit,WM_TEXMAP_BUTTON, texmapFromMBut[i],(LPARAM)m_pMtl );
					UpdateMapButtons();
					goto exit;
				}
			}


			switch (id)
			{
				case IDC_CHK_SPECENABLE:
					m_pShader->SetSpecularOn(IsDlgButtonChecked(hwndDlg, IDC_CHK_SPECENABLE), 0);
					break;			

				case IDC_CBO_APPLY_MODE:
					iSel = SendDlgItemMessage(
						hwndDlg, IDC_CBO_APPLY_MODE, CB_GETCURSEL, 0, 0);
					m_pShader->SetTextureApplyMode(iSel, 0);
					break;
			}


			break; // WM_COMMAND

		case CC_COLOR_SEL: 
			{
				int id = LOWORD(wParam);
				SelectEditColor(ColorIDCToIndex(id));
			}			
			break;

		case CC_COLOR_DROP:	{
			int id = LOWORD(wParam);
			SelectEditColor(ColorIDCToIndex(id));
			UpdateMtlDisplay();				
							}
							break;
		case CC_COLOR_BUTTONDOWN:
			theHold.Begin();
			break;
		case CC_COLOR_BUTTONUP:
			if (HIWORD(wParam)) theHold.Accept(GetString(IDS_DS_PARAMCHG));
			else theHold.Cancel();
			UpdateMtlDisplay();				
			break;
		case CC_COLOR_CHANGE: {			
			int id = LOWORD(wParam);
			int buttonUp = HIWORD(wParam); 
			int n = ColorIDCToIndex(id);
			if (buttonUp) theHold.Begin();

			DWORD curRGB = m_colorSwatch[n]->GetColor();

			SetMtlColor(n, Color(curRGB), m_pShader, m_colorSwatch, m_curTime);

			if (buttonUp) 
			{
				theHold.Accept(GetString(IDS_DS_PARAMCHG));
				// DS: 5/11/99-  this was commented out. I put it back in, because
				// it is necessary for the Reset button in the color picker to 
				// update the viewport.				
				UpdateMtlDisplay();  
			}
							  } break;
		case WM_PAINT: 
			if (!m_valid) 
			{
				m_valid = TRUE;
				ReloadDialog();
			}
			return FALSE;
		case WM_CLOSE:
		case WM_DESTROY: 
			break;
		case CC_SPINNER_CHANGE: 
			if (!theHold.Holding()) theHold.Begin();
			switch (id) 
			{
				case IDC_SPN_SHININESS: 
			//	pShader->SetGlossiness(PcToFrac( shSpin->GetIVal() ), curTime); 
			//	UpdateHilite();
				break;
			case IDC_SPN_ALPHA: 
			//	pMtl->SetOpacity(PcToFrac(alphaSpin->GetIVal()), curTime); 
				break;
				}
				//UpdateMtlDisplay();
				break;

		case CC_SPINNER_BUTTONDOWN:
			theHold.Begin();
			break;

		case WM_CUSTEDIT_ENTER:
		case CC_SPINNER_BUTTONUP: 
			if (HIWORD(wParam) || msg==WM_CUSTEDIT_ENTER) 
				theHold.Accept(GetString(IDS_DS_PARAMCHG));
			else 
				theHold.Cancel();
			UpdateMtlDisplay();
			break;

	}
exit:
	return FALSE;
}

void EzMaxShaderDialog::FillShaderDropdown(HWND hWndParent)
{
    HWND hWndCombo = GetDlgItem(hWndParent, IDC_CUSTOM_SHADER);
    SendMessage(hWndCombo, CB_RESETCONTENT, 0, 0);

    SendMessage(hWndCombo, CB_ADDSTRING, 0, (LPARAM) EZ_DEFAULT_SHADER_NAME);


	SendMessage(hWndCombo, CB_SELECTSTRING, -1, (LPARAM) EZ_DEFAULT_SHADER_NAME);

}


