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
#include <sys/time.h>

int GLUtils::randomInt(int end)
{
	static int seeded = 0;
	if (!seeded) {
		srandom((unsigned)time(NULL));
		seeded = 1;
	}
	return ((int)random() % end);
}

double GLUtils::now()
{
    struct timeval val;
    (void)gettimeofday(&val, NULL);
    return (double)val.tv_sec + ((double)val.tv_usec / 1000000.0);
}
