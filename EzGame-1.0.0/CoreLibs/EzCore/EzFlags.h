#ifndef _EZ_FLAGS_H_INCLUDE_
#define _EZ_FLAGS_H_INCLUDE_

#define EzDeclareFlags(type) \
    private: \
        type m_uFlags; \
        void setField(type uVal, type uMask, type uPos) \
        { \
            m_uFlags = (m_uFlags & ~uMask) | (uVal << uPos); \
        } \
        type getField(type uMask, type uPos) const \
        { \
            return (m_uFlags & uMask) >> uPos; \
        } \
        void setBit(bool bVal, type uMask) \
        { \
            if (bVal) \
            { \
                m_uFlags |= uMask; \
            } \
            else \
            { \
                m_uFlags &= ~uMask; \
            } \
        }\
        bool getBit(type uMask) const \
        { \
            return (m_uFlags & uMask) != 0; \
        }
#endif

