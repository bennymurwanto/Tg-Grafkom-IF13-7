//////////////////////////////////////////////////////////////////////
// Matrix.h
// Description:		4x4 matrix class
//
// By:				Ben Woodhouse 
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__B967030F_6805_4E94_A4F4_C1E8291A4717__INCLUDED_)
#define AFX_MATRIX_H__B967030F_6805_4E94_A4F4_C1E8291A4717__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Matrix4 
{
public:
	Matrix4();
	Matrix4(const Matrix4& src);

//		Matrix4(unsigned int glMatrix); //gl constructor

//		virtual ~Matrix4(){}

	void transpose();
	void loadIdentity();

	void translate(Vector3 &vec)				{translate(vec.x, vec.y, vec.z);}

	//transformations (multiplies the matrix by a transform matrix)
	void scale(float x, float y, float z);
	void translate(float x, float y, float z);
	void rotate(float angle, float x, float y, float z);

	Matrix4& operator*=(const Matrix4& b);
	Matrix4& operator+=(const Matrix4& b);
	Matrix4& operator-=(const Matrix4& b);

	Matrix4 getRotation(); //converts to a rotation matrix by removing tranlation and scaling

	void set(float m1, float m2, float m3, float m4,
			 float m5, float m6, float m7, float m8,
			 float m9, float m10, float m11, float m12,
			 float m13, float m14, float m15, float m16);

	void mul4(float *vec); //v' = M * v (array size 4 expected)
	void mul3(float *vec); //v' = M * v (array size 3 expected)
	void mul(Vector3 &vec); //v' = M * v 

	String toString();

	//operators

	Matrix4& operator=(const Matrix4& src)		{copyFrom(src); return *this;}
	Matrix4& operator=(float dat_in[16])		{copyFrom(dat_in); return *this;}
	float& operator[](int i)					{return dat[i];}

	Matrix4 operator* (const Matrix4& b);

	Matrix4 operator+ (const Matrix4& b)
	{
		Matrix4 temp(*this);
		temp+=*this;
		return temp;
	}

	Matrix4 operator- (const Matrix4& b)
	{
		Matrix4 temp(*this);
		temp-=*this;
		return temp;
	}
	//float *getData(){return dat;}
	float * getArray()			{return dat;}
	float get(int i, int j)		{return dat[(i<<2)+j];} 


/*		
	//OpenGL wrapper functions
	void glLoad();
	void glGet(unsigned int matrix);
	*/
	Matrix4 inverse();					//gets the inverse
	float determinant();				//gets the determinant
	
	float det3();						//treats matrix as a 3x3 matrix and gets the determinant
	void submat4To3(Matrix4& sub3, int row, int col);
protected:

	//copy functions
	void copyFrom(const Matrix4& src)	{memcpy(dat,src.dat,sizeof(float)*16);}
	void copyFrom(float src[16])		{memcpy(dat,src,sizeof(float)*16);}

	float dat[16];
};

#endif // !defined(AFX_MATRIX_H__B967030F_6805_4E94_A4F4_C1E8291A4717__INCLUDED_)
