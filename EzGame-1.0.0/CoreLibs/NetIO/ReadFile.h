#pragma once

#include <string>

class CReadFile 
{
public:
	CReadFile(const char* fileName);
	virtual ~CReadFile();

	int Read(void* buffer, int sizeToRead);
	bool Seek(int finalPos, bool relativeMovement = false);
	int GetLine(std::string& buffer);

	int isEof();
	virtual int GetSize();
	bool isOpen();
	int GetPos();
	const char* GetFileName();

private:

	void OpenFile();	

	std::string m_FileName;
	FILE* m_File;
	int m_FileSize;
};
