#include "GLSounds.h"
#include <AVFoundation/AVFoundation.h>
#include <list>

struct Context {
    std::list<AVAudioPlayer*> sounds[kMaxSounds];
};

void GLSounds::initContext()
{
    context = new Context;
}

void GLSounds::play(int which)
{
    Context *ctx = static_cast<Context*>(context);
    bool found = false;
    for (std::list<AVAudioPlayer*>::const_iterator it = ctx->sounds[which].begin(); it != ctx->sounds[which].end(); ++it) {
        AVAudioPlayer *player = *it;
        if (!player.isPlaying) {
            [player play];
            found = true;
            break;
        }
    }
    if (!found) {
        AVAudioPlayer *player = [[AVAudioPlayer alloc] initWithData:ctx->sounds[which].front().data error:nil];
        ctx->sounds[which].push_back(player);
        [player play];
    }
}

void GLSounds::load(int which, const unsigned char *buf, unsigned bufLen)
{
    Context *ctx = static_cast<Context*>(context);
    NSData *data = [NSData dataWithBytesNoCopy:(void*)buf length:bufLen freeWhenDone:NO];
    AVAudioPlayer *player = [[AVAudioPlayer alloc] initWithData:data error:nil];
    [player prepareToPlay];
    ctx->sounds[which].push_back(player);
}
