//
//  AppController.m
//  Glypha
//
//  Created by Kevin Wojniak on 7/18/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#import "AppController.h"
#include "GLGame.h"
#include "GLRenderer.h"

@implementation AppController

@synthesize window = _window;
@synthesize gameView = _gameView;

- (id)init
{
    self = [super init];
    if (self != nil) {
        game_ = new GLGame;
    }
    return self;
}

- (void)awakeFromNib
{
    [self.gameView setGame:game_];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Center the window, then set the autosave name. If the frame already has been saved, it'll override the centering.
    [self.window center];
    [self.window setFrameAutosaveName:@"MainWindow"];
}

@end

@implementation GameView

- (id)initWithFrame:(NSRect)frameRect
{
    NSOpenGLPixelFormatAttribute attr[] = {NSOpenGLPFAAccelerated, NSOpenGLPFADoubleBuffer, NSOpenGLPFADepthSize, 24, 0};
    NSOpenGLPixelFormat *format = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attr] autorelease];
    return [super initWithFrame:frameRect pixelFormat:format];
}

- (void)setGame:(GLGame *)game
{
    game_ = game;
}

- (void)prepareOpenGL
{
    // Synchronize buffer swaps with vertical refresh rate
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval]; 

    // Start the timer, and add it to other run loop modes so it redraws when a modal dialog is up or during event loops.
    renderTimer_ = [[NSTimer scheduledTimerWithTimeInterval:game_->updateFrequency() target:self selector:@selector(renderTimer) userInfo:nil repeats:YES] retain];
    [[NSRunLoop currentRunLoop] addTimer:renderTimer_ forMode:NSEventTrackingRunLoopMode];
    [[NSRunLoop currentRunLoop] addTimer:renderTimer_ forMode:NSModalPanelRunLoopMode];
}

- (void)renderTimer
{
	[self setNeedsDisplay:YES];
}

- (void)reshape
{
	NSRect bounds = [self bounds];
    game_->renderer()->resize(bounds.size.width, bounds.size.height);
	[[self openGLContext] update];
}

- (void)drawRect:(NSRect)rect
{
	[[self openGLContext] makeCurrentContext];
	game_->draw();
	[[self openGLContext] flushBuffer];
}

- (void)mouseDown:(NSEvent *)event
{
    NSPoint mouseLoc = [self convertPoint:[event locationInWindow] fromView:nil];
    GLPoint point;
    point.h = mouseLoc.x;
    point.v = game_->renderer()->bounds().height() - mouseLoc.y;
    game_->handleMouseDownEvent(point);
}

@end

int main(int argc, char *argv[])
{
    return NSApplicationMain(argc, (const char **)argv);
}
