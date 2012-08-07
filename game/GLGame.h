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
class GLRenderer;
class GLImage;

class GLGame {
public:
    GLGame();
    ~GLGame();
    
    GLRenderer* renderer();
    
    double updateFrequency();
    
    void draw();
    
private:
    GLRenderer *renderer_;
    GLImage *bgImg_;
    GLImage *torchesImg_;

    GLRect flameDestRects[2], flameRects[4];
    double lastFlameAni;
    int whichFlame1, whichFlame2;
    double flameAniFrequency;
};

#endif
