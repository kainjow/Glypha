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

#define kFlapImpulse			48
#define kGlideImpulse			12
#define kAirResistance			2
#define kMaxHVelocity			192
#define kMaxVVelocity			512
#define kNumLightningStrikes	5

#define kLavaHeight					456
#define kRoofHeight					2
#define kGravity					4

#define kInitNumLives				5
#define kMaxEnemies					8
#define kDontFlapVel				8

GLGame::GLGame() :
    renderer_(new GLRenderer()),
    isPlaying(false), evenFrame(true),
    lastFlameAni(0), whichFlame1(-1), whichFlame2(-1),
    numLightningStrikes(0),
    theKeys(kGLGameKeyNone)
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
	playerRects[3].setSize(48, 37);
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

	for (int i = 0; i < 6; i++) {
        platformCopyRects[i].setSize(191, 32);
        platformCopyRects[i].offsetBy(0, 32 * i);
	}
    platformCopyRects[6].set(233, 190, 424, 222);
	platformCopyRects[7].set(0, 105, 191, 137);
	platformCopyRects[8].set(449, 105, 640, 137);
 
    platformRects[0].set(206, 424, 433, 438);   //_______________
	platformRects[1].set(-256, 284, 149, 298);	//
	platformRects[2].set(490, 284, 896, 298);   //--3--     --4--
	platformRects[3].set(-256, 105, 149, 119);  //     --5--
	platformRects[4].set(490, 105, 896, 119);   //--1--     --2--
	platformRects[5].set(233, 190, 407, 204);   //_____--0--_____

	for (int i = 0; i < 6; i++) {
		touchDownRects[i] = platformRects[i];
		touchDownRects[i].setLeft(touchDownRects[i].left() + 23);
		touchDownRects[i].setRight(touchDownRects[i].right() - 23);
		touchDownRects[i].setBottom(touchDownRects[i].top());
		touchDownRects[i].setTop(touchDownRects[i].bottom() - 11);
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

void GLGame::loadImages()
{
    bgImg.load(background_png, background_png_len);
    torchesImg.load(torches_png, torches_png_len);
    platformImg.load(platforms_png, platforms_png_len);
    playerImg.load(player_png, player_png_len);
    playerIdleImg.load(playerIdle_png, playerIdle_png_len);
}

void GLGame::draw()
{
    GLRenderer *r = renderer_;
    double now = GLUtils::now();
    
    r->clear();
    
    // Create images the first time
    if (!bgImg.isLoaded()) {
        loadImages();
    }
    
    // Draw the background
    bgImg.draw(0, 0);
    
    // Draw the torches
    if (((now - lastFlameAni) >= (1.0f/25.0f)) || (whichFlame1 == -1)) {
        whichFlame1 = GLUtils::randomInt(4);
        whichFlame2 = GLUtils::randomInt(4);
        lastFlameAni = now;
    }
    torchesImg.draw(flameDestRects[0], flameRects[whichFlame1]);
    torchesImg.draw(flameDestRects[1], flameRects[whichFlame2]);
    
    // Draw lightning
    if ((numLightningStrikes > 0) && ((now - lastLightningStrike) >= (1.0f/15.0f))) {
        generateLightning(lightningPoint.h, lightningPoint.v);
        lastLightningStrike = now;
        --numLightningStrikes;
    }
    if (numLightningStrikes > 0) {
        drawLightning(r);
    }
    
    if (isPlaying) {
        drawPlatforms();
        movePlayer();
        drawPlayer();
        getPlayerInput();
    }
    
    evenFrame = !evenFrame;
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
    const short kLeftObeliskH = 172;
    const short kLeftObeliskV = 250;
    const short kRightObeliskH = 468;
    const short kRightObeliskV = 250;
    const short kWander = 16;
	short i, leftDeltaH, rightDeltaH, leftDeltaV, rightDeltaV, range;
	
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
	numLedges = 3;
	levelOn = 0;
    isPlaying = true;
    
    setUpLevel();
    resetPlayer(true);
}

void GLGame::setUpLevel()
{
	short waveMultiple;
	
	//KillOffEye();
	
	waveMultiple = levelOn % 5;
	
	switch (waveMultiple) {
		case 0:
            numLedges = 5;
            break;
            
		case 1:
            numLedges = 6;
            break;
            
		case 2:
            numLedges = 5;
            break;
            
		case 3:
            numLedges = 3;
            break;
            
		case 4:
            numLedges = 6;
            break;
	}
		
	//UpdateLevelNumbers();
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

void GLGame::drawPlayer()
{
	GLRect src;
	
	if ((evenFrame) && (thePlayer.mode == kIdle)) {
        playerIdleImg.draw(thePlayer.dest);
	} else if (thePlayer.mode == kBones) {
		src = playerRects[thePlayer.srcNum];
		src.setBottom(src.top() + thePlayer.frame);
        playerImg.draw(thePlayer.dest, src);
	} else {
        src = playerRects[thePlayer.srcNum];
        playerImg.draw(thePlayer.dest, src);
	}
    
	thePlayer.wasH = thePlayer.h;
	thePlayer.wasV = thePlayer.v;
	thePlayer.wasDest = thePlayer.dest;
}

void GLGame::drawPlatforms()
{
	if (numLedges > 3) {
        platformImg.draw(platformCopyRects[7], platformCopyRects[2]);
        platformImg.draw(platformCopyRects[8], platformCopyRects[4]);
	} else {
        platformImg.draw(platformCopyRects[7], platformCopyRects[3]);
        platformImg.draw(platformCopyRects[8], platformCopyRects[5]);
	}
	
	if (numLedges > 5) {
        platformImg.draw(platformCopyRects[6], platformCopyRects[0]);
	} else {
        platformImg.draw(platformCopyRects[6], platformCopyRects[1]);
	}
}

void GLGame::movePlayer()
{
	switch (thePlayer.mode) {
		case kIdle:
            handlePlayerIdle();
            break;
            
		case kFlying:
            handlePlayerFlying();
            break;
            
		case kWalking:
			handlePlayerWalking();
            break;
            
		case kSinking:
            printf("Unhandled kSinking\n");
            //HandlePlayerSinking();
            break;
            
		case kFalling:
            printf("Unhandled kFalling\n");
            //HandlePlayerFalling();
            break;
            
		case kBones:
            printf("Unhandled kBones\n");
            //HandlePlayerBones();
            break;
	}
}

void GLGame::handlePlayerIdle()
{
	thePlayer.frame--;
	if (thePlayer.frame == 0) {
		thePlayer.mode = kWalking;
    }
	setAndCheckPlayerDest();
}

void GLGame::handlePlayerWalking()
{
	short desiredHVel;
	
	if (thePlayer.walking) {
		if (evenFrame) {
			if (thePlayer.facingRight) {
				if (thePlayer.srcNum == 4)
					desiredHVel = 208;
				else
					desiredHVel = 128;
			} else {
				if (thePlayer.srcNum == 7)
					desiredHVel = -208;
				else
					desiredHVel = -128;
			}
			
			if (thePlayer.hVel < desiredHVel) {
				thePlayer.hVel += 80;
				if (thePlayer.hVel > desiredHVel) {
					thePlayer.hVel = desiredHVel;
					//PlayExternalSound(kWalkSound, kWalkPriority);
				} else {
					//PlayExternalSound(kScreechSound, kScreechPriority);
                }
			} else {
				thePlayer.hVel -= 80;
				if (thePlayer.hVel < desiredHVel) {
					thePlayer.hVel = desiredHVel;
					//PlayExternalSound(kWalkSound, kWalkPriority);
				} else {
					//PlayExternalSound(kScreechSound, kScreechPriority);
                }
			}
		}
	} else {
		thePlayer.hVel -= thePlayer.hVel / 4;
		if ((thePlayer.hVel < 4) && (thePlayer.hVel > -4)) {
			thePlayer.hVel = 0;
		} else {
			//PlayExternalSound(kScreechSound, kScreechPriority);
        }
	}
	
	if (thePlayer.vVel > kMaxVVelocity) {
		thePlayer.vVel = kMaxVVelocity;
    } else if (thePlayer.vVel < -kMaxVVelocity) {
		thePlayer.vVel = -kMaxVVelocity;
    }
	
	thePlayer.h += thePlayer.hVel;
	thePlayer.v += thePlayer.vVel;
	
	if (thePlayer.walking) {
		if (evenFrame) {
			if (thePlayer.facingRight) {
				thePlayer.srcNum = 9 - thePlayer.srcNum;
			} else {
				thePlayer.srcNum = 13 - thePlayer.srcNum;
            }
		}
	} else {
		if (thePlayer.facingRight) {
			thePlayer.srcNum = 5;
		} else {
			thePlayer.srcNum = 6;
        }
	}
	
	setAndCheckPlayerDest();
	
	checkTouchDownCollision();
	//KeepPlayerOnPlatform();
	//CheckPlayerEnemyCollision();
}

void GLGame::handlePlayerFlying()
{
	if (thePlayer.hVel > 0)
	{
		thePlayer.hVel -= kAirResistance;
		if (thePlayer.hVel < 0)
			thePlayer.hVel = 0;
	}
	else if (thePlayer.hVel < 0)
	{
		thePlayer.hVel += kAirResistance;
		if (thePlayer.hVel > 0)
			thePlayer.hVel = 0;
	}
	
	thePlayer.vVel += kGravity;
	
	if (thePlayer.vVel > kMaxVVelocity)
		thePlayer.vVel = kMaxVVelocity;
	else if (thePlayer.vVel < -kMaxVVelocity)
		thePlayer.vVel = -kMaxVVelocity;
	
	thePlayer.h += thePlayer.hVel;
	thePlayer.v += thePlayer.vVel;
	
	if (thePlayer.facingRight)
	{
		thePlayer.srcNum = 1;
		if (thePlayer.vVel < -kDontFlapVel)
		{
			if (thePlayer.flapping)
				thePlayer.srcNum = 0;
			else
				thePlayer.srcNum = 1;
		}
		else if (thePlayer.vVel > kDontFlapVel)
		{
			if (thePlayer.flapping)
				thePlayer.srcNum = 1;
			else
				thePlayer.srcNum = 0;
		}
		else if (thePlayer.flapping)
			thePlayer.srcNum = 0;
	}
	else
	{
		thePlayer.srcNum = 2;
		if (thePlayer.vVel < -kDontFlapVel)
		{
			if (thePlayer.flapping)
				thePlayer.srcNum = 3;
			else
				thePlayer.srcNum = 2;
		}
		else if (thePlayer.vVel > kDontFlapVel)
		{
			if (thePlayer.flapping)
				thePlayer.srcNum = 2;
			else
				thePlayer.srcNum = 3;
		}
		else if (thePlayer.flapping)
			thePlayer.srcNum = 3;
	}
	
	setAndCheckPlayerDest();
	
	checkLavaRoofCollision();
	//CheckPlayerEnemyCollision();
	checkPlatformCollision();
	checkTouchDownCollision();
}

void GLGame::checkTouchDownCollision()
{
	GLRect testRect;
	short offset;
	bool sected = false;
	
	for (int i = 0; i < numLedges; i++) {
		testRect = touchDownRects[i];
		if (thePlayer.mode == kWalking) {
            testRect.offsetBy(0, 11);
        }
		
        if (thePlayer.dest.sect(&testRect)) {
			if (thePlayer.mode == kFlying) {
				thePlayer.mode = kWalking;
				if (thePlayer.facingRight)
					thePlayer.srcNum = 5;
				else
					thePlayer.srcNum = 6;
				if (thePlayer.vVel > 0)
					thePlayer.vVel = 0;
				thePlayer.dest.setBottom(thePlayer.dest.bottom() + 11);
				thePlayer.wasDest.setBottom(thePlayer.wasDest.bottom() + 11);
				offset = thePlayer.dest.bottom() - testRect.bottom() - 1;
				thePlayer.dest.setBottom(thePlayer.dest.bottom() - offset);
				thePlayer.dest.setTop(thePlayer.dest.top() - offset);
				thePlayer.v = thePlayer.dest.top() << 4;
				//PlayExternalSound(kGrateSound, kGratePriority);
			}
			sected = true;
		}
	}
	
	if (!sected) {
		if (thePlayer.mode == kWalking) {
			thePlayer.mode = kFlying;
			thePlayer.dest.setBottom(thePlayer.dest.bottom() - 11);
			thePlayer.wasDest.setBottom(thePlayer.wasDest.bottom() - 11);
		}
	}
}

void GLGame::checkLavaRoofCollision()
{
	short offset;
	
	if (thePlayer.dest.bottom( )> kLavaHeight)
	{
		if (thePlayer.mode == kFalling) {
			//PlayExternalSound(kSplashSound, kSplashPriority);
		} else {
			//PlayExternalSound(kBirdSound, kBirdPriority);
        }
		thePlayer.mode = kSinking;
	}
	else if (thePlayer.dest.top() < kRoofHeight)
	{
		offset = kRoofHeight - thePlayer.dest.top();
		thePlayer.dest.setTop(thePlayer.dest.top() + offset);
		thePlayer.dest.setBottom(thePlayer.dest.bottom() + offset);
		thePlayer.v = thePlayer.dest.top() * 16;
		//PlayExternalSound(kGrateSound, kGratePriority);
		thePlayer.vVel = thePlayer.vVel / -2;
	}
}

void GLGame::setAndCheckPlayerDest()
{
	short wasTall, wasWide;
	
	wasTall = thePlayer.dest.bottom() - thePlayer.dest.top();
	wasWide = thePlayer.dest.right() - thePlayer.dest.left();
	
	thePlayer.dest.setLeft(thePlayer.h >> 4);
	thePlayer.dest.setRight(thePlayer.dest.left() + wasWide);
	thePlayer.dest.setTop(thePlayer.v >> 4);
	thePlayer.dest.setBottom(thePlayer.dest.top() + wasTall);
	
	if (thePlayer.dest.left() > 640)
	{
        thePlayer.dest.offsetBy(-640, 0);
		thePlayer.h = thePlayer.dest.left() << 4;
        thePlayer.wasDest.offsetBy(-640, 0);
	}
	else if (thePlayer.dest.right() < 0)
	{
        thePlayer.dest.offsetBy(640, 0);
		thePlayer.h = thePlayer.dest.left() << 4;
        thePlayer.wasDest.offsetBy(640, 0);
	}
}

void GLGame::checkPlatformCollision()
{
	GLRect hRect, vRect;
	short offset;
	
	for (int i = 0; i < numLedges; i++)
	{
		if (thePlayer.dest.sect(&platformRects[i]))
		{
			hRect.setLeft(thePlayer.dest.left());
			hRect.setRight(thePlayer.dest.right());
			hRect.setTop(thePlayer.wasDest.top());
			hRect.setBottom(thePlayer.wasDest.bottom());
			
			if (hRect.sect(&platformRects[i]))
			{
				if (thePlayer.h > thePlayer.wasH)			// heading right
				{
					offset = thePlayer.dest.right() - platformRects[i].left();
					thePlayer.dest.setLeft(thePlayer.dest.left() - offset);
					thePlayer.dest.setRight(thePlayer.dest.right() - offset);
					thePlayer.h = thePlayer.dest.left() << 4;
					if (thePlayer.hVel > 0)
						thePlayer.hVel = -(thePlayer.hVel >> 1);
					else
						thePlayer.hVel = thePlayer.hVel >> 1;
				}
				else if (thePlayer.h < thePlayer.wasH)		// heading left
				{
					offset = platformRects[i].right() - thePlayer.dest.left();
					thePlayer.dest.setLeft(thePlayer.dest.left() + offset);
					thePlayer.dest.setRight(thePlayer.dest.right() + offset);
					thePlayer.h = thePlayer.dest.left() << 4;
					if (thePlayer.hVel < 0)
						thePlayer.hVel = -(thePlayer.hVel >> 1);
					else
						thePlayer.hVel = thePlayer.hVel >> 1;
				}
				//PlayExternalSound(kGrateSound, kGratePriority);
			}
			else
			{
				vRect.setLeft(thePlayer.wasDest.left());
				vRect.setRight(thePlayer.wasDest.right());
				vRect.setTop(thePlayer.dest.top());
				vRect.setBottom(thePlayer.dest.bottom());
				
				if (vRect.sect(&platformRects[i]))
				{
					if (thePlayer.wasV < thePlayer.v)		// heading down
					{
						offset = thePlayer.dest.bottom() - platformRects[i].top();
						thePlayer.dest.setTop(thePlayer.dest.top() - offset);
						thePlayer.dest.setBottom(thePlayer.dest.bottom() - offset);
						thePlayer.v = thePlayer.dest.top() << 4;
						if (thePlayer.vVel > kDontFlapVel) {
							//PlayExternalSound(kGrateSound, kGratePriority);
                        }
						if (thePlayer.mode == kFalling)
						{
							if ((thePlayer.dest.right() - 16) > platformRects[i].right())							{
								thePlayer.hVel = 16;
								if (thePlayer.vVel > 0)
									thePlayer.vVel = -(thePlayer.vVel >> 1);
								else
									thePlayer.vVel = thePlayer.vVel >> 1;
							}
							else if ((thePlayer.dest.left() + 16) < platformRects[i].left())
							{
								thePlayer.hVel = -16;
								if (thePlayer.vVel > 0)
									thePlayer.vVel = -(thePlayer.vVel >> 1);
								else
									thePlayer.vVel = thePlayer.vVel >> 1;
							}
							else
							{
								//PlayExternalSound(kBoom1Sound, kBoom1Priority);
								thePlayer.vVel = 0;
								thePlayer.mode = kBones;
								thePlayer.frame = 22;
								thePlayer.dest.setTop(thePlayer.dest.bottom() - 22);
								thePlayer.v = thePlayer.dest.top() << 4;
								thePlayer.srcNum = 10;
							}
						}
						else
						{
							if (thePlayer.vVel > 0)
								thePlayer.vVel = -(thePlayer.vVel >> 1);
							else
								thePlayer.vVel = thePlayer.vVel >> 1;
						}
					}
					else if (thePlayer.wasV > thePlayer.v)	// heading up
					{
						offset = platformRects[i].bottom() - thePlayer.dest.top();
						thePlayer.dest.setTop(thePlayer.dest.top() + offset);
						thePlayer.dest.setBottom(thePlayer.dest.bottom() + offset);
						thePlayer.v = thePlayer.dest.top() << 4;
						//PlayExternalSound(kGrateSound, kGratePriority);
						if (thePlayer.vVel < 0)
							thePlayer.vVel = -(thePlayer.vVel >> 1);
						else
							thePlayer.vVel = thePlayer.vVel >> 1;
					}
				}
			}
		}
	}
}

void GLGame::getPlayerInput()
{
	thePlayer.flapping = false;
	thePlayer.walking = false;
	
    if ((theKeys & kGLGameKeySpacebar) || (theKeys & kGLGameKeyDownArrow)) {
        if (thePlayer.mode == kIdle) {
            thePlayer.mode = kWalking;
            thePlayer.frame = 0;
        } else if ((thePlayer.mode == kFlying) || (thePlayer.mode == kWalking)) {
            if (!flapKeyDown) {
                thePlayer.vVel -= kFlapImpulse;
                flapKeyDown = true;
                //PlayExternalSound(kFlapSound, kFlapPriority);
                thePlayer.flapping = true;
            }
        }
    } else {
        flapKeyDown = false;
    }
    
    if (((theKeys & kGLGameKeyRightArrow) ||
         (theKeys & kGLGameKeyS) ||
         (theKeys & kGLGameKeyQuote)) &&
        (thePlayer.hVel < kMaxHVelocity))
    {
        if (thePlayer.mode == kIdle)
        {
            thePlayer.mode = kWalking;
            thePlayer.frame = 0;
        }
        else if ((thePlayer.mode == kFlying) || (thePlayer.mode == kWalking))
        {
            if (!thePlayer.facingRight)
            {
                thePlayer.facingRight = true;
                if (thePlayer.clutched)
                {
                    thePlayer.dest.setLeft(thePlayer.dest.left() + 18);
                    thePlayer.dest.setRight(thePlayer.dest.right() + 18);
                    thePlayer.h = thePlayer.dest.left() << 4;
                    thePlayer.wasH = thePlayer.h;
                    thePlayer.wasDest = thePlayer.dest;
                }
            }
            else
            {
                if (thePlayer.mode == kFlying)
                    thePlayer.hVel += kGlideImpulse;
                else
                    thePlayer.walking = true;
            }
        }
    }
    else if (((theKeys & kGLGameKeyLeftArrow) ||
              (theKeys & kGLGameKeyA) ||
              (theKeys & kGLGameKeyColon)) &&
             (thePlayer.hVel > -kMaxHVelocity))
    {
        if (thePlayer.mode == kIdle)
        {
            thePlayer.mode = kWalking;
            thePlayer.frame = 0;
        }
        else if ((thePlayer.mode == kFlying) || (thePlayer.mode == kWalking))
        {
            if (thePlayer.facingRight)
            {
                thePlayer.facingRight = false;
                if (thePlayer.clutched)
                {
                    thePlayer.dest.setLeft(thePlayer.dest.left() - 18);
                    thePlayer.dest.setRight(thePlayer.dest.right() - 18);
                    thePlayer.h = thePlayer.dest.left() << 4;
                    thePlayer.wasH = thePlayer.h;
                    thePlayer.wasDest = thePlayer.dest;
                }
            }
            else
            {
                if (thePlayer.mode == kFlying)
                    thePlayer.hVel -= kGlideImpulse;
                else
                    thePlayer.walking = true;
            }
        }
    }
}

void GLGame::handleKeyDownEvent(GLGameKey key)
{
    theKeys |= key;
}

void GLGame::handleKeyUpEvent(GLGameKey key)
{
    theKeys &= ~key;
}
