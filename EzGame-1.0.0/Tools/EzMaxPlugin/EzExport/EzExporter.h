#pragma once

#include "../utils.h"
#include "../resource.h"
#include "EzExporter.h"


#define EzExport_CLASS_ID Class_ID(0x64d8488e, 0x6437168d)

LPCTSTR EzExportSection = TEXT("MaxEzExport");

class EzExporter : public SceneExport 
{
public:
	static HWND		hParams;
	int				mDlgResult;
	TSTR			iniFileName;
	TSTR			webSite;
	TSTR			wikiSite;

	int				ExtCount();					// Number of extensions supported
	const TCHAR		*Ext(int n);					// Extension #n (i.e. "3DS")
	const TCHAR		*LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR		*ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const TCHAR		*AuthorName();				// ASCII Author name
	const TCHAR		*CopyrightMessage();			// ASCII Copyright message
	const TCHAR		*OtherMessage1();			// Other message #1
	const TCHAR		*OtherMessage2();			// Other message #2
	unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
	void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

	BOOL			SupportsOptions(int ext, DWORD options);
	int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);
	int				DoExportInternal(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options);

	SDK_RESERVED_METHOD(1); // russom 02/26/01
	SDK_RESERVED_METHOD(2); // russom 02/26/01
	SDK_RESERVED_METHOD(3); // russom 02/26/01
	SDK_RESERVED_METHOD(4); // russom 02/26/01
	SDK_RESERVED_METHOD(5); // russom 02/26/01
	SDK_RESERVED_METHOD(6); // russom 02/26/01

	EzExporter();
	~EzExporter();		
};


class EzExportClassDesc : public ClassDesc2 
{
public:
	int 			IsPublic() { return TRUE; }
	void			*Create(BOOL loading = FALSE) 
	{
		char iniFile[1024];

		if(!getIniFileName(iniFile))
		{
			m_bLoad = false;
			return NULL;
		}

		return new EzExporter(); 
	}
	const TCHAR		*ClassName() { return GetString(IDS_EXPORT_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return EzExport_CLASS_ID; }
	const TCHAR		*Category() { return GetString(IDS_EXPORT_CATEGORY); }

	const TCHAR		*InternalName() { return GetString(IDS_EXPORT_INTERNALNAME); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle

protected:
	bool m_bLoad;


};


