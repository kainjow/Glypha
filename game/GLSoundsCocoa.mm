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
    const std::vector<AVAudioPlayer*>::const_iterator end = ctx->sounds[which].end();
    for (std::vector<AVAudioPlayer*>::const_iterator it = ctx->sounds[which].begin(); it != end; ++it) {
        AVAudioPlayer *player = *it;
        if (!player.isPlaying) {
            [player play];
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
            printf("Failed to prepare sound %d\n", which);
            continue;
        }
        ctx->sounds[which].push_back(player);
    }
}
