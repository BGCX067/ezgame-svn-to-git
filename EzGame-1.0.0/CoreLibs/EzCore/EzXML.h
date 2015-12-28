// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Nerilicht Engine" and the "irrXML" project.
// For conditions of distribution and use, see copyright notice in irrlicht.h and/or irrXML.h

#ifndef __XML_H_INCLUDED__
#define __XML_H_INCLUDED__


#include "EzFile.h"
#include <stdio.h>



//! Enumeration of all supported source text file formats
enum ETEXT_FORMAT
{
	//! ASCII, file without byte order mark, or not a text file
	ETF_ASCII,

	//! UTF-8 format
	ETF_UTF8,

	//! UTF-16 format, big endian
	ETF_UTF16_BE,

	//! UTF-16 format, little endian
	ETF_UTF16_LE,

	//! UTF-32 format, big endian
	ETF_UTfloat_BE,

	//! UTF-32 format, little endian
	ETF_UTfloat_LE
};


//! Enumeration for all xml nodes which are parsed by NeriXMLReader
enum EXML_NODE
{
	//! No xml node. This is usually the node if you did not read anything yet.
	EXN_NONE,

	//! An xml element such as &lt;foo&gt;
	EXN_ELEMENT,

	//! End of an xml element such as &lt;/foo&gt;
	EXN_ELEMENT_END,

	//! Text within an xml element: &lt;foo&gt; this is the text. &lt;foo&gt;
	EXN_TEXT,

	//! An xml comment like &lt;!-- I am a comment --&gt; or a DTD definition.
	EXN_COMMENT,

	//! An xml cdata section like &lt;![CDATA[ this is some CDATA ]]&gt;
	EXN_CDATA,

	//! Unknown element.
	EXN_UNKNOWN
};


//! Empty class to be used as parent class for NeriXMLReader.
/** If you need another class as base class for the xml reader, you can do this by creating
the reader using for example new EzXMLReaderImpl<char, YourBaseClass>(yourcallback);
The Nerilicht Engine for example needs IReferenceCounted as base class for every object to
let it automaticly reference countend, hence it replaces IXMLBase with IReferenceCounted.
See irrXML.cpp on how this can be done in detail. */

class EzXMLBase
{
};


template<class char_type, class super_class>
class EzXMLReader : public super_class
{
public:

	//! Destructor
	virtual ~EzXMLReader() {}

	//! Reads forward to the next xml node.
	/** \return Returns false, if there was no further node. */
	virtual bool read() = 0;

	//! Returns the type of the current XML node.
	virtual EXML_NODE getNodeType() const = 0;

	//! Returns attribute count of the current XML node.
	/** This is usually
	non null if the current node is EXN_ELEMENT, and the element has attributes.
	\return Returns amount of attributes of this xml node. */
	virtual unsigned int getAttributeCount() const = 0;

	//! Returns name of an attribute.
	/** \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
	\return Name of the attribute, 0 if an attribute with this index does not exist. */
	virtual const char_type* getAttributeName(int idx) const = 0;

	//! Returns the value of an attribute.
	/** \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
	\return Value of the attribute, 0 if an attribute with this index does not exist. */
	virtual const char_type* getAttributeValue(int idx) const = 0;

	//! Returns the value of an attribute.
	/** \param name: Name of the attribute.
	\return Value of the attribute, 0 if an attribute with this name does not exist. */
	virtual const char_type* getAttributeValue(const char_type* name) const = 0;

	//! Returns the value of an attribute in a safe way.
	/** Like getAttributeValue(), but does not
	return 0 if the attribute does not exist. An empty EzStringc ("") is returned then.
	\param name: Name of the attribute.
	\return Value of the attribute, and "" if an attribute with this name does not exist */
	virtual const char_type* getAttributeValueSafe(const char_type* name) const = 0;

	//! Returns the value of an attribute as integer.
	/** \param name Name of the attribute.
	\return Value of the attribute as integer, and 0 if an attribute with this name does not exist or
	the value could not be interpreted as integer. */
	virtual int getAttributeValueAsInt(const char_type* name) const = 0;

	//! Returns the value of an attribute as integer.
	/** \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
	\return Value of the attribute as integer, and 0 if an attribute with this index does not exist or
	the value could not be interpreted as integer. */
	virtual int getAttributeValueAsInt(int idx) const = 0;

	//! Returns the value of an attribute as float.
	/** \param name: Name of the attribute.
	\return Value of the attribute as float, and 0 if an attribute with this name does not exist or
	the value could not be interpreted as float. */
	virtual float getAttributeValueAsFloat(const char_type* name) const = 0;

	//! Returns the value of an attribute as float.
	/** \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
	\return Value of the attribute as float, and 0 if an attribute with this index does not exist or
	the value could not be interpreted as float. */
	virtual float getAttributeValueAsFloat(int idx) const = 0;

	//! Returns the name of the current node.
	/** Only non null, if the node type is EXN_ELEMENT.
	\return Name of the current node or 0 if the node has no name. */
	virtual const char_type* getNodeName() const = 0;

	//! Returns data of the current node.
	/** Only non null if the node has some
	data and it is of type EXN_TEXT or EXN_UNKNOWN. */
	virtual const char_type* getNodeData() const = 0;

	//! Returns if an element is an empty element, like &lt;foo />
	virtual bool isEmptyElement() const = 0;

	//! Returns format of the source xml file.
	/** It is not necessary to use
	this method because the parser will convert the input file format
	to the format wanted by the user when creating the parser. This
	method is useful to get/display additional informations. */
	virtual ETEXT_FORMAT getSourceFormat() const = 0;

	//! Returns format of the strings returned by the parser.
	/** This will be UTF8 for example when you created a parser with
	NeriXMLReaderUTF8() and UTfloat when it has been created using
	NeriXMLReaderUTfloat. It should not be necessary to call this
	method and only exists for informational purposes. */
	virtual ETEXT_FORMAT getParserFormat() const = 0;
};


//! defines the utf-16 type.
/** Not using wchar_t for this because
wchar_t has 16 bit on windows and 32 bit on other operating systems. */
typedef unsigned short char16;

//! defines the utf-32 type.
/** Not using wchar_t for this because
wchar_t has 16 bit on windows and 32 bit on other operating systems. */
typedef unsigned long char32;


typedef EzXMLReader<char, EzXMLBase> EzXMLReaderASCII;
typedef EzXMLReader<char16, EzXMLBase> EzMLReaderUTF16;
typedef EzXMLReader<char32, EzXMLBase> EzXMLReaderUTfloat;


EZCORE_ENTRY EzXMLReaderASCII* createEzXMLReader(const char* filename);
EZCORE_ENTRY EzXMLReaderASCII* createEzXMLReader(EzFile* file);

EZCORE_ENTRY EzMLReaderUTF16* createEzXMLReaderUTF16(const char* filename);
EZCORE_ENTRY EzMLReaderUTF16* createEzXMLReaderUTF16(EzFile* file);

EZCORE_ENTRY EzXMLReaderUTfloat* createEzXMLReaderUTfloat(const char* filename);
EZCORE_ENTRY EzXMLReaderUTfloat* createEzXMLReaderUTfloat(EzFile* file);


#endif // __XML_H_INCLUDED__

