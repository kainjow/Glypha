#include "GLSounds.h"
#include "GLResources.h"
#include <AVFoundation/AVFoundation.h>
#include <list>

class GLSounds::Imp {
public:
    Imp() {
        loadSound(kBirdSound, bird_aif, bird_aif_len);
        loadSound(kFlapSound, flap_aif, flap_aif_len);
        loadSound(kGrateSound, grate_aif, grate_aif_len);
        loadSound(kWalkSound, walk_aif, walk_aif_len);
        loadSound(kScreechSound, screech_aif, screech_aif_len);
    }
    
    void play(int which) {
        bool found = false;
        for (std::list<AVAudioPlayer*>::const_iterator it = sounds[which].begin(); it != sounds[which].end(); ++it) {
            AVAudioPlayer *player = *it;
            if (!player.isPlaying) {
                [player play];
                found = true;
                break;
            }
        }
        if (!found) {
            //NSLog(@"Copy %d", which);
            AVAudioPlayer *player = [[AVAudioPlayer alloc] initWithData:sounds[which].front().data error:nil];
            sounds[which].push_back(player);
            [player play];
        }
    }
    
    void loadSound(int which, const unsigned char *buf, unsigned bufLen) {
        NSData *data = [NSData dataWithBytesNoCopy:(void*)buf length:bufLen freeWhenDone:NO];
        AVAudioPlayer *player = [[AVAudioPlayer alloc] initWithData:data error:nil];
        [player prepareToPlay];
        sounds[which].push_back(player);
    }
    
private:
    std::list<AVAudioPlayer*> sounds[kMaxSounds];
};

GLSounds::GLSounds() :
    imp(new Imp)
{
}

void GLSounds::play(int which)
{
    imp->play(which);
}
