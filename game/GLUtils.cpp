//
//  GLUtils.cpp
//  Glypha
//
//  Created by Kevin Wojniak on 8/1/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#include "GLUtils.h"
#include <cstdio>
#include <time.h>
#include <cstdlib>

int GLUtils::randomInt(int end)
{
	static int seeded = 0;
	if (!seeded) {
		srandom((unsigned)time(NULL));
		seeded = 1;
	}
	return ((int)random() % end);
}
