//////////////////////////////////////////////////////////////////////
// Mouse.cpp
// Description:		Mouse input class
// By:				Ben Woodhouse 
// Date:			11/2002
//////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"
#include "Mouse.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Mouse::Mouse()
{
	posx=0;
	posy=0;
	oldPosx=0;
	oldPosy=0;
	cursorVisible=true;
	lButtonClicked=false;
	rButtonClicked=false;
	lButtonDown=false;
	rButtonDown=false;
	lButtonReleased=false;
	rButtonReleased=false;
	mButtonDown=false;
	mButtonReleased=false;
	mButtonClicked=false;

	mouseWheelDir=0;
}

Mouse::~Mouse()
{

}

void Mouse::showCursor()
{
	ShowCursor(TRUE);
	cursorVisible=true;
}

void Mouse::hideCursor()
{
	ShowCursor(FALSE);
	cursorVisible=false;
}

void Mouse::update(HWND hWnd)
{
	lButtonClicked=false;
	rButtonClicked=false;

	lButtonReleased=false;
	rButtonReleased=false;

	mButtonReleased=false;
	mButtonClicked=false;

	mouseWheelDir=0;

	POINT point;

	if (hWnd!=NULL) calcWindowCoords(hWnd);
	GetCursorPos(&point);

	oldPosx=posx;
	oldPosy=posy;
	posx=point.x-windowLeft;
	posy=(GLDisplay::inst().getHeight()-1)-(point.y-windowTop);
}

void Mouse::setPos(int x, int y)
{
	SetCursorPos(x+windowLeft,(GLDisplay::inst().getHeight()-1+windowTop)-y);
	posx=x;
	posy=y;
}


void Mouse::calcWindowCoords(HWND hWnd)
{
	RECT windRect;
	GetWindowRect(hWnd,&windRect);

	//RECT clientRect;
	//GetClientRect(hWnd,&clientRect);
	GLDisplay &glDisplay=GLDisplay::inst();

	int windWidth=windRect.right-windRect.left+1;
	int windHeight=windRect.bottom-windRect.top+1;

	int clientWidth=glDisplay.getWidth();
	int clientHeight=glDisplay.getHeight();
	int padLeft=(windWidth-clientWidth)/2;
	int padTop=(windHeight-clientHeight)-padLeft;

	windowLeft=windRect.left+padLeft;
	windowTop=windRect.top+padTop;

	//point.x=point.x-clientLeft;
	//point.y=point.y-clientTop;
}