//
//  GLGame.h
//  Glypha
//
//  Created by Kevin Wojniak on 7/19/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#ifndef GLGAME_H
#define GLGAME_H

#include "GLRect.h"
#include "GLPoint.h"
#include "GLRenderer.h"
class GLImage;

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
    bool isPlaying, evenFrame, flapKeyDown;
    
    GLImage *bgImg_;

    GLImage *torchesImg_;
    GLRect flameDestRects[2], flameRects[4];
    double lastFlameAni;
    int whichFlame1, whichFlame2;

    void generateLightning(short h, short v);
    void drawLightning(GLRenderer *r);
    void doLightning(const GLPoint& point);
    GLPoint leftLightningPts[kNumLightningPts], rightLightningPts[kNumLightningPts];
    GLPoint mousePoint;
    int numLightningStrikes;
    double lastLightningStrike;
    GLPoint lightningPoint;
    
    int numLedges, levelOn;//, livesLeft, lightH, lightV;
    
    playerType thePlayer;
    GLRect playerRects[11];
    void resetPlayer(bool initialPlace);
    GLImage *playerImg;
    GLImage *playerIdleImg;
    void drawPlayer();
    void movePlayer();
    void handlePlayerIdle();
    void handlePlayerWalking();
    void handlePlayerFlying();
    void setAndCheckPlayerDest();
    void checkTouchDownCollision();
    void checkPlatformCollision();
    void setUpLevel();
    void checkLavaRoofCollision();
    
    void getPlayerInput();
    int theKeys;
    GLRect platformRects[6], touchDownRects[6], enemyRects[24];
};

#endif
