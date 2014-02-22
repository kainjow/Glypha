#include "GLUtils.h"
#include <cstdio>
#include <cstdlib>
#include <time.h>
#if _WIN32
#include <strsafe.h>
#endif

GL::Utils::Utils()
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
int GL::Utils::randomInt(int end) const
{
#if _WIN32
    return ((int)rand() % end);
#else
	return ((int)random() % end);
#endif
}

// Returns the time in seconds with millisecond precision
double GL::Utils::now() const
{
#if _WIN32
    LARGE_INTEGER t;
    (void)QueryPerformanceCounter(&t);
    return (double)t.QuadPart / (double)freq.QuadPart;
#elif __APPLE__
    return mach_absolute_time() * mach_convert;
#else
	struct timespec t;
	(void)clock_gettime(CLOCK_MONOTONIC, &t);
	return (double)t.tv_sec + (t.tv_nsec * 1000000000.0);
#endif
}

#if _WIN32
// Handy function for logging. Works like printf() but outputs
// to the debugger window since we have no console in a Win32 GUI.
void GL::Utils::log(LPCWSTR format, ...)
{
    WCHAR buf[200];
    va_list ap;
    va_start(ap, format);
    (void)StringCbVPrintfW(buf, sizeof(buf)/sizeof(buf[0]), format, ap);
    va_end(ap);
    OutputDebugStringW(buf);
}

void GL::Utils::log(LPCSTR format, ...)
{
    CHAR buf[200];
    va_list ap;
    va_start(ap, format);
    (void)StringCbVPrintfA(buf, sizeof(buf), format, ap);
    va_end(ap);
    OutputDebugStringA(buf);
}
#endif
