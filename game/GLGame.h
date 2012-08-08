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
class GLRenderer;
class GLImage;

#define kNumLightningPts 8

class GLGame {
public:
    GLGame();
    ~GLGame();
    
    GLRenderer* renderer();
    
    double updateFrequency();
    
    void draw();
    
    void handleMouseDownEvent(const GLPoint& point);
    
private:
    GLRenderer *renderer_;
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
};

#endif
