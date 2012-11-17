//
//  GLUtils.cpp
//  Glypha
//
//  Created by Kevin Wojniak on 8/1/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#include "GLUtils.h"
#include <cstdio>
#include <cstdlib>
#include <time.h>
#if _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

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
