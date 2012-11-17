//
//  GLUtils.h
//  Glypha
//
//  Created by Kevin Wojniak on 8/1/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#ifndef GLUTILS_H
#define GLUTILS_H

#if _WIN32
#include <windows.h>
#endif

class GLUtils {
public:
    static int randomInt(int end);
    
    static double now();

#if _WIN32
    static void win32_log(LPCWSTR format, ...);
#endif
};

#endif
