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
#define kDontFlapVel				8

#define kLightningDelay (1.0 / 25.0)

#define kOwl						0
#define kWolf						1
#define kJackal						2

#define kEnemyImpulse			8

#define kOwlMaxHVel				96
#define kOwlMaxVVel				320
#define kOwlHeightSmell			96
#define kOwlFlapImpulse			32

#define kWolfMaxHVel			128
#define kWolfMaxVVel			400
#define kWolfHeightSmell		160
#define kWolfFlapImpulse		48

#define kJackalMaxHVel			192
#define kJackalMaxVVel			512
#define kJackalHeightSmell		240
#define kJackalFlapImpulse		72

#define kUpdateFreq (1.0/30.0)

GL::Game::Game(Callback callback, void *context)
    : callback_(callback)
    , callbackContext_(context)
    , renderer_(new Renderer())
    , now(utils.now())
    , lastTime(now)
    , accumulator(0)
    , playing(false), evenFrame(true)
    , lightningCount(0)
    , newGameLightning(-1)
    , flashObelisks(false)
    , keys_(KeyNone)
    , helpState(kHelpClosed)
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
    
    for (int i = 0; i < 12; i++) {
		enemyRects[i].set(0, 0, 48, 48);
		enemyRects[i].offsetBy(0, 48 * i);
	}
	for (int i = 0; i < 12; i++) {
		enemyRects[i + 12].set(0, 0, 64, 40);
		enemyRects[i + 12].offsetBy(0, 40 * i);
	}

    enemyInitRects[0].set(0, 0, 48, 1);
	enemyInitRects[0].offsetBy(72, 284);
	enemyInitRects[1].set(0, 0, 48, 1);
	enemyInitRects[1].offsetBy(520, 284);
	enemyInitRects[2].set(0, 0, 48, 1);
	enemyInitRects[2].offsetBy(72, 105);
	enemyInitRects[3].set(0, 0, 48, 1);
	enemyInitRects[3].offsetBy(520, 105);
	enemyInitRects[4].set(0, 0, 48, 1);
	enemyInitRects[4].offsetBy(296, 190);
    
    eggSrcRect.set(0, 0, 24, 24);
    
    for (int i = 0; i < 4; i++) {
        eyeRects[i].set(0, 0, 48, 31);
        eyeRects[i].offsetBy(0, i * 31);
    }
    
    helpSrcRect.set(0, 0, 231, 398);
}

GL::Game::~Game()
{
    delete renderer_;
}

GL::Renderer* GL::Game::renderer()
{
    return renderer_;   
}

void GL::Game::loadImages()
{
    bgImg.load(background_png, background_png_len);
    torchesImg.load(torches_png, torches_png_len);
    platformImg.load(platforms_png, platforms_png_len);
    playerImg.load(player_png, player_png_len);
    playerIdleImg.load(playerIdle_png, playerIdle_png_len);
    numbersImg.load(numbers_png, numbers_png_len);
    handImg.load(hand_png, hand_png_len);
    obelisksImg.load(obelisks_png, obelisks_png_len);
    enemyFly.load(enemyFly_png, enemyFly_png_len);
    enemyWalk.load(enemyWalk_png, enemyWalk_png_len);
    egg.load(egg_png, egg_png_len);
    eyeImg.load(eye_png, eye_png_len);
    helpImg.load(help_png, help_png_len);
}

void GL::Game::run()
{
    // Create images the first time
    if (!bgImg.isLoaded()) {
        loadImages();
    }
    
    // See http://gafferongames.com/game-physics/fix-your-timestep/
    // and http://sacredsoftware.net/tutorials/Animation/TimeBasedAnimation.xhtml
    double n0 = utils.now();
    double frameTime = n0 - lastTime;
    lastTime = n0;
    accumulator += frameTime;
    unsigned count = 0;
    while (accumulator >= kUpdateFreq) {
        update();
        now += kUpdateFreq;
        accumulator -= kUpdateFreq;
        ++count;
    }
    if (count > 2) {
        printf("%u cycles\n", count);
    }

    drawFrame();
}

void GL::Game::update()
{
    if (playing) {
        movePlayer();
        moveEnemies();
        handleHand();
        handleEye();
        getPlayerInput();
        handleCountDownTimer();
    }
    handleHelp();
    handleLightning();
    evenFrame = !evenFrame;
}

void GL::Game::drawFrame() const
{
    Renderer *r = renderer_;
    r->clear();
    drawBackground();
    drawTorches();
    if (playing) {
        drawPlatforms();
        drawHand();
        drawEye();
        drawPlayer();
        drawEnemies();
        drawObelisks();
        drawLivesNumbers();
        drawScoreNumbers();
        drawLevelNumbers();
    }
    drawHelp();
    drawObelisks();
    drawLightning();
}

void GL::Game::handleMouseDownEvent(const GL::Point& point)
{
    if (!playing) {
        doLightning(point, kNumLightningStrikes);
    }
}

void GL::Game::handleLightning()
{
    if ((lightningCount > 0) && ((now - lastLightningStrike) >= kLightningDelay)) {
        generateLightning(lightningPoint.h, lightningPoint.v);
        lastLightningStrike = now;
        --lightningCount;
    }
    if (lightningCount <= 0) {
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
                doLightning(GL::Point(thePlayer.dest.left + 24, thePlayer.dest.bottom - 24), kNumLightningStrikes);
            }
        }
    }
}

void GL::Game::doLightning(const GL::Point& point, int count)
{
    flashObelisks = true;
    sounds.play(kLightningSound);
    lightningCount = count;
    lightningPoint = point;
    generateLightning(lightningPoint.h, lightningPoint.v);
    lastLightningStrike = now;
}

void GL::Game::generateLightning(int h, int v)
{
    const int kLeftObeliskH = 172;
    const int kLeftObeliskV = 250;
    const int kRightObeliskH = 468;
    const int kRightObeliskV = 250;
    const int kWander = 16;
    int leftDeltaH, rightDeltaH, leftDeltaV, rightDeltaV, range;

    leftDeltaH = h - kLeftObeliskH;				// determine the h and v distances between
    rightDeltaH = h - kRightObeliskH;			// obelisks and the target point
    leftDeltaV = v - kLeftObeliskV;
    rightDeltaV = v - kRightObeliskV;

    for (int i = 0; i < kNumLightningPts; i++) { // calculate an even spread of points between
                                                 // obelisk tips and the target point
        leftLightningPts[i].h = (leftDeltaH * i) / (kNumLightningPts - 1) + kLeftObeliskH;
        leftLightningPts[i].v = (leftDeltaV * i) / (kNumLightningPts - 1) + kLeftObeliskV;
        rightLightningPts[i].h = (rightDeltaH * i) / (kNumLightningPts - 1) + kRightObeliskH;
        rightLightningPts[i].v = (rightDeltaV * i) / (kNumLightningPts - 1) + kRightObeliskV;
    }

    range = kWander * 2 + 1;					// randomly scatter the points vertically
    for (int i = 1; i < kNumLightningPts - 1; i++) { // but NOT the 1st or last points
        leftLightningPts[i].v += utils.randomInt(range) - kWander;
        rightLightningPts[i].v += utils.randomInt(range) - kWander;
    }
}

void GL::Game::drawLightning() const
{
    if (lightningCount <= 0 && newGameLightning < 0) {
        return;
    }
    
    Renderer *r = renderer_;
    r->setFillColor(255, 255, 0);
    r->beginLines(2.0f);
    r->moveTo(leftLightningPts[0].h, leftLightningPts[0].v);
    for (int i = 0; i < kNumLightningPts - 1; i++) {
        r->moveTo(leftLightningPts[i].h, leftLightningPts[i].v);
        r->lineTo(leftLightningPts[i + 1].h - 1, leftLightningPts[i + 1].v);
    }
    r->moveTo(rightLightningPts[0].h, rightLightningPts[0].v);
    for (int i = 0; i < kNumLightningPts - 1; i++) {
        r->moveTo(rightLightningPts[i].h, rightLightningPts[i].v);
        r->lineTo(rightLightningPts[i + 1].h - 1, rightLightningPts[i + 1].v);
    }
    r->endLines();
}

void GL::Game::newGame()
{
    countDownTimer = 0;
	numLedges = 3;
	levelOn = 0;
    livesLeft = kInitNumLives;
    score_ = 0L;
    playing = true;
    numOwls = 4;
    helpState = kHelpClosed;
    
    initHandLocation();
	theHand.mode = kLurking;
    
    setUpLevel();
    generateEnemies();
    resetPlayer(true);
    
    cursor.obscure();
    if (callback_) {
        callback_(EventStarted, callbackContext_);
    }
}

void GL::Game::endGame()
{
    playing = false;
    sounds.play(kMusicSound);
    //CheckHighScore();
    cursor.show();
    if (callback_) {
        callback_(EventEnded, callbackContext_);
    }
}

void GL::Game::showHelp()
{
    if (!playing) {
        openHelp();
    }
}

void GL::Game::setUpLevel()
{
	int waveMultiple;
	
	killOffEye();
	
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
}

void GL::Game::resetPlayer(bool initialPlace)
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
    
    if (lightningCount == 0) {
        doLightning(GL::Point(thePlayer.dest.left + 24, thePlayer.dest.bottom - 24), kNumLightningStrikes);
    }
}

void GL::Game::offAMortal()
{
    livesLeft--;

    if (livesLeft > 0) {
        resetPlayer(false);
    } else {
        endGame();
    }
}

void GL::Game::drawBackground() const
{
    bgImg.draw(0, 0);
}

void GL::Game::drawTorches() const
{
    int who = utils.randomInt(4);
    if (evenFrame) {
        torchesImg.draw(flameDestRects[0], flameRects[who]);
    } else {
        torchesImg.draw(flameDestRects[1], flameRects[who]);
    }
}

void GL::Game::drawHand() const
{
    if (theHand.mode == kOutGrabeth) {
        handImg.draw(theHand.dest, handRects[0]);
    } else if (theHand.mode == kClutching) {
        handImg.draw(theHand.dest, handRects[1]);
    }
}

void GL::Game::drawPlayer() const
{
    GL::Rect src;

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

    if (thePlayer.wrapping) {
        if (thePlayer.mode == kBones) {
            src = playerRects[thePlayer.srcNum];
            src.bottom = src.top + thePlayer.frame;
            playerImg.draw(thePlayer.wrap, src);
        } else {
            playerImg.draw(thePlayer.wrap, playerRects[thePlayer.srcNum]);
        }
    }
}

void GL::Game::drawPlatforms() const
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

void GL::Game::movePlayer()
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
            handlePlayerBones();
            break;
	}
    
    checkPlayerWrapAround();
    
    thePlayer.wasH = thePlayer.h;
	thePlayer.wasV = thePlayer.v;
	thePlayer.wasDest = thePlayer.dest;
}

void GL::Game::handlePlayerIdle()
{
	thePlayer.frame--;
	if (thePlayer.frame == 0) {
		thePlayer.mode = kWalking;
    }
	setAndCheckPlayerDest();
}

void GL::Game::handlePlayerWalking()
{
	int desiredHVel;
	
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
	keepPlayerOnPlatform();
	checkPlayerEnemyCollision();
}

void GL::Game::handlePlayerFlying()
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
	checkPlayerEnemyCollision();
	checkPlatformCollision();
	checkTouchDownCollision();
}

void GL::Game::handlePlayerSinking()
{
    thePlayer.hVel = 0;
    thePlayer.vVel = 16;
    if (thePlayer.dest.top > kLavaHeight) {
        offAMortal();
    }

    thePlayer.v += thePlayer.vVel;

    setAndCheckPlayerDest();
}

void GL::Game::checkTouchDownCollision()
{
	GL::Rect testRect;
	int offset;
	bool sected = false;
	
	for (int i = 0; i < numLedges; i++) {
		testRect = touchDownRects[i];
		if (thePlayer.mode == kWalking) {
            testRect.offsetBy(0, 11);
        }
		
        if (thePlayer.dest.sect(testRect)) {
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

void GL::Game::handlePlayerFalling()
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

void GL::Game::handlePlayerBones()
{
    if (evenFrame) {
        thePlayer.frame--;
        if (thePlayer.frame == 0) {
            offAMortal();
        } else {
            thePlayer.dest.top = thePlayer.dest.bottom - thePlayer.frame;
        }
    }
}

void GL::Game::keepPlayerOnPlatform()
{
    for (int i = 0; i < numLedges; i++) {
        if ((thePlayer.dest.sect(platformRects[i])) && (thePlayer.vVel > 0)) {
            int offset = thePlayer.dest.bottom - platformRects[i].top - 1;
            thePlayer.dest.top -= offset;
            thePlayer.dest.bottom -= offset;
            thePlayer.v = thePlayer.dest.top * 16;
        }
    }

    if (thePlayer.vVel > 0) {
        thePlayer.vVel = 0;
    }
}

void GL::Game::checkLavaRoofCollision()
{
	int offset;
	
	if (thePlayer.dest.bottom> kLavaHeight)
	{
		if (thePlayer.mode == kFalling) {
			sounds.play(kSplashSound);
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

void GL::Game::setAndCheckPlayerDest()
{
	int wasTall, wasWide;
	
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

void GL::Game::checkPlatformCollision()
{
	GL::Rect hRect, vRect;
	int offset;
	
	for (int i = 0; i < numLedges; i++)
	{
		if (thePlayer.dest.sect(platformRects[i]))
		{
			hRect.left = thePlayer.dest.left;
			hRect.right = thePlayer.dest.right;
			hRect.top = thePlayer.wasDest.top;
			hRect.bottom = thePlayer.wasDest.bottom;
			
			if (hRect.sect(platformRects[i]))
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
				
				if (vRect.sect(platformRects[i]))
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
								sounds.play(kBoom1Sound);
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

void GL::Game::getPlayerInput()
{
    Locker locker(lock_);
    int theKeys = keys_;
    
	thePlayer.flapping = false;
	thePlayer.walking = false;
	
    if ((theKeys & KeySpacebar) || (theKeys & KeyDownArrow)) {
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
    
    if (((theKeys & KeyRightArrow) ||
         (theKeys & KeyS) ||
         (theKeys & KeyQuote)) &&
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
    else if (((theKeys & KeyLeftArrow) ||
              (theKeys & KeyA) ||
              (theKeys & KeyColon)) &&
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

void GL::Game::handleKeyDownEvent(Key key)
{
    Locker locker(lock_);
    keys_ |= key;
    
    if (helpState == kHelpOpen) {
        if (key == KeyUpArrow) {
            scrollHelp(-3);
        } else if (key == KeyDownArrow) {
            scrollHelp(3);
        } else if (key == KeyPageDown) {
            scrollHelp(199);
        } else if (key == KeyPageUp) {
            scrollHelp(-199);
        }
    }
}

void GL::Game::handleKeyUpEvent(Key key)
{
    Locker locker(lock_);
    keys_ &= ~key;
}

void GL::Game::drawLivesNumbers() const
{
	int digit;
	
	digit = (livesLeft - 1) / 10;
	digit = digit % 10L;
	if ((digit == 0) && ((livesLeft - 1) < 100)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[0], numbersSrc[digit]);
	
	digit = (livesLeft - 1) % 10;
    numbersImg.draw(numbersDest[1], numbersSrc[digit]);
}

void GL::Game::addToScore(int value)
{
    long oldDigit, newDigit;
    oldDigit = score_ / 10000L;
    score_ += value;
    newDigit = score_ / 10000L;
    livesLeft += (newDigit - oldDigit);
}

void GL::Game::drawScoreNumbers() const
{
	long digit;
	
	digit = score_ / 100000L;
	digit = digit % 10L;
	if ((digit == 0) && (score_ < 1000000L)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[2], numbersSrc[digit]);
	
	digit = score_ / 10000L;
	digit = digit % 10L;
	if ((digit == 0) && (score_ < 100000L)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[3], numbersSrc[digit]);
	
	digit = score_ / 1000L;
	digit = digit % 10L;
	if ((digit == 0) && (score_ < 10000L)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[4], numbersSrc[digit]);
	
	digit = score_ / 100L;
	digit = digit % 10L;
	if ((digit == 0) && (score_ < 1000L)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[5], numbersSrc[digit]);
	
	digit = score_ / 10L;
	digit = digit % 10L;
	if ((digit == 0) && (score_ < 100L)) {
		digit = 10;
    }
    numbersImg.draw(numbersDest[6], numbersSrc[digit]);
	
	digit = score_ % 10L;
    numbersImg.draw(numbersDest[7], numbersSrc[digit]);
}

void GL::Game::drawLevelNumbers() const
{
	int digit;
	
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

void GL::Game::initHandLocation()
{
    theHand.dest.set(0, 0, 56, 57);
    theHand.dest.offsetBy(48, 460);
}

void GL::Game::handleHand()
{
    int hDiff, vDiff, pull, speed;

    switch (theHand.mode) {
        case kLurking:
            if (thePlayer.mode == kFlying && thePlayer.dest.sect(grabZone)) {
                theHand.mode = kOutGrabeth;
                initHandLocation();
            }
            break;
            
        case kOutGrabeth:
        case kClutching:
            if (thePlayer.dest.sect(grabZone)) {
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

void GL::Game::checkPlayerWrapAround()
{
    GL::Rect wrapRect;

    if (thePlayer.dest.right > 640) {
        thePlayer.wrapping = true;
        wrapRect = thePlayer.dest;
        wrapRect.left -= 640;
        wrapRect.right -= 640;
        thePlayer.wrap = wrapRect;
    } else if (thePlayer.dest.left < 0) {
        thePlayer.wrapping = true;
        wrapRect = thePlayer.dest;
        wrapRect.left += 640;
        wrapRect.right += 640;
        thePlayer.wrap = wrapRect;
    } else {
        thePlayer.wrapping = false;
    }
}

void GL::Game::drawObelisks() const
{
    if (flashObelisks) {
        obelisksImg.draw(obeliskRects[2], obeliskRects[0]);
        obelisksImg.draw(obeliskRects[3], obeliskRects[1]);
    } else {
        // Redraw obelisks and lava in "foreground"
        GL::Point pts[12];
        int o = 0;
        pts[o++] = GL::Point(161, 450);
        pts[o++] = GL::Point(161, 269);
        pts[o++] = GL::Point(172, 250);
        pts[o++] = GL::Point(182, 269);
        pts[o++] = GL::Point(182, 450);
        pts[o++] = GL::Point(161, 450);
        pts[o++] = GL::Point(457, 450);
        pts[o++] = GL::Point(457, 269);
        pts[o++] = GL::Point(468, 250);
        pts[o++] = GL::Point(478, 269);
        pts[o++] = GL::Point(478, 450);
        pts[o++] = GL::Point(457, 450);
        bgImg.draw(pts, 6, pts, 6);
        bgImg.draw(pts + 6, 6, pts + 6, 6);
        
        GL::Rect lava1(0, 450, 214, 10);
        GL::Rect lava2(425, 450, 215, 10);
        bgImg.draw(lava1, lava1);
        bgImg.draw(lava2, lava2);
    }
}

void GL::Game::handleCountDownTimer()
{
	if (countDownTimer == 0) {
		return;
	} else {
		countDownTimer--;
		if (countDownTimer == 0) {
			countDownTimer = 0;
			levelOn++;
			setUpLevel();
			generateEnemies();
		}
	}
}

void GL::Game::moveEnemies()
{
    doEnemyFlapSound = false;
    doEnemyScrapeSound = false;

    for (int i = 0; i < numEnemies; i++) {
        switch (theEnemies[i].mode) {
            case kIdle:
                handleIdleEnemies(i);
                break;
                
            case kFlying:
                handleFlyingEnemies(i);
                break;
                
            case kWalking:
                handleWalkingEnemy(i);
                break;
                
            case kSpawning:
                handleSpawningEnemy(i);
                break;
                
            case kFalling:
                handleFallingEnemy(i);
                break;
                
            case kEggTimer:
                handleEggEnemy(i);
                break;
        }
        
        theEnemies[i].wasDest = theEnemies[i].dest;
        theEnemies[i].wasH = theEnemies[i].h;
        theEnemies[i].wasV = theEnemies[i].v;
    }

    if (doEnemyFlapSound) {
        sounds.play(kFlap2Sound);
    }
    if (doEnemyScrapeSound) {
        sounds.play(kScrape2Sound);
    }
    if ((deadEnemies >= numEnemiesThisLevel) && (countDownTimer == 0)) {
        countDownTimer = 30;
    }
}

void GL::Game::checkEnemyWrapAround(int who) const
{
	GL::Rect wrapRect, src;
	
	if (theEnemies[who].dest.right > 640)
	{
		wrapRect = theEnemies[who].dest;
		wrapRect.left -= 640;
		wrapRect.right -= 640;
		
		if ((theEnemies[who].mode == kFalling) || (theEnemies[who].mode == kEggTimer))
		{
			if ((theEnemies[who].mode == kEggTimer) && (theEnemies[who].frame < 24))
			{
				src = eggSrcRect;
				src.bottom = src.top + theEnemies[who].frame;
			}
			else
				src = eggSrcRect;
            egg.draw(wrapRect, src);
		}
		else
		{
            enemyFly.draw(wrapRect, enemyRects[theEnemies[who].srcNum]);
		}
	}
	else if (theEnemies[who].dest.left < 0)
	{
		wrapRect = theEnemies[who].dest;
		wrapRect.left += 640;
		wrapRect.right += 640;
		
		if ((theEnemies[who].mode == kFalling) || (theEnemies[who].mode == kEggTimer))
		{
			if ((theEnemies[who].mode == kEggTimer) && (theEnemies[who].frame < 24))
			{
				src = eggSrcRect;
				src.bottom = src.top + theEnemies[who].frame;
			}
			else
				src = eggSrcRect;
            egg.draw(wrapRect, src);
		}
		else
		{
            enemyFly.draw(wrapRect, enemyRects[theEnemies[who].srcNum]);
		}
	}
}

void GL::Game::drawEnemies() const
{
    GL::Rect src;
    for (int i = 0; i < numEnemies; i++) {
        switch (theEnemies[i].mode) {
            case kSpawning:
                src = enemyRects[theEnemies[i].srcNum];
                src.bottom = src.top + theEnemies[i].frame;
                enemyWalk.draw(theEnemies[i].dest, src);
                break;
                
            case kFlying:
                enemyFly.draw(theEnemies[i].dest, enemyRects[theEnemies[i].srcNum]);
                checkEnemyWrapAround(i);
                break;
                
            case kWalking:
                enemyWalk.draw(theEnemies[i].dest, enemyRects[theEnemies[i].srcNum]);
                break;
                
            case kFalling:
                egg.draw(theEnemies[i].dest, eggSrcRect);
                checkEnemyWrapAround(i);
                break;
                
            case kEggTimer:
                if (theEnemies[i].frame < 24) {
                    src = eggSrcRect;
                    src.bottom = src.top + theEnemies[i].frame;
                } else {
                    src = eggSrcRect;
                }
                egg.draw(theEnemies[i].dest, src);
                checkEnemyWrapAround(i);
                break;
        }
    }
}

void GL::Game::generateEnemies()
{
	if ((levelOn % 5) == 4)			// Egg Wave
	{
		numEnemies = kMaxEnemies;
		numEnemiesThisLevel = numEnemies;
	}
	else
	{
		numEnemies = ((levelOn / 5) + 2) * 2;
		if (numEnemies > kMaxEnemies)
			numEnemies = kMaxEnemies;
		numEnemiesThisLevel = numEnemies * 2;
	}
	
	deadEnemies = 0;
	
	numOwls = 4 - ((levelOn + 2) / 5);
	if (numOwls < 0)
		numOwls = 0;
	
	spawnedEnemies = 0;
	
	for (int i = 0; i < numEnemies; i++)
		initEnemy(i, false);
}

bool GL::Game::setEnemyInitialLocation(GL::Rect& theRect)
{
	int where, possibilities;
	bool facing;
	
	possibilities = numLedges - 1;
	where = utils.randomInt(possibilities);
	theRect = enemyInitRects[where];
	
	switch (where)
	{
		case 0:
		case 2:
            facing = true;
            break;
            
		case 3:
            if (utils.randomInt(2) == 0)
                facing = true;
            else
                facing = false;
            break;
            
		default:
            facing = false;
            break;
	}
	
	if ((levelOn % 5) == 4)			// Egg Wave
	{
		theRect.left += 12 + utils.randomInt(48) - 24;
		theRect.right = theRect.left + 24;
		theRect.top = theRect.bottom - 24;
	}
	
	return (facing);
}

void GL::Game::initEnemy(int i, bool reincarnated)
{
	bool facing;
	
	if (spawnedEnemies < numEnemiesThisLevel)
	{
		facing = setEnemyInitialLocation(theEnemies[i].dest);
		theEnemies[i].wasDest = theEnemies[i].dest;
		theEnemies[i].h = theEnemies[i].dest.left << 4;
		theEnemies[i].v = theEnemies[i].dest.top << 4;
		theEnemies[i].wasH = theEnemies[i].h;
		theEnemies[i].wasV = theEnemies[i].v;
		theEnemies[i].targetAlt = theEnemies[i].v - (40 << 4);
		theEnemies[i].hVel = 0;
		theEnemies[i].vVel = 0;
		theEnemies[i].pass = 0;
		if ((levelOn % 5) == 4)			// Egg Wave
			theEnemies[i].mode = kEggTimer;
		else
			theEnemies[i].mode = kIdle;
		if (i < numOwls)
			theEnemies[i].kind = kOwl;
		else if (i > (numOwls + 6))
			theEnemies[i].kind = kJackal;
		else
			theEnemies[i].kind = kWolf;
		theEnemies[i].facingRight = facing;
		setEnemyAttributes(i);
		
		if (reincarnated)
			theEnemies[i].frame = utils.randomInt(48) + 8 + (numOwls * 32);
		else
			theEnemies[i].frame = utils.randomInt(48) + 32 + (64 * i) + (numOwls * 32);
		
		if ((levelOn % 5) == 4)			// Egg Wave
			theEnemies[i].kind--;
		
		spawnedEnemies++;
	}
}

void GL::Game::setEnemyAttributes(int i)
{
	int		h;
	
	h = (theEnemies[i].dest.left + theEnemies[i].dest.right) >> 1;
	if (h < 320)
		theEnemies[i].facingRight = true;
	else
		theEnemies[i].facingRight = false;
	
	switch (theEnemies[i].kind)
	{
		case kOwl:
            if (theEnemies[i].facingRight)
                theEnemies[i].srcNum = 0;
            else
                theEnemies[i].srcNum = 2;
            theEnemies[i].maxHVel = kOwlMaxHVel;
            theEnemies[i].maxVVel = kOwlMaxVVel;
            theEnemies[i].heightSmell = kOwlHeightSmell;
            theEnemies[i].flapImpulse = kOwlFlapImpulse;
            break;
            
		case kWolf:
            if (theEnemies[i].facingRight)
                theEnemies[i].srcNum = 4;
            else
                theEnemies[i].srcNum = 6;
            theEnemies[i].maxHVel = kWolfMaxHVel;
            theEnemies[i].maxVVel = kWolfMaxVVel;
            theEnemies[i].heightSmell = kWolfHeightSmell;
            theEnemies[i].flapImpulse = kWolfFlapImpulse;
            break;
            
		case kJackal:
            if (theEnemies[i].facingRight)
                theEnemies[i].srcNum = 8;
            else
                theEnemies[i].srcNum = 10;
            theEnemies[i].maxHVel = kJackalMaxHVel;
            theEnemies[i].maxVVel = kJackalMaxVVel;
            theEnemies[i].heightSmell = kJackalHeightSmell;
            theEnemies[i].flapImpulse = kJackalFlapImpulse;
            break;
	}
}

int GL::Game::assignNewAltitude(void)
{
	int which, altitude = 0;
	
	which = utils.randomInt(4);
	switch (which) {
		case 0:
            altitude = 65 << 4;
            break;
            
		case 1:
            altitude = 150 << 4;
            break;
            
		case 2:
            altitude = 245 << 4;
            break;
            
		case 3:
            altitude = 384 << 4;
            break;
	}
	
	return (altitude);
}

void GL::Game::checkEnemyPlatformHit(int h)
{
	GL::Rect hRect, vRect, whoCares;
	int i, offset;
	
	for (i = 0; i < numLedges; i++)
	{
		if (theEnemies[h].dest.sect(platformRects[i]))
		{
			hRect.left = theEnemies[h].dest.left;
			hRect.right = theEnemies[h].dest.right;
			hRect.top = theEnemies[h].wasDest.top;
			hRect.bottom = theEnemies[h].wasDest.bottom;
			
			if (hRect.sect(platformRects[i]))
			{
				if (theEnemies[h].h > theEnemies[h].wasH)	// moving to right
				{
					offset = theEnemies[h].dest.right - platformRects[i].left;
					theEnemies[h].dest.left -= offset;
					theEnemies[h].dest.right -= offset;
					theEnemies[h].h = theEnemies[h].dest.left << 4;
					theEnemies[h].wasH = theEnemies[h].h;
					if (theEnemies[h].hVel > 0)
						theEnemies[h].hVel = -(theEnemies[h].hVel >> 1);
					else
						theEnemies[h].hVel = theEnemies[h].hVel >> 1;
				}
				if (theEnemies[h].h < theEnemies[h].wasH)	// moving to left
				{
					offset = platformRects[i].right - theEnemies[h].dest.left;
					theEnemies[h].dest.left += offset;
					theEnemies[h].dest.right += offset;
					theEnemies[h].h = theEnemies[h].dest.left << 4;
					theEnemies[h].wasH = theEnemies[h].h;
					if (theEnemies[h].hVel < 0)
						theEnemies[h].hVel = -(theEnemies[h].hVel >> 1);
					else
						theEnemies[h].hVel = theEnemies[h].hVel >> 1;
				}
				doEnemyScrapeSound = true;
				theEnemies[h].facingRight = !theEnemies[h].facingRight;
			}
			else
			{
				vRect.left = theEnemies[h].wasDest.left;
				vRect.right = theEnemies[h].wasDest.right;
				vRect.top = theEnemies[h].dest.top;
				vRect.bottom = theEnemies[h].dest.bottom;
				
				if (vRect.sect(platformRects[i]))
				{
					if (theEnemies[h].mode == kFalling)
					{
						theEnemies[i].hVel -= (theEnemies[i].hVel >> 3);
						if ((theEnemies[i].hVel < 8) && (theEnemies[i].hVel > -8))
						{
							if (theEnemies[i].hVel > 0)
								theEnemies[i].hVel--;
							else if (theEnemies[i].hVel < 0)
								theEnemies[i].hVel++;
						}
					}
					
					if (theEnemies[h].v > theEnemies[h].wasV)		// heading down
					{
						offset = theEnemies[h].dest.bottom - platformRects[i].top;
						theEnemies[h].dest.top -= offset;
						theEnemies[h].dest.bottom -= offset;
						theEnemies[h].v = theEnemies[h].dest.top << 4;
						theEnemies[h].wasV = theEnemies[h].v;
						if (theEnemies[h].vVel > kDontFlapVel) {
							doEnemyScrapeSound = true;
                        }
						if (theEnemies[h].vVel > 0) {
							theEnemies[h].vVel = -(theEnemies[h].vVel >> 1);
						} else {
							theEnemies[h].vVel = theEnemies[h].vVel >> 1;
                        }
						if ((theEnemies[h].vVel < 8) && (theEnemies[h].vVel > -8) &&
                            (theEnemies[h].hVel == 0) && (theEnemies[h].mode == kFalling))
						{
							if (((theEnemies[h].dest.right - 8) > platformRects[i].right) &&
                                (theEnemies[h].hVel == 0))
							{				// if enemy has come to rest half off the edge
								theEnemies[h].hVel = 32;
							}
							else if (((theEnemies[h].dest.left + 8) < platformRects[i].left) &&
                                     (theEnemies[h].hVel == 0))
							{
								theEnemies[h].hVel = -32;
							}
							else
							{
								theEnemies[h].mode = kEggTimer;
								theEnemies[h].frame = (numOwls * 96) + 128;
								theEnemies[h].vVel = 0;
							}
						}
					}
					if (theEnemies[h].v < theEnemies[h].wasV)		// heading up
					{
						offset = theEnemies[h].dest.top - platformRects[i].bottom;
						theEnemies[h].dest.top -= offset;
						theEnemies[h].dest.bottom -= offset;
						theEnemies[h].v = theEnemies[h].dest.top << 4;
						theEnemies[h].wasV = theEnemies[h].v;
						doEnemyScrapeSound = true;
						if (theEnemies[h].vVel < 0)
							theEnemies[h].vVel = -(theEnemies[h].vVel >> 2);
						else
							theEnemies[h].vVel = theEnemies[h].vVel >> 2;
						if ((theEnemies[h].vVel < 8) && (theEnemies[h].vVel > -8) &&
                            (theEnemies[h].hVel == 0) && (theEnemies[h].mode == kFalling))
						{
							theEnemies[h].mode = kEggTimer;
							theEnemies[h].frame = (numOwls * 96) + 128;
							theEnemies[h].vVel = 0;
						}
					}
				}
			}
		}
	}
}

void GL::Game::checkEnemyRoofCollision(int i)
{
	int offset;
	
	if (theEnemies[i].dest.top < (kRoofHeight - 2))
	{
		offset = kRoofHeight - theEnemies[i].dest.top;
		theEnemies[i].dest.top += offset;
		theEnemies[i].dest.bottom += offset;
		theEnemies[i].v = theEnemies[i].dest.top << 4;
		doEnemyScrapeSound = true;
		theEnemies[i].vVel = -(theEnemies[i].vVel >> 2);
	}
	else if (theEnemies[i].dest.top > kLavaHeight)
	{
		theEnemies[i].mode = kDeadAndGone;
		deadEnemies++;
		
		sounds.play(kSplashSound);
		initEnemy(i, true);
	}
}

void GL::Game::handleIdleEnemies(int i)
{
	theEnemies[i].frame--;
	if (theEnemies[i].frame <= 0)
	{
		theEnemies[i].mode = kSpawning;
		theEnemies[i].wasH = theEnemies[i].h;
		theEnemies[i].wasV = theEnemies[i].v;
		theEnemies[i].hVel = 0;
		theEnemies[i].vVel = 0;
		theEnemies[i].frame = 0;
		setEnemyAttributes(i);
		sounds.play(kSpawnSound);
	}
}

void GL::Game::handleFlyingEnemies(int i)
{
	int dist;
	bool shouldFlap;
	
	theEnemies[i].vVel += kGravity;
	
	dist = thePlayer.dest.top - theEnemies[i].dest.top;
	if (dist < 0)
		dist = -dist;
	
	if ((dist < theEnemies[i].heightSmell) &&
        ((thePlayer.mode == kFlying) || (thePlayer.mode == kWalking)))
	{							// enemy will actively seek the player
		if (thePlayer.dest.left < theEnemies[i].dest.left)
		{
			dist = theEnemies[i].dest.left - thePlayer.dest.left;
			if (dist < 320)		// closest route is to the left
				theEnemies[i].facingRight = false;
			else				// closest route is to the right
				theEnemies[i].facingRight = true;
		}
		else if (thePlayer.dest.left > theEnemies[i].dest.left)
		{
			dist = thePlayer.dest.left - theEnemies[i].dest.left;
			if (dist < 320)		// closest route is to the right
				theEnemies[i].facingRight = true;
			else				// closest route is to the left
				theEnemies[i].facingRight = false;
		}
        // seek point 16 pixels above player
		if (((theEnemies[i].v + 16) > thePlayer.v) && (evenFrame))
			shouldFlap = true;
		else
			shouldFlap = false;
	}
	else
	{
		if ((theEnemies[i].v > theEnemies[i].targetAlt) && (evenFrame))
			shouldFlap = true;
		else
			shouldFlap = false;
	}
	
	if (shouldFlap)
	{
		theEnemies[i].vVel -= theEnemies[i].flapImpulse;
		doEnemyFlapSound = true;
	}
	
	if (theEnemies[i].facingRight)
	{
		theEnemies[i].hVel += kEnemyImpulse;
		if (theEnemies[i].hVel > theEnemies[i].maxHVel)
			theEnemies[i].hVel = theEnemies[i].maxHVel;
		
		switch (theEnemies[i].kind)
		{
			case kOwl:
                if (shouldFlap)
                    theEnemies[i].srcNum = 12;
                else
                    theEnemies[i].srcNum = 13;
                break;
                
			case kWolf:
                if (shouldFlap)
                    theEnemies[i].srcNum = 16;
                else
                    theEnemies[i].srcNum = 17;
                break;
                
			case kJackal:
                if (shouldFlap)
                    theEnemies[i].srcNum = 20;
                else
                    theEnemies[i].srcNum = 21;
                break;
		}
		
	}
	else
	{
		theEnemies[i].hVel -= kEnemyImpulse;
		if (theEnemies[i].hVel < -theEnemies[i].maxHVel)
			theEnemies[i].hVel = -theEnemies[i].maxHVel;
		
		switch (theEnemies[i].kind)
		{
			case kOwl:
                if (shouldFlap)
                    theEnemies[i].srcNum = 14;
                else
                    theEnemies[i].srcNum = 15;
                break;
                
			case kWolf:
                if (shouldFlap)
                    theEnemies[i].srcNum = 18;
                else
                    theEnemies[i].srcNum = 19;
                break;
                
			case kJackal:
                if (shouldFlap)
                    theEnemies[i].srcNum = 22;
                else
                    theEnemies[i].srcNum = 23;
                break;
		}
	}
	
	theEnemies[i].h += theEnemies[i].hVel;
	theEnemies[i].dest.left = theEnemies[i].h >> 4;
	theEnemies[i].dest.right = theEnemies[i].dest.left + 64;
	
	theEnemies[i].v += theEnemies[i].vVel;
	theEnemies[i].dest.top = theEnemies[i].v >> 4;
	theEnemies[i].dest.bottom = theEnemies[i].dest.top + 40;
	
	if (theEnemies[i].dest.left > 640)
	{
		theEnemies[i].dest.offsetBy(-640, 0);
		theEnemies[i].h = theEnemies[i].dest.left << 4;
		theEnemies[i].wasDest.offsetBy(-640, 0);
		theEnemies[i].pass++;
		if (theEnemies[i].pass > 2)		// after two screen passes
		{								// enemy patrols a new altitude
			theEnemies[i].targetAlt = assignNewAltitude();
			theEnemies[i].pass = 0;
		}
	}
	else if (theEnemies[i].dest.right < 0)
	{
		theEnemies[i].dest.offsetBy(640, 0);
		theEnemies[i].h = theEnemies[i].dest.left << 4;
		theEnemies[i].wasDest.offsetBy(640, 0);
		theEnemies[i].pass++;
		if (theEnemies[i].pass > 2)
		{
			theEnemies[i].targetAlt = assignNewAltitude();
			theEnemies[i].pass = 0;
		}
	}
	
	theEnemies[i].vVel -= theEnemies[i].vVel >> 4;	// friction
	
	if (theEnemies[i].vVel > theEnemies[i].maxVVel)
		theEnemies[i].vVel = theEnemies[i].maxVVel;
	else if (theEnemies[i].vVel < -theEnemies[i].maxVVel)
		theEnemies[i].vVel = -theEnemies[i].maxVVel;
	
	checkEnemyRoofCollision(i);
	checkEnemyPlatformHit(i);
}

void GL::Game::handleWalkingEnemy(int i)
{
	if (theEnemies[i].facingRight)
	{
		theEnemies[i].dest.left += 6;
		theEnemies[i].dest.right += 6;
		switch (theEnemies[i].kind)
		{
			case kOwl:
                theEnemies[i].srcNum = 1 - theEnemies[i].srcNum;
                break;
                
			case kWolf:
                theEnemies[i].srcNum = 9 - theEnemies[i].srcNum;
                break;
                
			case kJackal:
                theEnemies[i].srcNum = 17 - theEnemies[i].srcNum;
                break;
		}
		theEnemies[i].hVel = 6 << 4;
	}
	else
	{
		theEnemies[i].dest.left -= 6;
		theEnemies[i].dest.right -= 6;
		switch (theEnemies[i].kind)
		{
			case kOwl:
                theEnemies[i].srcNum = 5 - theEnemies[i].srcNum;
                break;
                
			case kWolf:
                theEnemies[i].srcNum = 13 - theEnemies[i].srcNum;
                break;
                
			case kJackal:
                theEnemies[i].srcNum = 21 - theEnemies[i].srcNum;
                break;
		}
		theEnemies[i].hVel = -(6 << 4);
	}
	theEnemies[i].frame++;
	if (theEnemies[i].frame >= 8)
	{
		theEnemies[i].mode = kFlying;
		theEnemies[i].frame = 0;
		switch (theEnemies[i].kind)
		{
			case kOwl:
                if (theEnemies[i].facingRight)
                    theEnemies[i].srcNum = 12;
                else
                    theEnemies[i].srcNum = 14;
                break;
                
			case kWolf:
                if (theEnemies[i].facingRight)
                    theEnemies[i].srcNum = 16;
                else
                    theEnemies[i].srcNum = 18;
                break;
                
			case kJackal:
                if (theEnemies[i].facingRight)
                    theEnemies[i].srcNum = 20;
                else
                    theEnemies[i].srcNum = 22;
                break;
		}
		
		theEnemies[i].dest.left -= 8;
		theEnemies[i].dest.right += 8;
		theEnemies[i].dest.bottom = theEnemies[i].dest.top + 40;
		theEnemies[i].h = theEnemies[i].dest.left * 16;
		theEnemies[i].v = theEnemies[i].dest.top * 16;
	}
}

void GL::Game::handleSpawningEnemy(int i)
{
	theEnemies[i].frame++;
	if (theEnemies[i].frame >= 48)
	{
		theEnemies[i].mode = kWalking;
		theEnemies[i].frame = 0;
		
		switch (theEnemies[i].kind)
		{
			case kOwl:
                if (theEnemies[i].facingRight)
                    theEnemies[i].srcNum = 0;
                else
                    theEnemies[i].srcNum = 2;
                break;
                
			case kWolf:
                if (theEnemies[i].facingRight)
                    theEnemies[i].srcNum = 4;
                else
                    theEnemies[i].srcNum = 6;
                break;
                
			case kJackal:
                if (theEnemies[i].facingRight)
                    theEnemies[i].srcNum = 8;
                else
                    theEnemies[i].srcNum = 10;
                break;
		}
	}
	else
		theEnemies[i].dest.top = theEnemies[i].dest.bottom - theEnemies[i].frame;
}

void GL::Game::handleFallingEnemy(int i)
{
	theEnemies[i].vVel += kGravity;
	
	if (theEnemies[i].vVel > theEnemies[i].maxVVel)
		theEnemies[i].vVel = theEnemies[i].maxVVel;
	else if (theEnemies[i].vVel < -theEnemies[i].maxVVel)
		theEnemies[i].vVel = -theEnemies[i].maxVVel;
	
	if (evenFrame)
	{
		theEnemies[i].hVel -= (theEnemies[i].hVel >> 5);
		if ((theEnemies[i].hVel < 32) && (theEnemies[i].hVel > -32))
		{
			if (theEnemies[i].hVel > 0)
				theEnemies[i].hVel--;
			else if (theEnemies[i].hVel < 0)
				theEnemies[i].hVel++;
		}
	}
	
	theEnemies[i].h += theEnemies[i].hVel;
	theEnemies[i].dest.left = theEnemies[i].h >> 4;
	theEnemies[i].dest.right = theEnemies[i].dest.left + 24;
	
	theEnemies[i].v += theEnemies[i].vVel;
	theEnemies[i].dest.top = theEnemies[i].v >> 4;
	theEnemies[i].dest.bottom = theEnemies[i].dest.top + 24;
	
	if (theEnemies[i].dest.left > 640)
	{
		theEnemies[i].dest.offsetBy(-640, 0);
		theEnemies[i].h = theEnemies[i].dest.left << 4;
		theEnemies[i].wasDest.offsetBy(-640, 0);
	}
	else if (theEnemies[i].dest.right < 0)
	{
		theEnemies[i].dest.offsetBy(640, 0);
		theEnemies[i].h = theEnemies[i].dest.left << 4;
		theEnemies[i].wasDest.offsetBy(640, 0);
	}
	
	checkEnemyRoofCollision(i);
	checkEnemyPlatformHit(i);
}

void GL::Game::handleEggEnemy(int i)
{
	int center;
	
	theEnemies[i].frame--;
	if (theEnemies[i].frame < 24) {
		theEnemies[i].dest.top = theEnemies[i].dest.bottom - theEnemies[i].frame;
		if (theEnemies[i].frame == 0) {		// a sphinx is born!
			theEnemies[i].frame = 0;
			sounds.play(kSpawnSound);
			center = (theEnemies[i].dest.left + theEnemies[i].dest.right) >> 1;
			theEnemies[i].dest.left = center - 24;
			theEnemies[i].dest.right = center + 24;
			theEnemies[i].wasDest = theEnemies[i].dest;
			theEnemies[i].h = theEnemies[i].dest.left << 4;
			theEnemies[i].v = theEnemies[i].dest.top << 4;
			theEnemies[i].wasH = theEnemies[i].h;
			theEnemies[i].wasV = theEnemies[i].v;
			theEnemies[i].hVel = 0;
			theEnemies[i].vVel = 0;
			theEnemies[i].mode = kSpawning;
			theEnemies[i].kind++;
			if (theEnemies[i].kind > kJackal) {
				theEnemies[i].kind = kJackal;
            }
			setEnemyAttributes(i);
		}
	}
}

void GL::Game::resolveEnemyPlayerHit(int i)
{
	int wasVel, diff, h, v;
	
	if ((theEnemies[i].mode == kFalling) || (theEnemies[i].mode == kEggTimer))
	{
		deadEnemies++;
		
		theEnemies[i].mode = kDeadAndGone;
        addToScore(500);
		sounds.play(kBonusSound);
		initEnemy(i, true);
	}
	else
	{
		diff = (theEnemies[i].dest.top + 25) - (thePlayer.dest.top + 19);
		
		if (diff < -2)		// player is bested
		{
			if (lightningCount == 0)
			{
                doLightning(GL::Point(thePlayer.dest.left + 24, thePlayer.dest.bottom - 24), 6);
			}
			
			thePlayer.mode = kFalling;
			if (thePlayer.facingRight) {
				thePlayer.srcNum = 8;
			} else {
				thePlayer.srcNum = 9;
            }
			thePlayer.dest.bottom = thePlayer.dest.top + 37;
			sounds.play(kBoom2Sound);
		}
		else if (diff > 2)	// enemy killed
		{
			if ((theEnemies[i].mode == kSpawning) && (theEnemies[i].frame < 16))
				return;
			
			h = (theEnemies[i].dest.left + theEnemies[i].dest.right) >> 1;
			if (theEnemies[i].mode == kSpawning)
				v = theEnemies[i].dest.bottom - 2;
			else
				v = (theEnemies[i].dest.top + theEnemies[i].dest.bottom) >> 1;
			theEnemies[i].dest.left = h - 12;
			theEnemies[i].dest.right = h + 12;
			if (theEnemies[i].mode == kSpawning)
				theEnemies[i].dest.top = v - 24;
			else
				theEnemies[i].dest.top = v - 12;
			theEnemies[i].dest.bottom = theEnemies[i].dest.top + 24;
			theEnemies[i].h = theEnemies[i].dest.left << 4;
			theEnemies[i].v = theEnemies[i].dest.top << 4;
			theEnemies[i].mode = kFalling;
			theEnemies[i].wasDest = theEnemies[i].dest;
			theEnemies[i].wasH = theEnemies[i].h;
			theEnemies[i].wasV = theEnemies[i].v;
			
			switch (theEnemies[i].kind)
			{
				case kOwl:
                    addToScore(500);
                    break;
                    
				case kWolf:
                    addToScore(1000);
                    break;
                    
				case kJackal:
                    addToScore(1500);
                    break;
			}
			sounds.play(kBoom2Sound);
		}
		else		// neither player nor enemy killed
		{
			if (theEnemies[i].hVel > 0)
				theEnemies[i].facingRight = true;
			else
				theEnemies[i].facingRight = false;
            sounds.play(kScreechSound);
		}
		
		wasVel = thePlayer.hVel;
		thePlayer.hVel = theEnemies[i].hVel;
		theEnemies[i].hVel = wasVel;
		wasVel = thePlayer.vVel;
		thePlayer.vVel = theEnemies[i].vVel;
		theEnemies[i].vVel = wasVel;
	}
}

void GL::Game::checkPlayerEnemyCollision()
{
	GL::Rect whoCares, playTest, wrapTest;
	int i;
	
	playTest = thePlayer.dest;
	playTest.inset(8, 8);
	if (thePlayer.wrapping) {
		wrapTest = thePlayer.wrap;
    }
	wrapTest.inset(8, 8);
	
	for (i = 0; i < numEnemies; i++) {
		if ((theEnemies[i].mode != kIdle) && (theEnemies[i].mode != kDeadAndGone)) {
			if (playTest.sect(theEnemies[i].dest)) {
				resolveEnemyPlayerHit(i);
			} else if (thePlayer.wrapping) {
				if (wrapTest.sect(theEnemies[i].dest)) {
					resolveEnemyPlayerHit(i);
                }
			}
		}
	}
}

void GL::Game::initEye()
{
    theEye.dest.set(0, 0, 48, 31);
    theEye.dest.offsetBy(296, 97);
    theEye.mode = kWaiting;
    theEye.frame = (numOwls + 2) * 720;
    theEye.srcNum = 0;
    theEye.opening = 1;
    theEye.killed = false;
    theEye.entering = false;
}

void GL::Game::killOffEye()
{
    if (theEye.mode == kStalking) {
        theEye.killed = true;
        theEye.opening = 1;
        theEye.entering = false;
        if (theEye.srcNum == 0) {
            theEye.srcNum = 1;
        }
    } else {
        initEye();
    }
}

void GL::Game::handleEye()
{
    int diffH = 0, diffV = 0, speed;

    if (theEye.mode == kStalking) {		// eye is about
        speed = (levelOn >> 4) + 1;
        if (speed > 3) {
            speed = 3;
        }
        
        if ((theEye.killed) || (theEye.entering)) {
            speed = 0;
        } else if ((thePlayer.mode != kFlying) && (thePlayer.mode != kWalking)) {
            diffH = theEye.dest.left - 296;
            diffV = theEye.dest.bottom - 128;
        } else {
            diffH = theEye.dest.left - thePlayer.dest.left;
            diffV = theEye.dest.bottom - thePlayer.dest.bottom;
        }
        
        if (diffH > 0) {
            if (diffH < speed) {
                theEye.dest.left -= diffH;
            } else {
                theEye.dest.left -= speed;
            }
            theEye.dest.right = theEye.dest.left + 48;
        } else if (diffH < 0) {
            if (-diffH < speed) {
                theEye.dest.left -= diffH;
            } else {
                theEye.dest.left += speed;
            }
            theEye.dest.right = theEye.dest.left + 48;
        }
        if (diffV > 0) {
            if (diffV < speed) {
                theEye.dest.bottom -= diffV;
            } else {
                theEye.dest.bottom -= speed;
            }
            theEye.dest.top = theEye.dest.bottom - 31;
        } else if (diffV < 0) {
            if (-diffV < speed) {
                theEye.dest.bottom -= diffV;
            } else {
                theEye.dest.bottom += speed;
            }
            theEye.dest.top = theEye.dest.bottom - 31;
        }
        
        theEye.frame++;
        
        if (theEye.srcNum != 0) {
            if (theEye.frame > 3) {		// eye-closing frame holds for 3 frames
                theEye.frame = 0;
                theEye.srcNum += theEye.opening;
                if (theEye.srcNum > 3) {
                    theEye.srcNum = 3;
                    theEye.opening = -1;
                    if (theEye.killed) {
                        initEye();
                    }
                } else if (theEye.srcNum <= 0) {
                    theEye.srcNum = 0;
                    theEye.opening = 1;
                    theEye.frame = 0;
                    theEye.entering = false;
                }
            }
        } else if (theEye.frame > 256) {
            theEye.srcNum = 1;
            theEye.opening = 1;
            theEye.frame = 0;
        }
        
        diffH = theEye.dest.left - thePlayer.dest.left;
        diffV = theEye.dest.bottom - thePlayer.dest.bottom;
        if (diffH < 0) {
            diffH = -diffH;
        }
        if (diffV < 0) {
            diffV = -diffV;
        }
        
        if ((diffH < 16) && (diffV < 16) && (!theEye.entering) &&
            (!theEye.killed)) {			// close enough to call it a kill
            if (theEye.srcNum == 0)	{		// if eye open, player is killed
                if (lightningCount == 0) {
                    doLightning(Point(thePlayer.dest.left + 24, thePlayer.dest.bottom - 24), 6);
                }
                thePlayer.mode = kFalling;
                if (thePlayer.facingRight) {
                    thePlayer.srcNum = 8;
                } else {
                    thePlayer.srcNum = 9;
                }
                thePlayer.dest.bottom = thePlayer.dest.top + 37;
                sounds.play(kBoom2Sound);
            } else { // wow, player killed the eye
                if (lightningCount == 0) {
                    doLightning(Point(theEye.dest.left + 24, theEye.dest.top + 16), 15);
                }
                addToScore(2000);
                sounds.play(kBonusSound);
                
                killOffEye();
            }
        }
    } else if (theEye.frame > 0) {
        theEye.frame--;
        if (theEye.frame == 0) {		// eye appears
            theEye.mode = kStalking;
            if (lightningCount == 0) {
                doLightning(Point(theEye.dest.left + 24, theEye.dest.top + 16), 6);
            }
            theEye.srcNum = 3;
            theEye.opening = 1;
            theEye.entering = true;
        }
    }
}

void GL::Game::drawEye() const
{
    if (theEye.mode == kStalking) {
        eyeImg.draw(theEye.dest, eyeRects[theEye.srcNum]);
    }
}

void GL::Game::openHelp()
{
    helpSrc.set(0, 0, 231, 0);
    helpDest = helpSrc;
    helpDest.offsetBy(204, 171);

    wallSrc.set(0, 0, 231, 199);
    wallSrc.offsetBy(204, 171);
    wallDest = wallSrc;

    helpPos = 0;
    helpState = kHelpOpening;
}

void GL::Game::handleHelp()
{
    if (helpState == kHelpOpening && helpPos >= 199) {
        helpState = kHelpOpen;
        return;
    }
    
    if (helpState == kHelpOpening) {
        int offsetBy = 3;
        helpSrc.bottom += offsetBy;
        helpDest.bottom += offsetBy;
        wallSrc.bottom -= offsetBy;
        wallDest.top += offsetBy;
        helpPos += offsetBy;
        
        if (helpPos > 199) {
            helpSrc.bottom = 199;
            wallSrc.bottom = 171;
            helpDest.bottom = wallDest.top = 370;
        }
    }
}

void GL::Game::drawHelp() const
{
    if (helpState != kHelpClosed) {
        helpImg.draw(helpDest, helpSrc);
        bgImg.draw(wallDest, wallSrc);
    }
}

void GL::Game::scrollHelp(short scrollDown)
{
    helpSrc.offsetBy(0, scrollDown);

    if (helpSrc.bottom > 398) {
        helpSrc.bottom = 398;
        helpSrc.top = helpSrc.bottom - 199;
    } else if (helpSrc.top < 0) {
        helpSrc.top = 0;
        helpSrc.bottom = helpSrc.top + 199;
    }
}
