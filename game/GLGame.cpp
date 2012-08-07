//
//  GLGame.cpp
//  Glypha
//
//  Created by Kevin Wojniak on 7/19/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#include "GLGame.h"
#include "GLRenderer.h"
#include "GLImage.h"
#include "GLResources.h"
#include "GLUtils.h"
#include <cstdio>

GLGame::GLGame() :
    renderer_(new GLRenderer()),
    bgImg_(NULL),
    lastFlameAni(0), whichFlame1(-1), whichFlame2(-1)
{
    flameDestRects[0].setSize(16, 16);
    flameDestRects[1].setSize(16, 16);
    flameDestRects[0].offsetBy(87, 325);
    flameDestRects[1].offsetBy(535, 325);
    for (int i = 0; i < 4; i++) {
        flameRects[i].setSize(16, 16);
        flameRects[i].offsetBy(0, i * 16);
    }
    flameAniFrequency = 1.0/30.0;
}

GLGame::~GLGame()
{
    delete renderer_;
}

GLRenderer* GLGame::renderer()
{
    return renderer_;   
}

double GLGame::updateFrequency()
{
    return 1.0/30.0;
}

void GLGame::draw()
{
    GLRenderer *r = renderer_;
    double now = GLUtils::now();
    
    r->clear();
    
    GLRect bounds = r->bounds();
    
    if (bgImg_ == NULL) {
        bgImg_ = new GLImage(background_png, background_png_len);
        torchesImg_ = new GLImage(torches_png, torches_png_len);
    }
    
    bgImg_->draw(0, 0);
    
    if (((now - lastFlameAni) >= flameAniFrequency) || (whichFlame1 == -1)) {
        whichFlame1 = GLUtils::randomInt(4);
        whichFlame2 = GLUtils::randomInt(4);
        lastFlameAni = now;
    }
    torchesImg_->draw(flameDestRects[0], flameRects[whichFlame1]);
    torchesImg_->draw(flameDestRects[1], flameRects[whichFlame2]);
}
