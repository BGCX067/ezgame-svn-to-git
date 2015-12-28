#include "CLoaderObject.h"

//namespace GameEasy
//{
//
//C3dLoaderObject::C3dLoaderObject(void)
//{
//}
//
//C3dLoaderObject::~C3dLoaderObject(void)
//{
//}
//
//bool C3dLoaderObject::read_vector3df(vector3df& value)
//{
//	s32 nRead;
//
//	nRead = m_pFile->read(&value, sizeof(vector3df));
//
//	return nRead == sizeof(vector3df);
//
//}
//
//bool C3dLoaderObject::read_s32(s32& nValue)
//{
//	s32 nRead;
//
//	nRead = m_pFile->read(&nValue, sizeof(s32));
//
//	return nRead == sizeof(s32);
//}
//
//bool C3dLoaderObject::read_u32(u32& nValue)
//{
//	s32 nRead;
//
//	nRead = m_pFile->read(&nValue, sizeof(u32));
//
//	return nRead == sizeof(u32);
//}
//bool C3dLoaderObject::read_u16(u16& nValue)
//{
//	s32 nRead;
//
//	nRead = m_pFile->read(&nValue, sizeof(u16));
//
//	return nRead == sizeof(u16);
//}
//
//bool C3dLoaderObject::read_u8(u8& nValue)
//{
//	s32 nRead;
//
//	nRead = m_pFile->read(&nValue, sizeof(u8));
//	return nRead == sizeof(u8);
//}
//
//bool C3dLoaderObject::read_string(EzStringc& str)
//{
//	s32 nRead;
//	u16 len;
//	c8* strdata;
//
//	if(!read_u16(len))
//		return false;
//
//	strdata = new c8[len+1];
//
//	nRead = m_pFile->read(strdata, len);
//	strdata[nRead] = NULL;
//	str = strdata;
//
//	delete [] strdata;
//
//	return true;
//}
//
//bool C3dLoaderObject::read_binary(u8* binaryValue, const u32 size)
//{
//	return true;
//}
//
//
//bool C3dLoaderObject::read_vector2df(vector2df& value)
//{
//	s32 nRead;
//
//	nRead = m_pFile->read(&value, sizeof(vector2df));
//	return nRead == sizeof(vector2df);
//
//	return true;
//}
//
//bool C3dLoaderObject::read_bool(bool& bValue)
//{
//	s32 nRead;
//	u8 data;
//
//	nRead = m_pFile->read(&data, sizeof(u8));
//
//	data ? bValue = true : bValue = false;
//
//
//	return true;
//}
//
//bool C3dLoaderObject::read_f32(f32& fValue)
//{
//	s32 nRead;
//
//	nRead = m_pFile->read(&fValue, sizeof(f32));
//	return nRead == sizeof(f32);
//
//}
//
//}
