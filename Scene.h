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

#include "Camera.h"
#include "Primitive.h"
#include "Material.h"
#include "Light.h"
#include <vector>

class Scene
{
	private:
		Camera							m_activeCamera;
		
		std::vector<Primitive*>			m_sceneObjects;
		std::vector<Material*>			m_objectMaterials;
		std::vector<Light*>				m_lights;

		Colour							m_background;
		Texture							*m_bgtex;
		double							m_sceneWidth;
		double							m_sceneHeight;

	public:
		Scene();
		~Scene();

		void InitDefaultScene();

		//void InitTexturedScene();

		inline void SetSceneWidth(double width)
		{
			m_sceneWidth = width;
		}

		inline Camera* GetSceneCamera()
		{
			return &m_activeCamera;
		}

		inline double GetSceneWidth() 
		{
			return m_sceneWidth;
		}

		inline double GetSceneHeight() 
		{
			return m_sceneHeight;
		}

		inline Colour& GetBackgroundColour()
		{
			return m_background;
		}
		
		inline Colour GetBackgroundColour(double u, double v)
		{
			return m_bgtex ? m_bgtex->GetTexelColour(u,v) : m_background;
		}

		RayHitResult IntersectByRay(Ray& ray, bool isShadowRay = false);

		inline std::vector<Light*>* GetLightList()
		{
			return &m_lights;
		}
		
		void		CleanupScene();
		
};

