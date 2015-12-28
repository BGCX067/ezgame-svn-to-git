#pragma once

#include <EzString>
#include <EzArray>
#include <EzPoint3>
#include <EzColor>

TSTR FormatText(const TCHAR* format,...);
EzStringc FormatString(const TCHAR* format,...);



typedef EzArray<EzStringc> stringlist;

bool wildmatch(const TCHAR* match, const TCHAR* value);
bool wildmatch(const EzStringc& match, const EzStringc& value);
bool wildmatch(const stringlist& matches, const EzStringc& value);

/////////////////////////////////////////////////////////////////////////////////////
// Generic IniFile reading routine
/////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline T getIniValue(LPCTSTR Section, LPCTSTR Setting, T Default, LPCTSTR iniFileName)
{
	T v;
	TCHAR buffer[1024];
	EzStringc str(Default);
	buffer[0] = 0;

	//if (0 < GetPrivateProfileString(Section, Setting, sstr.str().c_str(), buffer, sizeof(buffer), iniFileName)){
	//	stringstream sstr(buffer);
	//	sstr >> v;
	//	return v;
	//}
	return Default;
}

// Specific override for int values
template<>
inline int getIniValue<int>(LPCTSTR Section, LPCTSTR Setting, int Default, LPCTSTR iniFileName)
{
	return GetPrivateProfileInt(Section, Setting, Default, iniFileName);
}

// Specific override for string values
template<>
inline EzStringc getIniValue<EzStringc>(LPCTSTR Section, LPCTSTR Setting, EzStringc Default, LPCTSTR iniFileName)
{
	TCHAR buffer[1024];
	buffer[0] = 0;
	if (0 < GetPrivateProfileString(Section, Setting, Default.c_str(), buffer, sizeof(buffer), iniFileName))
	{
		return EzStringc(buffer);
	}
	return Default;
}

// Specific override for TSTR values
template<>
inline TSTR getIniValue<TSTR>(LPCTSTR Section, LPCTSTR Setting, TSTR Default, LPCTSTR iniFileName){
	TCHAR buffer[1024];
	buffer[0] = 0;
	if (0 < GetPrivateProfileString(Section, Setting, Default.data(), buffer, sizeof(buffer), iniFileName)){
		return TSTR(buffer);
	}
	return Default;
}

// Generic IniFile reading routine
inline void setIniValue(LPCTSTR Section, LPCTSTR Setting, int value, LPCTSTR iniFileName)
{
	EzStringc str(value);


	WritePrivateProfileString(Section, Setting, str.c_str(), iniFileName);
}

inline void setIniValue(LPCTSTR Section, LPCTSTR Setting, EzStringc value, LPCTSTR iniFileName)
{
	WritePrivateProfileString(Section, Setting, value.c_str(), iniFileName);
}

inline void setIniValue(LPCTSTR Section, LPCTSTR Setting, TSTR value, LPCTSTR iniFileName){
	WritePrivateProfileString(Section, Setting, value.data(), iniFileName);
}


inline EzPoint3 EZPOINT3(const Point3& v)
{
	return EzPoint3(v.x, v.z, v.y);
}

inline EzColor EZCOLOR(const Color& c)
{
	return EzColor(c.r, c.g, c.b, 1.0f);
}



bool getIniFileName(char *iniName);
Matrix3 getNodeLocalTM(INode *n, TimeValue t);

Mtl *getMaterial(INode *node, int subMtl);


#pragma region Enumeration support
// Enumeration support
typedef struct EnumLookupType {
	int value;
	const char *name;
} EnumLookupType;

TSTR EnumToString(int value, const EnumLookupType *table);
int StringToEnum(TSTR value, const EnumLookupType *table);
int EnumToIndex(int value, const EnumLookupType *table);

TSTR FlagsToString(int value, const EnumLookupType *table);
int StringToFlags(TSTR value, const EnumLookupType *table);
