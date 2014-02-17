#ifndef GLUTILS_H
#define GLUTILS_H

#if __APPLE__
#include <mach/mach_time.h>
#elif _WIN32
#include <windows.h>
#endif

namespace GL {

class Utils {
public:
    Utils();
    
    int randomInt(int end) const;
    
    double now() const;

#if _WIN32
    static void log(LPCWSTR format, ...) const;
    static void log(LPCSTR format, ...) const;
#endif
    
private:
#if __APPLE__
    double mach_convert;
#elif _WIN32
    LARGE_INTEGER freq;
#endif
};

}

#endif
