#include "GLPrefs.h"
#include <sstream>
#include <vector>
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

bool GL::Prefs::load(PrefsInfo& thePrefs)
{
#ifdef __APPLE__
    CFDataRef data = (CFDataRef)CFPreferencesCopyAppValue(CFSTR("prefs"), kCFPreferencesCurrentApplication);
    if (!data) {
        return false;
    }
    if (CFGetTypeID(data) != CFDataGetTypeID() || CFDataGetLength(data) != (CFIndex)sizeof(thePrefs)) {
        CFRelease(data);
        return false;
    }
    CFDataGetBytes(data, CFRangeMake(0, CFDataGetLength(data)), (UInt8*)&thePrefs);
    CFRelease(data);
#endif
    return true;
}

void GL::Prefs::save(const PrefsInfo& thePrefs)
{
#ifdef __APPLE__
    CFDataRef data = CFDataCreate(kCFAllocatorDefault, (const UInt8*)&thePrefs, sizeof(thePrefs));
    if (!data) {
        printf("Failed to create CFData!\n");
    } else {
        CFPreferencesSetAppValue(CFSTR("prefs"), data, kCFPreferencesCurrentApplication);
        CFRelease(data);
    }
#endif
}
