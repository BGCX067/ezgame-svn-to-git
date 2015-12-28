#include "EzTransform.h"


const EzPoint3 INVALID_TRANSLATE = EzPoint3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
const EzMatrix3 INVALID_ROTATE;
const float INVALID_SCALE = -FLT_MAX;

EzTransform::EzTransform(void)
: m_fScale(INVALID_SCALE), m_Rotation(INVALID_ROTATE), m_Translation(INVALID_TRANSLATE)
{

}
EzTransform::~EzTransform(void)
{

}

