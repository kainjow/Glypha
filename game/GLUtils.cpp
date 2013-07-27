//
//  Created by Kevin Wojniak on 8/1/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#include "GLUtils.h"
#include <cstdio>
#include <cstdlib>
#include <time.h>
#if _WIN32
#include <strsafe.h>
#else
#include <sys/time.h>
#endif

// Returns a random number from 0 - end
int GLUtils::randomInt(int end)
{
	static int seeded = 0;
	if (!seeded) {
#if _WIN32
        srand(unsigned(time(NULL)));
#else
		srandom((unsigned)time(NULL));
#endif
		seeded = 1;
	}
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
    LARGE_INTEGER freq, t;
    (void)QueryPerformanceFrequency(&freq);
    (void)QueryPerformanceCounter(&t);
    return (t.QuadPart * 1000.0) / freq.QuadPart;
#else
    struct timeval val;
    (void)gettimeofday(&val, NULL);
    return (double)val.tv_sec + ((double)val.tv_usec / 1000000.0);
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
