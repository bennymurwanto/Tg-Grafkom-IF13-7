//////////////////////////////////////////////////////////////////////
// GLDisplay.h
// Description:		Basic OpenGL window class
// By:				Ben Woodhouse 
// Date:			11/2002
//////////////////////////////////////////////////////////////////////

#ifndef GL_DISPLAY_H
#define GL_DISPLAY_H

class GLDisplay : public Singleton<GLDisplay>
{
	friend Singleton<GLDisplay>;
	GLDisplay();
public:
	//creates an openGL window

	bool createMainWindow
						(const char *title, 
						int width, 
						int height, 
						int colourBits, 
						int zBits, 
						bool lFullscreen); 

	bool destroyMainWindow();
	void resize(int width, int height);
	void initializeGL();
	int getWidth(){return width;}
	int getHeight(){return height;}

	bool resizeWindow(int width, int height, int colourBits, int zBits, bool fullscreen);

	//changes the screen resolution and bitdepth
	bool resizeScreen(int width, int height, int bits);
	void resizeGL(int width,int height);
	void swapBuffers(){SwapBuffers(hDC);}
	HWND getHWnd(){return hWnd;}
	bool getFullscreen(){return fullscreen;}
	void getWindowDimensions(int width, int height, int &width_out, int &height_out);

	void setFullscreen(bool fullscreen_in);
	HDC getDC(){return hDC;}

private:
	int width,height;
	HGLRC hRC;			//GL rendering context
	HDC hDC;			//device context
	HWND hWnd;			//window handle
//		HINSTANCE hInst;	//instance handle
	bool fullscreen;	//fullscreen flag
	bool windowExists;	//true if a main window exists
	char title[256];    //the window title

	int windowOffsetx;
	int windowOffsety;
	DWORD winStyleFlags;

	int colourBits;
	int zBits;


};

#endif 
