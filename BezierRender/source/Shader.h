//////////////////////////////////////////////////////////////////////
// Shader.h
// Description:		Specifies a per-pixel lighting effect. Inputs 
//					fragment information and outputs a colour. 
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#pragma once

class Shader
{
public:
	Shader()	
	{
		colour=WHITE; 
		specColour=WHITE; 
		shininess=2;
		light.set(0,0,1);
	}
	~Shader()	{}
	void init(uint *cBuffer_in, float *zBuffer_in)
	{
		cBuffer=cBuffer_in; 
		zBuffer=zBuffer_in; 
	}

	void setLightPos(const Vector3& light_in)	{light=light_in;}
	void setSpecColour(const Colour& spec)		{specColour=spec;}
	void setColour(const Colour& col)			{colour=col;}
	void setShininess(int shininess_in)			{shininess=shininess_in;}

	void setOpacity(float opacity_in)			{opacity=opacity_in;}

	void plotFragment(int fPos, const Fragment& fragment)
	{
		if (fragment.z<=zBuffer[fPos]) 
		{
			cBuffer[fPos]=getColour(fragment).rgba;
			zBuffer[fPos]=fragment.z;
		}
	}

	Colour &getColour(const Fragment& fragment)
	{
		aI=0.2f;
		norm=fragment.norm;
		norm.normalise();
		dI=norm.dot(light);
		if (dI<0.0f) dI=0.0f;			
		V=fragment.eyePos;
		V.normalise();
		H=light+V;		//calculate halfangle vector
		H.normalise();
		sI=norm.dot(H);	//get specular
		if (sI<0.0f) sI=0.0f;
		else
		{   //raise specular power according to shininess 
			for (int a=shininess;a>0;a--)
			{
				sI*=sI;
				sI*=sI;
				sI*=sI; 
			}
		}
		colS=specColour; //multiply the specular value by its colour
		colS*=sI; 

		col=colour;		//combine the specular, ambient and diffuse components to get the final colour
		col*=(dI+aI);
		col+=colS;
		return col;
	}

private:
	Colour specColour;
	Colour colour;

	int shininess;

	float opacity;

	//temporaries
	Colour col,colS;
	Vector3 H,V;
	Vector3 norm;
	float sI,dI,aI;

	Vector3 light;
	uint *cBuffer;
	float *zBuffer;
};
