#include <stdio.h>
#include <Application.h>
#include <Window.h>
#include <Menu.h>
#include <MenuItem.h>
#include <MenuBar.h>
#include <GLView.h>
#include "GLGame.h"

class App : public BApplication {
public:
	App();
};

class GameGLView : public BGLView {
public:
	GameGLView(BRect frame)
		: BGLView(frame, "", B_FOLLOW_ALL_SIDES, 0, BGL_RGB | BGL_DOUBLE)
		, game_(NULL)
	{
	}
	
	virtual void AttachedToWindow(void) {
		BGLView::AttachedToWindow();
		game_ = new GLGame();
		gReshape(Frame().Width(), Frame().Height());
		Render();
	}
	
	virtual void gReshape(int width, int height) {
		LockGL();
		game_->renderer()->resize(width, height);
		UnlockGL();
	}
	
	virtual void Render() {
		printf("Render\n");
		LockGL();
		game_->draw();
		SwapBuffers(true);
		UnlockGL();
	}
	
	virtual void FrameResized(float width, float height) {
		BGLView::FrameResized(width, height);
		printf("Resized\n");
		Render();
	}
		
private:
	GLGame *game_;
};

App::App() :
	BApplication("application/glyphaiii")
{
	BRect frame(0, 0, 640, 460);
	BWindow *win = new BWindow(frame, "Glypha III",
		B_TITLED_WINDOW, B_NOT_ZOOMABLE /*| B_NOT_RESIZABLE*/);
	
	BMenuBar *menuBar = new BMenuBar(BRect(0,0,0,0), NULL);
	BMenu *menu = new BMenu("File");
	BMenuItem *item = new BMenuItem("Quit", NULL, 'Q');
	menu->AddItem(item);
	menuBar->AddItem(menu);
	win->AddChild(menuBar);

	win->ResizeBy(0, menuBar->Frame().Height());
	
	GameGLView *glview = new GameGLView(BRect(0, menuBar->Frame().Height() + 1, 640, 480));
	win->AddChild(glview);
	
	win->CenterOnScreen();
	win->Show();
}

int main()
{
	App app;
	app.Run();
}
