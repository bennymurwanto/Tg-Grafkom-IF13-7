//////////////////////////////////////////////////////////////////////
// Matrix.cpp
// Description:		4x4 matrix class
//
// By:				Ben Woodhouse 
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#include "maths.h"
#include "vector.h"
#include "Matrix.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Matrix4::Matrix4()
{
	loadIdentity();
}

Matrix4::Matrix4(const Matrix4& src)
{
	copyFrom(src);
}


void Matrix4::transpose()
{
	float temp[16];
	for (int a=0;a<16;++a)
	{
		temp[a]=dat[a];
	}
	dat[1]=temp[4];
	dat[2]=temp[8];
	dat[3]=temp[12];
	
	dat[4]=temp[1];
	dat[6]=temp[9];
	dat[7]=temp[13];

	dat[8]= temp[2];
	dat[9]= temp[6];
	dat[11]=temp[14];

	dat[12]=temp[3];
	dat[13]=temp[7];
	dat[14]=temp[11];
}

void Matrix4::set(	float m1, float m2, float m3, float m4,
					float m5, float m6, float m7, float m8,
					float m9, float m10, float m11, float m12,
					float m13, float m14, float m15, float m16)
{
	dat[0]=m1;
	dat[1]=m2;
	dat[2]=m3;
	dat[3]=m4;
	dat[4]=m5;
	dat[5]=m6;
	dat[6]=m7;
	dat[7]=m8;
	dat[8]=m9;
	dat[9]=m10;
	dat[10]=m11;
	dat[11]=m12;
	dat[12]=m13;
	dat[13]=m14;
	dat[14]=m15;
	dat[15]=m16;
}


void Matrix4::loadIdentity()
{
	float lIdentity4 [16]	={	1,0,0,0,
								0,1,0,0,
								0,0,1,0,
								0,0,0,1
								};

	for (int a=0;a<16;++a)
		dat[a]=lIdentity4[a];
}

//v' = M * v
void Matrix4::mul(Vector3& vec)
{
	Vector3 temp;

	temp.x=	vec.x*dat[0]+
			vec.y*dat[1]+
			vec.z*dat[2]+
				    dat[3]; //vec.w is 1

	temp.y=	vec.x*dat[4]+
			vec.y*dat[5]+
			vec.z*dat[6]+
					dat[7];

	temp.z=	vec.x*dat[8]+
			vec.y*dat[9]+
			vec.z*dat[10]+
					dat[11];

	vec=temp;
}

void Matrix4::mul3(float *vec)
{
	float temp[4];

	temp[0]=vec[0]*dat[0]+
			vec[1]*dat[1]+
			vec[2]*dat[2]+
					dat[3];

	temp[1]=vec[0]*dat[4]+
			vec[1]*dat[5]+
			vec[2]*dat[6]+
					dat[7];

	temp[2]=vec[0]*dat[8]+
			vec[1]*dat[9]+
			vec[2]*dat[10]+
					dat[11];

	for (int a=0;a<3;a++) vec[a]=temp[a];
}

void Matrix4::mul4(float *vec)
{
	float temp[4];


	temp[0]=vec[0]*dat[0]+
			vec[1]*dat[1]+
			vec[2]*dat[2]+
			vec[3]*dat[3];

	temp[1]=vec[0]*dat[4]+
			vec[1]*dat[5]+
			vec[2]*dat[6]+
			vec[3]*dat[7];

	temp[2]=vec[0]*dat[8]+
			vec[1]*dat[9]+
			vec[2]*dat[10]+
			vec[3]*dat[11];

	temp[3]=vec[0]*dat[12]+
			vec[1]*dat[13]+
			vec[2]*dat[14]+
			vec[3]*dat[15];
	for (int a=0;a<4;a++) vec[a]=temp[a];
}

void Matrix4::translate(float x, float y, float z)
{
	Matrix4 transMat;
	transMat.loadIdentity();

	transMat[3]=x;
	transMat[7]=y;
	transMat[11]=z; 

	(*this)*=transMat;
}

void Matrix4::scale(float x, float y, float z)
{
	Matrix4 scaleMat;
	scaleMat[0]=x;
	scaleMat[5]=y;
	scaleMat[10]=z;

	(*this)*=scaleMat;
}


/////////////////////////////////////////////
/////////////////////////////////////////////
Matrix4 Matrix4::operator *(const Matrix4& b)
{
	int x,y;
	int mpos=0;

	Matrix4 newMat;

	for (y=0;y<4;y++)
	{
		for (x=0;x<4;x++)
		{
			newMat.dat[mpos]=
							dat[y*4]*b.dat[x]+
							dat[y*4+1]*b.dat[x+4]+
							dat[y*4+2]*b.dat[x+8]+
							dat[y*4+3]*b.dat[x+12];
			mpos++;
		}
	}
	return newMat;
}

Matrix4& Matrix4::operator*=(const Matrix4& b)
{
	*this=(*this)*b;
	return *this;
}

Matrix4& Matrix4::operator+=(const Matrix4& b)
{
	for (int i=0;i<16;i++)
	{
		dat[i]=dat[i]+b.dat[i];
	}
	return *this;
}

Matrix4& Matrix4::operator-=(const Matrix4& b)
{
	for (int i=0;i<16;i++)
	{
		dat[i]=dat[i]-b.dat[i];
	}
	return *this;
}

String Matrix4::toString()
{
	String s;
	for (int y=0;y<4;y++)
	{
		for (int x=0;x<4;x++)
		{
			int c=y*4+x;
			s.printf("%5f ",dat[c]);

		}
		s+="\n";
	}
	s+="\n";
	return s;
}

//NOT TESTED
void Matrix4::rotate(float angle, float x, float y, float z)
{
	//construct a rotation matrix and multiply by it
	Matrix4 r;
	float rads=angle*(float)degToRad;
	float c=cos(rads);
	float s=sin(rads);

	float oneMinusC=1.0f-c;
	float xo=x*oneMinusC;
	float yo=y*oneMinusC;
	float zo=z*oneMinusC;

	r[0] = c+x*xo;
	r[1] = z*s+y*xo;
	r[2] = -y*s+z*xo;

	r[4] = -z*s+x*yo;
	r[5] = c+y*yo;
	r[6] =  x*s+z*yo;

	r[8] =  y*s+x*zo;
	r[9] = -x*s+y*zo;
	r[10] =	c+z*zo;

	(*this)*=r;
}

Matrix4 Matrix4::getRotation() 
{
	Matrix4 r(*this);
	r[3]=r[7]=r[11]=r[12]=r[13]=r[14]=0.0f;
	r[15]=1.0f;
	return r;
}


Matrix4 Matrix4::inverse()
{
	Matrix4 matOut;
	float det=determinant();
	if (fabs(det)<0.0001) return matOut; //return identity
	float oneOverDet=1.0f/det;
	Matrix4 sub3;

	//int sign=-1;

	//assumes OpenGL matrix
	for (int x=0;x<4;x++)
	{
		//int sign=1-(x%2)*2;
		for (int y=0;y<4;y++)
		{
			int sign=1-((x+y)%2)*2;
			submat4To3(sub3,y,x);
			//matOut[y*4+x]= (sub3.det3() * sign) / det;
			matOut[y*4+x]=sub3.det3() * sign * oneOverDet;
		}
	}
	return matOut;
}

float Matrix4::determinant()
{
	Matrix4 sub3;
	float rv=0,i=1.0f;
	for (int a=0;a<4;a++)
	{
		submat4To3(sub3,0,a);
		rv+=sub3.det3()*dat[a]*i;
		i*=-1.0f;			
	}
	return rv;
}

void Matrix4::submat4To3(Matrix4& sub3, int row, int col)
{
	//assumes an OpenGL matrix
	int dc=0;
	for (int sx=0;sx<4;sx++)
	{
		if (sx==col) continue;
		for (int sy=0;sy<4;sy++)
		{
			if (sy==row) continue;
			sub3[dc]=dat[sx*4+sy];
			dc++;
		}
		sub3[dc]=0.0f;//set the last col (since this really is a 4x4 matrix)
		dc++; 
	}
}

float Matrix4::det3()
{
	//A * (EI - HF) - B * (DI - GF) + C * (DH - GE)
	return	dat[0]*(dat[5]*dat[10] - dat[9]*dat[6]) -
			dat[1]*(dat[4]*dat[10] - dat[8]*dat[6]) +
			dat[2]*(dat[4]*dat[9] - dat[8]*dat[5]);
}
