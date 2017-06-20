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

class PathTracer : public Renderer
{
public:
	// Gets constructors and destructors 
	using Renderer::Renderer;

	// Virtual methods with overrides to override the parent class 
	virtual void DoTrace(Scene* pScene) override;
	virtual Colour TraceScene(Scene* pScene, Ray& ray, Colour incolour, int multiRay, bool shadowray = false) override;
	Colour TraceReflection(Scene* pScene, Ray ray, Colour incolour, int multiRay);
	Colour TraceRefraction(Scene* pScene, Ray ray, Colour incolour, int multiRay);
};

