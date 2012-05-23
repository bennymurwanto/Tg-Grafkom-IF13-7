//////////////////////////////////////////////////////////////////////
// winApp.cpp
// Description:		This class contains the basic windows framework and 
//					the program starting point, as well as the high level
//					logic for the program. 
//
// By:				Ben Woodhouse 
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "winApp.h"
#include <time.h>



bool programActive=true;
double deltaT=1;

int WINAPI WinMain(	 HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR     lpCmdLine,
					int       nCmdShow)
{
	App &app=App::inst();
	app.start(hInstance,hPrevInstance,lpCmdLine,nCmdShow,640,480);
	return app.shutdown();
}

LRESULT CALLBACK WndProc(HWND hWndL,UINT msg,WPARAM wParam,LPARAM lParam)
{
	if (App::inited()) return App::inst().wndProc(hWndL, msg, wParam, lParam);
	else return DefWindowProc(hWndL,msg,wParam,lParam);
}

////////////////////////////////////////////////////////////////
// App class
////////////////////////////////////////////////////////////////
App::App()
{
	mainLog=new Log("log.txt");
	renderer.init(&screen);
	camera.init(&renderer);
	mainTimer.start();
	init(640,480);
	started=false;
	frameCount=0;
	t2=t1=0;
	deltaT=1;
}
App::~App()
{
	delete mainLog;
}

LRESULT App::wndProc(HWND hWndL, UINT msg,WPARAM wParam,LPARAM lParam)
{
	int width;
	int height;
	short wheelDir;
	switch (msg)									
	{
		case WM_ACTIVATE:	
			if (!HIWORD(wParam))					
			{
				programActive=true;						
			}
			else
			{
				programActive=false;
			}
			break;

		case WM_SIZE:
			width=LOWORD(lParam);
			height=HIWORD(lParam);
            screen.resize(width,height);
			camera.setPerspective(60.0f,(float)width/(float)height,30.0f,400.0f);
			break;


		case WM_MOUSEWHEEL:
			wheelDir = HIWORD(wParam);    // wheel rotation

			if (wheelDir>0)
			{
				Mouse::inst().setMouseWheel(1);
			}
			else
			{
				Mouse::inst().setMouseWheel(-1);
			}
			break;

		case WM_LBUTTONDOWN:
			Mouse::inst().setLButtonDown();
			break;

		case WM_LBUTTONUP:
			Mouse::inst().setLButtonUp();
			break;

		case WM_RBUTTONDOWN:
			Mouse::inst().setRButtonDown();
			break;

		case WM_RBUTTONUP:
			Mouse::inst().setRButtonUp();
			break;

		case WM_MBUTTONDOWN:
			Mouse::inst().setMButtonDown();
			break;

		case WM_MBUTTONUP:
			Mouse::inst().setMButtonUp();
			break;

		case WM_CHAR:
			keyBuffer.push_back((char)wParam);
			break;

		case WM_KEYDOWN:							
			Keys::inst().set((int)wParam);					
			break;

		case WM_KEYUP:								
			Keys::inst().unset((int)wParam);					
			break;

		case WM_CLOSE:								
			//post a quit message to terminate the program
			PostQuitMessage(0);	
			break;

		default:
			//pass unhandled messages to event handlers
			return DefWindowProc(hWndL,msg,wParam,lParam);
	}
	return 0L;
}


void App::init(int width, int height)
{
	quit=false;
	//create the window
	quit=!screen.init(width,height);
	srand( (unsigned)time( NULL ) ); 
}


void App::start(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow,
					int width, int height)
{
	this->hInstance=hInstance;
	this->hPrevInstance=hPrevInstance;
	this->lpCmdLine=lpCmdLine;
	this->nCmdShow=nCmdShow;

	screen.resize(width,height);
	started=true;
	programActive=true;
	mainLoop();
}



void App::mainLoop()
{
	//get references to singletons
	Mouse & mouse=Mouse::inst();
	Keys & keys=Keys::inst();
	camera.setPerspective(60.0f,4.0f/3.0f,30.0f,400.0f);
//		PluginManager &pluginManager=PluginManager::inst();

	while (!quit)
	{
		//windows message pump
		if (PeekMessage(&msg,GLDisplay::inst().getHWnd(),0,0,PM_REMOVE))
		{
//				if (msg==WM_MOUSEFIRST)
			if (msg.message==WM_QUIT)
			{
				quit=true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (programActive)
			{
				t2=mainTimer.getElapsed();
				deltaT=t2-t1;
				t1=t2;
				input();
				render();
				mouse.update(screen.getHWnd());
				keys.setOldKeys();			
				keyBuffer.clear();
				frameCount++;
			}
		}
	}
}

void App::input()
{
	static int oldPosx=screen.getWidth()/2;
	static int oldPosy=screen.getHeight()/2;
	Keys& keys=Keys::inst();
	Mouse& mouse=Mouse::inst();
	static int shininess=2;
	static uint renderType=RF_CONTROLMESH | RF_SHADED;

	cameraInput();
	if (keys.justPressed('H'))
	{
		patch.increaseSplatRes();
	}
	if (keys.justPressed('B'))
	{
		patch.decreaseSplatRes();
	}
	if (keys.justPressed('K'))
	{
		patch.increaseRes();
	}
	if (keys.justPressed('M'))
	{
		patch.decreaseRes();
	}
	if (keys.justPressed('J'))
	{
		shininess++;
		shininess=min(shininess,4);
		screen.getShader().setShininess(shininess);
	}
	if (keys.justPressed('N'))
	{
		shininess--;
		shininess=max(shininess,0);
		screen.getShader().setShininess(shininess);
	}
	if (keys.justPressed('E'))
	{
		renderType^=RF_CONTROLMESH;
		patch.setRenderType(renderType);
	}
	if (keys.justPressed('W'))
	{
		renderType^=RF_WIREFRAME;
		patch.setRenderType(renderType);
	}
	if (keys.justPressed('R'))
	{
		renderType^=RF_SHADED;
		patch.setRenderType(renderType);
	}
	if (keys.justPressed('S'))
	{
		patch.toggleSplatRendering();
	}
	if (keys.justPressed('L'))
	{
		renderer.setLightPos(-camera.getCameraNorm());
	}

	static bool dragging=false;
	if (renderType || RF_CONTROLMESH)
	{
		if (mouse.getLButtonClicked())
		{
			Vector3 cpWin;
			if (patch.findControlPoint(mouse.getX(),mouse.getY(),dragPoint,dragi,dragj))
			{
				oldPosx=mouse.getX();
				oldPosy=mouse.getY(); 
				mouse.setPos(screen.getWidth()/2,screen.getHeight()/2); 
				mouse.hideCursor();
				dragging=true;
			}
		}
		if (dragging)
		{
			if (mouse.getLButtonReleased())
			{
				dragging=false;
				mouse.setPos(oldPosx,oldPosy);
				mouse.showCursor();
			}
			if (mouse.getLButtonDown())
			{
				int hwidth=screen.getWidth()/2;
				int hheight=screen.getHeight()/2;
				int movex=mouse.getX()-hwidth;
				int movey=mouse.getY()-hheight;
				mouse.setPos(hwidth,hheight); 		
				Vector3 move=(camera.getUpVector()*(float)-movey*0.2f)+
							 (camera.getRightVector()*(float)-movex*0.2f);

				dragPoint+=move;
				patch.setControlPoint(dragi,dragj,dragPoint);
			}
		}
	}

	if (keys.justPressed(VK_SPACE)) //speed test (results written to log file)
	{
		mainLog->write("Testing speed");
		Timer timer;
		timer.start();
		for (int a=0;a<10;a++)
		{
			screen.clear();
			patch.render(renderer);
		}
		float t1=(float)timer.getElapsed();
		for (int a=0;a<10;a++)
		{
			screen.swapBuffers();
		}
		float t2=(float)timer.getElapsed()-t1;
		mainLog->write("Time to render 10 quads: %f",t1);
		mainLog->write("Time to swap the buffers 10 times: %f",t2);
	}
}

void App::render()
{
	screen.clear();
	patch.render(renderer);
	screen.swapBuffers();
}

void App::cameraInput()
{
	int movex,movey;
	static int oldPosx=screen.getWidth()/2;
	static int oldPosy=screen.getHeight()/2;
	static float desiredRot[2]={0,0};
	static float desiredRadius=100;

	Mouse &mouse=Mouse::inst();
	Keys &keys=Keys::inst();

	if (mouse.getRButtonClicked())
	{
		oldPosx=mouse.getX();
		oldPosy=mouse.getY(); 
		mouse.setPos(screen.getWidth()/2,screen.getHeight()/2); 
		mouse.hideCursor();
	}
	//camera movement:
	if (mouse.getRButtonDown())
	{
		int hwidth=screen.getWidth()/2;
		int hheight=screen.getHeight()/2;
		movex=mouse.getX()-hwidth;
		movey=mouse.getY()-hheight;
		mouse.setPos(hwidth,hheight); 		
		Vector3 n((float)movey,(float)movex,0);
		float l=n.length();
		if (l>0.001)
		{
			n.normalise();
			camera.rotate(l*0.5f,-n.x,n.y,0);
		}
	}
	if (mouse.getRButtonReleased())
	{
		mouse.setPos(oldPosx,oldPosy);
		mouse.showCursor();
	}	
	//camera radius
	if (mouse.getMouseWheelDir()==1) camera.addRadius(-5);
	else if (mouse.getMouseWheelDir()==-1) camera.addRadius(5);
	if (keys.down('A'))	camera.addRadius((float)(-100*deltaT));
	if (keys.down('Z'))	camera.addRadius((float)(100*deltaT));
	if (keys.down('C'))	camera.rotate((float)(-deltaT*80),0,0,1);
	if (keys.down('V'))	camera.rotate((float)(deltaT*80),0,0,1);
}


int App::shutdown()
{
//		PluginManager::inst().shutdownPlugins();
//		TextureManager::inst().shutdown();
	screen.shutdown();
	
	return (int)msg.wParam;
}
