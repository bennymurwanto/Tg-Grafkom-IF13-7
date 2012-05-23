//////////////////////////////////////////////////////////////////////
// Screen.cpp
// Description:		This class contains the colour buffer and Z buffer. 
//					It is responsible for sending the pixels to the 
//					screen for display. 
//
//					This class also handles rasterisation of primitives. 
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\screen.h"
#include "fragment.h"

Screen::Screen()
{
	renderType=RT_TEXSUBIMAGE;
//	renderType=RT_GLDRAWPIXELS;
	texSize=1024;
	cBuffer=NULL;
	zBuffer=NULL;
	edge=new int* [2];
	edge[0]=NULL;
	edge[1]=NULL;
	cullFace=false;
	createSplatFilters();
}

Screen::~Screen()
{
	if (cBuffer) delete [] cBuffer;
	if (zBuffer) delete [] zBuffer;
	if (edge)	
	{
		if (edge[0]) delete [] edge[0];
		if (edge[1]) delete [] edge[1];
		delete [] edge;
	} 
	destroySplatFilters();
}

bool Screen::init(int width_in, int height_in)
{
	if (GLDisplay::inst().createMainWindow("Bezier Patch Renderer",width_in,height_in,32,0,false))
	{
		GLDisplay::inst().resizeWindow(width_in,height_in,32,0,false);
		resize(width_in,height_in);
		initTex();

		return true;
	}
	return false;
}

bool Screen::shutdown()
{
	glDeleteTextures(1,&tex);
	GLDisplay::inst().destroyMainWindow();
	return true;
}

bool Screen::resize(int width_in, int height_in)
{
	if (width_in<1 || width_in>2048 || height_in<1 || height_in>2048)
		return false;

	width=width_in;
	height=height_in;
	GLDisplay::inst().resizeGL(width,height);
	numFragments=width*height;
	if (cBuffer) delete [] cBuffer;
	cBuffer=new uint[numFragments];
	if (zBuffer) delete [] zBuffer;
	zBuffer=new float[numFragments];
	if (edge)
	{
		if (edge[0]) delete [] edge[0];
		if (edge[1]) delete [] edge[1];
		edge[0]=new int[height];
		edge[1]=new int[height];
	}
	shader.init(cBuffer,zBuffer);
	return true;
}

void Screen::clear()
{
	memset(cBuffer,0,sizeof(uint)*numFragments);
	for (int i=0;i<numFragments;i++) zBuffer[i]=1.0f;
}

void Screen::line(float x1, float y1, float z1, float x2, float y2, float z2, const Colour& c)
{
	if (clipLine2D(x1,y1,x2,y2))
		_line(round(x1),round(y1),z1,round(x2),round(y2),z2,c.rgba);
}

//returns true if the line is visible, or false otherwise
bool Screen::clipLine2D(float &x1, float &y1, float &x2, float &y2)
{
	float dx=x2-x1;
	float dy=y2-x1;
	if (dx==0 && dy==0 && x1<width && x1>=0 && x2<width && x2>=0) return true;
	bool visible=false;
	int maxX=width-1;
	int maxY=height-1;
	float t1=0.0f;
	float t2=1.0f;
	if (clipEdge(dx,0-x1,t1,t2)) //left edge
	{
		if (clipEdge(-dx,x1-maxX,t1,t2)) //right edge
		{
			if (clipEdge(dy,0-y1,t1,t2)) //bottom edge
			{
				if (clipEdge(-dy,y1-maxY,t1,t2)) //top edge
				{
					visible=true;
					if (t2<1)
					{
						x2=x1+dx*t2;
						y2=y1+dy*t2;
					}
					if (t1>0)
					{
						x1+=dx*t1;
						y1+=dy*t1;
					}
				}
			}
		}
	}
	return visible;
}

bool Screen::clipEdge(float denom, float num, float &t1, float &t2)
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


//render a line that has already been clipped
void Screen::_line(int x1, int y1, float z1, int x2, int y2, float z2, uint col)
{
	int dx=x2-x1;

	if (dx<0) //swap endpoints so there are only 4 cases to deal with
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(z1,z2);
		dx=-dx;
	}
	int dy=y2-y1;
	float dz=z2-z1;
	float incZ;

	int incNE,incE,fIncNE,fIncE;
	int d;
	if (dy>=0)
	{
		if (dx<dy)
		{
			incE=2*dx;
			incNE=2*(dx-dy);
			fIncE=width;
			fIncNE=width+1;
			d=dx*2-dy;
			if (dy>0) incZ=dz/(float)dy;
			else incZ=0.0f;

		}else
		{
			incE=2*dy;
			incNE=2*(dy-dx);
			fIncE=1;
			fIncNE=width+1;
			d=dy*2-dx;
			if (dx>0) incZ=dz/(float)dx;
			else incZ=0.0f;
		}
	}else
	{
		if (dx<-dy)
		{
			incE=2*dx;
			incNE=2*(dx+dy);
			fIncE=-width;
			fIncNE=-width+1;
			d=dx*2+dy;
			if (dy<0) incZ=dz/(float)-dy;
			else incZ=0.0f;
		}else
		{
			incE=2*-dy;
			incNE=2*(-dy-dx);
			fIncE=1;
			fIncNE=-width+1;
			d=-dy*2-dx;
			if (dx>0) incZ=dz/(float)dx;
			else incZ=0.0f;
		}
	}
	int x=x1,y=y1;
	float z=z1;
	int fPos=y*width+x;
	int fPos2=y2*width+x2;
	_plot(fPos,z,col);
    while(fPos!=fPos2)
	{
		if (d<=0)
		{
			d+=incE;
			fPos+=fIncE;
		}else
		{
			d+=incNE;
			fPos+=fIncNE;
		}
		z+=incZ;
		_plot(fPos,z,col);
	}
	_plot(fPos,z1,col);
}


//renders a convex polygon
void Screen::_poly(const Vector3 *points, const Fragment *fragments, int n, const Colour& colour, bool cw)
{
	if (n>32 || n<3) return;
	int order[32];

	shader.setColour(colour);
	shader.setSpecColour(WHITE);

	//determine if the triangle is anticlockwise
	//bool cw=isPolyCW(points,n);
	if (cw & cullFace) return;

	getPointYOrder(points,n,order);

	//get the left and right lines
	PolyLine line[32];

	int numL=1,numR=0;
	int startR=0;
	int edgeNum=0; //start at left side
	int lastV=n-1;
	int nextPoint=order[0]+1;
	if (nextPoint==n) nextPoint=0;
	line[0].p1=order[0];
	line[0].p2=nextPoint;
	line[0].f1=fragments[order[0]];
	line[0].f2=fragments[nextPoint];
	line[0].edge=0;
	for (int a=1;a<n;a++)
	{
		if (nextPoint==order[lastV]) 
		{
			edgeNum=1;
			startR=a;
		}
		line[a].p1=nextPoint;
		line[a].f1=fragments[nextPoint];
		nextPoint++;
		if (nextPoint==n) nextPoint=0;
		line[a].p2=nextPoint;
		line[a].f2=fragments[nextPoint];
		line[a].edge=edgeNum;
		if (edgeNum==0) numL++;
		else numR++;
	}

	int startL=0;
	//if anticlockwise then swap the edges
	if (!cw) 
	{
		for (int a=0;a<n;a++)
		{
			if (line[a].edge==0) line[a].edge=1;
			else line[a].edge=0;
		}
		swap(startL,startR);
		swap(numL,numR);

		//flip the normals
		for (int a=0;a<n;a++)
		{
			line[a].f1.norm*=-1.0f;
			line[a].f2.norm*=-1.0f;
		}
	}
	PolyLine* lineL=&line[startL];
	PolyLine* lineR=&line[startR];	

	if (!cw)
	{
		//reverse the left-hand edges
		int halfNumL=numL>>1;
		for (int a=0;a<halfNumL;a++)
		{
			int b=numL-1-a;
			swap(lineL[a].p1, lineL[b].p1);
			swap(lineL[a].p2, lineL[b].p2);
			swap(lineL[a].f1, lineL[b].f1);
			swap(lineL[a].f2, lineL[b].f2);
		}
		//reverse the point order
		for (int a=0;a<numL;a++)
		{
			swap(lineL[a].p1,lineL[a].p2);
			swap(lineL[a].f1,lineL[a].f2);
		}
	}
	else
	{
		//reverse the right-hand edges
		int halfNumR=numR>>1;
		for (int a=0;a<halfNumR;a++)
		{
			int b=numR-1-a;
			swap(lineR[a].p1, lineR[b].p1);
			swap(lineR[a].p2, lineR[b].p2);
			swap(lineR[a].f1, lineR[b].f1);
			swap(lineR[a].f2, lineR[b].f2);
		}
		//reverse the point order
		for (int a=0;a<numR;a++)
		{
			swap(lineR[a].p1,lineR[a].p2);
			swap(lineR[a].f1,lineR[a].f2);
		}
	}

	//scan the edges
	for (int a=0;a<n;a++)
	{
		const Vector3& p1=points[ line[a].p1 ];
		const Vector3& p2=points[ line[a].p2 ];
		edgeScan(round(p1.y),round(p2.y),round(p1.x),round(p2.x),edge[ line[a].edge ]);
	}

	//build the vertical segments
	PolySegment segs[32];
	int numSegments=getPolySegments(points,lineL,lineR,numL,numR,segs);

	Fragment fL,fR;

	int y;
	//render the segment scanlines
	for (int a=0;a<numSegments;a++)
	{
		int y1=segs[a].y1;
		int y2=segs[a].y2;
		int sdy=y2-y1;
		if (sdy==0) continue; //skip empty segments 
//		assert(sdy>=0);
		if (sdy<=0) return;
		float t=1.0f/(float)sdy;
		Fragment fL=segs[a].fL1;
		Fragment fR=segs[a].fR1;
		Fragment fIncL=(segs[a].fL2-fL)*t;
		Fragment fIncR=(segs[a].fR2-fR)*t;
		for (y=y1;y<y2;y++)
		{
			scanLine(y,edge[0][y],edge[1][y],fL,fR);
			fL+=fIncL;
			fR+=fIncR;
		}
	}
	fL=segs[numSegments-1].fL1; //render the final scanline
	fR=segs[numSegments-1].fR1;
	y=segs[numSegments-1].y2;
	scanLine(y,edge[0][y],edge[1][y],fL,fR); 

/*	for (int a=0;a<n;a++)
	{
		Vector3 col=fragments[a].eyePos;
		cross(round(points[a].x),round(points[a].y), WHITE);
	}
*/	
}


int Screen::getPolySegments( const Vector3 *points, 
							 const PolyLine *lineL, const PolyLine *lineR, 
							 int numL, int numR, 
							 PolySegment *segments_out)
{
	int n=numL+numR;
	int maxLines=max(numL,numR);
	int s=0;
	int iL=-1,iR=-1;
	int nextL=0,nextR=0;
	bool doneL=false, doneR=false;
	for (int a=0;a<n;a++)
	{
		nextL=iL+1;
		nextR=iR+1;

		if (nextL>=numL)
			doneL=true;
		if (nextR>=numR)
			doneR=true;
		if (doneL && doneR) break;

		int nextYL= doneL ? height : round(points[ lineL[nextL].p1 ].y);
		int nextYR= doneR ? height : round(points[ lineR[nextR].p1 ].y);

		//the segment starts at the lowest nextY value
		if (nextYL<nextYR)
		{
			segments_out[s].y1=nextYL;
			segments_out[s].fL1=lineL[nextL].f1;

			//interpolate to get the right-hand fragment
			int yR1=round(points[lineR[iR].p1].y);
			int yR2=round(points[lineR[iR].p2].y);
			float lineDy=(float)(yR2-yR1);
			if (lineDy==0) segments_out[s].fR1= (lineR[iR].f1 + lineR[iR].f2)*0.5;
			else
			{
				float t=(float)(nextYL-yR1)/lineDy;
				segments_out[s].fR1=lineR[iR].f1+ (lineR[iR].f2-lineR[iR].f1)*t;
			}
			s++;
			iL++;
		}
		else if (nextYR<nextYL) //right side
		{
			segments_out[s].y1=nextYR;
			segments_out[s].fR1=lineR[nextR].f1;

			//interpolate to get the left-hand fragment
			int yL1=round(points[lineL[iL].p1].y);
			int yL2=round(points[lineL[iL].p2].y);
			float lineDy=(float)(yL2-yL1);
			if (lineDy==0) segments_out[s].fL1= (lineL[iL].f1 + lineL[iL].f2)*0.5;
			else
			{
				float t=(float)(nextYR-yL1)/lineDy;
				segments_out[s].fL1=lineL[iL].f1+ (lineL[iL].f2-lineL[iL].f1)*t;
			}
			s++;
			iR++;
		}
		else //nextYL==nextYR
		{
			segments_out[s].y1=nextYL;
			segments_out[s].fL1=lineL[nextL].f1;
			segments_out[s].fR1=lineR[nextR].f1;
			s++;
			iR++;
			iL++;
		}
	}
	for (int a=0;a<s-1;a++) //set segment y2 and f2 values
	{
		segments_out[a].y2=segments_out[a+1].y1;
		segments_out[a].fL2=segments_out[a+1].fL1;
		segments_out[a].fR2=segments_out[a+1].fR1;
	}
	int lastPoint=lineL[numL-1].p2; 
	segments_out[s-1].y2=round(points[ lastPoint ].y);
	segments_out[s-1].fL2=segments_out[s-1].fR2=lineL[numL-1].f2;
	return s;
}

void Screen::edgeScan(int y1, int y2, int x1, int x2, int *scanEdge)
{
	int num=x2-x1;
	int den=y2-y1;

	if (den==0) 
	{
		scanEdge[y1]=x1;
		return;
	}

	int inc=den;
	int x=x1;
	if (num>=0)
	{
		if (den>=num)
		{
			for (int y=y1;y<=y2;y++)
			{
				scanEdge[y]=x;
				inc+=num;
				if (inc>den)
				{
					x++;
					inc-=den;
				}
			}
		}else
		{
			for (int y=y1;y<=y2;y++)
			{
				scanEdge[y]=x;
				inc+=num;
				while (inc>den)
				{
					x++;
					inc-=den;
				}
			}
		}
	}
	else
	{
		if (den>=-num)
		{
			for (int y=y1;y<=y2;y++)
			{
				scanEdge[y]=x;
				inc-=num;
				if (inc>den)
				{
					x--;
					inc-=den;
				}
			}
		}else
		{
			for (int y=y1;y<=y2;y++)
			{
				scanEdge[y]=x;
				inc-=num;
				while (inc>den)
				{
					x--;
					inc-=den;
				}
			}
		}
	}
}


void Screen::scanLine(int y, int xL, int xR, const Fragment& fL, const Fragment& fR)
{
	int linePos=y*width;
	int posL=linePos+xL;
	int posR=linePos+xR;
	Fragment fInc=(fR-fL)/(float)(xR-xL+1);
	Fragment f=fL;

    for (int a=posL;a<=posR;a++)
	{
		shader.plotFragment(a,f);
		f+=fInc;
	}
//	plot(xL,y,RED);
//	plot(xR,y,BLUE);
}

bool Screen::isPolyCW(const Vector3 *points, int n)
{
	float maxD=-1.0f;
	for (int b=0;b<n-1;b++)
	{
		float lA=points[b+1].y-points[b].y; //form the line equation for the first line (Ax+By+C=0)
		float lB=-(points[b+1].x-points[b].x);
		float len=sqrt(lA*lA+lB*lB);
		if (len==0) continue;
		lA/=len;
		lB/=len;
		float lC=-(lA*points[b].x+lB*points[b].y);
		for (int a=0;a<n;a++)
		{
			if (a==b || a==b+1) continue;
			float px=points[a].x;
			float py=points[a].y;
			float d=lA*px+lB*py+lC;
			if (fabs(d)>=4.0f)  //break if the distance is >=4 pixels
			{
				if (d>0) return true;
				else return false;
				break;
			}
			else 
			{
				if (d>maxD) maxD=d;
			}
		}
	}
	if (maxD>=0) return true;
	else return false;
}

void Screen::cross(int x, int y, const Colour& c)
{
	for (int b=-3;b<3;b++)
	{
		plot((float)x+b,(float)y+b,c);
		plot((float)x-b,(float)y+b,c);
	}
}

void Screen::getPointYOrder(const Vector3* points, int n, int *order_out)
{
	//get the sorted order of the points, ordered by their Y values (using an early exit selection sort)
	for (int a=0;a<n;a++) order_out[a]=a;
	for (int a=0;a<n;a++)
	{
		bool done=true;
		int maxI=0;
		float maxY=points[order_out[0]].y;
		int nb=n-a;
		for (int b=1;b<nb;b++)
		{
			float cY=points[order_out[b]].y;
			if (cY>maxY)
			{
				maxY=cY;
				maxI=b;
			}
			else done=false;
		}
		if (done) break;
		swap(order_out[maxI],order_out[nb-1]);
	}

}

void Screen::swapBuffers()
{
	glClear(GL_COLOR_BUFFER_BIT);
	if (RT_GLDRAWPIXELS)
		glDrawPixels(width,height,GL_RGBA,GL_UNSIGNED_BYTE,(void *)cBuffer);
	else
	{
		int halfWidth=width>>1;
		int halfHeight=height>>1;
		glBindTexture(GL_TEXTURE_2D,tex);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor4f(1,1,1,1);
		double tcW=(double)width/(double)texSize;
		double tcH=(double)height/(double)texSize;
		glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,(GLubyte *)cBuffer);
		glBegin(GL_QUADS);
			glTexCoord2d(0.0,0.0);	glVertex2i(-halfWidth,-halfHeight);
			glTexCoord2d(0.0,tcH);	glVertex2i(-halfWidth,halfHeight);
			glTexCoord2d(tcW,tcH);	glVertex2i(halfWidth,halfHeight);
			glTexCoord2d(tcW,0.0);	glVertex2i(halfWidth,-halfHeight);
		glEnd();
	}
	GLDisplay::inst().swapBuffers();
}

void Screen::initTex()
{
	glEnable(GL_TEXTURE_2D);
	BYTE *temp=new BYTE[texSize*texSize*3];
	memset(temp,0,1024*1024*3);
	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texSize, texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, temp);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);	

	if (glGetError()!=GL_NO_ERROR) 
	{
		int a=1;
	}
	delete [] temp;
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}

void Screen::_polyLine(const Vector3 *points, int n, const Colour& c)
{
	if (n<3) return;
	for (int a=0;a<n;a++)
	{
		int next=a+1;
		if (next==n) next=0;
		_line(	round(points[a].x),round(points[a].y),points[a].z,
				round(points[next].x),round(points[next].y),points[next].z,c.rgba);
	}
}

void Screen::renderSplat(int x, int y, Fragment& fragment, const Colour& colour, int size)
{
	size=min(size,MAX_SPLATSIZE);
	shader.setColour(colour);
	shader.setSpecColour(WHITE);

	int halfSize=size>>1;
	int ox=x-halfSize,oy=y-halfSize;

	int fPos=oy*width+ox;
	Colour col=shader.getColour(fragment);
	float z=fragment.z;
	int c=0;
	float *splatFilter=splatFilters[size-1];
	for (int a=0;a<size;a++)
	{
		for (int b=0;b<size;b++,c++)
		{
			plotSplatPixel(fPos+b,col,splatFilter[c],z);
		}
		fPos+=width;
	}
}

void Screen::createSplatFilters()
{
	for (int a=0;a<MAX_SPLATSIZE;a++)
	{
		int size=a+1;
		splatFilters[a]=new float [size*size];
	}

	for (int a=0;a<MAX_SPLATSIZE;a++)
	{
		int size=a+1;
		float halfSize=(float)size*0.5f;
		float offsetX;
		float offsetY=-1.0f;
		float incOffset=2.0f/(float)size;

		//make a radial splat filter
		int c=0;
		for (int y=0;y<size;y++)
		{
			offsetX=-1.0f;
			for (int x=0;x<size;x++,c++)
			{
				float avX=offsetX+incOffset*0.5f;
				float avY=offsetY+incOffset*0.5f;
				float r=sqrt(avX*avX+avY*avY);
				splatFilters[a][c]=min(max(1.0f-r,0),1.0f);
				offsetX+=incOffset;
			}
			offsetY+=incOffset;
		}
	}
}

void Screen::destroySplatFilters()
{
	for (int a=0;a<MAX_SPLATSIZE;a++)
	{
		delete [] splatFilters[a];
	}
}
