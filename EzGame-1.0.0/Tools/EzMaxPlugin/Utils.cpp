#include "EzMaxPluginPch.h"
#include <shlwapi.h>
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include "Utils.h"


// sprintf for TSTR without having to worry about buffer size.
TSTR FormatText(const TCHAR* format,...)
{
	TCHAR buffer[512];
	TSTR text;
	va_list args;
	va_start(args, format);
	int nChars = _vsntprintf_s(buffer, _countof(buffer), format, args);
	if (nChars != -1)
	{
		text = buffer;
	} else 
	{
		size_t Size = _vsctprintf(format, args);
		text.Resize(Size);
		nChars = _vsntprintf_s(text.data(), Size, Size, format, args);
	}
	va_end(args);
	return text;
}

// sprintf for std::string without having to worry about buffer size.
EzStringc FormatString(const TCHAR* format,...)
{
	TCHAR buffer[512];
	EzStringc text;
	va_list args;
	va_start(args, format);
	int nChars = _vsntprintf_s(buffer, _countof(buffer), format, args);
	if (nChars != -1) 
	{
		text = buffer;
	} else 
	{
		size_t Size = _vsctprintf(format, args);
		TCHAR* pbuf = (TCHAR*)_alloca(Size);
		nChars = _vsntprintf_s(pbuf, Size, Size, format, args);
		text = pbuf;
	}
	va_end(args);
	return text;
}




// Original Source: Jack Handy www.codeproject.com
int wildcmp(const TCHAR *wild, const TCHAR *string) {
	const TCHAR *cp, *mp;

	while ((*string) && (*wild != '*')) {
		if ((*wild != *string) && (*wild != '?')) {
			return 0;
		}
		wild++;
		string++;
	}

	while (*string) {
		if (*wild == '*') {
			if (!*++wild) {
				return 1;
			}
			mp = wild;
			cp = string+1;
		} else if ((*wild == *string) || (*wild == '?')) {
			wild++;
			string++;
		} else {
			wild = mp;
			string = cp++;
		}
	}

	while (*wild == '*') {
		wild++;
	}
	return !*wild;
}

// Same as above but case insensitive
int wildcmpi(const TCHAR *wild, const TCHAR *string) {
	const TCHAR *cp, *mp;
	int f,l;
	while ((*string) && (*wild != '*')) {
		f = _totlower( (_TUCHAR)(*string) );
		l = _totlower( (_TUCHAR)(*wild) );
		if ((f != l) && (l != '?')) {
			return 0;
		}
		wild++, string++;
	}
	while (*string) {
		if (*wild == '*') {
			if (!*++wild) return 1;
			mp = wild, cp = string+1;
		} else {
			f = _totlower( (_TUCHAR)(*string) );
			l = _totlower( (_TUCHAR)(*wild) );
			if ((f == l) || (l == '?')) {
				wild++, string++;
			} else {
				wild = mp, string = cp++;
			}
		}
	}
	while (*wild == '*') wild++;
	return !*wild;
}

bool wildmatch(const TCHAR* match, const TCHAR* value)
{
	return (wildcmpi(match, value)) ? true : false;
}

bool wildmatch(const EzStringc& match, const EzStringc& value) 
{
	return (wildcmpi(match.c_str(), value.c_str())) ? true : false;
}

bool wildmatch(const stringlist& matches, const EzStringc& value)
{
	for (unsigned int n  = 0; n < matches.size(); n++)
	{
		if (wildcmpi(matches[n].c_str(), value.c_str()))
		{
			return true;
		}
	}
	return false;
}


Matrix3 getNodeLocalTM(INode *n, TimeValue t)
{
	Matrix3 m3 = n->GetNodeTM(t);
	Matrix3 m3p = n->GetParentTM(t);
	m3p.Invert();
	return m3 * m3p;
}




bool getIniFileName(char *iniName)
{
//#if VERSION_3DSMAX >= ((5000<<16)+(15<<8)+0) // Version 5+
	Interface *gi = GetCOREInterface();
//#else
//	Interface *gi = NULL;
//#endif
//	if (gi) 
//	{
//		LPCTSTR pluginDir = gi->GetDir(APP_PLUGCFG_DIR);
//		PathCombine(iniName, pluginDir, "EzMaxPlugin.ini");
//	} 
//	else
//	{
		GetModuleFileName(NULL, iniName, MAX_PATH);
		if (LPTSTR fname = PathFindFileName(iniName))
		{
			*fname = 0;
		}

		PathAddBackslash(iniName);
		PathAppend(iniName, "plugcfg");
		PathAppend(iniName, "EzMaxPlugin.ini");
	//}
	
	if (_access(iniName, 06) != 0) 
	{
		MessageBox(gi->GetMAXHWnd() , "EzMaxPlugin could not find a valid INI.  The plugin may not work correctly.\nPlease check for proper installation.", 
			"EzMaxPlugin", MB_OK|MB_ICONWARNING);
		return false;
	}

	return true;
}


Mtl *getMaterial(INode *node, int subMtl)
{
	Mtl *nodeMtl = node->GetMtl();
	if (nodeMtl)
	{
		int numSub = nodeMtl->NumSubMtls();
		if (numSub > 0)
		{
			return nodeMtl->GetSubMtl(subMtl % numSub);
		}

		return nodeMtl;
	}
	return NULL;
}



// Enumeration Support
TSTR EnumToString(int value, const EnumLookupType *table) {
	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
		if (itr->value == value) return TSTR(itr->name);
	}
	return FormatText("%x", value);
}

int EnumToIndex(int value, const EnumLookupType *table) {
	int i = 0;
	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr, ++i) {
		if (itr->value == value) return i;
	}
	return -1;
}

int StringToEnum(TSTR value, const EnumLookupType *table) {
	//Trim(value);
	if (value.isNull()) return 0;

	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
		if (0 == _tcsicmp(value, itr->name)) return itr->value;
	}
	char *end = NULL;
	return (int)strtol(value, &end, 0);
}

TSTR FlagsToString(int value, const EnumLookupType *table) {
	TSTR sstr;
	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
		if (itr->value && (itr->value & value) == itr->value) {
			if (!sstr.isNull()) sstr += " | ";
			sstr += itr->name;
			value ^= itr->value;
		}
	}
	if (value == 0 && sstr.isNull()) {
		return EnumToString(value, table);
	}
	if (value != 0) {
		if (!sstr.isNull()) sstr += "|";
		sstr += EnumToString(value, table);
	}
	return sstr;
}

int StringToFlags(TSTR value, const EnumLookupType *table) {
	int retval = 0;
	LPCTSTR start = value.data();
	LPCTSTR end = value.data() + value.Length();
	while(start < end) {
		LPCTSTR bar = _tcschr(start, '|');
		int len = (bar != NULL) ?  bar-start : end-start;
		TSTR subval = value.Substr(start-value.data(), len);
		retval |= StringToEnum(subval, table);
		start += (len + 1);
	}
	return retval;
}