#include "EzCoreUtility.h"
#include "EzLogger.h"

char EzLogger::ms_acBuffer[MAX_OUTPUT_LENGTH]; // for formatting
EzLogMessageOptions EzLogger::ms_aMessageOptions[EZMESSAGE_MAX_TYPES];
unsigned int EzLogger::ms_uiLogFileCount;
EzLogger::EzLogFile EzLogger::ms_aLogFile[MAX_NUM_LOGFILES];
EzFastCriticalSection EzLogger::ms_CriticalSection;


EzLogMessageOptions::EzLogMessageOptions() : m_bOutputToDebugWindow(false), 
    m_iLogID(-1), m_bPrependTimestamp (false)
{
}


EzLogger::EzLogger(int iMessageType, const char* pcFormat,  ...) : m_iMessageType(iMessageType)
{
    if(pcFormat)
    {
        ms_CriticalSection.lock();

        va_list kArgList;
        va_start( kArgList, pcFormat );

        formatOutput(pcFormat, kArgList);

        va_end(kArgList);

        logBuffer();

        ms_CriticalSection.unlock();
    }
}

EzLogger::EzLogger(const char* pcFormat, ...) : m_iMessageType(0)
{
    if(pcFormat)
    {
        ms_CriticalSection.lock();

        va_list kArgList;
        va_start( kArgList, pcFormat );

        formatOutput(pcFormat, kArgList);

        va_end(kArgList);

        logBuffer();

        ms_CriticalSection.unlock();
    }
}


void EzLogger::init()
{
    for (unsigned int uiLogFile = 0; uiLogFile < MAX_NUM_LOGFILES; 
        uiLogFile++)
    {
        ms_aLogFile[uiLogFile].free();        
    }

}
void EzLogger::shutdown()
{
	closeAllLogs();

}


void EzLogger::logBuffer()
{
    if (ms_aMessageOptions[m_iMessageType].m_bOutputToDebugWindow)
    {
#if defined(WIN32) || defined(_XENON)
        OutputDebugStringA(ms_acBuffer);
#endif
    }

    if (ms_aMessageOptions[m_iMessageType].m_iLogID >= 0)
    {
        EzLogger::EzLogFile* pLogFile = &ms_aLogFile[ms_aMessageOptions[m_iMessageType].m_iLogID];

        if (pLogFile->isActive())
            pLogFile->log(ms_acBuffer);
    }
}


void EzLogger::formatOutput(const char* pcFormat, va_list kArgList)
{
    char* pcBuffer = ms_acBuffer;
    unsigned int uiMaxOutputLength = MAX_OUTPUT_LENGTH;

    if (ms_aMessageOptions[m_iMessageType].m_bPrependTimestamp)
    {
        // prepend optional timestamp
        EzSprintf(pcBuffer, MAX_OUTPUT_LENGTH, "%f: ", 0.1f);
        unsigned int uiTimestampStrlen = strlen(pcBuffer);
        uiMaxOutputLength -= uiTimestampStrlen;
        pcBuffer += uiTimestampStrlen;

    }

    EzVsprintf(pcBuffer, uiMaxOutputLength, pcFormat, kArgList);
}


int EzLogger::openLog(int iMessageType, const char* pcFilename, EOpenMode eOpenMode, 
    bool bFlushOnWrite, bool bCommitToDisk)
{
    if (ms_uiLogFileCount == EzLogger::MAX_NUM_LOGFILES)
    {
        // No available slots.
        return -1;
    }

    // Find an open slot
    int iSlot = 0;
    
    while (iSlot < EzLogger::MAX_NUM_LOGFILES)
    {
        if (!EzLogger::ms_aLogFile[iSlot].isActive())
        {
            break;
        }

		iSlot++;
    }

    // Make sure we found one
    if (iSlot == EzLogger::MAX_NUM_LOGFILES)
    {
        // there are no slots available
        EZASSERT(!"EzLogger::OpenLog - no slots available.");
        return -1;
    }

    // Create the log file instance
    EzLogger::ms_aLogFile[iSlot].init(pcFilename, eOpenMode, 
        bFlushOnWrite, bCommitToDisk);
    if (!EzLogger::ms_aLogFile[iSlot].isActive())
    {
        // Failed to create the file!
        return -1;
    }

	EzLogger::ms_aMessageOptions[iMessageType].m_iLogID = iSlot;

    EzLogger::ms_uiLogFileCount++;

    return iSlot;
}

void EzLogger::closeLog(int iMessageType)
{
	int iLogID = EzLogger::ms_aMessageOptions[iMessageType].m_iLogID;

    if (iLogID >= 0 && iLogID < EzLogger::MAX_NUM_LOGFILES && 
        EzLogger::ms_aLogFile[iLogID].isActive())
    {
        EzLogger::ms_aLogFile[iLogID].free();
        EzLogger::ms_uiLogFileCount--;
    }
}

void EzLogger::closeAllLogs()
{
    for (unsigned int uiLogID = 0; uiLogID < EzLogger::MAX_NUM_LOGFILES; 
        uiLogID++)
    {
        EzLogger::ms_aLogFile[uiLogID].free();
    }
    EzLogger::ms_uiLogFileCount = 0;
}



EzLogger::EzLogFile::EzLogFile()
{
     m_acName[0] = '\0';
}

EzLogger::EzLogFile::~EzLogFile()
{
    closeFile();
}

void EzLogger::EzLogFile::closeFile()
{
    if (m_pFile)
    {
        fclose(m_pFile);
    }
    m_pFile = 0;
}

bool EzLogger::EzLogFile::openFile(EOpenMode eOpenMode, bool bCommitToDisk)
{
    char acOpenMode[5];

    switch (eOpenMode)
    {
    case ELOM_OPEN_APPEND:
        EzStringCpy(acOpenMode, 5, "a+t");
        break;
    case ELOM_OPEN_OVERWRITE:
        EzStringCpy(acOpenMode, 5, "wt");
        break;
    default:
        EZASSERT(!"EzLogger::OPEN_UNIQUENAME Not yet supported");
        m_pFile = 0;
        return false;
    }

    if (bCommitToDisk)
        EzStringCat(acOpenMode, 5, "c");

#if _MSC_VER >= 1400
    if (fopen_s(&m_pFile, m_acName, acOpenMode) != 0 || !m_pFile)
        return false;
#else //#if _MSC_VER >= 1400
    m_pFile = fopen(m_acName, acOpenMode);

    if (!m_pFile)
        return false;
#endif //#if _MSC_VER >= 1400

   

    return true;
}

void EzLogger::EzLogFile::init(const char* pcName, EOpenMode eOpenMode, bool bFlushOnWrite, bool bCommitToDisk)
{
    EZASSERT(pcName);

    EzStringCpy(m_acName, 256, pcName);
    m_bFlushOnWrite = bFlushOnWrite;

    if (!openFile(eOpenMode, bCommitToDisk))
    {
		OutputDebugString("EzLogger::EzLogFile::Init Error - Failed to open the following file:");
        OutputDebugString(pcName);
        m_acName[0] = '\0';
        m_pFile = 0;        
    }
}

void EzLogger::EzLogFile::free()
{
    closeFile();
    m_acName[0] = '\0';
}

bool EzLogger::EzLogFile::isActive()
{
    return m_acName[0] != '\0';
}

void EzLogger::EzLogFile::log(char* pcMessage)
{
    if (!m_pFile)
        return;

    fwrite(pcMessage, sizeof(char), strlen(pcMessage), m_pFile);
    if (m_bFlushOnWrite)
        fflush(m_pFile);
}

void EzLogger::flushAllLogs()
{
    for (unsigned int uiLogID = 0; uiLogID < EzLogger::MAX_NUM_LOGFILES; 
        uiLogID++)
    {
        if (EzLogger::ms_aLogFile[uiLogID].isActive())
            fflush(EzLogger::ms_aLogFile[uiLogID].m_pFile);
    }
}
