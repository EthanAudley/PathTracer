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

#include "Renderer.h"

class RayTracer : public Renderer
{
	public:
		using Renderer::Renderer;

		virtual void DoTrace( Scene* pScene ) override;
		virtual Colour TraceScene(Scene* pScene, Ray& ray, Colour incolour, int tracelevel, bool shadowray = false) override;
		Colour CalculateLighting(std::vector<Light*>* lights, Vector3* campos, RayHitResult* hitresult);
};

