//////////////////////////////////////////////////////////////////////
// Camera.h
// Description:		Orbital camera representation
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#pragma once

class Camera 
{
public:
	Camera();
	virtual ~Camera();

	void setPerspective(float fovy, float aspect, float nearClip, float farClip);

	void init(Renderer *renderer_in)		{renderer=renderer_in; addRadius(150); updateModelview();}

	void rotate(float a, float x, float y, float z);
	void addRadius(float r);					
	float getRadius()						{return radius;}

	Vector3 getCameraNorm();
	Vector3 getUpVector();
	Vector3 getRightVector();

//	Vector3 getCameraPos();
private:
	void fixAngles();
	void updateModelview();

	float radius;
	float nearClip, farClip;

	Renderer *renderer;
	Matrix4 modelview;
	Matrix4 projection;
};