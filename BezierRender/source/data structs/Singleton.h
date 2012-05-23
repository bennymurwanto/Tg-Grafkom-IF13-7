//////////////////////////////////////////////////////////////////////
// Singleton.h
// Description:		A Singleton class template. Classes derived from 
//					this class can only be instantiated once, and are
//					instantiated only when required. 
//
// By:				Ben Woodhouse 
// Date:			11/2002
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLETON_H__A9A99B88_16C4_46B7_9964_E7C597BE59E8__INCLUDED_)
#define AFX_SINGLETON_H__A9A99B88_16C4_46B7_9964_E7C597BE59E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template <typename T> 
class Singleton
{
	static T* instance;
public:
	static T& inst()
	{
		if (!instance) 
		{	
			instance=new T;
			atexit(_destroy);
		}
		return *instance;
	}
	static void _destroy()
	{
		if (instance!=0) 
		{
			delete instance;
			instance=0;
		}
	}
	static bool inited(){return instance!=0;}
};

template <typename T> T* Singleton <T>::instance = 0;

#endif // !defined(AFX_SINGLETON_H__A9A99B88_16C4_46B7_9964_E7C597BE59E8__INCLUDED_)
