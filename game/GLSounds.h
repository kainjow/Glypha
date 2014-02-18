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
        load(kBirdSound, bird_aif, bird_aif_len);
        load(kBonusSound, bonus_aif, bonus_aif_len);
        load(kBoom1Sound, boom1_aif, boom1_aif_len);
        load(kBoom2Sound, boom2_aif, boom2_aif_len);
        load(kFlap2Sound, flap2_aif, flap2_aif_len);
        load(kFlapSound, flap_aif, flap_aif_len);
        load(kGrateSound, grate_aif, grate_aif_len);
        load(kLightningSound, lightning_aif, lightning_aif_len);
        load(kMusicSound, music_aif, music_aif_len);
        load(kScrape2Sound, scrape2_aif, scrape2_aif_len);
        load(kScreechSound, screech_aif, screech_aif_len);
        load(kSpawnSound, spawn_aif, spawn_aif_len);
        load(kSplashSound, splash_aif, splash_aif_len);
        load(kWalkSound, walk_aif, walk_aif_len);
    }
    
    int preloadCount(int which) {
        switch (which) {
            case kBonusSound:
            case kFlapSound:
            case kGrateSound:
            case kLightningSound:
            case kSpawnSound:
            case kWalkSound:
                return 3;
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