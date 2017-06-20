/*---------------------------------------------------------------------
*
* Copyright © 2015  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#pragma once

#include <stdlib.h>

#include "Vector3.h"

typedef Vector3 Colour;

class Texture
{
public:
	enum TEXUNIT
	{
		TEXUNIT_DIFFUSE = 0,
		TEXUNIT_NORMAL = 1
	};
public:
	unsigned int	mWidth;
	unsigned int    mHeight;
	unsigned int    mChannels;
	unsigned char*	mImage;

	Texture()
	{
		mImage = NULL;
	}

	~Texture()
	{
		if (mImage) delete[] mImage;
	}

	Colour GetTexelColour(double u, double v)
	{
		Colour colour;

		int dx = u*mWidth;
		int dy = v*mHeight;
		int texel_stride = mChannels;
		int row_stride = mWidth;
		int texel_loc = (dx*texel_stride) + dy*row_stride*texel_stride;

		unsigned char* comp = mImage + texel_loc;

		colour[0] = *(comp) / 255.0f;
		colour[1] = *(comp + 1) / 255.0f;
		colour[2] = *(comp + 2) / 255.0f;

		return colour;
	}
};

class Material
{
	private:
		Colour mEmissive;
		Colour mAmbient;
		Colour mDiffuse;
		Colour mSpecular;
		double mSpecpower;
		Texture* mDiffuse_texture;
		Texture* mNormal_texture;
		bool mCastShadow;

	public:
		
		Material();
		~Material();

		void SetDefaultMaterial();
		void SetEmissiveColour(float r, float g, float b);
		void SetAmbientColour(float r, float g, float b);
		void SetDiffuseColour(float r, float g, float b);
		void SetSpecularColour(float r, float g, float b);
		void SetSpecPower(double spow);

		inline void SetCastShadow(bool castShadow)
		{
			mCastShadow = castShadow;
		}

		inline Colour& GetEmissiveColour()
		{
			return mEmissive;
		}

		inline Colour& GetAmbientColour()
		{
			return mAmbient;
		}
		
		inline Colour& GetDiffuseColour()
		{
			return mDiffuse;
		}

		inline Colour& GetSpecularColour()
		{
			return mSpecular;
		}

		inline double GetSpecPower()
		{
			return mSpecpower;
		}

		inline bool CastShadow()
		{
			return mCastShadow;
		}

		inline bool HasDiffuseTexture()
		{
			return mDiffuse_texture != NULL;
		}

		inline bool HasNormalTexture()
		{
			return mNormal_texture != NULL;
		}

		Colour SampleColour(Texture::TEXUNIT unit, double u, double v);
		void SetTextureFromFile(Texture::TEXUNIT unit, const char* filename);

};

