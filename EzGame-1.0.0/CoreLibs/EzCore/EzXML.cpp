// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Nerilicht Engine" and the "irrXML" project.
// For conditions of distribution and use, see copyright notice in irrlicht.h and/or irrXML.h


#include "EzCoreLibType.h"
#include "EzXML.h"
#include "EzArray.h"
#include "EzString.h"
#include "EzFast_atof.h"
#include "EzXMLReaderImpl.h"



//! Creates an instance of an UFT-8 or ASCII character xml parser. 
EZCORE_ENTRY EzXMLReaderASCII* createEzXMLReader(const char* filename)
{

	EzFile* pFile = EzFile::open(filename, EzFile::READ_ONLY);

	if(pFile)
		return createEzXMLReader(pFile); 

	return 0;
}


//! Creates an instance of an UFT-8 or ASCII character xml parser. 
EZCORE_ENTRY EzXMLReaderASCII* createEzXMLReader(EzFile* file)
{
	if (file && (file->getSize() >= 0))
		return new EzXMLReaderImpl<char, EzXMLBase>(file); 
	else
		return 0;
}


//! Creates an instance of an UTF-16 xml parser. 
EZCORE_ENTRY EzMLReaderUTF16* createEzXMLReaderUTF16(const char* filename)
{

	EzFile* pFile = EzFile::open(filename, EzFile::READ_ONLY);

	if(pFile) 
		return createEzXMLReaderUTF16(pFile); 

	return 0;
}


//! Creates an instance of an UTF-16 xml parser. 
EZCORE_ENTRY EzMLReaderUTF16* createEzXMLReaderUTF16(EzFile* file)
{
	if (file && (file->getSize() >= 0))
		return new EzXMLReaderImpl<char16, EzXMLBase>(file); 
	else
		return 0;
}


//! Creates an instance of an UTF-32 xml parser. 
EZCORE_ENTRY EzXMLReaderUTfloat* createEzXMLReaderUTfloat(const char* filename)
{

	EzFile* pFile = EzFile::open(filename, EzFile::READ_ONLY);

	if(pFile)
		return createEzXMLReaderUTfloat(pFile); 

	return 0;
}

//! Creates an instance of an UTF-32 xml parser. 
EZCORE_ENTRY EzXMLReaderUTfloat* createEzXMLReaderUTfloat(EzFile* file)
{
	if (file && (file->getSize() >= 0))
		return new EzXMLReaderImpl<char32, EzXMLBase>(file); 
	else
		return 0;
}


