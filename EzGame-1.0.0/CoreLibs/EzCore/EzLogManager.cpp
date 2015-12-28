#include "EzLogManager.h"
#include "EzLogger.h"

template<> 
EzLogManager* EzSingleton<EzLogManager>::ms_pSingleton = 0;

EzLogManager::EzLogManager(void)
{
}


EzLogManager::~EzLogManager(void)
{
}


void EzLogManager::init()
{
#ifdef _DEBUG
    EzLogger::setOutputToDebugWindow(EZMESSAGE_GENERAL_0, true);
    EzLogger::setOutputToDebugWindow(EZMESSAGE_GENERAL_1, true);
    EzLogger::setOutputToDebugWindow(EZMESSAGE_GENERAL_2, true);
#endif

}

void EzLogManager::shutdown()
{

}