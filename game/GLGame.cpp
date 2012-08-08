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
    lastFlameAni(0), whichFlame1(-1), whichFlame2(-1),
    numLightningStrikes(0)
{
    flameDestRects[0].setSize(16, 16);
    flameDestRects[1].setSize(16, 16);
    flameDestRects[0].offsetBy(87, 325);
    flameDestRects[1].offsetBy(535, 325);
    for (int i = 0; i < 4; i++) {
        flameRects[i].setSize(16, 16);
        flameRects[i].offsetBy(0, i * 16);
    }
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
    
    // Create images the first time
    if (bgImg_ == NULL) {
        bgImg_ = new GLImage(background_png, background_png_len);
        torchesImg_ = new GLImage(torches_png, torches_png_len);
    }
    
    // Draw the background
    bgImg_->draw(0, 0);
    
    // Draw the torches
    if (((now - lastFlameAni) >= (1.0f/25.0f)) || (whichFlame1 == -1)) {
        whichFlame1 = GLUtils::randomInt(4);
        whichFlame2 = GLUtils::randomInt(4);
        lastFlameAni = now;
    }
    torchesImg_->draw(flameDestRects[0], flameRects[whichFlame1]);
    torchesImg_->draw(flameDestRects[1], flameRects[whichFlame2]);
    
    // Draw lightning
    if ((numLightningStrikes > 0) && ((now - lastLightningStrike) >= (1.0f/15.0f))) {
        generateLightning(lightningPoint.h, lightningPoint.v);
        lastLightningStrike = now;
        --numLightningStrikes;
    }
    if (numLightningStrikes > 0) {
        drawLightning(r);
    }
}

void GLGame::handleMouseDownEvent(const GLPoint& point)
{
    doLightning(point);
}

void GLGame::doLightning(const GLPoint& point)
{
    numLightningStrikes = 4;
    lightningPoint = point;
    generateLightning(lightningPoint.h, lightningPoint.v);
    lastLightningStrike = GLUtils::now();
}

void GLGame::generateLightning(short h, short v)
{
#define kLeftObeliskH		172
#define kLeftObeliskV		250
#define kRightObeliskH		468
#define kRightObeliskV		250
#define kWander				16
	
	short		i, leftDeltaH, rightDeltaH, leftDeltaV, rightDeltaV, range;
	
	leftDeltaH = h - kLeftObeliskH;				// determine the h and v distances between…
	
	rightDeltaH = h - kRightObeliskH;			// obelisks and the target point
	leftDeltaV = v - kLeftObeliskV;
	rightDeltaV = v - kRightObeliskV;
	
	for (i = 0; i < kNumLightningPts; i++)		// calculate an even spread of points between…
	{											// obelisk tips and the target point
		leftLightningPts[i].h = (leftDeltaH * i) / (kNumLightningPts - 1) + kLeftObeliskH;
		leftLightningPts[i].v = (leftDeltaV * i) / (kNumLightningPts - 1) + kLeftObeliskV;
		rightLightningPts[i].h = (rightDeltaH * i) / (kNumLightningPts - 1) + kRightObeliskH;
		rightLightningPts[i].v = (rightDeltaV * i) / (kNumLightningPts - 1) + kRightObeliskV;
	}
	
	range = kWander * 2 + 1;					// randomly scatter the points vertically…
	for (i = 1; i < kNumLightningPts - 1; i++)	// but NOT the 1st or last points
	{
		leftLightningPts[i].v += GLUtils::randomInt(range) - kWander;
		rightLightningPts[i].v += GLUtils::randomInt(range) - kWander;
	}
}

void GLGame::drawLightning(GLRenderer *r)
{
    short i;

    r->setFillColor(255, 255, 0);
    r->beginLines(2.0f);
    // draw lightning bolts
    r->moveTo(leftLightningPts[0].h, leftLightningPts[0].v);
    for (i = 0; i < kNumLightningPts - 1; i++)
    {
        r->moveTo(leftLightningPts[i].h, leftLightningPts[i].v);
        r->lineTo(leftLightningPts[i + 1].h - 1, leftLightningPts[i + 1].v);
    }

    r->moveTo(rightLightningPts[0].h, rightLightningPts[0].v);
    for (i = 0; i < kNumLightningPts - 1; i++)
    {
        r->moveTo(rightLightningPts[i].h, rightLightningPts[i].v);
        r->lineTo(rightLightningPts[i + 1].h - 1, rightLightningPts[i + 1].v);
    }
    r->endLines();
}

