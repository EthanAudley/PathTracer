/********************************************************************/
/*This is a commandline Path Tracer prepared for 5CC510 Graphics II	*/
/*																	*/
/*Please note: The code presented here is adapted from Kevin Beason	*/
/*implementation smallpt at http://www.kevinbeason.com/smallpt/		*/
/*																	*/
/********************************************************************/
#include <iostream>
#include <math.h>  
#include <stdlib.h>
#include <stdio.h>

#define M_PI 3.14159265358979323846

#if defined(WIN32) || defined(_WINDOWS)
#include <Windows.h>
#include <gl/GL.h>
#endif

#include "PathTracer.h"
#include "Scene.h"
#include "Camera.h"
#include "perlin.h"
#include "time.h"

inline double getUniformDouble()
{
	return (double)rand() / (double)RAND_MAX;
}

Colour PathTracer::TraceScene(Scene* pScene, Ray& ray, Colour incolour, int multiRay, bool shadowray)
{
	//Intersect the ray with the scene
	RayHitResult result = pScene->IntersectByRay(ray);

	Colour outcolour = incolour; //the output colour based on the ray-primitive intersection

	if (result.data) //the ray has hit something
	{
		Primitive* prim = (Primitive*)result.data;
		Material* mat = prim->GetMaterial();

		Vector3 normal = result.normal.DotProduct(ray.GetRay()) < 0 ? result.normal : result.normal * -1;
		Vector3 f = mat->GetDiffuseColour();

		double p = f[0] > f[1] && f[0] > f[2] ? f[0] : f[1] > f[2] ? f[1] : f[2]; // max reflectance 

		if (--multiRay<0)
		{
			if (getUniformDouble()<p) //throw a dice and decide if the trace should terminate
			{
				f = f*(1 / p);
			}
			else
			{
				return (mat->GetEmissiveColour()); // R.R
			}
		}
		//Ideal DIFFUSE reflection 
		double r1 = 2 * M_PI*getUniformDouble(), r2 = getUniformDouble(), r2s = sqrt(r2);

		Vector3 w = normal;
		Vector3 u = (fabs(w[0]) > .1 ? Vector3(0, 1, 0) : Vector3(1, 0, 0)).CrossProduct(w).Normalise();
		Vector3 v = w.CrossProduct(u);
		Vector3 direction = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1 - r2));

		Ray setRay; setRay.SetRay(result.point + (direction * 0.01), direction);
		
		outcolour = mat->GetEmissiveColour() + (f * TraceScene(pScene, setRay, incolour, multiRay));
	}
	return outcolour;
}

Colour PathTracer::TraceReflection(Scene* pScene, Ray ray, Colour scenebg, int multiRay) 
{
	Colour colour = scenebg;

	RayHitResult result = pScene->IntersectByRay(ray);
	Ray newRay = ray;

	//Check tracefag 
	if (m_traceflag & TRACE_REFLECTION)
	{
		//If the m_primtype is PRIMTYPE_Sphere or PRIMTYPE_Box enter into the statement
		if (((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Sphere
			|| ((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Box)
		{
			//Set the direction and the origin of the ray
			Vector3 reflectionDiretion = ray.GetRay().Reflect(result.normal);
			newRay.SetRay(result.point + (reflectionDiretion * 0.01), reflectionDiretion.Normalise());
		}
	}
		colour = TraceScene(pScene, newRay, scenebg, multiRay) * (1. / multiRay);

		for (int i = 0; i < multiRay; i++)
		{
			//Loop until all of the primary rays have been accumulated
			colour = colour + TraceScene(pScene, newRay, scenebg, multiRay) * (1. / multiRay);
		}
	
	return colour;
}

Colour PathTracer::TraceRefraction(Scene* pScene, Ray ray, Colour scenebg, int multiRay)
{
	Colour colour = scenebg;

	RayHitResult result = pScene->IntersectByRay(ray);
	Ray newRay = ray;

	//Check tracefag 
	if (m_traceflag & TRACE_REFRACTION)
	{
		//If the m_primtype is PRIMTYPE_Sphere or PRIMTYPE_Box enter into the statement
		if (((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Sphere
			|| ((Primitive*)result.data)->m_primtype == Primitive::PRIMTYPE_Box)
		{
			//Set the direction and the origin of the ray
			Vector3 refractDiretion = ray.GetRay().Refract(result.normal, 0.99);
			newRay.SetRay(result.point + (refractDiretion * 0.01), refractDiretion.Normalise());
		}
	}

	colour = TraceScene(pScene, newRay, scenebg, multiRay) * (1. / multiRay);

	//Loop until all of the primary rays have been accumulated
	for (int i = 0; i < multiRay; i++)
	{
		colour = colour + TraceScene(pScene, newRay, scenebg, multiRay) * (1. / multiRay);
	}
	return colour;
}

void PathTracer::DoTrace(Scene* pScene)
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

	// Statement to decide what type of traceflag and the amount of samples to render
	int samples = m_traceflag & TRACE_AMBIENT ? 50 : m_traceflag & TRACE_DIFFUSE_AND_SPEC ? 100 : m_traceflag & TRACE_SHADOW ? 500 : m_traceflag & TRACE_REFLECTION ? 250 : m_traceflag & TRACE_REFRACTION ? 250 : 250;

	clock_t time = clock();

	if (m_renderCount == 0)
	{
		fprintf(stdout, "\rTrace start.\n");

		Colour colour;
		//TinyRay on multiprocessors using OpenMP!!!
#pragma omp parallel for schedule (dynamic, 1) private(colour)
		for (int i = 0; i < m_buffHeight; i += 1) {
			fprintf(stdout, "\rRendering (%d spp) %5.2f%% (%.2fs Time taken)", samples, 100.* i / (m_buffHeight - 1), (double)(clock() - time) / CLOCKS_PER_SEC);
			for (int j = 0; j < m_buffWidth; j += 1) {

				//calculate the metric size of a pixel in the view plane (e.g. framebuffer)
				Vector3 pixel;

				// Anti-Aliasing
				//for (float x = 0.25f; x <= 1.f; x += 0.25f)
				//{
				//	for (float y = 0.25f; y <= 1.f; y += 0.25f)
				//	{
						pixel[0] = start[0] + (i + 0.5) * camUpVector[0] * pixelDY
							+ (j + 0.5) * camRightVector[0] * pixelDX;
						pixel[1] = start[1] + (i + 0.5) * camUpVector[1] * pixelDY
							+ (j + 0.5) * camRightVector[1] * pixelDX;
						pixel[2] = start[2] + (i + 0.5) * camUpVector[2] * pixelDY
							+ (j + 0.5) * camRightVector[2] * pixelDX;

						/*
						* setup first generation view ray
						* In perspective projection, each view ray originates from the eye (camera) position
						* and pierces through a pixel in the view plane
						*/

						Ray viewray;
						viewray.SetRay(camPosition, (pixel - camPosition).Normalise());

						double u = (double)j / (double)m_buffWidth;
						double v = (double)i / (double)m_buffHeight;

						//trace the scene using the view ray
						//default colour is the background colour, unless something is hit along the way
						scenebg = pScene->GetBackgroundColour();

						int multiRay = 5;

						/// Very inefficient way of getting a Reflection and Refraction ray.
						/// I would suggest commenting out both TraceReflection and TraceRefraction to increase
						/// render rate.
						// loop until the primary rays have been accumulated
						colour = TraceScene(pScene, viewray, scenebg, multiRay) * (1. / samples);
						colour = colour + TraceReflection(pScene, viewray, scenebg, multiRay) * (1. / samples);
						colour = colour + TraceRefraction(pScene, viewray, scenebg, multiRay) * (1. / samples);
						for (int i = 0; i < samples; i++)
						{
							//change
							if (TRACE_REFLECTION)
							{
								colour = colour + TraceReflection(pScene, viewray, scenebg, multiRay) * (1. / samples);
							}
							else if (TRACE_REFRACTION)
							{
								colour = colour + TraceRefraction(pScene, viewray, scenebg, multiRay) * (1. / samples);
							}
							else
							{
								colour = colour + TraceScene(pScene, viewray, scenebg, multiRay) * (1. / samples);
							}
						}
	/*				}
				}*/
				/*
				* Draw the pixel as a coloured rectangle
				*/
				m_framebuffer->WriteRGBToFramebuffer(colour, j, i);
			}
			//Draw each pixel to the frame buffer, commected out due to it increasing the render time
			glFlush();
		}
		//glDrawPixels(m_buffWidth, m_buffHeight, GL_RGBA, GL_FLOAT, m_framebuffer->GetBuffer());

		fprintf(stdout, "\r\nDone!!!", ((float)time / CLOCKS_PER_SEC));
		m_renderCount++;
	}
}