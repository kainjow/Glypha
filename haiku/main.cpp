#include <stdio.h>
#include <Application.h>
#include <Window.h>

class App : public BApplication {
public:
	App(void);
};

App::App() :
	BApplication("application/glyphaiii")
{
	BRect frame(0, 0, 640, 460);
	BWindow *win = new BWindow(frame, "Glypha III",
		B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE | B_NOT_ZOOMABLE);
	win->CenterOnScreen();
	win->Show();
}

int main()
{
	App app;
	app.Run();
}
