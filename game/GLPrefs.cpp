#include "GLPrefs.h"
#ifdef GLYPHA_QT
#include <QSettings>
#elif defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#endif

bool GL::Prefs::load(PrefsInfo& thePrefs)
{
#ifdef GLYPHA_QT
    QSettings settings;
    QByteArray data = settings.value("prefs", QByteArray()).toByteArray();
    if (data.size() != sizeof(thePrefs)) {
        return false;
    }
    memcpy(&thePrefs, data.data(), sizeof(thePrefs));
    return true;
#elif defined(__APPLE__)
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
    return true;
#else
    (void)thePrefs;
    return false;
#endif
}

void GL::Prefs::save(const PrefsInfo& thePrefs)
{
#ifdef GLYPHA_QT
    QSettings settings;
    settings.setValue("prefs", QByteArray((const char*)&thePrefs, sizeof(thePrefs)));
#elif defined(__APPLE__)
    CFDataRef data = CFDataCreate(kCFAllocatorDefault, (const UInt8*)&thePrefs, sizeof(thePrefs));
    if (!data) {
        printf("Failed to create CFData!\n");
    } else {
        CFPreferencesSetAppValue(CFSTR("prefs"), data, kCFPreferencesCurrentApplication);
        CFRelease(data);
    }
#else
    (void)thePrefs;
#endif
}
