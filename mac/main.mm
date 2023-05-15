#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>
#include "GLGame.h"
#include "GLRenderer.h"
#include "GLResources.h"

#define GAME_MAX_SCALE 4.0

@interface GameView : NSOpenGLView
{
    CVDisplayLinkRef displayLink_;
    GL::Game *game_;
}

- (void)setGame:(GL::Game *)game;
- (void)render;

@end

@interface AppController : NSObject <NSApplicationDelegate, NSWindowDelegate>
{
    GL::Game *game_;
    NSWindow *window_;
    NSMenuItem *about_;
    NSMenuItem *newGame_;
    NSMenuItem *pauseGame_;
    NSMenuItem *endGame_;
    NSMenuItem *fullScreen_;
    NSMenuItem *helpMenuItem_;
    NSMenuItem *highScores_;
    NSMenuItem *resetHighScores_;
    GameView *gameView_;
}

- (IBAction)newGame:(id)sender;
- (void)handleGameEvent:(GL::Game::Event)event;
- (void)handleHighScoreName:(const char *)name place:(int)place;

@end

static void callback(GL::Game::Event event, void *context)
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [(AppController*)context handleGameEvent:event];
    });
}

static void highScoreNameCallback(const char *highName, int place, void *context)
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [(AppController*)context handleHighScoreName:highName place:place];
    });
}

@implementation AppController

- (void)setupMenuBar:(NSString *)appName
{
    NSMenu *menubar = [[[NSMenu alloc] init] autorelease];
    [NSApp setMainMenu:menubar];
    NSMenuItem *item;
    
    NSMenu *appMenu = [[[NSMenu alloc] initWithTitle:appName] autorelease];
    [appMenu setAutoenablesItems:NO];
    NSMenuItem *appMenuItem = [[[NSMenuItem alloc] init] autorelease];
    NSMenu *gameMenu = [[[NSMenu alloc] initWithTitle:@"Game"] autorelease];
    [gameMenu setAutoenablesItems:NO];
    NSMenuItem *gameMenuItem = [[[NSMenuItem alloc] init] autorelease];
    NSMenu *optionsMenu = [[[NSMenu alloc] initWithTitle:@"Options"] autorelease];
    [optionsMenu setAutoenablesItems:NO];
    NSMenuItem *helpMenuItem = [[[NSMenuItem alloc] init] autorelease];
    [appMenuItem setSubmenu:appMenu];
    [gameMenuItem setSubmenu:gameMenu];
    [helpMenuItem setSubmenu:optionsMenu];
    [menubar addItem:appMenuItem];
    [menubar addItem:gameMenuItem];
    [menubar addItem:helpMenuItem];
    
    about_ = [appMenu addItemWithTitle:[NSString stringWithFormat:@"About %@", appName] action:@selector(showAbout:) keyEquivalent:@""];
    [about_ setTarget:self];
    [appMenu addItem:[NSMenuItem separatorItem]];
    NSString *quitText = [NSString stringWithFormat:@"Quit %@", appName];
    item = [appMenu addItemWithTitle:quitText action:@selector(terminate:) keyEquivalent:@"q"];
    [item setTarget:NSApp];
    newGame_ = [gameMenu addItemWithTitle:@"New Game" action:@selector(newGame:) keyEquivalent:@"n"];
    [newGame_ setTarget:self];
    pauseGame_ = [gameMenu addItemWithTitle:@"Pause Game" action:@selector(pauseResumeGame:) keyEquivalent:@"p"];
    [pauseGame_ setTarget:self];
    [pauseGame_ setEnabled:NO];
    endGame_ = [gameMenu addItemWithTitle:@"End Game" action:@selector(endGame:) keyEquivalent:@"e"];
    [endGame_ setTarget:self];
    [endGame_ setEnabled:NO];
    [gameMenu addItem:[NSMenuItem separatorItem]];
    fullScreen_ = [gameMenu addItemWithTitle:@"Enter Full Screen" action:@selector(toggleFullScreen:) keyEquivalent:@"f"];
    [fullScreen_ setKeyEquivalentModifierMask:NSEventModifierFlagControl | NSEventModifierFlagCommand];
    helpMenuItem_ = [optionsMenu addItemWithTitle:@"Help" action:@selector(showHelp:) keyEquivalent:@"h"];
    [helpMenuItem_ setTarget:self];
    [optionsMenu addItem:[NSMenuItem separatorItem]];
    highScores_ = [optionsMenu addItemWithTitle:@"High Scores" action:@selector(showHighScores:) keyEquivalent:@"s"];
    [highScores_ setTarget:self];
    resetHighScores_ = [optionsMenu addItemWithTitle:@"Reset Scores..." action:@selector(resetHighScores:) keyEquivalent:@""];
    [resetHighScores_ setTarget:self];
}

- (id)init
{
    self = [super init];
    if (self != nil) {
        NSString *appName = [NSString stringWithUTF8String:GL_GAME_NAME];
#if defined(MAC_OS_X_VERSION_10_12) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_12
        NSUInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
#else
        NSUInteger style = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;
#endif
        NSSize size = NSMakeSize(GL_GAME_WIDTH, GL_GAME_HEIGHT);
        window_ = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, size.width, size.height) styleMask:style backing:NSBackingStoreBuffered defer:NO];
        [window_ setCollectionBehavior:[window_ collectionBehavior] |NSWindowCollectionBehaviorFullScreenPrimary];
        [window_ setTitle:appName];
        [window_ setDelegate:self];
        [window_ setContentAspectRatio:size];
        [window_ setContentMinSize:NSMakeSize(size.width / window_.backingScaleFactor, size.height / window_.backingScaleFactor)];
        [window_ setContentMaxSize:NSMakeSize(GAME_MAX_SCALE * GL_GAME_WIDTH, GAME_MAX_SCALE * GL_GAME_HEIGHT)];
        gameView_ = [[GameView alloc] initWithFrame:[[window_ contentView] frame]];
        [gameView_ setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
        [[window_ contentView] addSubview:gameView_];
        [self setupMenuBar:appName];
        game_ = new GL::Game(callback, highScoreNameCallback, self);
        [gameView_ setGame:game_];
    }
    return self;
}

- (NSSize)contentSizeForWindow:(NSWindow *)window proposedSize:(NSSize)proposedSize
{
    CGFloat baseRatio = MIN(proposedSize.width / GL_GAME_WIDTH, proposedSize.height / GL_GAME_HEIGHT);
    CGFloat backingScaleFactor = [window backingScaleFactor];
    CGFloat adjustedRatio = MIN(GAME_MAX_SCALE, floor(baseRatio * backingScaleFactor) / backingScaleFactor);
    return NSMakeSize(GL_GAME_WIDTH * adjustedRatio, GL_GAME_HEIGHT * adjustedRatio);
}

- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)frameSize
{
    NSRect contentRect = [NSWindow contentRectForFrameRect:NSMakeRect(0, 0, frameSize.width, frameSize.height) styleMask:window.styleMask];
    NSSize size = [self contentSizeForWindow:window proposedSize:contentRect.size];
    NSRect frameRect = [NSWindow frameRectForContentRect:NSMakeRect(0, 0, size.width, size.height) styleMask:window.styleMask];
    return frameRect.size;
}

- (NSSize)window:(NSWindow *)window willUseFullScreenContentSize:(NSSize)proposedSize
{
    return [self contentSizeForWindow:window proposedSize:proposedSize];
}

- (void)windowDidExitFullScreen:(NSNotification * __unused)notification
{
    [fullScreen_ setTitle:@"Enter Full Screen"];
}

- (void)windowDidEnterFullScreen:(NSNotification * __unused)notification
{
    [fullScreen_ setTitle:@"Exit Full Screen"];
}

- (void)applicationDidFinishLaunching:(__unused NSNotification*)note
{
    // Center the window, then set the autosave name. If the frame already has been saved, it'll override the centering.
    [window_ center];
    [window_ setFrameAutosaveName:@"MainWindow"];
    
    // Show the window only after its frame has been adjusted.
    [window_ makeKeyAndOrderFront:nil];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(__unused NSApplication *)sender
{
    return YES;
}

- (void)newGame:(__unused id)sender
{
    game_->newGame();
}

- (void)pauseResumeGame:(__unused id)sender
{
    game_->pauseResumeGame();
    if ([[pauseGame_ keyEquivalent] isEqualToString:@"p"]) {
        [pauseGame_ setTitle:@"Resume Game"];
        [pauseGame_ setKeyEquivalent:@"r"];
    } else {
        [pauseGame_ setTitle:@"Pause Game"];
        [pauseGame_ setKeyEquivalent:@"p"];
    }
}

- (void)endGame:(__unused id)sender
{
    game_->endGame();
}

- (void)showHelp:(__unused id)sender
{
    game_->showHelp();
}

- (void)showHighScores:(__unused id)sender
{
    game_->showHighScores();
}

- (void)resetHighScores:(__unused id)sender
{
    NSAlert *alert = [[[NSAlert alloc] init] autorelease];
    [[alert addButtonWithTitle:@"No"] setKeyEquivalent:@"\033"];
    [[alert addButtonWithTitle:@"Yes"] setKeyEquivalent:@"\r"];
    [alert setMessageText:@"Are you sure you want to reset " GL_GAME_NAME "'s scores?"];
#if defined(MAC_OS_X_VERSION_10_12) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_12
    [alert setAlertStyle:NSAlertStyleCritical];
#else
    [alert setAlertStyle:NSCriticalAlertStyle];
#endif
#if defined(MAC_OS_X_VERSION_10_10) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_10
    [alert beginSheetModalForWindow:window_ completionHandler:^(NSModalResponse returnCode) {
        [self resetHighScoresAlertDidEnd:alert returnCode:returnCode contextInfo:NULL];
    }];
#else
    [alert beginSheetModalForWindow:window_ modalDelegate:self didEndSelector:@selector(resetHighScoresAlertDidEnd:returnCode:contextInfo:) contextInfo:NULL];
#endif
}

- (void)resetHighScoresAlertDidEnd:(NSAlert * __unused)alert returnCode:(NSInteger)returnCode contextInfo:(void *  __unused)contextInfo
{
    if (returnCode == NSAlertSecondButtonReturn) {
        game_->resetHighScores();
    }
}

- (void)showAbout:(__unused id)sender
{
    game_->showAbout();
}

- (void)handleHighScoreName:(const char *)name place:(int)place
{
    NSAlert *alert = [[[NSAlert alloc] init] autorelease];
    [alert setMessageText:[NSString stringWithFormat:@"Your score #%d of the ten best! Enter your name (15 chars.).", place]];
    NSTextField *textField = [[[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 200, 24)] autorelease];
    [textField setStringValue:[NSString stringWithUTF8String:name]];
    [alert setAccessoryView:textField];
    int *placeCopy = new int;
    *placeCopy = place;
#if defined(MAC_OS_X_VERSION_10_10) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_10
    [alert beginSheetModalForWindow:window_ completionHandler:^(NSModalResponse returnCode) {
        [self highScoreNameAlertDidEnd:alert returnCode:returnCode contextInfo:placeCopy];
    }];
#else
    [alert beginSheetModalForWindow:window_ modalDelegate:self didEndSelector:@selector(highScoreNameAlertDidEnd:returnCode:contextInfo:) contextInfo:placeCopy];
#endif
}

- (void)highScoreNameAlertDidEnd:(NSAlert *)alert returnCode:(NSInteger __unused)returnCode contextInfo:(void *)contextInfo
{
    int *place = (int*)contextInfo;
    NSTextField *textField = (NSTextField*)[alert accessoryView];
    game_->processHighScoreName([[textField stringValue] UTF8String], *place);
    delete place;
}

- (void)handleGameEvent:(GL::Game::Event)event
{
    switch (event) {
        case GL::Game::EventStarted:
            [about_ setEnabled:NO];
            [newGame_ setEnabled:NO];
            [pauseGame_ setEnabled:YES];
            [endGame_ setEnabled:YES];
            [helpMenuItem_ setEnabled:NO];
            [highScores_ setEnabled:NO];
            [resetHighScores_ setEnabled:NO];
            break;
        case GL::Game::EventEnded:
            [about_ setEnabled:YES];
            [newGame_ setEnabled:YES];
            [pauseGame_ setEnabled:NO];
            [pauseGame_ setTitle:@"Pause Game"];
            [pauseGame_ setKeyEquivalent:@"p"];
            [endGame_ setEnabled:NO];
            [helpMenuItem_ setEnabled:YES];
            [highScores_ setEnabled:YES];
            [resetHighScores_ setEnabled:YES];
            break;
    }
}

@end

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink __unused, const CVTimeStamp* now __unused, const CVTimeStamp* outputTime __unused, CVOptionFlags flagsIn __unused, CVOptionFlags* flagsOut __unused, void* displayLinkContext)
{
    [(GameView*)displayLinkContext render];
    return kCVReturnSuccess;
}

@implementation GameView

- (id)initWithFrame:(NSRect)frameRect
{
    NSOpenGLPixelFormatAttribute attr[] = {NSOpenGLPFAAccelerated, NSOpenGLPFADoubleBuffer, NSOpenGLPFADepthSize, 24, 0};
    NSOpenGLPixelFormat *format = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attr] autorelease];
    return [super initWithFrame:frameRect pixelFormat:format];
}

- (void)dealloc
{
    CVDisplayLinkRelease(displayLink_);
    [super dealloc];
}

- (void)setGame:(GL::Game *)game
{
    game_ = game;
}

- (void)prepareOpenGL
{
    [super prepareOpenGL];

    // Synchronize buffer swaps with vertical refresh rate
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLContextParameterSwapInterval]; 

    // Create a display link capable of being used with all active displays
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink_);
    
    // Set the renderer output callback function
    CVDisplayLinkSetOutputCallback(displayLink_, &displayLinkCallback, self);
    
    // Set the display link for the current renderer
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink_, cglContext, cglPixelFormat);
    
    // Activate the display link
    CVDisplayLinkStart(displayLink_);
}

- (void)reshape
{
    [super reshape];

	NSRect bounds = [self bounds];
    game_->renderer()->resize(static_cast<int>(bounds.size.width), static_cast<int>(bounds.size.height));
	[[self openGLContext] update];
}

- (void)render
{
    if (game_) {
        NSOpenGLContext *ctx = [self openGLContext];
        [ctx makeCurrentContext];
        CGLLockContext((CGLContextObj)[ctx CGLContextObj]);
        game_->run();
        [ctx flushBuffer];
        CGLUnlockContext((CGLContextObj)[ctx CGLContextObj]);;
    }
}

- (void)drawRect:(__unused NSRect)rect
{
    [self render];
}

- (GL::Point)pointForEvent:(NSEvent *)event
{
    const NSPoint mouseLoc = [self convertPoint:[event locationInWindow] fromView:nil];
    CGFloat ratio = self.bounds.size.width / GL_GAME_WIDTH;
    return GL::Point(static_cast<int>(mouseLoc.x / ratio), static_cast<int>((game_->renderer()->bounds().height() - mouseLoc.y) / ratio));
}

- (void)mouseDown:(NSEvent *)event
{
    game_->handleMouseDownEvent([self pointForEvent:event]);
}

- (void)doKey:(NSEvent *)event up:(BOOL)up
{
    NSString *chars = [event characters];
    for (NSUInteger i = 0; i < [chars length]; ++i) {
        unichar ch = [chars characterAtIndex:i];
        GL::Game::Key key;
        switch (ch) {
            case ' ':
                key = GL::Game::KeySpacebar;
                break;
            case NSUpArrowFunctionKey:
                key = GL::Game::KeyUpArrow;
                break;
            case NSDownArrowFunctionKey:
                key = GL::Game::KeyDownArrow;
                break;
            case NSLeftArrowFunctionKey:
                key = GL::Game::KeyLeftArrow;
                break;
            case NSRightArrowFunctionKey:
                key = GL::Game::KeyRightArrow;
                break;
            case 'a':
                key = GL::Game::KeyA;
                break;
            case 's':
                key = GL::Game::KeyS;
                break;
            case ';':
                key = GL::Game::KeyColon;
                break;
            case '"':
                key = GL::Game::KeyQuote;
                break;
            case NSPageUpFunctionKey:
                key = GL::Game::KeyPageUp;
                break;
            case NSPageDownFunctionKey:
                key = GL::Game::KeyPageDown;
                break;
            case 'f':
                key = GL::Game::KeyF;
                break;
            default:
                key = GL::Game::KeyNone;
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
