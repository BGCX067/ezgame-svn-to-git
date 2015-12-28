// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_TEXTURE_H_INCLUDED__
#define __I_TEXTURE_H_INCLUDED__


#include "..\EzMain3D\EzObjectNET.h"


EzSmartPointer(EzTexture);

class EzTexture : public EzObjectNET
{
	EzDeclareRootRTTI(EzTexture);


public:

	//! constructor
	EzTexture() : m_bExternalTexture(false), m_iPitch(0)
	{
	}

	EzTexture(const char* name, const EzDimension2d& size) : m_Name(name), m_bExternalTexture(true),
		m_TextureSize(size), m_ImageSize(size), m_iPitch(0)
	{
		m_Name.make_lower();
	}

	EzTexture(const char* name) : m_Name(name), m_bExternalTexture(true),
		m_TextureSize(0, 0), m_ImageSize(0, 0), m_iPitch(0)
	{
		m_Name.make_lower();
	}


	virtual void* lock(bool readOnly = false) { return NULL; }
	virtual void unlock() {};
	const EzDimension2d& getOriginalSize() const { return m_ImageSize; }
	const EzDimension2d& getSize() const { return m_TextureSize; }
	virtual EDRIVER_TYPE getDriverType() const  { return EDT_NULL; }
	virtual ECOLOR_FORMAT getColorFormat() const { return m_eColorFormat; }
	virtual bool hasMipMaps() const { return false; }
	virtual void regenerateMipMapLevels() {};
	virtual bool isRenderTarget() const { return false; }
	const char* getName() const { return m_Name.c_str(); }

	inline unsigned int getPitch() const { return m_iPitch; }

	void setExternalTexture(const char* Name)
	{
		m_Name = Name;
		m_Name.make_lower();
		m_bExternalTexture = true;
	}
	inline bool isExternalTexture() { return m_bExternalTexture; }


protected:
	bool				m_bExternalTexture;
	EzStringc			m_Name;
	int					m_iPitch;

	ECOLOR_FORMAT		m_eColorFormat;
	EzDimension2d		m_TextureSize;
	EzDimension2d		m_ImageSize;
};



#endif