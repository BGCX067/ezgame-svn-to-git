#include "stdafx.h"
#include "readfile.h"


CReadFile::CReadFile(const char* fileName)
: m_FileSize(0)
{

	m_FileName = fileName;
	OpenFile();
}



CReadFile::~CReadFile()
{
	if(m_File)
        fclose(m_File);
}

inline bool CReadFile::isOpen()
{
	return m_File != 0;
}

int CReadFile::isEof()
{
	return feof(m_File);
}


int CReadFile::Read(void* buffer, int sizeToRead)
{
    if (!isOpen())
        return 0;

	return (int) fread(buffer, 1, sizeToRead, m_File);
}

int CReadFile::GetLine(std::string& buffer)
{
	char buff[2048];
	int iSize;
	int ch = 0;

	buffer.clear();

	while(ch != '\n')
	{
		ch = fgetc(m_File);

		if(ch == EOF)
		{
			break;
		}

		buffer.push_back(ch);
	}
	
	return buffer.size();
}



bool CReadFile::Seek(int finalPos, bool relativeMovement)
{
	if (!isOpen())
		return false;

	return fseek(m_File, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
}

int CReadFile::GetSize()
{
	return m_FileSize;
}

int CReadFile::GetPos()
{
	return ftell(m_File);
}


void CReadFile::OpenFile()
{
	if (m_FileName.size() == 0) 
	{
		m_File = 0;
		return; 
	}

	m_File = fopen(m_FileName.c_str(), "rb");

	if (m_File)
	{
		fseek(m_File, 0, SEEK_END);
		m_FileSize = ftell(m_File);
		fseek(m_File, 0, SEEK_SET);
	}
}


const char* CReadFile::GetFileName()
{
	return m_FileName.c_str();
}


