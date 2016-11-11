#include "GLSounds.h"
#include <AVFoundation/AVFoundation.h>
#include <vector>

struct Context {
    std::vector<AVAudioPlayer*> sounds[kMaxSounds];
};

void GL::Sounds::initContext()
{
    context = new Context;
}

void GL::Sounds::play(int which)
{
    Context *ctx = static_cast<Context*>(context);
    bool found = false;
    for (const auto& player : ctx->sounds[which]) {
        if (!player.isPlaying) {
            if (![player play]) {
                printf("Can't play sound %d\n", which);
            }
            found = true;
            break;
        }
    }
    if (!found) {
        printf("Preloaded sound not available for %d (maybe increase preload count?)\n", which);
    }
}

void GL::Sounds::load(int which, const unsigned char *buf, unsigned bufLen)
{
    Context *ctx = static_cast<Context*>(context);
    NSData *data = [NSData dataWithBytesNoCopy:(void*)buf length:bufLen freeWhenDone:NO];
    int count = preloadCount(which);
    for (int i = 0; i < count; ++i) {
        NSError *err = nil;
        AVAudioPlayer *player = [[AVAudioPlayer alloc] initWithData:data error:&err];
        if (!player) {
            printf("Can't load sound %d: %s\n", which, err.localizedDescription.UTF8String);
            continue;
        }
        if (![player prepareToPlay]) {
            [player release];
            printf("Failed to prepare sound %d\n", which);
            continue;
        }
        ctx->sounds[which].push_back(player);
    }
}
