#pragma once

#include "../EzCore/EzCore.h"
#include "../EzCore/EzFile.h"
#include "EzAVObject.h"
#include "EzBound.h"


class EzTriStrips;
class EzTriShape;
class EzTriShapeData;
class EzTriStripsData ;
class EzTexturingProperty;
class EzMaterialProperty;
class EzAnimationSequence;
class EzFile;


class EzStream
{
public:

	struct EzfInfo
	{
		unsigned int	uiVersion;		
		EzStringc		createName;
		unsigned int				uiUserVersion;		
		EzStringc		exportInfo;
	};


	EzStream(void);
	~EzStream(void);

	bool load(const char* FileName);
	bool write(const char* FileName, EzAVObject* pRoot, const EzfInfo &info);

//	EzAnimationSequence* loadKF(const char* filename, EzNode* pNode);
	EzAVObjectPtr getAt(unsigned int uiIndex) { return m_aRoots[uiIndex]; }
	unsigned int getCount() { return m_aRoots.size(); }
	EzFile* getFile();
	unsigned int getLinkIDFromObject(EzObject* pObject);
	EzObject* getObjectFromLinkID(const char* ClassName , unsigned int uiLinkID);

protected:
	void loadBinary(EzAVObject* pParent);
	void saveBinary(EzAVObject* pParent);
	void writeTreeNode(EzAVObject* pParent);
	EzAVObject* loadTreeNode();

	typedef EzMap<EzObject*, unsigned int>	EzLinkObjectMap;
	typedef EzMap<unsigned int, EzObject*>	EzLinkIDMap;

	EzLinkObjectMap					m_Link_map;
	EzLinkIDMap						m_LinkIDMap;

	void saveHeader(const EzfInfo &info);
	bool loadHeader();



protected:
	EzArray<EzAVObjectPtr>		m_aRoots;

	EzFile*				m_file;

	bool				m_bLittleEndian;
	unsigned int					m_lastlinkID;

	EzfInfo				m_fileInfo;



};

inline EzFile* EzStream::getFile()
{
	return m_file;
}

template <class T>
inline int EzStreamLoad(EzStream& Stream, T& value, unsigned int uiNumElement = 1)
{
	EzFile*	 file = Stream.getFile();
	EZASSERT(file);

	EZASSERT(uiNumElement > 0);
	unsigned int uiSize = sizeof(T);

	return file->read((void*) &value, uiSize * uiNumElement);
}

inline int EzStreamStringLoad(EzStream& Stream, EzStringc &value)
{
	EzFile*	 file = Stream.getFile();
	EZASSERT(file);

	unsigned int uiSize;
	char* pStr;

	file->read((void*) &uiSize, sizeof(unsigned int));

	pStr = EzAlloc(char, uiSize + 1);

	file->read((void*) pStr, uiSize * sizeof(char));

	pStr[uiSize] = NULL;

	value = pStr;

	EzFree(pStr);

	return uiSize;
}



template <class T>
inline int EzStreamWrite(EzStream& Stream, const T& value, unsigned int uiNumElement = 1)
{
	EzFile*	 file = Stream.getFile();
	EZASSERT(file);

	EZASSERT(uiNumElement > 0);
    unsigned int uiSize = sizeof(T);

	return file->write((void*) &value, uiSize * uiNumElement);
}


template <class T>
inline int EzStreamWriteArray(EzStream& Stream, const T* value, unsigned int uiNumCount)
{
	EzFile*	 file = Stream.getFile();
	EZASSERT(file);
	unsigned int uiTotal = 0;

    unsigned int uiSize = sizeof(T);

	for(unsigned int ui = 0; ui < uiNumCount; ++ui)
	{
		uiTotal += file->write((void*) &value[ui], uiSize);
	}

	return uiTotal;
}

template <class T>
inline int EzStreamLoadArray(EzStream& Stream, T* value, unsigned int uiNumCount)
{
	EzFile*	 file = Stream.getFile();
	EZASSERT(file);
	unsigned int uiTotal = 0;

	unsigned int uiSize = sizeof(T);

	for(unsigned int ui = 0; ui < uiNumCount; ++ui)
	{
		uiTotal += file->read((void*) &value[ui], uiSize);
	}

	return uiTotal;
}





inline int EzStreamWriteTransform(EzStream& Stream, const EzTransform& value)
{
	EzFile*	 file = Stream.getFile();
	EZASSERT(file);
	int iRet;

	EzPoint3 Translation = value.getTranslation();
	EzMatrix3 Rot =  value.getToMatrixRotation();
	float fScal = value.getScale();

	
	iRet = file->write((const void*) &Translation, sizeof(EzPoint3));
	iRet += file->write((const void*) &Rot, sizeof(EzMatrix3));
	iRet += file->write((const void*) &fScal, sizeof(float));

	return iRet;
}


inline int EzStreamLoadTransform(EzStream& Stream, EzTransform& value)
{
	EzFile*	 file = Stream.getFile();
	EZASSERT(file);
	int iRet;

	EzPoint3 Translation;
	EzMatrix3 Rot;
	float fScal;


	iRet = file->read((void*) &Translation, sizeof(EzPoint3));
	iRet += file->read((void*) &Rot, sizeof(EzMatrix3));
	iRet += file->read((void*) &fScal, sizeof(float));

	value.setTranslation(Translation);
	value.setRotation(Rot);
	value.setScale(fScal);

	return iRet;
}

inline int EzStreamWriteBound(EzStream& Stream, const EzBound& value)
{
	EzFile*	 file = Stream.getFile();
	EZASSERT(file);
	int iRet;

	EzAabbox3d BBox = value.getBoundBox();

	iRet = file->write((const void*) &BBox.MinEdge, sizeof(EzPoint3));
	iRet += file->write((const void*) &BBox.MaxEdge, sizeof(EzPoint3));

	return iRet;

}


inline int EzStreamLoadBound(EzStream& Stream, EzBound& value)
{
	EzFile*	 file = Stream.getFile();
	EZASSERT(file);
	int iRet;

	EzAabbox3d BBox;


	iRet = file->read((void*) &BBox.MinEdge, sizeof(EzPoint3));
	iRet += file->read((void*) &BBox.MaxEdge, sizeof(EzPoint3));

	value.setBoundBox(BBox);





	return iRet;

}

inline int EzStreamStringWrite(EzStream& Stream, const char* value)
{
	EzFile*	 file = Stream.getFile();
	EZASSERT(file);

    unsigned int uiSize = strlen(value);

	file->write((void*) &uiSize, sizeof(unsigned int));

	return file->write((void*) value, uiSize * sizeof(char));
}


inline void EzStreamWriteObject(EzStream& Stream, EzObject* pObject)
{
	if(pObject)
	{
		unsigned int uiLinkID  = Stream.getLinkIDFromObject(pObject);
	}
	else
	{
		EzStreamWrite<unsigned int>(Stream, 0);
	}
}

EzObject* EzStreamLoadObject(EzStream& Stream);


