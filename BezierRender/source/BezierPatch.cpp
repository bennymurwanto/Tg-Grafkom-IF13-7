//////////////////////////////////////////////////////////////////////
// BezierPatch.cpp
// Description:		Class to store and render a bezier patch with 
//					various rendering modes.
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\bezierpatch.h"

BezierPatch::BezierPatch(void)
{
	sizeX=100;
	sizeY=100;
	normals=NULL;
	points=NULL;
	renderFlags=RF_CONTROLMESH | RF_SHADED;
	splatRendering=false;
	mB.set(	-1,	3,	-3,	1,
			3,	-6,	3,	0,
			-3,	3,	0,	0,
			1,	0,	0,	0);
	mBt=mB;
	mBt.transpose();
	splatSize=6.0f;

	splatNorms=NULL;
	splatPoints=NULL;
	sort=NULL;
	splatZ=NULL;
	splatOrder=NULL;

	resizePoints(16);
	resizeSplats(128);
	init();
}

BezierPatch::~BezierPatch(void)
{
	clearPoints();
	clearSplats();
}

void BezierPatch::init()
{
	float startX=-sizeX*0.5f;
	float startY=-sizeY*0.5f;
	float incX=sizeX/3.0f;
	float incY=sizeY/3.0f;
	float fx,fy=startY;
	int i=0;
	//set the control points to an interesting shape
	for (int y=0;y<4;y++,fy+=incY)
	{
		fx=startX;
		for (int x=0;x<4;x++,i++,fx+=incX)
		{
			controlPoints[0][i]=fx;
			controlPoints[1][i]=fy;
			controlPoints[2][i]=(sin(fx*50.0f))*30-cos(fy*2)*20.0f;
		}
	}
	calcPoints();
	calcSplats();
}


void BezierPatch::calcPoints()
{
	float t=0.0f, s=0.0f;
	float incTS=1.0f/(pointRes-1.0f);
    
	for (int y=0;y<pointRes;y++)
	{
		t=0.0f;
		for (int x=0;x<pointRes;x++)
		{
			//points[y][x].set(fx,fy,0);
			getSurfacePoint(s,t,points[y][x],normals[y][x]);
			t+=incTS;
		}
		s+=incTS;
	}
}

void BezierPatch::calcSplats()
{
	float t=0.0f, s=0.0f;
	float incTS=1.0f/(splatRes-1.0f);
    
	for (int y=0;y<splatRes;y++)
	{
		t=0.0f;
		for (int x=0;x<splatRes;x++)
		{
			getSurfacePoint(s,t,splatPoints[y][x],splatNorms[y][x]);
			t+=incTS;
		}
		s+=incTS;
	}
}


void BezierPatch::render(Renderer &renderer)
{
	if (renderFlags & RF_WIREFRAME) renderSurface(renderer, true);
	if (renderFlags & RF_CONTROLMESH) renderControlMesh(renderer);
	if (renderFlags & RF_SHADED) 
	{
		if (splatRendering) renderSplats(renderer);
		else renderSurface(renderer, false);
	}
}

void BezierPatch::renderControlMesh(Renderer &renderer)
{
	renderer.setDepthOffset(10.0f);
	Vector3 quad[4];
	Vector3 norm[4];
	renderer.setPolyFill(false);
	renderer.setColour(Colour(1.0f,0.0f,1.0f));
	for (int y=0;y<3;y++)
	{
		for (int x=0;x<3;x++)
		{
			quad[0]=getControlPoint(x,y);
			quad[1]=getControlPoint(x,y+1);
			quad[2]=getControlPoint(x+1,y+1);
			quad[3]=getControlPoint(x+1,y);
			renderer.quad(quad,norm);
		}
	}
	renderer.setDepthOffset(0.0f);

	//get the control point window coords
	for (int y=0;y<4;y++)
	{
		for (int x=0;x<4;x++)
		{
			winControlPoints[y][x]=renderer.project(getControlPoint(x,y));
		}
	}
}


void BezierPatch::renderSurface(Renderer &renderer, bool wireframe)
{
	if (wireframe)
	{
		renderer.setDepthOffset(10.0f);
		renderer.setColour(Colour(0.5,0.5,0.5));
		renderer.setPolyFill(false);
	}
	else
	{
		renderer.setColour(Colour(1.0,0.0,0.0));
		renderer.setPolyFill(true);
	}
	Vector3 quad[4];
	Vector3 norm[4];

	for (int y=0;y<quadRes;y++)
	{
		for (int x=0;x<quadRes;x++)
		{
			quad[0]=points[y][x];
			quad[1]=points[y+1][x];
			quad[2]=points[y+1][x+1];
			quad[3]=points[y][x+1];
			norm[0]=normals[y][x];
			norm[1]=normals[y+1][x];
			norm[2]=normals[y+1][x+1];
			norm[3]=normals[y][x+1];
			renderer.quad(quad,norm);
		}
	}
	if (wireframe) renderer.setDepthOffset(0.0f);
}



void BezierPatch::resizePoints(int quadRes_in)
{
	clearPoints();
	quadRes=quadRes_in;
	pointRes=quadRes+1;
	normals=new Vector3*[pointRes];
	points=new Vector3*[pointRes];
	for (int y=0;y<pointRes;y++)
	{
        normals[y]=new Vector3[pointRes];
		points[y]=new Vector3[pointRes];
	}
	calcPoints();
}




void BezierPatch::resizeSplats(int splatRes_in)
{
	clearSplats();
	splatRes=splatRes_in; 
	splatSize=420.0f/(float)splatRes;
	splatNorms=new Vector3*[splatRes];
	splatPoints=new Vector3*[splatRes];
	for (int y=0;y<splatRes;y++)
	{
        splatNorms[y]=new Vector3[splatRes];
		splatPoints[y]=new Vector3[splatRes];
	}
	splatZ=new float[splatRes*splatRes];
	splatOrder=new int[splatRes*splatRes];
	sort=new RadixSort(splatRes*splatRes);

	calcSplats();
}

void BezierPatch::clearPoints()
{
	if (normals)
	{
		for (int y=0;y<pointRes;y++) delete [] normals[y];
		delete [] normals;
		normals=NULL;
	}
	if (points)
	{
		for (int y=0;y<pointRes;y++) delete [] points[y];
		delete [] points;
		points=NULL;
	}
}

void BezierPatch::clearSplats()
{
	if (splatNorms)
	{
		for (int y=0;y<splatRes;y++) delete [] splatNorms[y];
		delete [] splatNorms;
		splatNorms=NULL;
	}
	if (splatPoints)
	{
		for (int y=0;y<splatRes;y++) delete [] splatPoints[y];
		delete [] splatPoints;
		splatPoints=NULL;
	}
	if (sort) 
	{
		delete sort;
		sort=NULL;
	}
	if (splatZ) 
	{
		delete [] splatZ;
		splatZ=NULL;
	}
	if (splatOrder) 
	{
		splatOrder=NULL;
		delete [] splatOrder;
	}
}

void BezierPatch::getSurfacePoint(float s, float t, Vector3& point, Vector3& norm)
{
	Vector3 tanT,tanS;
	//Q(s,t) = S . MB . Gb . mBt . Tt

	//rewritten to use transposed vectors:
	//Q(s,t) = MBt . St . Gb . mB . Tt

	float GMttv[4];
	float GMT[4];
	float St[4],Tt[4]; // S and T transposed
	St[3]=1.0f;
    St[2]=s;
	St[1]=s*s;
	St[0]=St[1]*s;
	Tt[3]=1.0f;
	Tt[2]=t;
	Tt[1]=t*t;
	Tt[0]=Tt[1]*t;


	//differentiate for tangent vectors s and t (also transposed)
	float tsv[4],ttv[4];
	tsv[0]=3.0f*s*s; 
	tsv[1]=2.0f*s;
	tsv[2]=1.0f;
	tsv[3]=0.0f;
	ttv[0]=3.0f*t*t;
	ttv[1]=2.0f*t;
	ttv[2]=1.0f;
	ttv[3]=0.0f;

	mBt.mul4(St); //St = St.mBt
	mB.mul4(Tt);  //Tt = Tt.mB

	mBt.mul4(tsv); 
	mBt.mul4(ttv);
	for (int a=0;a<3;a++)
	{
		//calculate Gb[x/y/z] . (mB.Tt)
		for (int b=0;b<4;b++) GMT[b]=Tt[b];
		controlPoints[a].mul4(GMT);

		//calculate (MBt . St) . (Gb[x/y/z] . mB . Tt) to get point [x/y/z] value
		point[a]=St[0]*GMT[0] + St[1]*GMT[1] + St[2]*GMT[2] + St[3]*GMT[3];

		//calculate (MBt . tsv) . (Gb[x/y/z] . mB . Tt) to get tanS [x/y/z] value
		tanS[a]=tsv[0]*GMT[0] + tsv[1]*GMT[1] + tsv[2]*GMT[2] + tsv[3]*GMT[3];

		for (int b=0;b<4;b++) GMttv[b]=ttv[b];
		controlPoints[a].mul4(GMttv);

		//calculate (MBt . St) . (Gb[x/y/z] . mBt . ttv) to get tanT [x/y/z] value
		tanT[a]=St[0]*GMttv[0] + St[1]*GMttv[1] + St[2]*GMttv[2] + St[3]*GMttv[3];
	}
	norm=tanT.cross(tanS).normal();
} 

Vector3 BezierPatch::getControlPoint(int x, int y)
{
	int i=(y<<2)+x;
	return Vector3(controlPoints[0][i], controlPoints[1][i], controlPoints[2][i]);
}


void BezierPatch::setControlPoint(int i, int j, const Vector3 & point)
{
	int k=(i<<2)+j;
	controlPoints[0][k]=point.x;
	controlPoints[1][k]=point.y;
	controlPoints[2][k]=point.z;
	calcPoints();
	calcSplats();
}

bool BezierPatch::findControlPoint(int wx, int wy, Vector3 & cpOut, int &i, int &j)
{
	for (int y=0;y<4;y++)
	{
		for (int x=0;x<4;x++)
		{
			if (abs(winControlPoints[y][x].x-wx)<=6 && 
				abs(winControlPoints[y][x].y-wy)<=6)
			{
				i=y;
				j=x;
				cpOut=getControlPoint(x,y);
				return true;
			}
		}
	}
	return false;
}

void BezierPatch::renderSplats(Renderer &renderer) 
{
	renderer.setColour(Colour(1.0f,0.0f,0.0f));
	int c=0;

	int numSplats=splatRes*splatRes;
	//sort the splats by their distance from the eye
	for (int y=0;y<splatRes;y++)
	{
		for (int x=0;x<splatRes;x++,c++)
		{
			Vector3 sv;
			renderer.modelviewTrans(splatPoints[y][x],sv);
			splatZ[c]=abs(sv.z);
			splatOrder[c]=c;
		}
	}
	sort->sort(splatZ,numSplats,splatOrder);
	//render in order of increasing Z
	for (int a=numSplats-1;a>=0;a--)
	{
		int x=splatOrder[a]%splatRes;
		int y=splatOrder[a]/splatRes;
		renderer.splat(splatPoints[y][x],splatNorms[y][x],splatSize);
	}
}

