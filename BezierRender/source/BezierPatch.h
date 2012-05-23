//////////////////////////////////////////////////////////////////////
// BezierPatch.h
// Description:		Class to store and render a bezier patch with 
//					various rendering modes.
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Renderer.h"
#include "RadixSort.h"

enum RenderFlags
{
	RF_CONTROLMESH=1,
	RF_WIREFRAME=2,
	RF_SHADED=4
};

class BezierPatch
{
public:
	BezierPatch(void);
	~BezierPatch(void);

	void resizePoints(int quadRes_in);
	void resizeSplats(int splatRes_in);

	void init();
	void calcPoints();
	void calcSplats();

	void setRenderType(uint flags)							{renderFlags=flags;}
	void toggleSplatRendering()								{splatRendering=!splatRendering;}
	void render(Renderer &renderer);
	void renderControlMesh(Renderer &renderer);
	void renderSurface(Renderer &renderer, bool wireframe);
	void renderSplats(Renderer &renderer);
	void increaseRes()										{if (quadRes<32) resizePoints(quadRes+1);}
	void decreaseRes()										{if (quadRes>1) resizePoints(quadRes-1);}
	bool findControlPoint(int wx, int wy, Vector3 & cpOut, int &i, int &j);
	void setControlPoint(int i, int j, const Vector3 & point);
	void increaseSplatRes()									{if (splatRes<256) {resizeSplats(splatRes*2);};}
	void decreaseSplatRes()									{if (splatRes>32) {resizeSplats(splatRes/2);};}

private: 
	void getSurfacePoint(float s, float t, Vector3& point, Vector3& norm);
	void clearPoints(); 
	void clearSplats(); 

	Vector3 getControlPoint(int x, int y);

	Matrix4 controlPoints[3];
	Vector3 **points;
	Vector3 **normals;

	Vector3 **splatPoints;
	Vector3 **splatNorms;

	Matrix4 mB;			//bezier basis matrix
	Matrix4 mBt;		//transpose of mB

	Vector3 winControlPoints[4][4]; //control points in window coordinates
	//Vector3 eyeControlPoints[4][4]; //control points in eye coordinates

	float *splatZ;
	int *splatOrder;
	uint renderFlags;
	int pointRes;
	int quadRes;
	float splatSize;
	int splatRes;
	float sizeX, sizeY;
	RadixSort *sort;

	bool splatRendering;
};
