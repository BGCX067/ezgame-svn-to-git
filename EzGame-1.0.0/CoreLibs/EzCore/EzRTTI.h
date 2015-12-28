#ifndef __CRTTI_H_INCLUDE__
#define __CRTTI_H_INCLUDE__

#include "EzCoreLibType.h"

class EZCORE_ENTRY EzRTTI
{
public:
    EzRTTI (const char* pcName, const EzRTTI* pkBaseRTTI)
	: m_pcName(pcName), m_pkBaseRTTI(pkBaseRTTI)
	{

	}

    const char* getName() const {return m_pcName;}
    const EzRTTI* getBaseRTTI() const {return m_pkBaseRTTI;}

protected:
    const char* m_pcName;
    const EzRTTI* m_pkBaseRTTI;
};

// insert in root class declaration
#define EzDeclareRootRTTI(classname) \
    public: \
        static const EzRTTI ms_RTTI; \
        virtual const EzRTTI* getRTTI() const {return &ms_RTTI;} \
        static bool isExactKindOf(const EzRTTI* pkRTTI, \
            const classname* pkObject) \
        { \
            if (!pkObject) \
            { \
                return false; \
            } \
            return pkObject->isExactKindOf(pkRTTI); \
        } \
        bool isExactKindOf(const EzRTTI* pkRTTI) const \
        { \
            return (getRTTI() == pkRTTI); \
        } \
        static bool isKindOf(const EzRTTI* pkRTTI, \
            const classname* pkObject) \
        { \
            if (!pkObject) \
            { \
                return false; \
            } \
            return pkObject->isKindOf(pkRTTI); \
        } \
        bool isKindOf(const EzRTTI* pkRTTI) const \
        { \
            const EzRTTI* pkTmp = getRTTI(); \
            while (pkTmp) \
            { \
                if (pkTmp == pkRTTI) \
                { \
                    return true; \
                } \
                pkTmp = pkTmp->getBaseRTTI(); \
            } \
            return false; \
        } \
        static classname* DynamicCast(const EzRTTI* pkRTTI, \
            const classname* pkObject) \
        { \
            if (!pkObject) \
            { \
                return false; \
            } \
            return pkObject->DynamicCast(pkRTTI); \
        } \
        classname* DynamicCast(const EzRTTI* pkRTTI) const \
        { \
            return (isKindOf(pkRTTI) ? (classname*) this : 0 ); \
        }

// insert in class declaration
#define EzDeclareRTTI \
    public: \
        static const EzRTTI ms_RTTI; \
        virtual const EzRTTI* getRTTI() const {return &ms_RTTI;}\
		virtual void deleteThis() { EzDelete this; }

// insert in root class source file
#define EzImplementRootRTTI(rootclassname) \
    const EzRTTI rootclassname::ms_RTTI(#rootclassname, 0)

// insert in class source file
#define EzImplementRTTI(classname, baseclassname) \
    const EzRTTI classname::ms_RTTI(#classname, &baseclassname::ms_RTTI)

// macros for run-time type testing
#define EzIsExactKindOf(classname, pkObject) \
    classname::isExactKindOf(&classname::ms_RTTI, pkObject)

#define EzIsKindOf(classname, pkObject) \
    classname::isKindOf(&classname::ms_RTTI, pkObject)

// macro for run-time type casting
#define EzStaticCast(classname, pkObject) \
    ((classname*) pkObject)

#define EzDynamicCast(classname, pkObject) \
    ((classname*) classname::DynamicCast(&classname::ms_RTTI, pkObject))


#endif