//////////////////////////////////////////////////////////////////////
// Mouse.h
// Description:		Mouse input class
// By:				Ben Woodhouse 
// Date:			11/2002
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUSE_H__45026241_8B3C_49B3_86A9_4CE477EC3FF8__INCLUDED_)
#define AFX_MOUSE_H__45026241_8B3C_49B3_86A9_4CE477EC3FF8__INCLUDED_

class Mouse : public Singleton<Mouse>
{
	friend Singleton<Mouse>;
	Mouse();
public:
	virtual ~Mouse();

	void hideCursor();
	void showCursor();
	int getX(){return posx;}
	int getY(){return posy;}
	bool getLButtonClicked(){return lButtonClicked;}
	bool getLButtonDown(){return lButtonDown;}
	bool getLButtonReleased(){return lButtonReleased;}

	bool getRButtonClicked(){return rButtonClicked;}
	bool getRButtonDown(){return rButtonDown;}
	bool getRButtonReleased(){return rButtonReleased;}

	bool getMButtonClicked(){return mButtonClicked;}
	bool getMButtonDown(){return mButtonDown;}
	bool getMButtonReleased(){return mButtonReleased;}
	
	int getMouseWheelDir(){return mouseWheelDir;}


	void setPos(int x,int y);

	//called from WinProc
	void setLButtonDown()
	{
		lButtonDown=true;
		lButtonClicked=true;
	}
	void setLButtonUp()
	{
		lButtonDown=false;
		lButtonReleased=true;
	}
	void setRButtonDown()
	{
		rButtonDown=true;
		rButtonClicked=true;
	}
	void setRButtonUp()
	{
		rButtonDown=false;
		rButtonReleased=true;
	}
	void setMButtonUp()
	{
		mButtonDown=false;
		mButtonReleased=true;
	}
	void setMButtonDown()
	{
		mButtonDown=true;
		mButtonClicked=true;
	}
	void setMouseWheel(int dir)
	{
		mouseWheelDir=dir;
	}
	void update(HWND hWnd); //must be called after everything else in render loop

	//gets how much the mouse has moved since the previous frame
	float getXMove(){return (float)(oldPosx-posx);}
	float getYMove(){return (float)(oldPosy-posy);}


private:
	void calcWindowCoords(HWND hWnd);

	int posx,posy;
	int oldPosx, oldPosy;
	bool lButtonDown;
	bool lButtonClicked;
	bool lButtonReleased;
	bool rButtonDown;
	bool rButtonClicked;
	bool rButtonReleased;
	bool mButtonDown;
	bool mButtonClicked;
	bool mButtonReleased;

	int mouseWheelDir;
	int cursorType;
	bool cursorVisible;

	int windowLeft,windowTop;
};

#endif // !defined(AFX_MOUSE_H__45026241_8B3C_49B3_86A9_4CE477EC3FF8__INCLUDED_)
