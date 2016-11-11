#ifndef GLUTILS_H
#define GLUTILS_H

#ifdef __APPLE__
#include <mach/mach_time.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace GL {

class Utils {
public:
    Utils();
    
    int randomInt(int end) const;
    
    double now() const;

#ifdef _WIN32
    static void log(LPCWSTR format, ...);
    static void log(LPCSTR format, ...);
#endif
    
private:
#ifdef __APPLE__
    double mach_convert;
#elif defined(_WIN32)
    LARGE_INTEGER freq;
#endif
};

}

#endif
