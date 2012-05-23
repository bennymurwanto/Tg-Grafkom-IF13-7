//////////////////////////////////////////////////////////////////////
// Timer.h
// Description:		High performance timer class. 
//
// By:				Ben Woodhouse 
// Date:			11/2002
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMER_H__A0CE5758_A04A_4616_833C_D7B11E422683__INCLUDED_)
#define AFX_TIMER_H__A0CE5758_A04A_4616_833C_D7B11E422683__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Timer  
{
	public:
		Timer();
		void start();
		double getElapsed();
		void display(char *outStr);

	private:
		__int64 getPerfTime()
		{
			__int64 perfTime;	
			QueryPerformanceCounter((LARGE_INTEGER *) &perfTime);
			return perfTime;
		}	
		__int64 startTime;
		__int64 perfFreq;
};


int timeToSeconds(float time);
int timeToMinutes(float time);
int timeToCents(float time);
void displayTime(float time, char *outStr);

#endif // !defined(AFX_TIMER_H__A0CE5758_A04A_4616_833C_D7B11E422683__INCLUDED_)
