#include "iplatfrm.h"

#if defined(WIN32)

long _i_timer()
{
	return clock();
}

#elif defined(__unix)

long _i_timer()
{
	static struct timezone tz={ 0,0 };
	struct timeval time;
	gettimeofday(&time,&tz);
	return (time.tv_sec*1000+time.tv_usec/1000);
}

#else

long _i_timer()
{
	return (clock()*1000)/CLK_TCK;
}

#endif

