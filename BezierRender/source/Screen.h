//////////////////////////////////////////////////////////////////////
// Screen.h
// Description:		This class contains the colour buffer and Z buffer. 
//					It is responsible for sending the pixels to the 
//					screen for display. 
//
//					This class also handles primitive rasterisation. 
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#pragma once
#include "fragment.h"
#include "shader.h"

#define MAX_SPLATSIZE 20

struct PolyLine
{
	int p1,p2;
	int edge;
	Fragment f1, f2;
};
struct PolySegment
{
	int y1,y2;
	Fragment fL1,fR1,fL2,fR2;
};

enum RenderType
{
	RT_GLDRAWPIXELS,
	RT_TEXSUBIMAGE
};

class Renderer;
class Screen
{
	friend Renderer;
public:
	Screen();
	~Screen();

	bool init(int width_in, int height_in);
	bool shutdown();
	bool resize(int width_in, int height_in);
	void swapBuffers();
	void clear();

	//shape drawing
	void plot(float x, float y, float z, const Colour& c)	
	{
		if (x<(float)width && y<(float)height && x>=0.0f && y>=0.0f) _plot((int)x,(int)y,z,c.rgba);
	}
	void plot(float x, float y, const Colour& c)	
	{
		if (x<width && y<height && x>=0 && y>=0) _plot(round(x),round(y),0,c.rgba);
	}
	void cross(int x, int y, const Colour& c);

	void line(float x1, float y1, float z1, float x2, float y2, float z2, const Colour& c);
	void triangle(const Vector3 points[3], const Vector3 norms[3]);

	Shader& getShader()	{return shader;}
	int getWidth()	{return width;}
	int getHeight()	{return height;}
	HWND getHWnd()	{return GLDisplay::inst().getHWnd();}


private:
	void createSplatFilters();
	void destroySplatFilters();

	bool clipLine2D(float &x1, float &y1, float &x2, float &y2);
	bool clipEdge(float denom, float num, float &t1, float &t2);

	//shape drawing, post clipping
	void _plot(int x, int y, float z, uint col)	
	{
		int fPos=y*width+x;
		if (z<=zBuffer[fPos]) 
		{
			cBuffer[fPos]=col;
			zBuffer[fPos]=z;
		}
	}
	void _plot(int fPos, float z, uint col)	
	{
		if (z<=zBuffer[fPos]) 
		{
			cBuffer[fPos]=col;
			zBuffer[fPos]=z;
		}
	}
	void _line(int x1, int y1, float z1, int x2, int y2, float z2, uint col);

	//splat rendering
	void renderSplat(int x, int y, Fragment& fragment, const Colour& colour, int size);
	void plotSplatPixel(int fPos, Colour col, float opacity, float z)
	{
		if (z<=zBuffer[fPos])
		{
			Colour cBufCol(cBuffer[fPos]);
			cBufCol.lerp(opacity,col);
			cBuffer[fPos]=cBufCol.rgba;
			zBuffer[fPos]=z;
		}
	}


	//polygon rendering
	void _poly(const Vector3 *points, const Fragment *fragments, int n, const Colour& colour, bool cw);
	void _polyLine(const Vector3 *points, int n, const Colour& c);

	int getPolySegments(const Vector3 *points, const PolyLine *lineL, const PolyLine *lineR, int numL, int numR, 
						PolySegment *segments_out);

	void scanLine(int y, int xL, int xR, const Fragment& fL, const Fragment& fR);
	void edgeScan(int y1, int y2, int x1, int x2, int *scanEdge);
	bool isPolyCW(const Vector3 *points, int n);
	void getPointYOrder(const Vector3* points, int n, int *order_out);

	void initTex();

//	void _triangle(const Vector3 points[3], const Vector3 norms[3]);

//	void halfTri(Vector3& leftPos, Vector3& rightPos, Vector3& leftNorm, Vector3& rightNorm, float bottomY, float topY);

	int width;
	int height;
	int numFragments;
	uint *cBuffer;
	float *zBuffer;
	int **edge;
	bool cullFace;

	int texSize;
	int renderType;
//	Vector3 eye;
	GLuint tex;
	Shader shader;

	float *splatFilters[MAX_SPLATSIZE];
};
