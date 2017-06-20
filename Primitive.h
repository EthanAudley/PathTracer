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

#include "Ray.h"

class Material;

class Primitive
{
	private:
		Material				*m_pMaterial;
		
	public:
		enum PRIMTYPE
		{
			PRIMTYPE_Plane = 0,
			PRIMTYPE_Sphere,
			PRIMTYPE_Triangle,
			PRIMTYPE_Box,
			PRIMTYPE_TRIMESH
		};

		PRIMTYPE				m_primtype;

								Primitive(){ m_pMaterial = nullptr; }
		virtual					~Primitive(){ ; }


		virtual RayHitResult			IntersectByRay(Ray& ray) = 0;

		inline void				SetMaterial(Material* pMat)
		{
			m_pMaterial = pMat;
		}

		inline Material*		GetMaterial()
		{
			return m_pMaterial;
		}
};
