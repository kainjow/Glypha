#ifndef GLSOUNDS_H
#define GLSOUNDS_H

#include "GLResources.h"

enum {
    kBirdSound = 0,
    kBonusSound = 1,
    kBoom1Sound = 2,
    kBoom2Sound = 3,
    kSplashSound = 4,
    kFlapSound = 5,
    kGrateSound = 6,
    kLightningSound = 7,
    kMusicSound = 8,
    kScreechSound = 9,
    kSpawnSound = 10,
    kWalkSound = 11,
    kFlap2Sound = 12,
    kScrape2Sound = 13,
    
    kMaxSounds
};

namespace GL {

class Sounds {
public:
    Sounds() {
        initContext();
        load(kBirdSound, bird_wav, bird_wav_len);
        load(kBonusSound, bonus_wav, bonus_wav_len);
        load(kBoom1Sound, boom1_wav, boom1_wav_len);
        load(kBoom2Sound, boom2_wav, boom2_wav_len);
        load(kFlap2Sound, flap2_wav, flap2_wav_len);
        load(kFlapSound, flap_wav, flap_wav_len);
        load(kGrateSound, grate_wav, grate_wav_len);
        load(kLightningSound, lightning_wav, lightning_wav_len);
        load(kMusicSound, music_wav, music_wav_len);
        load(kScrape2Sound, scrape2_wav, scrape2_wav_len);
        load(kScreechSound, screech_wav, screech_wav_len);
        load(kSpawnSound, spawn_wav, spawn_wav_len);
        load(kSplashSound, splash_wav, splash_wav_len);
        load(kWalkSound, walk_wav, walk_wav_len);
    }
    
    int preloadCount(int which) {
        switch (which) {
            case kBonusSound:
            case kBoom2Sound:
            case kSplashSound:
            case kFlapSound:
            case kLightningSound:
            case kWalkSound:
                return 3;
            case kSpawnSound:
            case kGrateSound:
            case kFlap2Sound:
            case kScrape2Sound:
            case kScreechSound:
                return 8;
        }
        return 1;
    }
    
    void initContext();
    void load(int which, const unsigned char *buf, unsigned bufLen);
    void play(int which);
    
private:
    void *context;
};
    
}

#endif
