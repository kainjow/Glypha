#include "GLSounds.h"
#include <AVFoundation/AVFoundation.h>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <thread>

class Context {
public:
    std::vector<AVAudioPlayer*> sounds[kMaxSounds];
    std::thread thread;
    std::mutex mutex;
    std::condition_variable cond;
    std::vector<int> play_queue;
    
    void addToQueue(int which);
    
    static void thread_main(Context* ctx);
    void run();
    
    Context()
        : thread{thread_main, this}
    {
    }

private:
    void play(int which);
};

void GL::Sounds::initContext()
{
    context = new Context;
}

void GL::Sounds::play(int which)
{
    Context *ctx = static_cast<Context*>(context);
    ctx->addToQueue(which);
}

void Context::addToQueue(int which)
{
    std::unique_lock<std::mutex> locker{mutex};
    play_queue.push_back(which);
    cond.notify_one();
}

void Context::thread_main(Context* ctx)
{
    ctx->run();
}

void Context::run()
{
    std::vector<int> play_now;
    for (;;) {
        {
            std::unique_lock<std::mutex> locker{mutex};
            cond.wait(locker, [this]{
                return !play_queue.empty();
            });
            play_now = play_queue;
            play_queue.clear();
        }
        
        for (const auto& which : play_now) {
            play(which);
        }
    }
}

void Context::play(int which)
{
    bool found = false;
    for (const auto& player : sounds[which]) {
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
