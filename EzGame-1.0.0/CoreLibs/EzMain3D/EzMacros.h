#pragma once


class EzStream;
class EzObject;


#define EzDeclareAbstractStream \
	public: \
	virtual void loadBinary(EzStream& Stream); \
	virtual void saveBinary(EzStream& Stream);


#define EzDeclareStream \
	EzDeclareAbstractStream; \
	static EzObject* createObject()

#define EzImplementCreateObject(classname) \
	EzObject* classname::createObject() \
{ \
	classname* pkObject = EzNew classname; \
	EZASSERT(pkObject != NULL); \
	return pkObject; \
}



#define EzDeclareShader \
public:\
	static EzShader* createShader()

