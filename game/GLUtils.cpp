//
//  Created by Kevin Wojniak on 8/1/12.
//  Copyright (c) 2012-2013 Kevin Wojniak. All rights reserved.
//

#include "GLUtils.h"
#include <cstdio>
#include <cstdlib>
#include <time.h>
#if _WIN32
#include <strsafe.h>
#endif

GLUtils::GLUtils()
{
#if __APPLE__
    mach_timebase_info_data_t timebaseInfo;
    (void)mach_timebase_info(&timebaseInfo);
    mach_convert = ((double)timebaseInfo.numer / (double)timebaseInfo.denom) / NSEC_PER_SEC;
#elif _WIN32
    (void)QueryPerformanceFrequency(&freq);
#endif
    
#if _WIN32
    srand(unsigned(time(NULL)));
#else
    srandom((unsigned)time(NULL));
#endif
}

// Returns a random number from 0 - end
int GLUtils::randomInt(int end)
{
#if _WIN32
    return ((int)rand() % end);
#else
	return ((int)random() % end);
#endif
}

// Returns the time in seconds with millisecond precision
double GLUtils::now()
{
#if _WIN32
    LARGE_INTEGER t;
    (void)QueryPerformanceCounter(&t);
    return (t.QuadPart * 1000.0) / freq.QuadPart;
#elif __APPLE__
    return mach_absolute_time() * mach_convert;
#elif __HAIKU__
	struct timespec t;
	(void)clock_gettime(CLOCK_MONOTONIC, &t);
	return (double)t.tv_sec + (t.tv_nsec * 1000000000.0);
#else
	#error unknown platform
#endif
}

#if _WIN32
// Handy function for logging. Works like printf() but outputs
// to the debugger window since we have no console in a Win32 GUI.
void GLUtils::win32_log(LPCWSTR format, ...)
{
    WCHAR buf[200];
    va_list ap;
    va_start(ap, format);
    (void)StringCbVPrintf(buf, sizeof(buf)/sizeof(buf[0]), format, ap);
    va_end(ap);
    OutputDebugString(buf);
}
#endif
