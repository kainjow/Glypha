//
//  GLGame.cpp
//  Glypha
//
//  Created by Kevin Wojniak on 7/19/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#include "GLGame.h"
#include "GLImage.h"
#include "GLResources.h"
#include "GLUtils.h"
#include <cstdio>

#define kIdle						-1	// enemy & player mode
#define kFlying						0	// enemy & player mode
#define kWalking					1	// enemy & player mode
#define kSinking					2	// player mode
#define kSpawning					3	// enemy mode
#define kFalling					4	// enemy mode & player mode
#define kEggTimer					5	// enemy mode
#define kDeadAndGone				6	// enemy mode
#define kBones						7	// player mode
#define kLurking					10	// hand mode
#define kOutGrabeth					11	// hand mode
#define kClutching					12	// hand mode
#define kWaiting					15	// eye mode
#define kStalking					16	// eye mode

GLGame::GLGame() :
    renderer_(new GLRenderer()),
    bgImg_(NULL),
    lastFlameAni(0), whichFlame1(-1), whichFlame2(-1),
    numLightningStrikes(0),
    isPlaying(false)
{
    flameDestRects[0].setSize(16, 16);
    flameDestRects[1].setSize(16, 16);
    flameDestRects[0].offsetBy(87, 325);
    flameDestRects[1].offsetBy(535, 325);
    for (int i = 0; i < 4; i++) {
        flameRects[i].setSize(16, 16);
        flameRects[i].offsetBy(0, i * 16);
    }
    
    playerRects[0].setSize(48, 37);
	playerRects[0].offsetBy(0, 0);
	playerRects[1].setSize(48, 37);
    playerRects[1].offsetBy(0, 37);
	playerRects[2].setSize(48, 37);
	playerRects[2].offsetBy(0, 74);
	playerRects[3].setSize(8, 37);
	playerRects[3].offsetBy(0, 111);
	playerRects[4].setSize(48, 48);
	playerRects[4].offsetBy(0, 148);
	playerRects[5].setSize(48, 48);
	playerRects[5].offsetBy(0, 196);
	playerRects[6].setSize(48, 48);
	playerRects[6].offsetBy(0, 244);
	playerRects[7].setSize(48, 48);
	playerRects[7].offsetBy(0, 292);
	playerRects[8].setSize(48, 37);		// falling bones rt.
	playerRects[8].offsetBy(0, 340);
	playerRects[9].setSize(48, 37);		// falling bones lf.
	playerRects[9].offsetBy(0, 377);
	playerRects[10].setSize(48, 22);	// pile of bones
	playerRects[10].offsetBy(0, 414);
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

void GLGame::mainLoop()
{
}

void GLGame::draw()
{
    GLRenderer *r = renderer_;
    double now = GLUtils::now();
    
    mainLoop();
    
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
    
    // Draw player
    if (isPlaying) {
        r->setFillColor(255, 0, 0);
        r->fillRect(thePlayer.dest);
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

void GLGame::newGame()
{
    isPlaying = true;
    resetPlayer(true);
}

void GLGame::resetPlayer(bool initialPlace)
{
	int location;
	
	thePlayer.srcNum = 5;
	thePlayer.frame = 320;
	
	if (initialPlace)
		location = 0;
	else
		location = GLUtils::randomInt(numLedges);
	
	switch (location) {
		case 0:
            thePlayer.h = 296 << 4;		// bottom center
            thePlayer.v = 377 << 4;
            break;
            
		case 1:
            thePlayer.h = 102 << 4;
            thePlayer.v = 237 << 4;
            break;
            
		case 2:
            thePlayer.h = 489 << 4;
            thePlayer.v = 237 << 4;
            break;
            
		case 3:
            thePlayer.h = 102 << 4;
            thePlayer.v = 58 << 4;
            break;
            
		case 4:
            thePlayer.h = 489 << 4;
            thePlayer.v = 58 << 4;
            break;
            
		case 5:
            thePlayer.h = 296 << 4;
            thePlayer.v = 143 << 4;
            break;
	}
	
	thePlayer.dest = playerRects[thePlayer.srcNum];
	thePlayer.dest.zeroCorner();
	thePlayer.dest.offsetBy(thePlayer.h >> 4, thePlayer.v >> 4);
	thePlayer.wasDest = thePlayer.dest;
	
	thePlayer.hVel = 0;
	thePlayer.vVel = 0;
	thePlayer.facingRight = true;
	thePlayer.flapping = false;
	thePlayer.wrapping = false;
	thePlayer.clutched = false;
	thePlayer.mode = kIdle;
}
