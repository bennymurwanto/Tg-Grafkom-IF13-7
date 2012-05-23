//////////////////////////////////////////////////////////////////////
// GLDisplay.cpp
// Description:		Basic OpenGL window class
// By:				Ben Woodhouse 
// Date:			11/2002
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GLDisplay.h"
#include <stdio.h>

GLDisplay::GLDisplay()
{
	hRC=NULL;
	hDC=NULL;
	hWnd=NULL;
	colourBits=0;
	zBits=0;
	fullscreen=false;
	windowExists=false;
}
void GLDisplay::initializeGL()
{
}

void GLDisplay::resizeGL(int width,int height)
{
	double halfwidth=(width-1.0)/2.0;
	double halfheight=(height-1.0)/2.0;

	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-halfwidth,halfwidth,-halfheight,halfheight,0,2.0);
	glMatrixMode(GL_MODELVIEW);
	this->width=width;
	this->height=height;
}

bool GLDisplay::destroyMainWindow()
{
	
	if (fullscreen)
		ChangeDisplaySettings(NULL,0); //change res back

	if (hRC)											
	{
		wglMakeCurrent(NULL,NULL); //release the RC and DC

		wglDeleteContext(hRC); //delete the RC
		hRC=NULL;
	}

	if (hDC)					
	{
		ReleaseDC(hWnd,hDC); //release the DC
		hDC=NULL;
	}

	if (hWnd)
	{
		DestroyWindow(hWnd); //destroy the window
		hWnd=NULL;										
	}

	UnregisterClass(title,NULL); //unregister the window class

	windowExists=false;
	return true;
}

bool GLDisplay::createMainWindow(	const char *title, 
									int width, 
									int height, 
									int colourBits, 
									int zBits, 
									bool lFullscreen)
{
	sprintf(this->title,"%s",title); //store the title

	this->width=width;
	this->height=height;
	//this->hInst=hInstance;
	if (windowExists)
	{
		MessageBox(NULL,"The main window already exists","Error",MB_ICONERROR);
		return false;
	}
	WNDCLASS wc;
	
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC ;
	wc.lpfnWndProc = &WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
//	wc.hInstance = hInst;
	wc.hInstance = NULL;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH ); 
	wc.lpszMenuName = NULL;
	wc.lpszClassName = title;

	//register the class

	if(RegisterClass(&wc) == 0)
	{
		//or not as the case may be
		return false;
	}
	SetCursor(wc.hCursor); 

	// Create the main application window	
	if (lFullscreen) 
	{
		//attempt to change the screen res
		if (!resizeScreen(width,height,colourBits))
		{
			MessageBox(NULL,"The screen mode was not supported by your display\nWindowed mode will be used instead","Error",MB_ICONERROR);
			lFullscreen=false;
		}
		else
		{
			lFullscreen=true;
		}
	}

	this->fullscreen=lFullscreen;

	if (lFullscreen)
		winStyleFlags=WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
	else 
		winStyleFlags=WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;
	
	int winWidth=width;
	int winHeight=height;
	if (!lFullscreen)
		getWindowDimensions(width,height,winWidth,winHeight);

	hWnd = CreateWindow(
		title,
		title,
		winStyleFlags,
		0, 0,
		winWidth, winHeight,  
		NULL,
		NULL,
		NULL,
		NULL);


	//quit if createwindow fails
	if(hWnd == NULL) 
	{
		MessageBox(NULL,"Can't create the window","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	
	//setup the pixelformat for the opengl window
	static	PIXELFORMATDESCRIPTOR pfd=				
	{
		sizeof(PIXELFORMATDESCRIPTOR),				
		1,											
		PFD_DRAW_TO_WINDOW |						
		PFD_SUPPORT_OPENGL |						
		PFD_DOUBLEBUFFER,							//double buffer
		PFD_TYPE_RGBA,								//rgba
		colourBits,										
		0, 0, 0, 0, 0, 0,							
		0,											
		0,											
		0,											
		0, 0, 0, 0,									
		zBits,										
		0,											//stencil bits
		0,											
		PFD_MAIN_PLANE,								//main draw layer
		0,											
		0, 0, 0										
	};

	
	hDC=GetDC(hWnd);
	if (!hDC)
	{
		MessageBox(NULL,"Can't get a device context","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;							
	}

	GLuint pixelFormat=ChoosePixelFormat(hDC,&pfd); //choose a pixel format
	if(!pixelFormat)		
	{
		MessageBox(NULL,"Error selecting a pixel format","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								
	}

	if(!SetPixelFormat(hDC,pixelFormat,&pfd))		// set the pixel format
	{
		MessageBox(NULL,"Can't set the pixelformat","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								
	}

	hRC=wglCreateContext(hDC); //get a rendering context
	if (!hRC)				
	{
		int er=GetLastError();
		MessageBox(NULL,"Can't get a rendering context","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								
	}

	if(!wglMakeCurrent(hDC,hRC)) //make the rc current				
	{
		MessageBox(NULL,"Can't activate the RC","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								
	}
	

	//show the window and give it the input focus
	ShowWindow(hWnd,SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);									
	resizeGL(width, height); //sets up perspective matrix for window size
	initializeGL();			 //sets up GL states

	windowExists=true;
	return true;			 //success					
}

void GLDisplay::setFullscreen(bool fullscreen_in)
{
	if (fullscreen_in!=fullscreen) 
	{
		int style=GetWindowLong(hWnd, GWL_STYLE);

		//change to fullscreen
		if (!fullscreen && fullscreen_in)
		{
			style &= (~WS_OVERLAPPEDWINDOW);
			resizeScreen(width,height,colourBits);	
		}
		//change to windowed mode
		else 
		{
			style |= WS_OVERLAPPEDWINDOW;
			ChangeDisplaySettings(NULL,0);
		}
		SetWindowLong(hWnd,GWL_STYLE,style);
	}
	this->fullscreen=fullscreen_in;

}

void GLDisplay::getWindowDimensions(int width, int height, int &width_out, int &height_out)
{
	RECT rect;
	rect.left=0;
	rect.top=0;
	rect.bottom=height-1;
	rect.right=width-1;

	//adjust the window size to make room for the full client area
	if (AdjustWindowRect(&rect,winStyleFlags,FALSE))
	{
		width_out=rect.right-rect.left+1;
		height_out=rect.bottom-rect.top+1;
	}
}
bool GLDisplay::resizeWindow(int width, int height, int colourBits, int zBits, bool fullscreen)
{
	this->zBits=zBits;
	this->width=width;
	this->height=height;
	this->colourBits=colourBits;

	setFullscreen(fullscreen);

	int winWidth=width,winHeight=height;
	if (!fullscreen)
		getWindowDimensions(width,height,winWidth,winHeight);

	BOOL s=
		SetWindowPos(
			hWnd,             // handle to window
			HWND_TOP,  // placement-order handle
			0,                 // horizontal position
			0,                 // vertical position
			winWidth,                // width
			winHeight,                // height
			0// window-positioning flags
		);

	if (!s) return false;

	return true;

}


bool GLDisplay::resizeScreen(int width, int height, int bits)
{
	//setup a devmode for the desired resolution
	DEVMODE screenDM;	//screen devmode							
	
	ZeroMemory(&screenDM,sizeof(screenDM));	//clear the devmode struct

	//set the width and height, and bits per pixel
	screenDM.dmSize=sizeof(screenDM);	//size of the struct
	screenDM.dmPelsWidth	= width;				
	screenDM.dmPelsHeight	= height;				
	screenDM.dmBitsPerPel	= bits;					
	screenDM.dmFields=DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	//change the settings
	if (ChangeDisplaySettings(&screenDM,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	{
		return false; //fail
	} 
	return true; //succeed
}
