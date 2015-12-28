#pragma once


#include "../EzCore/EzCore.h"
#include "EzTexture.h"
#include "EzImage.h"


class EzVideoDriver;
class EzImageLoader;

class EzTextureSource : public EzSingleton<EzTextureSource>
{
public:
	EzTexture* createTexture(const char* filename);

	inline void setTexture(const char* filename)
	{ 
		m_TextureFolderName = filename; 
	}

	inline void setVideoDriver(EzVideoDriver* pDriver) 
	{ 
		m_pDriver = pDriver; 
	}

	static void init();
	static void shutdown();

protected:
	EzTextureSource(void);
	~EzTextureSource(void);

	struct SSurface
	{
		EzTexture* Surface;

		bool operator < (const SSurface& other) const
		{
			return Surface->getName() < other.Surface->getName();
		}
	};

	struct SDummyTexture : public EzTexture
	{
		SDummyTexture(const char* name) : EzTexture(name), size(0,0) {};

		virtual void* lock(bool readOnly = false) { return 0; };
		virtual void unlock(){}
		virtual const EzDimension2d& getOriginalSize() const { return size; }
		virtual const EzDimension2d& getSize() const { return size; }
		virtual ECOLOR_FORMAT getColorFormat() const { return ECF_A1R5G5B5; };
		virtual unsigned int getPitch() const { return 0; }
		virtual void regenerateMipMapLevels() {};
		EzDimension2d size;
	};

	EzTexture* loadTextureFromFile(const char* FileName);
	EzTexture* findTexture(const char* filename);
	EzImage* createImageFromFile(const char* FileName);


	EzStringc							m_TextureFolderName;

	EzArray<SSurface>			m_aTextures;
	EzArray<EzImageLoader*>		m_aSurfaceLoader;

	EzVideoDriver*				m_pDriver;

	
};


inline void setTextureFolder(const char* filename)
{
	return EzTextureSource::get().setTexture(filename);
}



inline EzTexture* createTexture(const char* filename)
{
	return EzTextureSource::get().createTexture(filename);
}



