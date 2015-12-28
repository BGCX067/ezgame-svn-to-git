#ifndef __EZ_ERROR_DEFINES__H_INCLUDED__
#define __EZ_ERROR_DEFINES__H_INCLUDED__

#define EZ_ERROR_D3D_DLL													0x00000001		// could not load d3d9.dll.
#define EZ_ERROR_INITIALIZING_D3D											0x00000002		// initializing D3D Fail
#define EZ_ERROR_GET_ADAPTER_DISPLAY_MODE									0x00000003		// Could not get Adapter Display mode.
#define EZ_ERROR_CREATE_FAIL_D3D											0x00000004		// Was not able to create Direct3D9 device.
#define EZ_ERROR_TEXTURE_IS_NOT_BYD3D										0x00000005		// Tried to set a texture not owned by this driver.
#define EZ_ERROR_UNABLE_D3D9STREAM_SOURCE_RESET								0x00000006		// Unable to reset unused D3D9 stream source.
#define EZ_ERROR_UNABLE_D3D9STREAM_SOURCE_BIND								0x00000007		// Unable to reset unused D3D9 stream source.
#define EZ_ERROR_ALREADY_HAVE_SEQUENCE_NUMBER								0x00000008		// There is already Sequence Number in the ActorManager.


#define EZ_WARN_NOT_SUPPORT_MULTISAMPLE										0x10000001		// Not Support this MultiSampleTyped.
#define EZ_WARN_UNABLE_CALLING_VERTEXSHADER									0x10000002		// Error calling SetVertexShader.
#define EZ_WARN_UNABLE_UPLOAD_VERTEXSHADER_PARAM							0x10000003		// Unable to upload vertex shader float parameters.
#define EZ_WARN_ALREADY_ANIMATING_SEQUENCE									0x10000004		// This sequence is already animating!		

#define EZ_WARN_NOT_FIND_BONE_NODE											0x10000004		// There is not Bone in node
#define EZ_WARN_DONOT_HAVE_CONTROLLER										0x10000004		// don't have Controller.



#define EZ_DIRVER_INFORMATION											0x20000001		
#define EZ_DEVICE_INFORMATION											0x20000002		






#endif