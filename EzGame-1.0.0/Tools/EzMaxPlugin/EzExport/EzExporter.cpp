#include "../EzMaxPluginPch.h"
#include "../Utils.h"
#include "Exporter.h"
#include "EzExporter.h"
#include <shlwapi.h>
#include <io.h>


static EzExportClassDesc EzExportDesc;
ClassDesc2* GetEzExportDesc() 
{ 
	return &EzExportDesc; 
}


INT_PTR CALLBACK EzExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	static Exporter *exp = NULL;
	char buffer[256];
	static bool setCursor = false;

	switch (message) 
	{
	case WM_INITDIALOG:
		{
			exp = (Exporter *)lParam;

			GetWindowText(hWnd, buffer, _countof(buffer));
			_tcscat_s(buffer, 255, TEXT(" "));
			_tcscat_s(buffer, 255, GAMEEASY_EZF_VERSION_STRING);
			SetWindowText(hWnd, buffer);

			CenterWindow(hWnd,GetParent(hWnd));

			SetDlgItemText(hWnd, IDC_ED_TEXPREFIX, exp->m_TexPrefix.c_str());


			CheckDlgButton(hWnd, IDC_CHK_STRIPS, exp->m_TriStrips);
			CheckDlgButton(hWnd, IDC_CHK_VERTEXCOLORS, exp->m_VertexColors);
			
			// Populate Type options
			SendDlgItemMessage(hWnd, IDC_CBO_ANIM_TYPE, CB_ADDSTRING, EZF_WITHOUT_ANI, LPARAM("EZF without Animation"));
			SendDlgItemMessage(hWnd, IDC_CBO_ANIM_TYPE, CB_ADDSTRING, EZF_WO_KF, LPARAM("EZF with Animation"));
			SendDlgItemMessage(hWnd, IDC_CBO_ANIM_TYPE, CB_ADDSTRING, SINGLE_KF_WITH_EZF, LPARAM("Single KF with EZF"));
			SendDlgItemMessage(hWnd, IDC_CBO_ANIM_TYPE, CB_ADDSTRING, SINGLE_KF_WO_EZF, LPARAM("Single KF without EZF"));
			SendDlgItemMessage(hWnd, IDC_CBO_ANIM_TYPE, CB_ADDSTRING, MULTI_KF_WITH_EZF, LPARAM("Multi KF with EZF"));
			SendDlgItemMessage(hWnd, IDC_CBO_ANIM_TYPE, CB_ADDSTRING, MULTI_KF_WO_EZF, LPARAM("Multi KF without EZF"));
			SendDlgItemMessage(hWnd, IDC_CBO_ANIM_TYPE, CB_ADDSTRING, EZF_WITH_MGR, LPARAM("EZF with Manager"));

			SendDlgItemMessage(hWnd, IDC_CBO_ANIM_TYPE, CB_SETCURSEL, WPARAM(exp->m_ExportType), 0);


		}

		return TRUE;

	case WM_CLOSE:
		//EndDialog(hWnd, exp->mDlgResult);
		return TRUE;

	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED)
		{
			bool close = false;
			char tmp[MAX_PATH];

			switch (LOWORD(wParam))
			{
			case IDOK:

				GetDlgItemText(hWnd, IDC_ED_TEXPREFIX, tmp, MAX_PATH);
				exp->m_TexPrefix = tmp;
				exp->m_ExportType = SendDlgItemMessage(hWnd, IDC_CBO_ANIM_TYPE, CB_GETCURSEL, 0, 0);

				exp->m_TriStrips = IsDlgButtonChecked(hWnd, IDC_CHK_STRIPS);
				exp->m_VertexColors = IsDlgButtonChecked(hWnd, IDC_CHK_VERTEXCOLORS);
				EndDialog(hWnd, IDOK);
				close = true;
				break;

			case IDCANCEL:
				EndDialog(hWnd, IDCANCEL);
				close = true;
				break;
			}

			if (close)
				SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////////////////////////
// EzExport
//////////////////////////////////////////////////////////////////////////////////////////////
EzExporter::EzExporter()
{
}

EzExporter::~EzExporter() 
{

}

int EzExporter::ExtCount()
{
	return 2;
}

const TCHAR *EzExporter::Ext(int n)
{		
	switch (n)
	{
	case 0: return _T("KF");
	case 1: return _T("EZF");
	}
	return NULL;
}

const TCHAR *EzExporter::LongDesc()
{
	return GetString(IDS_EXPORT_LONGDESCRIPTION);
}

const TCHAR *EzExporter::ShortDesc() 
{			
	return GetString(IDS_EXPORT_SHORTDESCRIPTION);
}

const TCHAR *EzExporter::AuthorName()
{			
	return GetString(IDS_EXPORT_AUTHORNAME);
}

const TCHAR *EzExporter::CopyrightMessage() 
{	
	return _T("http://niftools.sourceforge.net");
}

const TCHAR *EzExporter::OtherMessage1() 
{		
	return _T("http://niftools.sourceforge.net");
}

const TCHAR *EzExporter::OtherMessage2() 
{		
	return _T("http://niftools.sourceforge.net");
}

unsigned int EzExporter::Version()
{				
	return 0;
}

void EzExporter::ShowAbout(HWND hWnd)
{			

}

BOOL EzExporter::SupportsOptions(int ext, DWORD options)
{
	return TRUE;
}

static DWORD WINAPI dummyProgress(LPVOID arg) 
{
	return(0);
}

int	EzExporter::DoExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options)
{

	
	TCHAR title[1024];
	_stprintf_s(title, 1024, "Exporting '%s'...", PathFindFileName(name));
	i->PushPrompt(PathFindFileName(name));

	if (!suppressPrompts)
	{
		i->ProgressStart(PathFindFileName(name), TRUE, dummyProgress, NULL);
	}

	Exporter::Result ret;

	ret = (Exporter::Result) DoExportInternal(name, ei, i, suppressPrompts, options);

	switch(ret)
	{
	case Exporter::Error:
		MessageBox(NULL, "Export Error", "Export Error", MB_OK);
		break;

	}

	i->PopPrompt();

	if (!suppressPrompts)
		i->ProgressEnd();

	return true;
}

int EzExporter::DoExportInternal(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options)
{
	TCHAR path[MAX_PATH];
	GetFullPathName(name, MAX_PATH, path, NULL);
	PathRenameExtension(path, ".ezf");

	Exporter exp(i);

	exp.m_SelectedOnly = ((options&SCENE_EXPORT_SELECTED) != 0);
	
	if(!exp.initialize(i))
	{
		return Exporter::Error;
	}
	
	if(!exp.readConfig(i))
	{
		return Exporter::Error;
	}

	if(!suppressPrompts)
	{
		if (DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EXPORT_PANEL), GetActiveWindow(), EzExportOptionsDlgProc, (LPARAM)&exp) != IDOK)
			return true;
		// write config to registry
		exp.writeConfig(i);
	}

	EzNode* root = EzDynamicCast(EzNode, EzCreateObject("EzNode"));

	if(!root)
		return Exporter::Error;

	Exporter::Result result = exp.doExport(root, i->GetRootNode());

	if (result!=Exporter::Ok && result!=Exporter::Skip)
		return (int) result;


	if(exp.m_ExportType == EZF_WITHOUT_ANI || exp.m_ExportType == EZF_WITH_MGR)
	{
		EzStream stream;
		EzStream::EzfInfo info;

		info.uiVersion = GAMEEASY_EZF_VERSION;
		info.createName = exp.m_CreatorName;
		info.uiUserVersion = exp.m_uiUserVersion;
		info.exportInfo = "Game Easy tools 3ds Max Plugins ";
		info.exportInfo += GAMEEASY_EZF_VERSION_STRING;

		stream.write(path, root, info);
	}


	return true;
}


