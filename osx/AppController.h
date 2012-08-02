//
//  AppController.h
//  Glypha
//
//  Created by Kevin Wojniak on 7/18/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#import <Cocoa/Cocoa.h>

class GLGame;
@class GameView;

@interface AppController : NSObject <NSApplicationDelegate>
{
    GLGame *game_;
}

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet GameView *gameView;

@end

@interface GameView : NSOpenGLView
{
    NSTimer *renderTimer_;
    GLGame *game_;
}

- (void)setGame:(GLGame *)game;

@end
