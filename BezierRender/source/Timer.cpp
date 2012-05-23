//////////////////////////////////////////////////////////////////////
// Timer.cpp
// Description:		High performance timer class. 
//
// By:				Ben Woodhouse 
// Date:			11/2002
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Timer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Timer::Timer()
{
	startTime=getPerfTime();
	QueryPerformanceFrequency((LARGE_INTEGER *)&perfFreq);
}

//starts the timer
void Timer::start()
{
	startTime=getPerfTime();
}


//gets elapsed time in seconds
double Timer::getElapsed()
{
	double elapsed=((double)getPerfTime()-(double)startTime)/(double)perfFreq;
	return elapsed;
}

void Timer::display(char *outStr)
{
	displayTime((float)getElapsed(),outStr);
}

////////////////////////////////////////////////
//Other time-related functions
////////////////////////////////////////////////

int timeToSeconds(float time)
{
	return (int)(time-(int)(time/60)*60);
}

int timeToMinutes(float time)
{
	return (int)(time/60.0);
}

int timeToCents(float time)
{
	return (int)(time-(int)time)*100;
}

void displayTime(float time, char *outStr)
{
	int min=timeToMinutes(time);
	int sec=timeToSeconds(time);
	int cen=timeToCents(time);
	sprintf(outStr,"%02d:%02d:%02d",min,sec,cen);
}

