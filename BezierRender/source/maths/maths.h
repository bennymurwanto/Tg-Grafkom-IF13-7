//////////////////////////////////////////////////////////////////////
// Maths.h
// Description:		Various maths functions
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#ifndef MATH_H
#define MATH_H

//conversion multipliers
const double radToDeg=57.29577951;
const double degToRad=0.017453293;

const double oneOver255=1.0/255.0;

#include <math.h>
#include <stdio.h>
#include "Vector.h"
#include "..\data structs\string.h"
#include "Matrix.h"


_inline int round(float val)
{
	int ival=(int)val;
	val=val-ival;
	if (val>0.5) ival++;
	return ival;
}

_inline int roundUp(float val)
{
	int ival=(int)val;
	val=val-ival;
	if (val>0) ival++;
	return ival;
}

_inline int roundDown(float val)
{
	return (int)val;
}

template <class T>
_inline void swap(T& a, T& b)
{
	T temp=a;
	a=b;
	b=temp;
}

_inline void clamp(float& v)
{
	if (v<0.0) v=0.0;
	if (v>1.0) v=1.0;
}

_inline void clamp(float& v, float lower, float upper)
{
	if (v<lower) v=lower;
	if (v>upper) v=upper;
}



const float LOG2E=(float)1.442695040888963387;

_inline float log2(float val)
{
	return log(val)*LOG2E;
}

_inline void fixAngle(float &angle)
{
	int eger;
	if (angle>180)
	{
		eger=(int)((angle+180)/360);
		angle-=eger*360;			
	}
	else
	{
		if (angle<-180)
		{
			eger=(int)((angle-180)/-360);
			angle+=eger*360;			
		}
	}
}

_inline float angleSubtract(float a, float b) //returns a-b
{
	float rv=a-b;
	fixAngle(rv);
	return rv;

}


#endif
