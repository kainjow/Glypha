//
//  Created by Kevin Wojniak on 7/27/13.
//  Copyright (c) 2013 Kevin Wojniak. All rights reserved.
//

#ifndef GLSOUNDS_H
#define GLSOUNDS_H

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

class GLSounds {
public:
    GLSounds();
    
    void play(int which);
    
private:
    class Imp;
    Imp *imp;
};

#endif