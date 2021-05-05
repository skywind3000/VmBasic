#ifndef __I_PLATFORM_

#include "ifstream.h"

#if defined(WIN32)
#define __I_PLATFORM_ "WIN32"
#include <windows.h>
#include <time.h>

long _i_timer();


#elif defined(__unix)

#define __I_PLATFORM_ "UNIX"

#include <sys/time.h>

long _i_timer();

#else

#define __I_PLATFORM_ "UNKNOW"

#include <time.h>

long _i_timer();

#endif

#endif
