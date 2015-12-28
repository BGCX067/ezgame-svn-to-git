#ifndef EZMAX_H
#define EZMAX_H

// Temporarily disable "for" scope conformance option
#pragma conform(forScope, push, f, off)


//#include "shaders.h"
//#include "iColorMan.h"
//#include "toneop.h"
//#include "3dsmaxport.h"

#include "max.h"
#include "shaders.h"

#include "macrorec.h"
#include "gport.h"
#include "utilapi.h"
#include "simpobj.h"
#include "modstack.h"
#include "stdmat.h"
#include "spline3d.h"
#include "splshape.h"
#include "decomp.h"
#include "bmmlib.h"
#include "ikctrl.h"
#include "strclass.h"
#include "interpik.h"
#include "notetrck.h"

#include "iparamb2.h"
#include "iparamm2.h"
#include "simpspl.h"
#include "ISkin.h"



TCHAR *GetString(int id);

//// Max 6-8 are API compatable
//#if MAX_RELEASE == 6000 || MAX_RELEASE == 6900 || MAX_RELEASE == 7000
//#include "NiMAX60ExtraTypes.h"
//#elif MAX_RELEASE == 7800 || MAX_RELEASE == 8000
//#include "NiMAX60ExtraTypes.h"
//#include <XRef\IXRefMaterial.h>
//// Max 9+ are API compatable
//#elif MAX_RELEASE == 8901 || MAX_RELEASE == 9000 || MAX_RELEASE == 10000
//#include "NiMAX90ExtraTypes.h"
//#else
//#error "Unknown MAX Version!"
//#endif

//#ifndef NO_CSTUDIO
//#include "phyexp.h"
//#endif

// Re-enable "for" scope conformance option
#pragma conform(forScope, pop, f)

#include "cassert"
//#endif

#endif