#ifndef MemoryLeak_h
#define MemoryLeak_h

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include "crtdbg.h"

#ifdef _DEBUG
#define NORMAL_DEBUG_NEW new(_NORMAL_BLOCK, __FILE__,__LINE__)
#else
#define NORMAL_DEBUG_NEW
#endif

#ifdef _DEBUG
#define new NORMAL_DEBUG_NEW
#endif

#endif