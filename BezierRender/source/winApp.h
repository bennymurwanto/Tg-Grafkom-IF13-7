//////////////////////////////////////////////////////////////////////
// winApp.h
// Description:		This class contains the basic windows framework and 
//					the program starting point, as well as the high level
//					logic for the program. 
//
// By:				Ben Woodhouse 
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#ifndef WINAPP_H
#define WINAPP_H

#include "bezierPatch.h"
#include "renderer.h"
#include "data structs\Singleton.h"
#include "camera.h"

int WINAPI WinMain(	HINSTANCE hInstance,
						HINSTANCE hPrevInstance,
						LPSTR     lpCmdLine,
						int       nCmdShow);

extern double deltaT;
extern bool programActive;

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif


LRESULT CALLBACK WndProc(HWND hWndL,UINT msg,WPARAM wParam,LPARAM lParam);


class App : public Singleton<App>
{
	friend Singleton<App>;
	App();

public:
	virtual ~App();
	void start(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow,
				int width, int height);

	LRESULT wndProc(HWND hWndL, UINT msg, WPARAM wParam, LPARAM lParam);

	void quitGame()										{quit=true;}
	bool isStarted()									{return started;}
	int shutdown();
	long getFrameCount()								{return frameCount;}

private:
	void mainLoop();

	void input();
	void render();

	void cameraInput();

	void init(int width, int height);

	BezierPatch patch;
	bool started;
	bool tryEventHandlers(UINT msg, WPARAM wParam, LPARAM lParam);

	bool quit;
	MSG msg;

	HINSTANCE hInstance, hPrevInstance;
	LPSTR lpCmdLine;
	int nCmdShow;
 
	Log* mainLog;
	long frameCount;
	Renderer renderer;
	Screen screen;
	Camera camera;

	Vector3 dragPoint;
	int dragi,dragj;
	
	double t1,t2;
	Timer mainTimer;
};
#endif
