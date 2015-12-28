#ifndef __EZCOMPILE_CONFIG_H_INCLUDED__
#define __EZCOMPILE_CONFIG_H_INCLUDED__

//! EZ SDK Version
#define EZ_SDK_VERSION				"1.0.0"
#define EZ_MULTITHREADED

#include <stdio.h> // TODO: Although included elsewhere this is required at least for mingw

#ifdef WIN32

#ifndef _EZ_STATIC_LIB_
#ifdef EZ_EXPORTS
#define EZGAME_API_ENTRAY __declspec(dllexport)
#else
#define EZGAME_API_ENTRAY __declspec(dllimport)
#endif // NERI_EXPORTS
#else
#define EZGAME_API_ENTRAY
#endif // _NERI_STATIC_LIB_

// Declare the calling convention.
#if defined(_STDCALL_SUPPORTED)
#define EZCALLCONV __stdcall
#else
#define EZCALLCONV __cdecl
#endif // STDCALL_SUPPORTED

#else
#define EZGAME_API_ENTRAY
#define EZCALLCONV
#endif // _WINDOWS_API_



#endif 



#include <wchar.h>
#ifdef _WINDOWS_API_
//! Defines for s{w,n}printf because these methods do not match the ISO C
//! standard on Windows platforms, but it does on all others.
//! These should be int snprintf(char *str, size_t size, const char *format, ...);
//! and int swprintf(wchar_t *wcs, size_t maxlen, const wchar_t *format, ...);
#if defined(_MSC_VER) && _MSC_VER > 1310 && !defined (_WIN32_WCE)
#define swprintf swprintf_s
#define snprintf sprintf_s
#else
#define swprintf _snwprintf
#define snprintf _snprintf
#endif

// define the wchar_t type if not already built in.
#ifdef _MSC_VER
#ifndef _WCHAR_T_DEFINED
//! A 16 bit wide character type.
/**
	Defines the wchar_t-type.
	In VS6, its not possible to tell
	the standard compiler to treat wchar_t as a built-in type, and
	sometimes we just don't want to include the huge stdlib.h or wchar.h,
	so we'll use this.
*/
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif // wchar is not defined
#endif // microsoft compiler
#endif // _WINDOWS_API_



