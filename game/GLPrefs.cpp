#include "GLPrefs.h"
#ifdef GLYPHA_QT
#include <QSettings>
#elif defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#elif defined(_WIN32)
#include <windows.h>
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
#elif defined(_WIN32)
    DWORD size = 0;
    LPCWSTR subkey = L"SOFTWARE\\" GL_GAME_NAME_W;
    LPCWSTR value = L"Prefs";
    if (RegGetValueW(HKEY_CURRENT_USER, subkey, value, RRF_RT_REG_BINARY, nullptr, nullptr, &size) != ERROR_SUCCESS || size != sizeof(thePrefs)) {
        return false;
    }
    return RegGetValueW(HKEY_CURRENT_USER, subkey, value, RRF_RT_REG_BINARY, nullptr, (PVOID)&thePrefs, &size) == ERROR_SUCCESS;
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
#elif defined(_WIN32)
    LPCWSTR subkey = L"SOFTWARE\\" GL_GAME_NAME_W;
    LPCWSTR value = L"Prefs";
    HKEY key = nullptr;
    LSTATUS status = RegCreateKeyExW(HKEY_CURRENT_USER, subkey, 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &key, nullptr);
    if (status == ERROR_SUCCESS) {
        RegSetValueExW(key, value, 0, REG_BINARY, (const BYTE*)&thePrefs, (DWORD)sizeof(thePrefs));
        RegCloseKey(key);
    }
#else
    (void)thePrefs;
#endif
}
