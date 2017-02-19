#include <stdio.h>
#include <Application.h>
#include <Window.h>
#include <Menu.h>
#include <MenuItem.h>
#include <MenuBar.h>
#include <GLView.h>
#include "GLGame.h"

namespace {
	const int kMsgAnimate = 'glya';
	const int kMsgNewGame = 'glyn';
}

class GameGLView : public BGLView {
public:
	GameGLView(BRect frame)
		: BGLView(frame, "", B_FOLLOW_ALL_SIDES, 0, BGL_RGB | BGL_DOUBLE)
		, game_(new GL::Game(NULL, NULL, NULL))
	{
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
	
	void NewGame() {
		game_->newGame();
	}
	
private:
	GL::Game *game_;
};

class MainWindow : public BWindow {
public:
	MainWindow()
		: BWindow(BRect(0, 0, 640, 460), GL_GAME_NAME, B_TITLED_WINDOW,
			B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE)
	{
		BMenuBar *menuBar = new BMenuBar(BRect(0,0,0,0), NULL);
		BMenu *menu = new BMenu("File");
		BMenuItem *item;
		item = new BMenuItem("New Game", new BMessage(kMsgNewGame), 'N');
		menu->AddItem(item);
		menu->AddSeparatorItem();
		item = new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q');
		menu->AddItem(item);
		menuBar->AddItem(menu);
		AddChild(menuBar);
	
		ResizeBy(0, menuBar->Frame().Height());
		
		glview_ = new GameGLView(BRect(0, menuBar->Frame().Height() + 1, 640, 480));
		AddChild(glview_);
		
		BMessageRunner *runner = new BMessageRunner(glview_, new BMessage(kMsgAnimate), 1000/30);
		if (runner->InitCheck() != B_OK) {
			printf("InitCheck failed\n");
		}
	}
	
protected:
	virtual void MessageReceived(BMessage *msg) {
		switch (msg->what) {
		    case kMsgNewGame:
			    glview_->NewGame();
		    	break;
			default:
				BWindow::MessageReceived(msg);
				break;
		}
	}

private:
	GameGLView *glview_;	
};

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
