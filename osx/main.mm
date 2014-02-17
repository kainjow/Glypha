#import <Cocoa/Cocoa.h>
#include "GLGame.h"
#include "GLRenderer.h"

@interface GameView : NSOpenGLView
{
    NSTimer *renderTimer_;
    GLGame *game_;
}

- (void)setGame:(GLGame *)game;

@end

@interface AppController : NSObject <NSApplicationDelegate>
{
    GLGame *game_;
    NSWindow *window_;
    NSMenuItem *newGame_;
    NSMenuItem *endGame_;
    GameView *gameView_;
}

- (IBAction)newGame:(id)sender;
- (void)handleGameEvent:(GLGameEvent)event;

@end

static void callback(GLGameEvent event, void *context)
{
    [(AppController*)context handleGameEvent:event];
}

@implementation AppController

- (void)setupMenuBar
{
    NSMenu *menubar = [[[NSMenu alloc] init] autorelease];
    [NSApp setMainMenu:menubar];
    NSString *appName = [[NSProcessInfo processInfo] processName];
    NSMenuItem *item;
    
    NSMenu *appMenu = [[[NSMenu alloc] initWithTitle:appName] autorelease];
    NSMenuItem *appMenuItem = [[[NSMenuItem alloc] init] autorelease];
    NSMenu *gameMenu = [[[NSMenu alloc] initWithTitle:@"Game"] autorelease];
    [gameMenu setAutoenablesItems:NO];
    NSMenuItem *gameMenuItem = [[[NSMenuItem alloc] init] autorelease];
    [appMenuItem setSubmenu:appMenu];
    [gameMenuItem setSubmenu:gameMenu];
    [menubar addItem:appMenuItem];
    [menubar addItem:gameMenuItem];
    
    item = [appMenu addItemWithTitle:[NSString stringWithFormat:@"About %@", appName] action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
    [item setTarget:NSApp];
    [appMenu addItem:[NSMenuItem separatorItem]];
    NSString *quitText = [NSString stringWithFormat:@"Quit %@", appName];
    item = [appMenu addItemWithTitle:quitText action:@selector(terminate:) keyEquivalent:@"q"];
    [item setTarget:NSApp];
    newGame_ = [gameMenu addItemWithTitle:@"New Game" action:@selector(newGame:) keyEquivalent:@"n"];
    [newGame_ setTarget:self];
    endGame_ = [gameMenu addItemWithTitle:@"End Game" action:@selector(endGame:) keyEquivalent:@"e"];
    [endGame_ setTarget:self];
    [endGame_ setEnabled:NO];
}

- (id)init
{
    self = [super init];
    if (self != nil) {
        NSUInteger style = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;
        window_ = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 640, 460) styleMask:style backing:NSBackingStoreBuffered defer:NO];
        gameView_ = [[GameView alloc] initWithFrame:[[window_ contentView] frame]];
        [[window_ contentView] addSubview:gameView_];
        [self setupMenuBar];
        game_ = new GLGame(callback, self);
        [gameView_ setGame:game_];
    }
    return self;
}

- (void)applicationDidFinishLaunching:(__unused NSNotification*)note
{
    // Center the window, then set the autosave name. If the frame already has been saved, it'll override the centering.
    [window_ center];
    [window_ setFrameAutosaveName:@"MainWindow"];
    
    // Show the window only after its frame has been adjusted.
    [window_ makeKeyAndOrderFront:nil];
}

- (IBAction)newGame:(__unused id)sender
{
    game_->newGame();
}

- (IBAction)endGame:(__unused id)sender
{
    game_->endGame();
}

- (void)handleGameEvent:(GLGameEvent)event
{
    switch (event) {
        case kGLGameStarted:
            [newGame_ setEnabled:NO];
            [endGame_ setEnabled:YES];
            break;
        case kGLGameEnded:
            [newGame_ setEnabled:YES];
            [endGame_ setEnabled:NO];
            break;
    }
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
    renderTimer_ = [[NSTimer scheduledTimerWithTimeInterval:1/60.0 target:self selector:@selector(renderTimer) userInfo:nil repeats:YES] retain];
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

- (void)drawRect:(__unused NSRect)rect
{
    if (!game_) {
        return;
    }
    NSOpenGLContext *ctx = [self openGLContext];
	[ctx makeCurrentContext];
	game_->run();
	[ctx flushBuffer];
}

- (void)mouseDown:(NSEvent *)event
{
    NSPoint mouseLoc = [self convertPoint:[event locationInWindow] fromView:nil];
    GLPoint point(mouseLoc.x, game_->renderer()->bounds().height() - mouseLoc.y);
    game_->handleMouseDownEvent(point);
}

- (void)doKey:(NSEvent *)event up:(BOOL)up
{
    NSString *chars = [event characters];
    for (NSUInteger i = 0; i < [chars length]; ++i) {
        unichar ch = [chars characterAtIndex:i];
        GLGameKey key;
        switch (ch) {
            case ' ':
                key = kGLGameKeySpacebar;
                break;
            case NSDownArrowFunctionKey:
                key = kGLGameKeyDownArrow;
                break;
            case NSLeftArrowFunctionKey:
                key = kGLGameKeyLeftArrow;
                break;
            case NSRightArrowFunctionKey:
                key = kGLGameKeyRightArrow;
                break;
            case 'a':
                key = kGLGameKeyA;
                break;
            case 's':
                key = kGLGameKeyS;
                break;
            case ';':
                key = kGLGameKeyColon;
                break;
            case '"':
                key = kGLGameKeyQuote;
                break;
            default:
                key = kGLGameKeyNone;
                break;
        }
        if (up) {
            game_->handleKeyUpEvent(key);
        } else {
            game_->handleKeyDownEvent(key);
        }
    }
}

- (void)keyDown:(NSEvent *)event
{
    [self doKey:event up:NO];
}

- (void)keyUp:(NSEvent *)event
{
    [self doKey:event up:YES];
}

- (BOOL)canBecomeKeyView
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

@end

int main(int argc, char *argv[])
{
    NSApplication *app = [NSApplication sharedApplication];
    AppController *controller = [[[AppController alloc] init] autorelease];
    app.delegate = controller;
    return NSApplicationMain(argc, (const char **)argv);
}
