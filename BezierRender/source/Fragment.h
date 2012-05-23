/////////////////////////////////////////////////////////////////////
//
// Fragment.h
// Class to represent fragment information. Supports addition and 
// scalar multiplication 
//
// By Ben Woodhouse
//////////////////////////////////////////////////////////////////////

#pragma once

struct Fragment
{
public:
	Fragment(){}
	Fragment(const Vector3& norm_in, float z_in)	{norm=norm_in; z=z_in;}
	Fragment(const Fragment& src)					{*this=src;}
	~Fragment(){}

	Fragment& operator=(const Fragment& src)
	{
		norm=src.norm;
		eyePos=src.eyePos;
		z=src.z;
		return *this;
	}
	Fragment& operator+=(const Fragment& rhs)
	{
		norm+=rhs.norm;
		eyePos+=rhs.eyePos;
		z+=rhs.z;
		return *this;
	}
	Fragment& operator-=(const Fragment& rhs)
	{
		norm-=rhs.norm;
		eyePos-=rhs.eyePos;
		z-=rhs.z;
		return *this;
	}
	Fragment& operator*=(float rhs)
	{
		norm*=rhs;
		eyePos*=rhs;
		z*=rhs;
		return *this;
	}
	Fragment& operator/=(float rhs)
	{
		float oneOverRhs=1.0f/rhs;
		norm*=oneOverRhs;
		eyePos*=oneOverRhs;
		z*=oneOverRhs;
		return *this;
	}

	Fragment operator-(const Fragment& rhs) const
	{
		Fragment rv(*this);
		return rv-=rhs;
	}
	Fragment operator+(const Fragment& rhs) const
	{
		Fragment rv(*this);
		return rv+=rhs;
	}
	Fragment operator*(float rhs) const
	{
		Fragment rv(*this);
		return rv*=rhs;
	}
	Fragment operator/(float rhs) const
	{
		Fragment rv(*this);
		return rv/=rhs;
	}
	Vector3 eyePos;
	Vector3 norm;
	float z;
};

