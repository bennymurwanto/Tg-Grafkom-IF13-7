//////////////////////////////////////////////////////////////////////
// Renderer.cpp
// Description:		Class used to project and clip primitives before
//				    sending them to the screen class for rasterisation
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\renderer.h"

Renderer::Renderer(void)
{
	screen=NULL;
	depthOffset=0.0f;
	light.set(0,0,1);
	polyFill=true;
}

Renderer::~Renderer(void)
{
}

void Renderer::splat(const Vector3& pos, const Vector3& norm, float size)
{
	Vector3 mPos;
	Vector3 mNorm;

	modelviewRotate(norm,mNorm); 	//modelview transform 
	modelviewTrans(pos,mPos); 

	if (mNorm.dot(mPos)>0.0f) //flip the normals if the eyepoint is away from the splat
		mNorm*=-1;

	//get the vertex fragment information
	Fragment frag;
	frag.eyePos=-mPos; 
	frag.norm=mNorm;

	Vector3 point2=mPos+Vector3(1,0,0); //perspective offset point
	perspectiveTrans(mPos,mPos);  //perspective transform
	perspectiveTrans(point2,point2);   

	//culling
	if (fabs(mPos.x)>=0.95f ||
		fabs(mPos.y)>=0.95f ||
		mPos.z>=1.0f ||
		mPos.z<=0.0f) return;

	viewportTrans(mPos,mPos); //viewport transform
	viewportTrans(point2,point2);  
	frag.z=mPos.z;


	int mSize=(int)((point2-mPos).length()*size);
	mSize=min(mSize,MAX_SPLATSIZE);
	screen->renderSplat(round(mPos.x),round(mPos.y),frag, colour, mSize);
}

void Renderer::quad(const Vector3 points[4], const Vector3 norms[4]) 
{
	Vector3 mVerts[4];
	Vector3 mpVerts[4];
	Vector3 mNorms[4];

	//modelview transform 
	for (int a=0;a<4;a++)
	{
		modelviewRotate(norms[a],mNorms[a]);
		modelviewTrans(points[a],mVerts[a]);
	}

	//get the vertex fragment information
	Fragment frag[16];
	for (int a=0;a<4;a++) 
	{
		frag[a].eyePos=-mVerts[a]; 
		frag[a].norm=mNorms[a];
	}

	//determine if the quad is facing the camera
	Vector3 avNorm;
	Vector3 avEye;
	for (int a=0;a<4;a++)
	{
		avNorm+=mNorms[a];
		avEye-=mVerts[a];
	}
	avNorm.normalise();
	avEye.normalise();
	bool facing;
	if (avNorm.dot(avEye)<0.0f) facing=false;
	else facing=true;

	//perspective transform
	for (int a=0;a<4;a++)
	{
		perspectiveTrans(mVerts[a],mpVerts[a]);
	}

	//clipping
	ClipVert clippedVerts[16];
	int numClippedVerts=clipPoly(mpVerts,frag,4,clippedVerts);
	if (numClippedVerts<3) return;

	//viewport transform
	Vector3 viewVert[16];
	Fragment clipFragments[16];
	for (int a=0;a<numClippedVerts;a++)
	{
		viewportTrans(clippedVerts[a].v, viewVert[a]);
		clipFragments[a]=clippedVerts[a].f;
		clipFragments[a].z=viewVert[a].z;
	}

//	screen->renderSplat(round(viewVert[0].x),round(viewVert[0].y),clipFragments[0], colour, 20.0f);

	//render the quad
	if (polyFill) 
		screen->_poly(viewVert,clipFragments,numClippedVerts,colour,facing); 
	else 
		screen->_polyLine(viewVert,numClippedVerts,colour);
}

void Renderer::modelviewRotate(const Vector3& in, Vector3& out)
{
	out=in;
	mvRotate.mul(out);
}
void Renderer::modelviewTrans(const Vector3& in, Vector3& out)
{
	out=in;
	modelview.mul(out);
}
void Renderer::perspectiveTrans(const Vector3& in, Vector3& out)
{
	float inz=in.z;
	float vertH[4]={in.x,in.y,in.z,1.0f};
	projection.mul4(vertH);
	float oneOverZ=1.0f/vertH[2];
	out=vertH; 
	out*=oneOverZ;
	out.z=-(inz+depthOffset+nearClip)/(farClip-nearClip);
}



void Renderer::viewportTrans(const Vector3& in, Vector3& out)
{
	float halfW=(float)screen->width*0.5f;
	float halfH=(float)screen->height*0.5f;
	out.x=in.x*halfW+halfW;
	out.y=in.y*halfH+halfH;
	out.z=in.z*in.z;
	screenClamp(out);
}


void Renderer::line(const Vector3& p1, const Vector3 p2, const Colour& c)
{
	//modelview transform 
	
	Vector3 points[2]={p1,p2};
	for (int a=0;a<2;a++)
	{
		modelviewTrans(points[a],points[a]);
		perspectiveTrans(points[a],points[a]);
	}

	if (clipLine3D(points[0],points[1]))
	{
		for (int a=0;a<2;a++)
		{
			viewportTrans(points[a],points[a]);
		}
		screen->_line(	(int)points[0].x,(int)points[0].y,points[0].z,
						(int)points[1].x,(int)points[1].y,points[1].z,c.rgba);
	}
}

void Renderer::setProjection(const Matrix4& projection_in)
{
	projection=projection_in;
}
void Renderer::setModelview(const Matrix4& modelview_in)
{
	modelview=modelview_in;
	mvRotate=modelview.getRotation();

	//convert the light position to eye coordinates
	Vector3 mLight=light;
	modelviewRotate(light,mLight);
	screen->getShader().setLightPos(mLight); //update the shader's light position
}
//clips a polygon in post-perspective space 
//returns the number of vertices
int Renderer::clipPoly(Vector3 *verts, Fragment *frags, int n, ClipVert *vertsOut)
{
	ClipVert vertsA[16];
	for (int a=0;a<n;a++)
	{
		vertsOut[a].v=verts[a];
		vertsOut[a].f=frags[a];
		vertsA[a]=vertsOut[a];
	}
	int codeAnd=0,codeOr=0;
	for (int a=0;a<n;a++)
	{
		int code=getCSCode(verts[a]);
		codeAnd&=code;
		codeOr|=code;
	}
	if (codeAnd!=0) return 0;
	if (codeOr==0) return n;

	for (int b=0;b<6;b++)
	{
		int nOut=0;
		for (int a=0;a<n;a++)
		{
			int next=a+1;
			if (next==n) next=0;
			nOut+=clipPolyLine(vertsA[a], vertsA[next], b, &vertsOut[nOut]);			
		}
		n=nOut;
		for (int a=0;a<nOut;a++) 
		{
			vertsA[a]=vertsOut[a];
		}		
	}
	return n;
}

//cohen-sutherland encoding
int Renderer::getCSCode(const Vector3& point)
{
	int code=0;
	if (point.y>1.0f) 
		code|=1;
	if (point.y<-1.0f)
		code|=2;
	if (point.x>1.0f)
		code|=4;
	if (point.x<-1.0f)
		code|=8;
	if (point.z<0.0f)
		code|=16;
	if (point.z>1.0f)
		code|=32;
	return code;
}


//returns the number of points returned (0,1 or 2)
int Renderer::clipPolyLine(const ClipVert& cv1, const ClipVert& cv2, int edge, ClipVert* cvOut)
{
	const Vector3 &p1=cv1.v;
	const Vector3 &p2=cv2.v;
	Vector3 d=p2-p1;
	bool visible=false;
	float t1=0.0f;
	float t2=1.0f;

	switch(edge)
	{
	case 0:
		visible=clipEdge(d.x, -(p1.x+1.0f), t1,t2); //left
		break;
	case 1:
        visible=clipEdge(-d.x, p1.x-1.0f, t1,t2); //right
		break;
	case 2:
		visible=clipEdge(d.y, -(p1.y+1.0f), t1,t2); //bottom
		break;
	case 3:
		visible=clipEdge(-d.y, p1.y-1.0f, t1,t2); //top
		break;
	case 4:
		visible=clipEdge(d.z, -(p1.z), t1,t2); //front
		break;
	case 5:
		visible=clipEdge(-d.z, p1.z-1.0f, t1,t2); //back
	}
	if (!visible) 
	{
		return 0;
	}
	else
	{
		Fragment df=cv2.f-cv1.f;
		if (t2<1.0f) 
		{
			cvOut[0].v=p1+d*t2;
			cvOut[0].f=cv1.f+df*t2;
			return 1;
		}
		if (t1>0.0f) 
		{
			cvOut[0].v=p1+d*t1;
			cvOut[1].v=p2;

			cvOut[0].f=cv1.f+df*t1;
			cvOut[1].f=cv2.f;
			return 2;
		}
		cvOut[0].v=p2;
		cvOut[0].f=cv2.f;
		return 1;
	}
}

bool Renderer::clipLine3D(Vector3& p1, Vector3& p2)
{
	Vector3 d=p2-p1;
	bool visible=false;
	float t1=0.0f;
	float t2=1.0f;
	if (clipEdge(d.x, -(p1.x+1.0f), t1,t2)) //left
	{
		if (clipEdge(-d.x, p1.x-1.0f, t1,t2)) //right
		{
			if (clipEdge(d.y, -(p1.y+1.0f), t1,t2)) //bottom
			{
				if (clipEdge(-d.y, p1.y-1.0f, t1,t2)) //top
				{
					if (clipEdge(d.z, -(p1.z), t1,t2)) //front
					{
						if (clipEdge(-d.z, p1.z-1.0f ,t1,t2)) //back
						{
							visible=true;
							if (t2<1)
								p2=p1+d*t2;
							if (t1>0)
								p1+=d*t1;
						}
					}
				}
			}
		}
	}
	return visible;
}

void Renderer::screenClamp(Vector3& p1)
{
	//points should be clipped first!
	//to account for fp inaccuracy from viewport transform
	clamp(p1.x,0,(float)screen->width-1.0f);
	clamp(p1.y,0,(float)screen->height-1.0f);
}

bool Renderer::clipEdge(float denom, float num, float &t1, float &t2)
{
	float t;
	if (denom==0)
	{
		if (num>0) return false;
	}
	else if (denom>0)
	{
		t=num/denom;
		if (t>t2) return false;
		else if (t>t1) t1=t;
	}
	else if (denom<0)
	{
		t=num/denom;
		if (t<t1) return false;
		else if (t<t2) t2=t;
	}
	return true;
}

Vector3 Renderer::project(const Vector3& point)
{
	Vector3 out;
	modelviewTrans(point, out);
	perspectiveTrans(out, out);
	viewportTrans(out, out);
	return out;
}
