#pragma once

#include "EzCore.h"
#include <stdio.h>
#include "EzRefObject.h"


class EZCORE_ENTRY EzFile : public EzRefObject
{
public:
	EzFile(void);
	~EzFile(void);

    enum EMODE
    {
        READ_ONLY,
        WRITE_ONLY,
        APPEND_ONLY
    };


	static EzFile* open(const char* FileName, EMODE eMode);
	bool EzFile::isEof()
	{
		return m_lFileSize == getPos();
	}



	void close();

	//! returns how much was read

	int read(void* buffer, unsigned int sizeToRead);
	int gets(EzStringc& buffer);


	// write function
	int puts(const char* str);
	int write(const void* buffer, unsigned int sizeToWrite);

	///////////////////////



	//! changes position in file, returns true if successful
	bool seek(long finalPos, bool relativeMovement = false);

	//! returns size of file
	long getSize()
	{
		return m_lFileSize;
	}

	//! returns if file is open
	bool isOpen()
	{
		return m_pFile != 0;
	}

	//! returns where in the file we are.
	long getPos();


private:
	FILE*	m_pFile;
	long	m_lFileSize;
	EMODE	m_eMode;
};

