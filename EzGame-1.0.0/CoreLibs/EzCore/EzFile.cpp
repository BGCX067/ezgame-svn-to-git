#include "EzFile.h"


EzFile::EzFile(void)
{
}

EzFile::~EzFile(void)
{
}


int EzFile::puts(const char* str)
{
	EZASSERT(m_eMode == WRITE_ONLY);

	return fputs(str, m_pFile);
}


int EzFile::read(void* buffer, unsigned int sizeToRead)
{
	if (!isOpen())
		return 0;

	return (int)fread(buffer, 1, sizeToRead, m_pFile);
}

int EzFile::write(const void* buffer, unsigned int sizeToWrite)
{
	if (!isOpen())
		return 0;

	return (int)fwrite(buffer, 1, sizeToWrite, m_pFile);
}

int EzFile::gets(EzStringc& buffer)
{
	int iSize = 0;

	while(1)
	{
		unsigned char c;

		if(isEof())
		{
			return -1;
		}

		c = fgetc(m_pFile);

		
		iSize++;

		if(c == '\n')
		{
			break;
		}

		buffer.append(c);

	}

	return iSize;

}


bool EzFile::seek(long finalPos, bool relativeMovement)
{
	if (!isOpen())
		return false;

	return fseek(m_pFile, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
}

long EzFile::getPos()
{
	return ftell(m_pFile);
}

void EzFile::close()
{
	fclose(m_pFile);
	m_pFile = NULL;

	EzDelete this;

}

EzFile* EzFile::open(const char* FileName, EMODE eMode)
{
	EzFile* pFile = EzNew EzFile;


	switch(eMode)
	{
	case READ_ONLY:
		if(fopen_s(&pFile->m_pFile, FileName, "rb"))
		{
			EzDelete pFile;
			return 0;
		}
		break;

	case WRITE_ONLY:
		if(fopen_s(&pFile->m_pFile, FileName, "wb+"))
		{
			EzDelete pFile;
			return 0;
		}

		break;

	case APPEND_ONLY:
		if(fopen_s(&pFile->m_pFile, FileName, "ab"))
		{
			EzDelete pFile;
			return 0;
		}
		
		break;
	}


	if (pFile->m_pFile)
	{
		// get FileSize

		fseek(pFile->m_pFile, 0, SEEK_END);
		pFile->m_lFileSize = pFile->getPos();
		fseek(pFile->m_pFile, 0, SEEK_SET);
		pFile->m_eMode = eMode;

		return pFile;
	}

	EzDelete pFile;
	return NULL;

}
