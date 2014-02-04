#ifndef GLGAME_H
#define GLGAME_H

#include "GLRect.h"
#include "GLPoint.h"
#include "GLRenderer.h"
#include "GLImage.h"
#include "GLSounds.h"
#include "GLUtils.h"

#define kNumLightningPts 8

struct playerType {
	GLRect dest, wasDest, wrap;
	int h, v;
	int wasH, wasV;
	int hVel, vVel;
	int srcNum, mode;
	int frame;
	bool facingRight, flapping;
	bool walking, wrapping;
	bool clutched;
};

enum GLGameKey {
    kGLGameKeyNone = 0,
    
    kGLGameKeySpacebar = 1,
    kGLGameKeyDownArrow = 2,
    kGLGameKeyLeftArrow = 4,
    kGLGameKeyRightArrow = 8,
    kGLGameKeyA = 16,
    kGLGameKeyS = 32,
    kGLGameKeyColon = 64,
    kGLGameKeyQuote = 128,
};

class GLGame {
public:
    GLGame();
    ~GLGame();
    
    GLRenderer* renderer();
    
    double updateFrequency();
    
    void draw();
    
    void handleMouseDownEvent(const GLPoint& point);
    void handleKeyDownEvent(GLGameKey key);
    void handleKeyUpEvent(GLGameKey key);
    
    void newGame();
    
private:
    GLRenderer *renderer_;
    GLUtils utils;
    double now;
    void loadImages();
    bool isPlaying, evenFrame, flapKeyDown;
    
    GLImage bgImg;

    GLImage torchesImg;
    GLRect flameDestRects[2], flameRects[4];
    double lastFlameAni;
    int whichFlame1, whichFlame2;

    void drawLightning();
    void generateLightning(short h, short v);
    void strikeLightning();
    void doLightning(const GLPoint& point);
    GLPoint leftLightningPts[kNumLightningPts], rightLightningPts[kNumLightningPts];
    GLPoint mousePoint;
    int lightningCount;
    double lastLightningStrike;
    GLPoint lightningPoint;
    int newGameLightning;
    double lastNewGameLightning;
    
    int numLedges, levelOn, livesLeft;
    
    playerType thePlayer;
    GLRect playerRects[11];
    void resetPlayer(bool initialPlace);
    void offAMortal();
    GLImage playerImg;
    GLImage playerIdleImg;
    void drawPlayer();
    void movePlayer();
    void handlePlayerIdle();
    void handlePlayerWalking();
    void handlePlayerFlying();
    void handlePlayerSinking();
    void handlePlayerFalling();
    void setAndCheckPlayerDest();
    void checkTouchDownCollision();
    void checkPlatformCollision();
    void setUpLevel();
    void checkLavaRoofCollision();
    void checkPlayerWrapAround();
    
    void getPlayerInput();
    int theKeys;
    GLRect platformRects[6], touchDownRects[6], enemyRects[24];
    
    GLRect platformCopyRects[9];
    void drawPlatforms();
    GLImage platformImg;
    
    GLSounds sounds;
    
    long theScore, wasTensOfThousands;
    GLImage numbersImg;
    GLRect numbersSrc[11], numbersDest[11];
    void updateLivesNumbers();
    void updateScoreNumbers();
    void updateLevelNumbers();
    
    typedef struct {
        GLRect dest;
        int mode;
    } handInfo;
    GLImage handImg;
    handInfo theHand;
    GLRect grabZone;
    GLRect handRects[2];
    void initHandLocation();
    void handleHand();
};

#endif
