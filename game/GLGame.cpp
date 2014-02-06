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

#define kLightningDelay (1.0 / 30.0)

GLGame::GLGame()
    : renderer_(new GLRenderer())
    , isPlaying(false), evenFrame(true)
    , lastFlameAni(0), whichFlame1(-1), whichFlame2(-1)
    , lightningCount(0)
    , newGameLightning(-1)
    , flashObelisks(false)
    , theKeys(kGLGameKeyNone)
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
		touchDownRects[i].left += 23;
		touchDownRects[i].right -= 23;
		touchDownRects[i].bottom = touchDownRects[i].top;
		touchDownRects[i].top = touchDownRects[i].bottom - 11;
	}
	
    for (int i = 0; i < 11; i++) {
		numbersSrc[i].setSize(8, 11);
		numbersSrc[i].offsetBy(0, 11 * i);
        numbersDest[i].setSize(8, 11);
	}
    numbersDest[0].offsetBy(229, 443);	// # of lives digit 1
	numbersDest[1].offsetBy(237, 443);	// # of lives digit 2
	numbersDest[2].offsetBy(293, 443);	// score digit 1
	numbersDest[3].offsetBy(301, 443);	// score digit 2
	numbersDest[4].offsetBy(309, 443);	// score digit 3
	numbersDest[5].offsetBy(317, 443);	// score digit 4
	numbersDest[6].offsetBy(325, 443);	// score digit 5
	numbersDest[7].offsetBy(333, 443);	// score digit 6
	numbersDest[8].offsetBy(381, 443);	// # of level digit 1
	numbersDest[9].offsetBy(389, 443);	// # of level digit 2
	numbersDest[10].offsetBy(397, 443);	// # of level digit 3
    
    handRects[0].set(0, 0, 56, 57);
    handRects[1].set(0, 0, 56, 57);
    handRects[1].offsetBy(0, 57);
    grabZone.set(0, 0, 96, 108);
    grabZone.offsetBy(48, 352);
    
    obeliskRects[0].set(0, 0, 20, 209);
    obeliskRects[1].set(0, 0, 20, 209);
    obeliskRects[1].offsetBy(0, 209);
    obeliskRects[2].set(0, 0, 20, 209);
    obeliskRects[2].offsetBy(161, 250);
    obeliskRects[3].set(0, 0, 20, 209);
    obeliskRects[3].offsetBy(457, 250);
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
    numbersImg.load(numbers_png, numbers_png_len);
    handImg.load(hand_png, hand_png_len);
    obelisksImg.load(obelisks_png, obelisks_png_len);
}

void GLGame::draw()
{
    GLRenderer *r = renderer_;
    now = utils.now();
    
    r->clear();
    
    // Create images the first time
    if (!bgImg.isLoaded()) {
        loadImages();
    }
    
    // Draw the background
    bgImg.draw(0, 0);
    
    // Draw the torches
    if (((now - lastFlameAni) >= (1.0f/25.0f)) || (whichFlame1 == -1)) {
        whichFlame1 = utils.randomInt(4);
        whichFlame2 = utils.randomInt(4);
        lastFlameAni = now;
    }
    torchesImg.draw(flameDestRects[0], flameRects[whichFlame1]);
    torchesImg.draw(flameDestRects[1], flameRects[whichFlame2]);
    
    // Draw hand
    if (theHand.mode == kOutGrabeth) {
        handImg.draw(theHand.dest, handRects[0]);
    } else if (theHand.mode == kClutching) {
        handImg.draw(theHand.dest, handRects[1]);
    }
    
    if (isPlaying) {
        drawPlatforms();
        movePlayer();
        handleHand();
        drawPlayer();
        checkPlayerWrapAround();
        drawObelisks();
        updateLivesNumbers();
        updateScoreNumbers();
        updateLevelNumbers();
        getPlayerInput();
    } else {
        drawLightning();
        drawObelisks();

    }
    
    drawLightning();
    
    evenFrame = !evenFrame;
}

void GLGame::handleMouseDownEvent(const GLPoint& point)
{
    if (!isPlaying) {
        doLightning(point);
    }
}

void GLGame::drawLightning()
{
    if ((lightningCount > 0) && ((now - lastLightningStrike) >= kLightningDelay)) {
        generateLightning(lightningPoint.h, lightningPoint.v);
        lastLightningStrike = now;
        --lightningCount;
    }
    if (lightningCount > 0) {
        strikeLightning();
    } else {
        flashObelisks = false;
    }
    
    if (newGameLightning >= 0) {
        if ((now - lastNewGameLightning) >= kLightningDelay) {
            lastNewGameLightning = now;
            switch (newGameLightning) {
                case 6:
                    generateLightning(320, 429);	// platform 0
                    break;
                case 5:
                    generateLightning(95, 289);		// platform 1
                    break;
                case 4:
                    generateLightning(95, 110);		// platform 3
                    break;
                case 3:
                    generateLightning(320, 195);	// platform 5
                    break;
                case 2:
                    generateLightning(545, 110);	// platform 4
                    break;
                case 1:
                    generateLightning(545, 289);	// platform 2
                    break;
            }
            --newGameLightning;
            if (newGameLightning == -1) {
                doLightning(GLPoint(thePlayer.dest.left + 24, thePlayer.dest.bottom - 24));
            }
        }
    }
    if (newGameLightning >= 0) {
        strikeLightning();
    }
}

void GLGame::doLightning(const GLPoint& point)
{
    flashObelisks = true;
    sounds.play(kLightningSound);
    lightningCount = kNumLightningStrikes;
    lightningPoint = point;
    generateLightning(lightningPoint.h, lightningPoint.v);
    lastLightningStrike = now;
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
		leftLightningPts[i].v += utils.randomInt(range) - kWander;
		rightLightningPts[i].v += utils.randomInt(range) - kWander;
	}
}

void GLGame::strikeLightning()
{
    GLRenderer *r = renderer_;
    short i;

    r->setFillColor(255, 255, 0);
    r->beginLines(2.0f);
    // draw lightning bolts
    r->moveTo(leftLightningPts[0].h, leftLightningPts[0].v);
    for (i = 0; i < kNumLightningPts - 1; i++) {
        r->moveTo(leftLightningPts[i].h, leftLightningPts[i].v);
        r->lineTo(leftLightningPts[i + 1].h - 1, leftLightningPts[i + 1].v);
    }

    r->moveTo(rightLightningPts[0].h, rightLightningPts[0].v);
    for (i = 0; i < kNumLightningPts - 1; i++) {
        r->moveTo(rightLightningPts[i].h, rightLightningPts[i].v);
        r->lineTo(rightLightningPts[i + 1].h - 1, rightLightningPts[i + 1].v);
    }
    r->endLines();
}

void GLGame::newGame()
{
	numLedges = 3;
	levelOn = 0;
    livesLeft = kInitNumLives;
    theScore = 0L;
    isPlaying = true;
    
    initHandLocation();
	theHand.mode = kLurking;
    
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
    
    newGameLightning = 6;
    lastNewGameLightning = 0;
    
	updateLevelNumbers();
}

void GLGame::resetPlayer(bool initialPlace)
{
	int location;
	
	thePlayer.srcNum = 5;
	thePlayer.frame = 320;
	
	if (initialPlace)
		location = 0;
	else
		location = utils.randomInt(numLedges);
	
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

void GLGame::offAMortal()
{
	livesLeft--;
	
	if (livesLeft > 0) {
		resetPlayer(false);
		updateLivesNumbers();
	} else {
		isPlaying = false;
	}
}

void GLGame::drawPlayer()
{
	GLRect src;
	
	if ((evenFrame) && (thePlayer.mode == kIdle)) {
        playerIdleImg.draw(thePlayer.dest);
	} else if (thePlayer.mode == kBones) {
		src = playerRects[thePlayer.srcNum];
		src.bottom = (src.top + thePlayer.frame);
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
            handlePlayerSinking();
            break;
            
		case kFalling:
            handlePlayerFalling();
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
                    sounds.play(kWalkSound);
				} else {
                    sounds.play(kScreechSound);
                }
			} else {
				thePlayer.hVel -= 80;
				if (thePlayer.hVel < desiredHVel) {
					thePlayer.hVel = desiredHVel;
                    sounds.play(kWalkSound);
				} else {
                    sounds.play(kScreechSound);
                }
			}
		}
	} else {
		thePlayer.hVel -= thePlayer.hVel / 4;
		if ((thePlayer.hVel < 4) && (thePlayer.hVel > -4)) {
			thePlayer.hVel = 0;
		} else {
            sounds.play(kScreechSound);
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

void GLGame::handlePlayerSinking()
{
    thePlayer.hVel = 0;
    thePlayer.vVel = 16;
    if (thePlayer.dest.top > kLavaHeight) {
        offAMortal();
    }

    thePlayer.v += thePlayer.vVel;

    setAndCheckPlayerDest();
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
				thePlayer.dest.bottom += 11;
				thePlayer.wasDest.bottom += 11;
				offset = thePlayer.dest.bottom - testRect.bottom - 1;
				thePlayer.dest.bottom -= offset;
				thePlayer.dest.top -= offset;
				thePlayer.v = thePlayer.dest.top << 4;
                sounds.play(kGrateSound);
			}
			sected = true;
		}
	}
	
	if (!sected) {
		if (thePlayer.mode == kWalking) {
			thePlayer.mode = kFlying;
			thePlayer.dest.bottom -= 11;
			thePlayer.wasDest.bottom -= 11;
		}
	}
}

void GLGame::handlePlayerFalling()
{
    if (thePlayer.hVel > 0) {
        thePlayer.hVel -= kAirResistance;
        if (thePlayer.hVel < 0) {
            thePlayer.hVel = 0;
        }
    } else if (thePlayer.hVel < 0) {
        thePlayer.hVel += kAirResistance;
        if (thePlayer.hVel > 0) {
            thePlayer.hVel = 0;
        }
    }

    thePlayer.vVel += kGravity;

    if (thePlayer.vVel > kMaxVVelocity) {
        thePlayer.vVel = kMaxVVelocity;
    } else if (thePlayer.vVel < -kMaxVVelocity) {
        thePlayer.vVel = -kMaxVVelocity;
    }

    thePlayer.h += thePlayer.hVel;
    thePlayer.v += thePlayer.vVel;

    setAndCheckPlayerDest();

    checkLavaRoofCollision();
    checkPlatformCollision();
}

void GLGame::checkLavaRoofCollision()
{
	short offset;
	
	if (thePlayer.dest.bottom> kLavaHeight)
	{
		if (thePlayer.mode == kFalling) {
			//PlayExternalSound(kSplashSound, kSplashPriority);
		} else {
            sounds.play(kBirdSound);
        }
		thePlayer.mode = kSinking;
	}
	else if (thePlayer.dest.top < kRoofHeight)
	{
		offset = kRoofHeight - thePlayer.dest.top;
		thePlayer.dest.top += offset;
		thePlayer.dest.bottom += offset;
		thePlayer.v = thePlayer.dest.top * 16;
        sounds.play(kGrateSound);
		thePlayer.vVel = thePlayer.vVel / -2;
	}
}

void GLGame::setAndCheckPlayerDest()
{
	short wasTall, wasWide;
	
	wasTall = thePlayer.dest.bottom - thePlayer.dest.top;
	wasWide = thePlayer.dest.right - thePlayer.dest.left;
	
	thePlayer.dest.left = (thePlayer.h >> 4);
	thePlayer.dest.right = (thePlayer.dest.left + wasWide);
	thePlayer.dest.top = (thePlayer.v >> 4);
	thePlayer.dest.bottom = (thePlayer.dest.top + wasTall);
	
	if (thePlayer.dest.left > 640)
	{
        thePlayer.dest.offsetBy(-640, 0);
		thePlayer.h = thePlayer.dest.left << 4;
        thePlayer.wasDest.offsetBy(-640, 0);
	}
	else if (thePlayer.dest.right < 0)
	{
        thePlayer.dest.offsetBy(640, 0);
		thePlayer.h = thePlayer.dest.left << 4;
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
			hRect.left = thePlayer.dest.left;
			hRect.right = thePlayer.dest.right;
			hRect.top = thePlayer.wasDest.top;
			hRect.bottom = thePlayer.wasDest.bottom;
			
			if (hRect.sect(&platformRects[i]))
			{
				if (thePlayer.h > thePlayer.wasH)			// heading right
				{
					offset = thePlayer.dest.right - platformRects[i].left;
					thePlayer.dest.left -= offset;
					thePlayer.dest.right -= offset;
					thePlayer.h = thePlayer.dest.left << 4;
					if (thePlayer.hVel > 0)
						thePlayer.hVel = -(thePlayer.hVel >> 1);
					else
						thePlayer.hVel = thePlayer.hVel >> 1;
				}
				else if (thePlayer.h < thePlayer.wasH)		// heading left
				{
					offset = platformRects[i].right - thePlayer.dest.left;
					thePlayer.dest.left += offset;
					thePlayer.dest.right += offset;
					thePlayer.h = thePlayer.dest.left << 4;
					if (thePlayer.hVel < 0)
						thePlayer.hVel = -(thePlayer.hVel >> 1);
					else
						thePlayer.hVel = thePlayer.hVel >> 1;
				}
                sounds.play(kGrateSound);
			}
			else
			{
				vRect.left = thePlayer.wasDest.left;
				vRect.right = thePlayer.wasDest.right;
				vRect.top = thePlayer.dest.top;
				vRect.bottom = thePlayer.dest.bottom;
				
				if (vRect.sect(&platformRects[i]))
				{
					if (thePlayer.wasV < thePlayer.v)		// heading down
					{
						offset = thePlayer.dest.bottom - platformRects[i].top;
						thePlayer.dest.top -= offset;
						thePlayer.dest.bottom -= offset;
						thePlayer.v = thePlayer.dest.top << 4;
						if (thePlayer.vVel > kDontFlapVel) {
                            sounds.play(kGrateSound);
                        }
						if (thePlayer.mode == kFalling)
						{
							if ((thePlayer.dest.right - 16) > platformRects[i].right)							{
								thePlayer.hVel = 16;
								if (thePlayer.vVel > 0)
									thePlayer.vVel = -(thePlayer.vVel >> 1);
								else
									thePlayer.vVel = thePlayer.vVel >> 1;
							}
							else if ((thePlayer.dest.left + 16) < platformRects[i].left)
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
								thePlayer.dest.top = (thePlayer.dest.bottom - 22);
								thePlayer.v = thePlayer.dest.top << 4;
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
						offset = platformRects[i].bottom - thePlayer.dest.top;
						thePlayer.dest.top += offset;
						thePlayer.dest.bottom += offset;
						thePlayer.v = thePlayer.dest.top << 4;
                        sounds.play(kGrateSound);
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
                sounds.play(kFlapSound);
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
                    thePlayer.dest.left += 18;
                    thePlayer.dest.right += 18;
                    thePlayer.h = thePlayer.dest.left << 4;
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
                    thePlayer.dest.left -= 18;
                    thePlayer.dest.right -= 18;
                    thePlayer.h = thePlayer.dest.left << 4;
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

void GLGame::updateLivesNumbers()
{
	short digit;
	
	digit = (livesLeft - 1) / 10;
	digit = digit % 10L;
	if ((digit == 0) && ((livesLeft - 1) < 100)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[0], numbersSrc[digit]);
	
	digit = (livesLeft - 1) % 10;
    numbersImg.draw(numbersDest[1], numbersSrc[digit]);
}

void GLGame::updateScoreNumbers()
{
	long digit;
	
	digit = theScore / 100000L;
	digit = digit % 10L;
	if ((digit == 0) && (theScore < 1000000L)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[2], numbersSrc[digit]);
	
	digit = theScore / 10000L;
	if (digit > wasTensOfThousands) {
		livesLeft++;
		updateLivesNumbers();
		wasTensOfThousands = digit;
	}
	
	digit = digit % 10L;
	if ((digit == 0) && (theScore < 100000L)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[3], numbersSrc[digit]);
	
	digit = theScore / 1000L;
	digit = digit % 10L;
	if ((digit == 0) && (theScore < 10000L)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[4], numbersSrc[digit]);
	
	digit = theScore / 100L;
	digit = digit % 10L;
	if ((digit == 0) && (theScore < 1000L)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[5], numbersSrc[digit]);
	
	digit = theScore / 10L;
	digit = digit % 10L;
	if ((digit == 0) && (theScore < 100L)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[6], numbersSrc[digit]);
	
	digit = theScore % 10L;
    numbersImg.draw(numbersDest[7], numbersSrc[digit]);
}

void GLGame::updateLevelNumbers()
{
	short digit;
	
	digit = (levelOn + 1) / 100;
	digit = digit % 10L;
	if ((digit == 0) && ((levelOn + 1) < 1000)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[8], numbersSrc[digit]);
	
	digit = (levelOn + 1) / 10;
	digit = digit % 10L;
	if ((digit == 0) && ((levelOn + 1) < 100)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[9], numbersSrc[digit]);
	
	digit = (levelOn + 1) % 10;
    numbersImg.draw(numbersDest[10], numbersSrc[digit]);
}

void GLGame::initHandLocation()
{
    theHand.dest.set(0, 0, 56, 57);
    theHand.dest.offsetBy(48, 460);
}

void GLGame::handleHand()
{
    int hDiff, vDiff, pull, speed;

    switch (theHand.mode) {
        case kLurking:
            if (thePlayer.mode == kFlying && thePlayer.dest.sect(&grabZone)) {
                theHand.mode = kOutGrabeth;
                initHandLocation();
            }
            break;
            
        case kOutGrabeth:
        case kClutching:
            if (thePlayer.dest.sect(&grabZone)) {
                hDiff = theHand.dest.left - thePlayer.dest.left;
                vDiff = theHand.dest.top - thePlayer.dest.top;
                
                if (thePlayer.facingRight)
                    hDiff -= 3;
                else
                    hDiff -= 21;
                vDiff -= 29;
                
                speed = (levelOn >> 3) + 1;
                if (hDiff < 0) {
                    theHand.dest.left += speed;
                    theHand.dest.right += speed;
                } else if (hDiff > 0) {
                    theHand.dest.left -= speed;
                    theHand.dest.right -= speed;
                }
                if (vDiff < 0) {
                    theHand.dest.top += speed;
                    theHand.dest.bottom += speed;
                } else if (vDiff > 0) {
                    theHand.dest.top -= speed;
                    theHand.dest.bottom -= speed;
                }
                
                if (hDiff < 0) {
                    hDiff = -hDiff;
                }
                if (vDiff < 0) {
                    vDiff = -vDiff;
                }
                if ((hDiff < 8) && (vDiff < 8)) {
                    theHand.mode = kClutching;
                    thePlayer.clutched = true;
                    thePlayer.hVel = thePlayer.hVel >> 3;
                    thePlayer.vVel = thePlayer.vVel >> 3;
                    pull = levelOn << 2;
                    if (pull > 48)
                        pull = 48;
                    thePlayer.vVel += pull;
                    theHand.dest.top = thePlayer.dest.top + 29;
                    theHand.dest.bottom = theHand.dest.top + 57;
                    if (thePlayer.facingRight) {
                        theHand.dest.left = thePlayer.dest.left + 3;
                    } else {
                        theHand.dest.left = thePlayer.dest.left + 21;
                    }
                    theHand.dest.right = theHand.dest.left + 58;
                } else {
                    thePlayer.clutched = false;
                    theHand.mode = kOutGrabeth;
                }
            } else {
                theHand.dest.top++;
                theHand.dest.bottom++;
                if (theHand.dest.top > 460) {
                    theHand.mode = kLurking;
                } else {
                    theHand.mode = kOutGrabeth;
                }
                thePlayer.clutched = false;
            }
            break;
        }
}

void GLGame::checkPlayerWrapAround()
{
    GLRect wrapRect, wasWrapRect, src;

    if (thePlayer.dest.right > 640) {
        thePlayer.wrapping = true;
        wrapRect = thePlayer.dest;
        wrapRect.left -= 640;
        wrapRect.right -= 640;
        
        wasWrapRect = thePlayer.wasDest;
        wasWrapRect.left -= 640;
        wasWrapRect.right -= 640;
        
        if (thePlayer.mode == kBones) {
            src = playerRects[thePlayer.srcNum];
            src.bottom = src.top + thePlayer.frame;
#if 0
            CopyMask(GetPortBitMapForCopyBits(playerSrcMap),
                     GetPortBitMapForCopyBits(playerMaskMap),
                     GetPortBitMapForCopyBits(workSrcMap),
                     &src, &src, &wrapRect);
#endif
        } else {
            playerImg.draw(wrapRect, playerRects[thePlayer.srcNum]);
        }
        thePlayer.wrap = wrapRect;
    } else if (thePlayer.dest.left < 0) {
        thePlayer.wrapping = true;
        wrapRect = thePlayer.dest;
        wrapRect.left += 640;
        wrapRect.right += 640;
        
        wasWrapRect = thePlayer.wasDest;
        wasWrapRect.left += 640;
        wasWrapRect.right += 640;
        
        if (thePlayer.mode == kBones) {
            src = playerRects[thePlayer.srcNum];
            src.bottom = src.top + thePlayer.frame;
#if 0
            CopyMask(GetPortBitMapForCopyBits(playerSrcMap),
                     GetPortBitMapForCopyBits(playerMaskMap),
                     GetPortBitMapForCopyBits(workSrcMap),
                     &src, &src, &wrapRect);
#endif
        } else {
            playerImg.draw(wrapRect, playerRects[thePlayer.srcNum]);
        }
        thePlayer.wrap = wrapRect;
    } else {
        thePlayer.wrapping = false;
    }
}

void GLGame::drawObelisks()
{
    if (flashObelisks) {
        obelisksImg.draw(obeliskRects[2], obeliskRects[0]);
        obelisksImg.draw(obeliskRects[3], obeliskRects[1]);
    } else {
        bgImg.draw(obeliskRects[2], obeliskRects[2]);
        bgImg.draw(obeliskRects[3], obeliskRects[3]);
    }
}