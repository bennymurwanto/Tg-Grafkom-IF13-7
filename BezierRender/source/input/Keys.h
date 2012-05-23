//////////////////////////////////////////////////////////////////////
// Keys.h
// Description:		Keyboard user input class
// By:				Ben Woodhouse 
// Date:			11/2002
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYS_H__AB70C6B7_EBD0_4BE7_8119_D7BC61128CF7__INCLUDED_)
#define AFX_KEYS_H__AB70C6B7_EBD0_4BE7_8119_D7BC61128CF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern std::vector<char> keyBuffer;

class Keys : public Singleton<Keys>
{
	friend Singleton<Keys>;
	Keys();
public:
	virtual ~Keys();

	bool pressedWRepeat(int keycode) 
	{
		return keysRepeat[keycode];
	}
	//returns true if the key has been pressed since the last frame
	bool justPressed(int keycode)
	{
		return (keys[keycode] && !oldKeys[keycode]);
	}

	//returns true if the key has been released since the last frame
	bool justReleased(int keycode)
	{
		return (!keys[keycode] && oldKeys[keycode]);
	}

	//returns true if the key is currently down
	bool down(int keycode)
	{
		return keys[keycode];
	}

	//returns true if the key is currently up
	bool up(int keycode)
	{
		return !keys[keycode];
	}

	//set a key 
	void set(int keycode) 
	{
		keysRepeat[keycode]=true;
		keys[keycode]=true;
	}

	//unset a key
	void unset(int keycode)
	{
		keys[keycode]=false;
	}

	void setOldKeys(); //copies keys into oldkeys

private:
	bool keys[256]; //the keyboard array for this frame (true is down, false is up)
	bool oldKeys[256]; //the keyboard array for the previous frame
	bool keysRepeat[256];//keys pressed since the last frame are true (includes keyboard repeat)
};
#endif // !defined(AFX_KEYS_H__AB70C6B7_EBD0_4BE7_8119_D7BC61128CF7__INCLUDED_)
