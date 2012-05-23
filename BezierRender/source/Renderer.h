//////////////////////////////////////////////////////////////////////
// Renderer.h
// Description:		Class used to project and clip primitives before
//				    sending them to the screen class for rasterisation
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Screen.h"
#include "Colour.h"

struct ClipVert;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void init(Screen *screen_in)			{screen=screen_in;}
	void quad(const Vector3 points[4], const Vector3 norms[4]);
	void line(const Vector3& p1, const Vector3 p2, const Colour& c);

	void setProjection(const Matrix4& projection_in);
	void setModelview(const Matrix4& modelview_in); 

	void setClipPlanes(float nearClip_in, float farClip_in)	
											{nearClip=nearClip_in; farClip=farClip_in;} 

	void setPolyFill(bool fill_in)			{polyFill=fill_in;}
	void setColour(Colour& c)				{colour=c;}

	const Matrix4& getModelview()			{return modelview;}
	const Matrix4& getProjection()			{return projection;}
	void setDepthOffset(float val)			{depthOffset=val;}
	void setLightPos(Vector3& light_in)		
	{
		light=light_in;
		Vector3 mLight=light;
		modelviewRotate(light,mLight);
		screen->getShader().setLightPos(mLight); //update the shader's light position
	}

	Vector3 project(const Vector3& point); //returns the projected point

	void splat(const Vector3& pos, const Vector3& norm, float size);

	void modelviewRotate(const Vector3& in, Vector3& out);
	void modelviewTrans(const Vector3& in, Vector3& out);
	void perspectiveTrans(const Vector3& in, Vector3& out);
	void viewportTrans(const Vector3& in, Vector3& out);
private:
	int clipPoly(Vector3 *verts, Fragment* frags, int n, ClipVert *vertsOut);
	int clipPolyLine(const ClipVert& cv1, const ClipVert& cv2, int edge, ClipVert* cvOut);
	bool clipEdge(float denom, float num, float &t1, float &t2);
	bool clipLine3D(Vector3& p1, Vector3& p2);

	int getCSCode(const Vector3& point);

	void screenClamp(Vector3& p1);
	Screen * screen;
	Matrix4 projection;
	Matrix4 modelview;
	Matrix4 mvRotate;
	Colour colour;
	float farClip,nearClip;
	bool polyFill;
	float depthOffset;
	bool multisample;

	Vector3 light; //light position in world coordinates
};

struct ClipVert
{
	ClipVert(){}
	ClipVert(const ClipVert& src)	{*this=src;}
	ClipVert& operator=(const ClipVert& src) 
	{
		v=src.v;
		f=src.f;
		return *this;
	}
	Vector3 v;
	Fragment f;
};