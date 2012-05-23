//////////////////////////////////////////////////////////////////////
// Matrix.h
// Description:		3-component vector class
//
// By:				Ben Woodhouse 
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR_H__5D129568_AF73_4ACD_97AF_F52CBEC0DB27__INCLUDED_)
#define AFX_VECTOR_H__5D129568_AF73_4ACD_97AF_F52CBEC0DB27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <memory.h>

class Vector3  
{
	friend float dot (const Vector3& a, const Vector3& b);
	friend Vector3 cross(const Vector3& a, const Vector3& b);
	friend Vector3 operator+(const Vector3& a, const Vector3& b);
	friend Vector3 operator-(const Vector3& a, const Vector3& b);

public:
	Vector3()									{x=y=z=0;}
	Vector3(const float *source)				{*this=source;}
	Vector3(const Vector3& source)				{*this=source;}
	Vector3(float x_in, float y_in, float z_in)	{x=x_in; y=y_in; z=z_in;}
//	virtual ~Vector3()							{}
	
	float length() const						{return (float)sqrt(x*x+y*y+z*z);}
	float& operator[](int i)					
	{
		switch(i)
		{ 
			case 0: return x;
			case 1: return y;
			case 2: return z;
		}
		return x;
	}

	void tofp(float *fp_out)					{fp_out[0]=x; fp_out[1]=y; fp_out[2]=z;}
	Vector3 abs()								{return Vector3(fabs(x),fabs(y),fabs(z));}

	/*
	float get(int i) const						
	{
		switch(i)
		{ 
			case 0: return x;
			case 1: return y;
			case 2: return z;
		}
		return 0;
	}*/

	void set(float x_in, float y_in, float z_in){x=x_in; y=y_in; z=z_in;}

	void copy(float *dest)		{dest[0]=x; dest[1]=y; dest[2]=z;}


	bool operator==(const Vector3& b) const
	{
		float v=fabs(x-b.x)+fabs(y-b.y)+fabs(z-b.z);
		return (v<0.001);
//			return (x==b.x && y==b.y && z==b.z);
	}

	bool operator!=(const Vector3& b) const
	{
		float v=fabs(x-b.x)+fabs(y-b.y)+fabs(z-b.z);
		return (v>0.001);
	}

	Vector3& operator=(const Vector3& source)	
	{
		x=source.x; y=source.y; z=source.z;
		return *this;
	}
	Vector3& operator=(const float * source)	
	{
		x=source[0]; y=source[1]; z=source[2];
		return *this;
	}

	Vector3& operator+=(const Vector3& b)		
	{
		x+=b.x; y+=b.y; z+=b.z;			
		return *this;
	}
	Vector3& operator-=(const Vector3& b)		
	{
		x-=b.x; y-=b.y; z-=b.z;			
		return *this;
	}


	Vector3& operator=(float source)		
	{
		x=y=z=source;	
		return *this;
	}
	Vector3& operator+=(float b)			
	{
		x+=b; y+=b; z+=b;	
		return *this;
	}
	Vector3& operator-=(float b)			
	{
		x-=b; y-=b; z-=b;	
		return *this;
	}

	Vector3& operator*=(float s)
	{
		x*=s; y*=s;	z*=s;
		return *this;
	}

	Vector3& operator/=(float s)
	{
		float oneOverS=1.0f/s;
		x*=oneOverS; y*=oneOverS; z*=oneOverS;
		return *this;
	}

	Vector3 operator/ (float s)
	{
		Vector3 rv(*this);
		rv/=s;
		return rv;
	}

	Vector3 operator* (float s)
	{
		Vector3 rv(*this);
		rv*=s;
		return rv;
	}

	Vector3 operator+ (float s)
	{
		Vector3 rv(*this);
		rv+=s;
		return rv;
	}

	//cross product
	Vector3 cross(const Vector3& b) const
	{
		Vector3 result;
		result[0]= y*b.z - z*b.y;
		result[1]= z*b.x - x*b.z;
		result[2]= x*b.y - y*b.x;
		return result;
	}
	float dot(const Vector3& b)	const
	{		
		return x*b.x+y*b.y+z*b.z;
	}

	void normalise()
	{
		float oneOverLength=1.0f/length();
		x*=oneOverLength;
		y*=oneOverLength;
		z*=oneOverLength;
	}
	Vector3 normal() const
	{
		Vector3 rv(*this);
		rv.normalise();
		return rv;
	}

	void normalise_safe()
	{
		float len=length();
		if (len>0.001) (*this)/=len;
	}
	Vector3 normal_safe()
	{
		Vector3 rv(*this);
		rv.normalise_safe();
		return rv;
	}

	float squared() 
	{
		return dot(*this);
	}

	float x,y,z;
};

//friend functions
__inline Vector3 cross(const Vector3& a, const Vector3& b)
{
	Vector3 result;
	result[0]= a.y*b.z - a.z*b.y;
	result[1]= a.z*b.x - a.x*b.z;
	result[2]= a.x*b.y - a.y*b.x;
	return result;
}

__inline float dot(const Vector3 &a, const Vector3 &b)
{
	return a.x*b.x+a.y*b.y+a.z*b.z;
}

__inline Vector3 operator+(const Vector3& a, const Vector3& b)
{
	Vector3 result;
	result[0]=a.x+b.x;
	result[1]=a.y+b.y;
	result[2]=a.z+b.z;
	return result;
}
__inline Vector3 operator-(const Vector3& a, const Vector3& b)
{
	Vector3 result;
	result[0]=a.x-b.x;
	result[1]=a.y-b.y;
	result[2]=a.z-b.z;
	return result;
}

__inline Vector3 operator*(const Vector3& a, float s)
{
	Vector3 result;
	result[0]=a.x*s;
	result[1]=a.y*s;
	result[2]=a.z*s;
	return result;
}

__inline Vector3 operator-(const Vector3& a)
{
	return Vector3(-a.x,-a.y,-a.z);
}




#endif // !defined(AFX_VECTOR_H__5D129568_AF73_4ACD_97AF_F52CBEC0DB27__INCLUDED_)
