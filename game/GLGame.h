#ifndef GLGAME_H
#define GLGAME_H

#include "GLRect.h"
#include "GLPoint.h"
#include "GLRenderer.h"
#include "GLImage.h"
#include "GLSounds.h"
#include "GLCursor.h"
#include "GLUtils.h"

namespace GL {
    
#define kNumLightningPts 8
#define kMaxEnemies 8

class Game {
public:
    enum Key {
        KeyNone = 0,
        KeySpacebar = 1,
        KeyDownArrow = 2,
        KeyLeftArrow = 4,
        KeyRightArrow = 8,
        KeyA = 16,
        KeyS = 32,
        KeyColon = 64,
        KeyQuote = 128,
    };
    
    enum Event {
        EventStarted = 0,
        EventEnded = 1,
    };

    typedef void (*Callback)(Event event, void *context);

    Game(Callback callback, void *context);
    ~Game();
    
    Renderer* renderer();
    
    void run();
    
    void handleMouseDownEvent(const Point& point);
    void handleKeyDownEvent(Key key);
    void handleKeyUpEvent(Key key);
    
    void newGame();
    void endGame();
    
private:
    Callback callback_;
    void *callbackContext_;
    
    Renderer *renderer_;
    Cursor cursor;
    Sounds sounds;
    Utils utils;
    
    double now;
    void loadImages();
    bool playing, evenFrame, flapKeyDown;
    
    void update();
    void drawFrame();
    
    Image bgImg;
    void drawBackground() const;

    Image torchesImg;
    Rect flameDestRects[2], flameRects[4];
    void drawTorches() const;

    void drawLightning();
    void generateLightning(short h, short v);
    void strikeLightning();
    void doLightning(const Point& point);
    Point leftLightningPts[kNumLightningPts], rightLightningPts[kNumLightningPts];
    Point mousePoint;
    int lightningCount;
    double lastLightningStrike;
    Point lightningPoint;
    int newGameLightning;
    double lastNewGameLightning;
    Rect obeliskRects[4];
    Image obelisksImg;
    bool flashObelisks;
    void drawObelisks() const;
    
    int numLedges, levelOn, livesLeft;
    
    struct playerType {
        Rect dest, wasDest, wrap;
        int h, v;
        int wasH, wasV;
        int hVel, vVel;
        int srcNum, mode;
        int frame;
        bool facingRight, flapping;
        bool walking, wrapping;
        bool clutched;
    };
    playerType thePlayer;
    Rect playerRects[11];
    void resetPlayer(bool initialPlace);
    void offAMortal();
    Image playerImg;
    Image playerIdleImg;
    void drawPlayer() const;
    void movePlayer();
    void handlePlayerIdle();
    void handlePlayerWalking();
    void handlePlayerFlying();
    void handlePlayerSinking();
    void handlePlayerFalling();
    void handlePlayerBones();
    void setAndCheckPlayerDest();
    void checkTouchDownCollision();
    void checkPlatformCollision();
    void setUpLevel();
    void checkLavaRoofCollision();
    void checkPlayerWrapAround();
    void keepPlayerOnPlatform();
    
    void getPlayerInput();
    int theKeys;
    Rect platformRects[6], touchDownRects[6], enemyRects[24];
    
    Rect platformCopyRects[9];
    void drawPlatforms() const;
    Image platformImg;
    
    long theScore;
    Image numbersImg;
    Rect numbersSrc[11], numbersDest[11];
    void drawLivesNumbers() const;
    void drawScoreNumbers() const;
    void drawLevelNumbers() const;
    void addToScore(int value);
    
    typedef struct {
        Rect dest;
        int mode;
    } handInfo;
    Image handImg;
    handInfo theHand;
    Rect grabZone;
    Rect handRects[2];
    void initHandLocation();
    void handleHand();
    
    int countDownTimer;
    void handleCountDownTimer();
    
    int numEnemies;
    int numEnemiesThisLevel;
    int deadEnemies;
    int numOwls;
    int spawnedEnemies;
    struct enemyType {
        Rect dest, wasDest;
        int h, v;
        int wasH, wasV;
        int hVel, vVel;
        int srcNum, mode;
        int kind, frame;
        int heightSmell, targetAlt;
        int flapImpulse, pass;
        int maxHVel, maxVVel;
        bool facingRight;
    };
    enemyType theEnemies[kMaxEnemies];
    Rect enemyInitRects[5];
    Rect eggSrcRect;
    bool doEnemyFlapSound;
	bool doEnemyScrapeSound;
    Image enemyFly;
    Image enemyWalk;
    Image egg;
    void moveEnemies();
    void checkEnemyWrapAround(int who) const;
    void drawHand() const;
    void drawEnemies() const;
    void generateEnemies();
    bool setEnemyInitialLocation(Rect *theRect);
    void initEnemy(short i, bool reincarnated);
    void setEnemyAttributes(int i);
    int assignNewAltitude();
    void checkEnemyPlatformHit(int h);
    void checkEnemyRoofCollision(int i);
    void handleIdleEnemies(int i);
    void handleFlyingEnemies(int i);
    void handleWalkingEnemy(int i);
    void handleSpawningEnemy(int i);
    void handleFallingEnemy(int i);
    void handleEggEnemy(int i);
    void resolveEnemyPlayerHit(int i);
    void checkPlayerEnemyCollision();
};

}

#endif
