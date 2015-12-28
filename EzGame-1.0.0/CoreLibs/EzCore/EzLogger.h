#pragma once

#include "EzCoreLibType.h"
#include "EzCriticalSection.h"
#include <Stdio.h>


#define EZMESSAGE_GENERAL_0 0
#define EZMESSAGE_GENERAL_1 1
#define EZMESSAGE_GENERAL_2 2 
#define EZMESSAGE_MEMORY_COMPLETE 			3
#define EZMESSAGE_MEMORY_LEAK			 	4
#define EZMESSAGE_MEMORY_2 					5
#define EZMESSAGE_MEMORY_TIMESTAMP 			6
#define EZMESSAGE_RESERVED_FOR_LOGDIRECT 	7
#define EZMESSAGE_MAX_INTERNAL_INDEX NIMESSAGE_RESERVED_FOR_LOGDIRECT
#define EZMESSAGE_MAX_TYPES 32

#define LOG_CONCAT2(name, l) name##l
#define LOG_CONCAT1(name, l) LOG_CONCAT2(name, l)
#if defined (_MSC_VER) && (_MSC_VER >= 1310)
    #define INSERT_UNIQUE_NAME(name) LOG_CONCAT1(name,__COUNTER__)
#else //#if defined (_MSC_VER) && (_MSC_VER >= 1310)
    // IMPORTANT NOTE: This macro do not expand properly if compiled with
    // Visual Studio's Edit And Continue enabled (/ZI compiler option).
    #define INSERT_UNIQUE_NAME(name) LOG_CONCAT1(name,__LINE__)
#endif //#if defined (_MSC_VER) && (_MSC_VER >= 1310)


#if !defined _LOGGER_DISABLE 
    #ifdef __GNUC__
        #define EZLOG EzLogger __attribute__ ((__unused__)) \
            INSERT_UNIQUE_NAME(xx_Log_)
        #define EZLOGDIRECT EzLoggerDirect __attribute__ ((__unused__)) \
            INSERT_UNIQUE_NAME(xx_Log_)
    #else
        #define EZLOG EzLogger INSERT_UNIQUE_NAME(xx_Log_)
    #endif // #ifdef __GNUC__

#else
    #if defined(__GNUC__) || defined(__MWERKS__)
        inline void EzNullFunction(int, const char*,  ...) { }
        inline void EzNullFunction(const char*,  ...) { }
        #define EZLOG EzNullFunction

    #elif defined (_MSC_VER) && (_MSC_VER >= 1300)
        #define EZLOG __noop
    #else
        #define EZLOG ((void)0)
    #endif // #ifdef __GNUC__
#endif
// End macro defines.

class EZCORE_ENTRY EzLogMessageOptions
{
public:
    EzLogMessageOptions();
    bool m_bOutputToDebugWindow;
    int m_iLogID;
    bool m_bPrependTimestamp;
};


class EZCORE_ENTRY EzLogger
{
public:
    EzLogger(int iMessageType, const char* pcFormat,  ...);
    EzLogger(const char* pcFormat,  ...);
        
    enum EOpenMode
    {
        ELOM_OPEN_APPEND,
        ELOM_OPEN_OVERWRITE,
        ELOM_OPEN_UNIQUENAME
    };

	static void init();
	static void shutdown();

    static int openLog(int iMessageType, const char* pcFilename, EOpenMode eOpenMode = 
        ELOM_OPEN_OVERWRITE, bool bFlushOnWrite = false,
        bool bCommitToDisk = false);
    static void closeLog(int iMessageType);
    static void closeAllLogs();
    static void flushAllLogs();

    // Get/Set MessageBehavior options for each message type
    static void setOutputToDebugWindow(int iMessageType, bool bOutput);
    static bool getOutputToDebugWindow(int iMessageType);

    static void setPrependTimestamp(int iMessageType, bool bTimestamp);
    static bool getPrependTimestamp(int iMessageType);

    static void setOutputToLogFile(int iMessageType, int iLogID);
    static int getOutputToLogFile(int iMessageType);


protected:
    EzLogger() { }
    class EzLogFile
    {
    public:
        char m_acName[256];

        FILE* m_pFile;

        EzLogFile();
        ~EzLogFile();

        void init(const char* pcName, EOpenMode eOpenMode, bool bFlushOnWrite,
            bool bCommitToDisk);
        void free();

        void log(char* pcMessage);
        bool isActive();

    protected:
        bool openFile(EOpenMode eOpenMode, bool bCommitToDisk);
        void closeFile();
        bool m_bFlushOnWrite;
    };

    enum
    {
        MAX_NUM_LOGFILES = 16
    };

    enum 
    {
        MAX_OUTPUT_LENGTH = 1024
    };

    void formatOutput(const char* pcFormat, va_list ArgList);
    void logBuffer();

    int m_iMessageType;

    static char ms_acBuffer[MAX_OUTPUT_LENGTH]; // for formatting
    static EzLogMessageOptions ms_aMessageOptions[EZMESSAGE_MAX_TYPES];
    static unsigned int ms_uiLogFileCount;
    static EzLogFile ms_aLogFile[MAX_NUM_LOGFILES];
    static EzFastCriticalSection ms_CriticalSection;
};

inline void EzLogger::setOutputToDebugWindow(int iMessageType, bool bOutput)
{
    ms_aMessageOptions[iMessageType].m_bOutputToDebugWindow = bOutput;
}

inline bool EzLogger::getOutputToDebugWindow(int iMessageType)
{
    return ms_aMessageOptions[iMessageType].m_bOutputToDebugWindow;
}

inline void EzLogger::setPrependTimestamp(int iMessageType, bool bTimestamp)
{
    ms_aMessageOptions[iMessageType].m_bPrependTimestamp = bTimestamp;
}

inline bool EzLogger::getPrependTimestamp(int iMessageType)
{
    return ms_aMessageOptions[iMessageType].m_bPrependTimestamp;
}
inline void EzLogger::setOutputToLogFile(int iMessageType, int iLogID)
{
    ms_aMessageOptions[iMessageType].m_iLogID = iLogID;
}

inline int EzLogger::getOutputToLogFile(int iMessageType)
{
    return ms_aMessageOptions[iMessageType].m_iLogID;
}

