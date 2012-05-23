//////////////////////////////////////////////////////////////////////
// Camera.cpp
// Description:		Camera representation
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "camera.h"


Camera::Camera()
{
	renderer=NULL;
	radius=0;
}
Camera::~Camera()
{
}

void Camera::setPerspective(float fovy, float aspect, float nearClip, float farClip)
{
	this->nearClip=nearClip;
	this->farClip=farClip; 
	float cot = 1.0f/(float)tan(0.5f*fovy*degToRad); 
	projection[0]=cot/aspect;
	projection[5]=cot;
	projection[10]=-(nearClip+farClip)/(farClip-nearClip);
	projection[11]=-2.0f*nearClip*farClip/(farClip-nearClip);
	projection[14]=-1.0f;
	projection.transpose();
	renderer->setProjection(projection);
	renderer->setClipPlanes(nearClip, farClip);
}


void Camera::addRadius(float r)					
{
	float oldRadius=radius;
	radius+=r; 
	clamp(radius,50,400);
	float rd=radius-oldRadius;
	Matrix4 mr;
	mr.translate(0,0,-rd);
	mr*=modelview;
	modelview=mr;
	updateModelview();
}

void Camera::updateModelview()
{
	renderer->setModelview(modelview);
}

void Camera::rotate(float a, float x, float y, float z)
{
	//rotate around the camera's axes (axises?) by a
	Vector3 vy=getUpVector()*y;
	Vector3 vx=getRightVector()*x;
	Vector3 vz=getCameraNorm()*z;
	Vector3 total=vx+vy+vz;
	if (total.length()>0.001)
	{
		total.normalise();
		modelview.rotate(a,total.x,total.y,total.z);
		updateModelview();
	}	
}
Vector3 Camera::getCameraNorm()
{
	return Vector3(-modelview[8],-modelview[9],-modelview[10]);
}
Vector3 Camera::getUpVector()
{
	return Vector3(-modelview[4],-modelview[5],-modelview[6]);
}
Vector3 Camera::getRightVector()
{
	return Vector3(-modelview[0],-modelview[1],-modelview[2]);
}

/*
Vector3 Camera::getCameraPos()
{
	float negRot[3] ={-rotX,-rotY,0};
	Vector3 vector=anglesToCoord(negRot,radius);
	return vector+pos;
}*/
/*
void Camera::unproject( int x,int y, 
						double& outx, double& outy, double& outz)
{
	double modelview[16];
	double projection[16];
	int viewport[4];

	float z;
	glReadPixels(x,y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&z);

	glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
	glGetDoublev(GL_PROJECTION_MATRIX,projection),
	glGetIntegerv(GL_VIEWPORT,viewport);

	gluUnProject(x,y,(double)z,modelview,projection,
						viewport,outx,outy,outz);
}*/
