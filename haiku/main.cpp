#include <Alert.h>
#include <Application.h>
#include <GLView.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Window.h>
#include "GLGame.h"

namespace {
	const int kMsgAnimate =   'glya';
	const int kMsgNewGame =   'glyn';
	const int kMsgPauseGame = 'glyp';
	const int kMsgEndGame =   'glye';
	const int kMsgHelp =      'glyh';
	const int kMsgAbout =     'glyb';
	const int kMsgScores =    'glys';
	const int kMsgReset =     'glyt';
}

class GameGLView;

class MainWindow : public BWindow {
public:
	MainWindow();
	
	void gameCallback(GL::Game::Event event);
	
protected:
	virtual void MessageReceived(BMessage *msg);

private:
	GameGLView *glview_;
	BMenuItem *newGameItem_;
	BMenuItem *pauseGameItem_;
	BMenuItem *endGameItem_;
	BMenuItem *helpItem_;
	BMenuItem *aboutItem_;
	BMenuItem *scoresItem_;
	BMenuItem *resetItem_;
	
	void ResetHighScores();
};

class GameGLView : public BGLView {
public:
	GameGLView(BRect frame)
		: BGLView(frame, "", B_FOLLOW_ALL_SIDES, 0, BGL_RGB | BGL_DOUBLE)
		, game_(new GL::Game(callback, NULL, this))
	{
	}
	
	static void callback(GL::Game::Event event, void *context) {
		GameGLView *view = (GameGLView*)context;
		MainWindow *win = (MainWindow*)view->Window();
		win->gameCallback(event);
	}
	
	virtual void AttachedToWindow(void) {
		BGLView::AttachedToWindow();
		gReshape((unsigned int)Frame().Width(), (unsigned int)Frame().Height());
		Render();
	}
	
	void gReshape(int width, int height) {
		LockGL();
		game_->renderer()->resize(width, height);
		UnlockGL();
	}
	
	virtual void Render() {
		LockGL();
		game_->run();
		SwapBuffers(true);
		UnlockGL();
	}
	
	virtual void FrameResized(float width, float height) {
		BGLView::FrameResized(width, height);
		Render();
	}
	
	virtual void MessageReceived(BMessage *msg) {
		switch (msg->what) {
		    case kMsgAnimate:
		        Render();
		        break;
			default:
				BGLView::MessageReceived(msg);
		}
	}
	
	virtual void MouseDown(BPoint point) {
		game_->handleMouseDownEvent(GL::Point((unsigned)point.x, (unsigned)point.y));
	}
	
	virtual void KeyDown(const char *bytes, int32 numBytes) {
		HandleKey(bytes, numBytes, true);
		BGLView::KeyDown(bytes, numBytes);
	}

	virtual void KeyUp(const char *bytes, int32 numBytes) {
		HandleKey(bytes, numBytes, false);
		BGLView::KeyUp(bytes, numBytes);
	}
	
	void HandleKey(const char *bytes, int32 numBytes, bool isDown) {
		for (int32 i = 0; i < numBytes; ++i) {
			GL::Game::Key key = GL::Game::KeyNone;
			switch (bytes[i]) {
				case B_SPACE:
					key = GL::Game::KeySpacebar;
					break;
				case B_LEFT_ARROW:
					key = GL::Game::KeyLeftArrow;
					break;
				case B_RIGHT_ARROW:
					key = GL::Game::KeyRightArrow;
					break;
				case B_DOWN_ARROW:
					key = GL::Game::KeyDownArrow;
					break;
				case B_UP_ARROW:
					key = GL::Game::KeyUpArrow;
					break;
				case B_PAGE_UP:
					key = GL::Game::KeyPageUp;
					break;
				case B_PAGE_DOWN:
					key = GL::Game::KeyPageDown;
					break;
			}
			if (isDown) {	
				game_->handleKeyDownEvent(key);
			} else {
				game_->handleKeyUpEvent(key);
			}
		}
	}
	
	void NewGame() {
		game_->newGame();
	}
	
	bool PauseGame() {
		game_->pauseResumeGame();
		return game_->paused();
	}
	
	void EndGame() {
		game_->endGame();
	}
	
	void ShowHelp() {
		game_->showHelp();
	}
	
	void ShowAbout() {
		game_->showAbout();
	}
	
	void ShowHighScores() {
		game_->showHighScores();
	}
	
	void ResetHighScores() {
		game_->resetHighScores();
	}
	
private:
	GL::Game *game_;
};

MainWindow::MainWindow()
		: BWindow(BRect(0, 0, 640, 460), GL_GAME_NAME, B_TITLED_WINDOW,
			B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE)
	{
		BMenuBar *menuBar = new BMenuBar(BRect(0,0,0,0), NULL);
		BMenu *gameMenu = new BMenu("Game");
		BMenu *optionsMenu = new BMenu("Options");
		BMenuItem *item;
		newGameItem_ = new BMenuItem("New Game", new BMessage(kMsgNewGame), 'N');
		gameMenu->AddItem(newGameItem_);
		pauseGameItem_ = new BMenuItem("Pause Game", new BMessage(kMsgPauseGame), 'P');
		pauseGameItem_->SetEnabled(false);
		gameMenu->AddItem(pauseGameItem_);
		endGameItem_ = new BMenuItem("End Game", new BMessage(kMsgEndGame), 'E');
		endGameItem_->SetEnabled(false);
		gameMenu->AddItem(endGameItem_);
		gameMenu->AddSeparatorItem();
		item = new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q');
		gameMenu->AddItem(item);
		helpItem_ = new BMenuItem("Help", new BMessage(kMsgHelp), 'H');
		optionsMenu->AddItem(helpItem_);
		optionsMenu->AddSeparatorItem();
		scoresItem_ = new BMenuItem("High Scores", new BMessage(kMsgScores), 'S');
		optionsMenu->AddItem(scoresItem_);
		resetItem_ = new BMenuItem("Reset Scores" B_UTF8_ELLIPSIS, new BMessage(kMsgReset));
		optionsMenu->AddItem(resetItem_);
		optionsMenu->AddSeparatorItem();
		aboutItem_ = new BMenuItem("About " GL_GAME_NAME B_UTF8_ELLIPSIS, new BMessage(kMsgAbout), 'H');
		optionsMenu->AddItem(aboutItem_);
		menuBar->AddItem(gameMenu);
		menuBar->AddItem(optionsMenu);
		AddChild(menuBar);
	
		ResizeBy(0, menuBar->Frame().Height());
		
		glview_ = new GameGLView(BRect(0, menuBar->Frame().Height() + 1, 640, 480));
		AddChild(glview_);
		glview_->MakeFocus(); // make focus so it receives key events	
		
		BMessageRunner *runner = new BMessageRunner(glview_, new BMessage(kMsgAnimate), 1000/30);
		if (runner->InitCheck() != B_OK) {
			printf("InitCheck failed\n");
		}
	}
	
	void MainWindow::gameCallback(GL::Game::Event event) {
		switch (event) {
			case GL::Game::EventStarted:
				newGameItem_->SetEnabled(false);
				pauseGameItem_->SetEnabled(true);
				endGameItem_->SetEnabled(true);
				helpItem_->SetEnabled(false);
				scoresItem_->SetEnabled(false);
				resetItem_->SetEnabled(false);
				aboutItem_->SetEnabled(false);
				break;
			case GL::Game::EventEnded:
				newGameItem_->SetEnabled(true);
				pauseGameItem_->SetEnabled(false);
				endGameItem_->SetEnabled(false);
				helpItem_->SetEnabled(true);
				scoresItem_->SetEnabled(true);
				resetItem_->SetEnabled(true);
				aboutItem_->SetEnabled(true);
				break;
		}
	}
	
	void MainWindow::MessageReceived(BMessage *msg) {
		switch (msg->what) {
		    case kMsgNewGame:
			    glview_->NewGame();
		    	break;
		    case kMsgPauseGame: {
		    	uint32 modifiers;
		    	pauseGameItem_->Shortcut(&modifiers);
			    if (glview_->PauseGame()) {
			    	pauseGameItem_->SetShortcut('R', modifiers);
			    	pauseGameItem_->SetLabel("Resume Game");
			    } else {
			    	pauseGameItem_->SetShortcut('P', modifiers);
			    	pauseGameItem_->SetLabel("Pause Game");
			    }
		    	break;
		    }
		    case kMsgEndGame:
			    glview_->EndGame();
		    	break;
		    case kMsgHelp:
		    	glview_->ShowHelp();
		    	break;
		    case kMsgScores:
		    	glview_->ShowHighScores();
		    	break;
		    case kMsgReset:
		    	ResetHighScores();
			    break;
		    case kMsgAbout:
		    	glview_->ShowAbout();
		    	break;
			default:
				BWindow::MessageReceived(msg);
				break;
		}
	}
	
	void MainWindow::ResetHighScores() {
		BAlert *alert = new BAlert(NULL,
			"Are you sure you want to reset " GL_GAME_NAME "'s scores?",
			"Yes", "No", NULL, B_WIDTH_AS_USUAL, B_WARNING_ALERT);
		int32 but = alert->Go();
		delete alert;
		if (but == 0) {
			glview_->ResetHighScores();
		}
	}

class App : public BApplication {
public:
	App()
		: BApplication("application/x-vnd.kainjow-glyphaiii")
	{
		MainWindow *win = new MainWindow;
		win->CenterOnScreen();
		win->Show();
	}
};

int main()
{
	App *app = new App();
	app->Run();
	delete app;
	return 0;
}
