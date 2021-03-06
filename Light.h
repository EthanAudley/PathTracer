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

#include "Vector3.h"
#include "Material.h"

class Light
{
	private:
		Vector3			mPosition;
		Colour			mColour;

	public:
		Light();
		~Light();

		void InitDefaultLight();
		void SetLightPosition(double x, double y, double z);
		void SetLightColour(double r, double g, double b);

		inline Vector3& GetLightPosition()
		{
			return mPosition;
		}
		inline Colour& GetLightColour()
		{
			return mColour;
		}
};

