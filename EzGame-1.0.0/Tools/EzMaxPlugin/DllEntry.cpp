#include "EzMaxPluginPch.h"
#include "Utils.h"
#include <notify.h>
#include "resource.h"

extern void DoNotifyNodeHide(void *param, NotifyInfo *info);
extern void DoNotifyNodeUnHide(void *param, NotifyInfo *info);


extern ClassDesc2* GetEzExportDesc();


enum ClassDescType
{
	CD_Export,
	CD_KFExport,
	CD_Count
};


static void InitializeLibSettings();
static void InitializeHavok();

HINSTANCE hInstance;
static int g_controlsInit = FALSE;
static int g_nClasses = 0;
static ClassDesc2* g_classDescriptions[20];
static bool g_classDescEnabled[CD_Count];





// This function is called by Windows when the DLL is loaded.  This 
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
	hInstance = hinstDLL;				// Hang on to this DLL's instance handle.

	if (!g_controlsInit) 
	{
		g_controlsInit = TRUE;

	
		InitCommonControls();
//		RegisterNotification(DoNotifyNodeHide, NULL, NOTIFY_NODE_HIDE); 
//		RegisterNotification(DoNotifyNodeUnHide, NULL, NOTIFY_NODE_UNHIDE); 
	}

	switch(fdwReason) {
		case DLL_PROCESS_ATTACH:
			EzMain3DInit();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			EzMain3DShutdown();
			break;
	}

	return(TRUE);
}




__declspec( dllexport ) const TCHAR* LibDescription()
{
	return GetString(IDS_LIBDESCRIPTION);
}


// This function returns the number of plug-in classes this DLL
//TODO: Must change this number when adding a new class
__declspec( dllexport ) int LibNumberClasses()
{
	return 2;
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
	switch(i)
	{
	case 0:
		return GetEzExportDesc();
	case 1:
		return GetEzShaderDesc();

	default:
		return 0;
	}
}



// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion()
{
	return ULONG(VERSION_3DSMAX);
}


TCHAR *GetString(int id)
{
	static TCHAR buf[256];
	if(hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;

	return NULL;
}


//static void DoNotifyNodeHide(void *param, NotifyInfo *info)
//{
//	int code = info->intcode;
//	INode *node = (INode*)info->callParam;
//	if (Object* obj = node->GetObjectRef())
//	{
//		// Look for messages in network\Max.log
//		// MAXScript_interface->Log()->LogEntry(SYSLOG_DEBUG, NO_DIALOG, "NifTools Max Plugin", 
//		// 	"Entered DoNotifyNodeHide; node is -%s- and class ID is %ld\n", node->GetName(), obj->ClassID().PartA());
//
//		if (obj->ClassID() == BHKLISTOBJECT_CLASS_ID)
//		{
//			const int PB_MESHLIST = 1;
//			IParamBlock2* pblock2 = obj->GetParamBlockByID(0);
//			int nBlocks = pblock2->Count(PB_MESHLIST);
//			for (int i = 0;i < pblock2->Count(PB_MESHLIST); i++)
//			{
//				INode *tnode = NULL;
//				pblock2->GetValue(PB_MESHLIST,0,tnode,FOREVER,i);	
//				if (tnode != NULL)
//				{
//					tnode->Hide(TRUE);
//				}
//			}
//		}
//	}
//}

//static void DoNotifyNodeUnHide(void *param, NotifyInfo *info)
//{
//	int code = info->intcode;
//	INode *node = (INode*)info->callParam;
//	if (Object* obj = node->GetObjectRef())
//	{
//		// Look for messages in network\Max.log
//		// MAXScript_interface->Log()->LogEntry(SYSLOG_DEBUG, NO_DIALOG, "NifTools Max Plugin", 
//		// 	"Entered DoNotifyNodeUnHide; node is -%s- and class ID is %ld\n", node->GetName(), obj->ClassID().PartA());
//
//		if (obj->ClassID() == BHKLISTOBJECT_CLASS_ID)
//		{
//			const int PB_MESHLIST = 1;
//			IParamBlock2* pblock2 = obj->GetParamBlockByID(0);
//			int nBlocks = pblock2->Count(PB_MESHLIST);
//			for (int i = 0;i < pblock2->Count(PB_MESHLIST); i++)
//			{
//				INode *tnode = NULL;
//				pblock2->GetValue(PB_MESHLIST,0,tnode,FOREVER,i);	
//				if (tnode != NULL)
//				{
//					tnode->Hide(FALSE);
//				}
//			}
//		}
//	}
//}


