//////////////////////////////////////////////////////////////////////
// Colour.cpp
// Description:		Represents a colour
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#pragma once

struct Colour
{
	Colour()
	{
		arr=(BYTE *)&rgba;
		rgba=0;
	}
	Colour(const Colour& src)	
	{
		arr=(BYTE *)&rgba;
		rgba=src.rgba;
	}
	Colour(uint colInt)	
	{
		arr=(BYTE *)&rgba;
		rgba=colInt;
	}
	Colour(float r_in, float g_in, float b_in)		
	{
		clamp(r_in);
		clamp(g_in);
		clamp(b_in);
		arr=(BYTE *)&rgba;
		arr[0]=(BYTE)(r_in*255);
		arr[1]=(BYTE)(g_in*255);
		arr[2]=(BYTE)(b_in*255);
		arr[3]=1;
	}
	/*
	Colour(BYTE r_in, BYTE g_in, BYTE b_in)			
	{
		BYTE *arr=(BYTE *)rgba;
		arr[0]=r_in; 
		arr[1]=g_in; 
		arr[2]=b_in; 
		arr[3]=1;
	}*/
	Colour(float r_in, float g_in, float b_in, float a_in)		
	{
		if (r_in>1) r_in=1;
		if (g_in>1) g_in=1;
		if (b_in>1) b_in=1;
		if (a_in>1) a_in=1;
		arr=(BYTE *)&rgba;
		arr[0]=(BYTE)(r_in*255);
		arr[1]=(BYTE)(g_in*255);
		arr[2]=(BYTE)(b_in*255);
		arr[3]=(BYTE)(a_in*255);
	}
	Colour& operator=(const Colour& src)
	{
		rgba=src.rgba;
		return *this;
	}
	Colour& operator*=(float s)
	{
		for (int a=0;a<4;a++)
		{
			int val=(int)((int)arr[a]*s);
			arr[a]=min(255,val);
		}
		return *this;
	}
	Colour& operator+=(const Colour& b)
	{
		for (int a=0;a<4;a++)
		{
			int val=(int)arr[a]+(int)b.arr[a];
			arr[a]=min(255,val);
		}
		return *this;
	}

	Colour& operator-=(const Colour& b)
	{
		for (int a=0;a<4;a++)
		{
			int val=(int)arr[a]-(int)b.arr[a];
			arr[a]=max(0,val);
		}
		return *this;
	}

	void lerp(float t, const Colour& colb)
	{
		float oneMinusT=1.0f-t;
		for (int a=0;a<4;a++)
		{
			int val=(int)(((float)colb.arr[a]-(float)arr[a])*t)+arr[a];
			arr[a]=min(max(0,val),255);
		}
	}
	/*
	Colour(BYTE r_in, BYTE g_in, BYTE b_in, BYTE a_in)			
	{
		BYTE *arr=(BYTE *)rgba;
		arr[0]=r_in; 
		arr[1]=g_in; 
		arr[2]=b_in; 
		arr[3]=a_in;
	}*/

	BYTE *arr;
	uint rgba;
};

const Colour WHITE(1,1,1,1);
const Colour BLACK(0,0,0,1);
const Colour RED(1,0,0,1);
const Colour GREEN(0,1,0,1);
const Colour BLUE(0,0,1,1);

