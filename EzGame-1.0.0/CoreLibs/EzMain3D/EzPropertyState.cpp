#include "EzPropertyState.h"


EzImplementRootRTTI(EzPropertyState);

EzPropertyState::EzPropertyState(void)
{
}

EzPropertyState::~EzPropertyState(void)
{
}

EzPropertyState& EzPropertyState::operator=(const EzPropertyState& State)
{
    unsigned int ui;
    for (ui = 0; ui < EzProperty::MAX_TYPES; ui++)
        m_aspProps[ui] = State.m_aspProps[ui];

    return *this;
}


