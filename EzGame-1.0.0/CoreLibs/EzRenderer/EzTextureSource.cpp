#include "EzTextureSource.h"
#include "EzImageLoader.h"
#include "EzVideoDriver.h"



template<> 
EzTextureSource* EzSingleton<EzTextureSource>::ms_pSingleton = 0;


EzTextureSource::EzTextureSource(void)
{
	m_pDriver = NULL;
	m_aSurfaceLoader.push_back(createImageLoaderDDS());
	m_aSurfaceLoader.push_back(createImageLoaderBMP());
	m_aSurfaceLoader.push_back(createImageLoaderTGA());

}

EzTextureSource::~EzTextureSource(void)
{
}

void EzTextureSource::init()
{
	EzExternalNew EzTextureSource;
}
void EzTextureSource::shutdown()
{
	EzExternalDelete ms_pSingleton;
	ms_pSingleton = NULL;
}


//! Creates a software image from a file.
EzImage* EzTextureSource::createImageFromFile(const c8* FileName)
{
	EzImage* pImage = 0;

	u32 i;

	EzFile* file = EzFile::open(FileName, EzFile::READ_ONLY);

	if(file)
	{
		// try to load file based on file extension
		for (i=0; i<m_aSurfaceLoader.size(); ++i)
		{
			if (m_aSurfaceLoader[i]->isALoadableFileExtension(FileName))
			{
				// reset file position which might have changed due to previous loadImage calls
				file->seek(0);

				pImage = m_aSurfaceLoader[i]->loadImage(file);

				if(pImage)
				{
					file->release();
					return pImage;
				}
			}
		}

		// try to load file based on what is in it
		for (i=0; i<m_aSurfaceLoader.size(); ++i)
		{
			// dito
			file->seek(0);
			if (m_aSurfaceLoader[i]->isALoadableFileFormat(file))
			{
				file->seek(0);
				pImage = m_aSurfaceLoader[i]->loadImage(file);
			}
		}

		file->release();
	}



	return pImage; // failed to load
}

//! opens the file and loads it into the surface
EzTexture* EzTextureSource::loadTextureFromFile(const c8* FileName)
{
	EzTexture* texture = 0;

	if (m_pDriver)
	{
		EzImage* pImage = createImageFromFile(FileName);

		// create texture from surface
		texture = m_pDriver->createDeviceDependentTexture(pImage, FileName );
	//	os::Printer::log("Loaded texture", file->getFileName());
		//image->drop();
	}

	return texture;
}


EzTexture* EzTextureSource::createTexture(const c8* filename)
{

	// Identify textures by their absolute filenames if possible.
	EzStringc absolutePath = m_TextureFolderName + filename;

	EzTexture* texture = findTexture(filename);
	if (texture)
		return texture;


	texture = loadTextureFromFile(absolutePath.c_str());
	
	return texture;

}


EzTexture* EzTextureSource::findTexture(const c8* filename)
{
	return NULL;

}

