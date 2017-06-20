/*---------------------------------------------------------------------
*
* Copyright © 2016  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"
#include "TriMesh.h"
#include "ImageIO.h"

Scene::Scene()
{
	m_bgtex = NULL;
	InitDefaultScene();
}


Scene::~Scene()
{
	CleanupScene();
	if (m_bgtex) delete m_bgtex;
}

void Scene::InitDefaultScene()
{
	//Create a box and its material
	Primitive* newobj = new Box(Vector3(-4.0, 4.0, -20.0), 10.0, 15.0, 4.0);
	Material* newmat = new Material();
	//mat for the box1
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(0.8, 0.0, 0.0);
	newmat->SetSpecularColour(1.0, 1.0, 1.0);
	newmat->SetSpecPower(20);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	m_objectMaterials.push_back(newmat);

	newobj = new Box(Vector3(4.0, 4.0, -15.0), 4.0, 20.0, 4.0);
	newmat = new Material();
	//mat for the box2
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(0.8, 0.8, 0.8);
	newmat->SetSpecularColour(1.0, 1.0, 1.0);
	newmat->SetSpecPower(20);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	m_objectMaterials.push_back(newmat);

	//light
	newobj = new Box(Vector3(0.0, 21.5, -10.0), 10.0, 4.0, 10.0);
	newmat = new Material();
	//mat for the box2
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(0.0, 0.0, 0.0);
	newmat->SetEmissiveColour(12, 12, 12);
	newmat->SetSpecularColour(1.0, 1.0, 1.0);
	newmat->SetSpecPower(20);
	newmat->SetCastShadow(false);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	m_objectMaterials.push_back(newmat);

	//Create sphere 1 and its material
	newobj = new Sphere(3.0, 2, -3.5, 2.0); //sphere 2
	newmat = new Material();
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(0.0, 0.8, 0.0);
	newmat->SetSpecularColour(1.0, 1.0, 1.0);
	newmat->SetSpecPower(2);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	m_objectMaterials.push_back(newmat);

	//Create sphere 2 and its material
	newobj = new Sphere(-2.0, 3.0, -5.0, 3.0); //sphere 3
	newmat = new Material();
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(0.0, 0.0, 0.8);
	newmat->SetSpecularColour(1.0, 1.0, 1.0);
	newmat->SetSpecPower(20);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	m_objectMaterials.push_back(newmat);


	newobj = new Plane(); //an xz plane at the origin, floor
	static_cast<Plane*>(newobj)->SetPlane(Vector3(0.0, 1.0, 0.0), 0.0);
	newmat = new Material();
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(0.7, 0.7, 0.7);
	newmat->SetSpecularColour(0.0, 0.0, 0.0);
	newmat->SetSpecPower(10);
	newmat->SetCastShadow(false);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	m_objectMaterials.push_back(newmat);

	newobj = new Plane(); //an xz plane 40 units above, ceiling
	static_cast<Plane*>(newobj)->SetPlane(Vector3(0.0, -1.0, 0.0), -20.0);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);

	//material for front and back walls
	newmat = new Material();
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(0.0, 0.7, 0.0);
	newmat->SetSpecularColour(0.0, 0.0, 0.0);
	newmat->SetSpecPower(10);
	newmat->SetCastShadow(false);

	newobj = new Plane(); //an xy plane 40 units along -z axis, 
	static_cast<Plane*>(newobj)->SetPlane(Vector3(0.0, 0.0, 1.0), -40.0);
	m_sceneObjects.push_back(newobj);
	newobj->SetMaterial(newmat);

	newobj = new Plane(); //an xy plane 40 units along the z axis
	static_cast<Plane*>(newobj)->SetPlane(Vector3(0.0, 0.0, -1.0), -40.0);
	m_sceneObjects.push_back(newobj);
	newobj->SetMaterial(newmat);

	newobj = new Plane(); //an yz plane 20 units along -x axis
	static_cast<Plane*>(newobj)->SetPlane(Vector3(1.0, 0.0, 0.0), -20.0);
	newmat = new Material();
	newmat->SetAmbientColour(0.0, 0.0, 0.0);
	newmat->SetDiffuseColour(0.0, 0.0, 0.7);
	newmat->SetSpecularColour(0.0, 0.0, 0.0);
	newmat->SetSpecPower(10);
	newmat->SetCastShadow(false);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);
	m_objectMaterials.push_back(newmat);

	newobj = new Plane(); //an yz plane 20 units along +x axis
	static_cast<Plane*>(newobj)->SetPlane(Vector3(-1.0, 0.0, 0.0), -20.0);
	newobj->SetMaterial(newmat);
	m_sceneObjects.push_back(newobj);

	//Create one light source for the scene
	Light *newlight = new Light();
	newlight->SetLightPosition(0.0, 20.0, 10.0);
	m_lights.push_back(newlight);

	//A blue background
	m_background.SetVector(0.25, 0.6, 1.0);

	//default scene width and height;
	m_sceneWidth = 1.33333333;
	m_sceneHeight = 1.0;

	//default camera position and look at
	m_activeCamera.SetPositionAndLookAt(Vector3(-20.0, 7.0, 23.0), Vector3(-15.0, 10.0, 10.0));
}
//#if 1
//void Scene::InitTexturedScene()
//{
//	//the default scene consists of 3 spheres and a plane as the ground
//
//	//Create a box and its material
//	//Primitive* newobj = new Box(Vector3(-2.0, 4.0, -8.0), 3.0, 10.0, 4.0);
//	//Primitive* newobj = new Sphere(0.0, 7, 0.0, 2.0); //sphere 2
//	Primitive* newobj = new TriMesh("../asset/house.obj"); //sphere 2
//	Material* newmat = new Material();
//	//mat for the box
//	newmat->SetAmbientColour(0.0, 0.0, 0.0);
//	newmat->SetDiffuseColour(1.0, 0.0, 0.0);
//	newmat->SetSpecularColour(0.0, 0.0, 0.0);
//	newmat->SetSpecPower(20);
//	newmat->SetTextureFromFile(Texture::TEXUNIT_DIFFUSE, "../asset/house_diffuse.tga");
//	newmat->SetTextureFromFile(Texture::TEXUNIT_NORMAL, "../asset/house_normal.tga");
//	newobj->SetMaterial(newmat);
//	m_sceneObjects.push_back(newobj);
//	m_objectMaterials.push_back(newmat);
//
//	newobj = new TriMesh("../asset/windmill.obj");
//	newmat = new Material();
//	//mat for the box
//	newmat->SetAmbientColour(0.0, 0.0, 0.0);
//	newmat->SetDiffuseColour(1.0, 0.0, 0.0);
//	newmat->SetSpecularColour(0.0, 0.0, 0.0);
//	newmat->SetSpecPower(20);
//	newmat->SetTextureFromFile(Texture::TEXUNIT_DIFFUSE, "../asset/windmill_diffuse.tga");
//	newmat->SetTextureFromFile(Texture::TEXUNIT_NORMAL, "../asset/windmill_normal.tga");
//	newobj->SetMaterial(newmat);
//	m_sceneObjects.push_back(newobj);
//	m_objectMaterials.push_back(newmat);
//
//	newobj = new TriMesh("../asset/canyon.obj");
//	newmat = new Material();
//	//mat for the box
//	newmat->SetAmbientColour(0.0, 0.0, 0.0);
//	newmat->SetDiffuseColour(1.0, 0.0, 0.0);
//	newmat->SetSpecularColour(0.0, 0.0, 0.0);
//	newmat->SetSpecPower(20);
//	newmat->SetTextureFromFile(Texture::TEXUNIT_DIFFUSE, "../asset/canyon_diffuse.tga");
//	newobj->SetMaterial(newmat);
//	newmat->SetCastShadow(false);
//	m_sceneObjects.push_back(newobj);
//	m_objectMaterials.push_back(newmat);
//
//	int texwidth, texheight, nchannels, bpp;
//	m_bgtex = new Texture();
//	ImageIO::LoadTGA("../asset/sky.tga", &(m_bgtex->mImage), &texwidth, &texheight, &bpp, &nchannels);
//
//	m_bgtex->mChannels = nchannels;
//	m_bgtex->mHeight = texheight;
//	m_bgtex->mWidth = texwidth;
//
//	Light *newlight = new Light();
//	newlight->SetLightPosition(-3.0, 10.0, 10.0);
//	newlight->SetLightColour(1.0, 1.0, 1.0);
//	m_lights.push_back(newlight);
//
//	//A blue background
//	m_background[0] = 0.25;
//	m_background[1] = 0.6;
//	m_background[2] = 1.0;
//
//	//default scene width and height;
//	m_sceneWidth = 1.33333333;
//	m_sceneHeight = 1.0;
//
//	//default camera position and look at
//	m_activeCamera.SetPositionAndLookAt(Vector3(4.0, 1.0, 7.0), Vector3(-2.0, 3.0, 0.0));
//}
//#endif
void Scene::CleanupScene()
{
	//Cleanup object list
	std::vector<Primitive*>::iterator prim_iter = m_sceneObjects.begin();

	while (prim_iter != m_sceneObjects.end())
	{
		delete *prim_iter;
		prim_iter++;
	}

	m_sceneObjects.clear();

	//Cleanup material list
	std::vector<Material*>::iterator mat_iter = m_objectMaterials.begin();

	while (mat_iter != m_objectMaterials.end())
	{
		delete *mat_iter;
		mat_iter++;
	}
	m_objectMaterials.clear();

	//cleanup light list
	std::vector<Light*>::iterator lit_iter = m_lights.begin();

	while (lit_iter != m_lights.end())
	{
		delete *lit_iter;
		lit_iter++;
	}

	m_lights.clear();
}

RayHitResult Scene::IntersectByRay(Ray& ray, bool isShadowRay)
{
	RayHitResult result = Ray::s_defaultHitResult;

	std::vector<Primitive*>::iterator prim_iter = m_sceneObjects.begin();

	if (!isShadowRay)
	{
		while (prim_iter != m_sceneObjects.end())
		{
			RayHitResult current;

			current = (*prim_iter)->IntersectByRay(ray);

			if (current.t > 0.0 && current.t < result.t)
			{
				result = current;
			}

			prim_iter++;
		}
	}
	else
	{
		while (prim_iter != m_sceneObjects.end())
		{
			RayHitResult current = result;
			bool castShadow = (*prim_iter)->GetMaterial()->CastShadow();

			current = (*prim_iter)->IntersectByRay(ray);

			if (castShadow && current.t > 0.0 && current.t < result.t)
			{
				std::vector<Light*>::iterator iter = m_lights.begin();

				while (iter != m_lights.end())
				{
					Vector3 r = ray.GetRay();
					double ldotr = (*iter)->GetLightPosition().DotProduct(r);
					double pdotr = result.point.DotProduct(r);

					if (pdotr < ldotr)
					{
						result = current;
					}

					iter++;
				}

			}

			prim_iter++;
		}
	}

	return result;
}
