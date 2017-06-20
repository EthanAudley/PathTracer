/*---------------------------------------------------------------------
*
* Copyright © 2015  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#if defined(WIN32) || defined(_WINDOWS)
#include <Windows.h>
#include <gl/GL.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif

#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"
#include "Camera.h"
#include "perlin.h"

void RayTracer::DoTrace( Scene* pScene )
{
	Camera* cam = pScene->GetSceneCamera();
	
	Vector3 camRightVector = cam->GetRightVector();
	Vector3 camUpVector = cam->GetUpVector();
	Vector3 camViewVector = cam->GetViewVector();
	Vector3 centre = cam->GetViewCentre();
	Vector3 camPosition = cam->GetPosition();

	double sceneWidth = pScene->GetSceneWidth();
	double sceneHeight = pScene->GetSceneHeight();

	double pixelDX = sceneWidth / m_buffWidth;
	double pixelDY = sceneHeight / m_buffHeight;
	
	int total = m_buffHeight*m_buffWidth;
	int done_count = 0;
	
	Vector3 start;

	start[0] = centre[0] - ((sceneWidth * camRightVector[0])
		+ (sceneHeight * camUpVector[0])) / 2.0;
	start[1] = centre[1] - ((sceneWidth * camRightVector[1])
		+ (sceneHeight * camUpVector[1])) / 2.0;
	start[2] = centre[2] - ((sceneWidth * camRightVector[2])
		+ (sceneHeight * camUpVector[2])) / 2.0;
	
	Colour scenebg = pScene->GetBackgroundColour();

	if (m_renderCount == 0)
	{
		fprintf(stdout, "Trace start.\n");

		clock_t time = clock();
#pragma omp parallel for schedule (dynamic, 1)
		for (int i = 0; i < m_buffHeight; i+=1) {
			fprintf(stdout, "\rRendering %5.2f%% (%.2fs Time taken)", 100.* i / (m_buffHeight - 1), (double)(clock() - time) / CLOCKS_PER_SEC);
			for (int j = 0; j < m_buffWidth; j+=1) {

				//calculate the metric size of a pixel in the view plane (e.g. framebuffer)
				Vector3 pixel;

				// Anti-Aliasing
				for (float x = 0.25f; x <= 1.f; x += 0.25f)
				{
					for (float y = 0.25f; y <= 1.f; y += 0.25f)
					{
						pixel[0] = start[0] + (i + x) * camUpVector[0] * pixelDY
							+ (j + y) * camRightVector[0] * pixelDX;
						pixel[1] = start[1] + (i + x) * camUpVector[1] * pixelDY
							+ (j + y) * camRightVector[1] * pixelDX;
						pixel[2] = start[2] + (i + x) * camUpVector[2] * pixelDY
							+ (j + y) * camRightVector[2] * pixelDX;

						/*
						* setup view ray
						* In perspective projection, each view ray originates from the eye (camera) position
						* and pierces through a pixel in the view plane
						*/
						Ray viewray;
						viewray.SetRay(camPosition, (pixel - camPosition).Normalise());
						//viewray.SetRay(pixel, Vector3(0.0, 0.0, -1.0));

						double u = (double)j / (double)m_buffWidth;
						double v = (double)i / (double)m_buffHeight;

						scenebg = pScene->GetBackgroundColour(u, v);

						//trace the scene using the view ray
						//default colour is the background colour, unless something is hit along the way
						Colour colour;
						colour = TraceScene(pScene, viewray, scenebg, m_traceLevel);

						/*
						* Draw the pixel as a coloured rectangle
						*/
						m_framebuffer->WriteRGBToFramebuffer(colour, j, i);
					}
				}
			}
			glDrawPixels(m_buffWidth, m_buffHeight, GL_RGBA, GL_FLOAT, m_framebuffer->GetBuffer());
			glFlush();
		}

		fprintf(stdout, "\r\nDone!!!\n");
		m_renderCount++;
	}
}

Colour RayTracer::TraceScene(Scene* pScene, Ray& ray, Colour incolour, int tracelevel, bool shadowray)
{
	RayHitResult result;
	Colour outcolour = incolour;
	std::vector<Light*> *light_list = pScene->GetLightList();

	if (tracelevel <= 0)
	{
		result = pScene->IntersectByRay(ray, shadowray);
		if (shadowray && result.data)
		{
			outcolour[0] = incolour[0]*0.3;
			outcolour[1] = incolour[1]*0.3;
			outcolour[2] = incolour[2]*0.3;
		}

		return outcolour;
	}

	result = pScene->IntersectByRay(ray, shadowray);

	if (result.data) //the ray has hit something
	{
		Vector3 start = ray.GetRayStart();
		outcolour = CalculateLighting(light_list,
			&start,
			&result);
		
		if(m_traceflag & TRACE_REFLECTION)
		{
			//If the m_primtype is PRIMTYPE_Sphere or PRIMTYPE_Box enter into the statement
			if (((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Sphere
				|| ((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Box)
			{
				//Set the direction and the origin of the ray
				Vector3 rayDirection = ray.GetRay().Reflect(result.normal);
				Vector3 rayOrigin = result.point;
				Ray reflectiveRay;
				reflectiveRay.SetRay(rayOrigin + rayDirection, rayDirection);

				//Set the new outcolour
				outcolour = TraceScene(pScene, reflectiveRay, incolour, --tracelevel, shadowray) * outcolour;
			}
		}

		if (m_traceflag & TRACE_REFRACTION)
		{
			//If the m_primtype is PRIMTYPE_Sphere or PRIMTYPE_Box enter into the statement
			if (((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Sphere
				|| ((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Box)
			{
				//Set the direction and the origin of the ray
				Vector3 rayOrigin = result.point;
				Vector3 rayDirection = ray.GetRay().Refract(result.normal, 0.9);
				Ray refractionRay;
				refractionRay.SetRay(result.point + rayDirection * 0.01, rayDirection);

				//Set the new outcolour
				outcolour = (outcolour * 0.2) + (TraceScene(pScene, refractionRay, incolour, --tracelevel, shadowray) * 0.8);
			}
		}
		
		//////Check if this is in shadow
		if ( m_traceflag & TRACE_SHADOW )
		{
			
			std::vector<Light*>::iterator lit_iter = light_list->begin();
			while (lit_iter != light_list->end())
			{

				Vector3 lightdir = (*lit_iter)->GetLightPosition() - result.point;
				lightdir.Normalise();
				Ray shadowray;
				shadowray.SetRay(result.point + lightdir*0.1, lightdir);
				outcolour = TraceScene(pScene, shadowray, outcolour, 0, true);

				lit_iter++;
			}
		}
	}
		
	return outcolour;
}

Colour RayTracer::CalculateLighting(std::vector<Light*>* lights, Vector3* campos, RayHitResult* hitresult)
{
	Colour outcolour;
	std::vector<Light*>::iterator lit_iter = lights->begin();

	Primitive* prim = (Primitive*)hitresult->data;
	Material* mat = prim->GetMaterial();

	outcolour = mat->GetAmbientColour();
	
	if (((Primitive*)hitresult->data)->m_primtype == Primitive::PRIMTYPE_Plane)
	{
		int dx = hitresult->point[0]/2.0;
		int dy = hitresult->point[1]/2.0;
		int dz = hitresult->point[2]/2.0;

		if (dx % 2 || dy % 2 || dz % 2 )
		{
			outcolour = Vector3(0.1, 0.1, 0.1);
		}
		else
		{
			outcolour = mat->GetDiffuseColour();
		}

	}

	////Go through all lights in the scene
	////Note the default scene only has one light source
	if (m_traceflag & TRACE_DIFFUSE_AND_SPEC)
	{
		while (lit_iter != lights->end())
		{
			Vector3 normal = hitresult->normal;
			Vector3 lightvec = (*lit_iter)->GetLightPosition() - hitresult->point;

			lightvec.Normalise();
						
			Colour diffusecolour = mat->HasDiffuseTexture()? 
				mat->SampleColour(Texture::TEXUNIT_DIFFUSE, hitresult->texcoord[0], hitresult->texcoord[1]) : 
				mat->GetDiffuseColour();

			if (((Primitive*)hitresult->data)->m_primtype == Primitive::PRIMTYPE_Plane)
			{
				int dx = hitresult->point[0] / 2.0;
				int dy = hitresult->point[1] / 2.0;
				int dz = hitresult->point[2] / 2.0;

				if (dx % 2 || dy % 2 || dz % 2)
				{
					diffusecolour = Vector3(0.1, .1, .1);
				}
			}

						
			//diffuse component;
			double ndotl = normal.DotProduct(lightvec);
			
			ndotl = ndotl < 0.0 ? 0.0 : ndotl;
			
			////Specular Reflectance (Blinn-Phong Model)
			Vector3 lightVector = (lit_iter[0]->GetLightPosition() - hitresult->point).Normalise();
			Vector3 viewDirectionVector = (*campos - hitresult->point).Normalise();
			Vector3 lightPlusViewVector = (lightVector + viewDirectionVector);
			Vector3 halfVector = lightPlusViewVector / lightPlusViewVector.Norm();
			float halfAngle = (max(min(halfVector.DotProduct(hitresult->normal), 1), 0));
			Colour specular = mat->GetSpecularColour() * lit_iter[0]->GetLightColour() * pow(halfAngle, mat->GetSpecPower());

			outcolour = outcolour + specular + diffusecolour*ndotl;

			lit_iter++;
		}
	}
	return outcolour;
}

