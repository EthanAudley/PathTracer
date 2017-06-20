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

#include "Material.h"
#include "Ray.h"
#include "Scene.h"
#include "Vector3.h"
#include "Framebuffer.h"

class Renderer
{
public:

	Framebuffer		*m_framebuffer;
	int				m_buffWidth;
	int				m_buffHeight;
	int				m_renderCount;
	int				m_traceLevel;

	enum TraceFlags
	{
		TRACE_AMBIENT = 0x1,					//trace ambient colour only
		TRACE_DIFFUSE_AND_SPEC = 0x1 << 1,		//trace and compute diffuse and specular lighting components 
		TRACE_SHADOW = 0x1 << 2,				//trace shadow rays
		TRACE_REFLECTION = 0x1 << 3,			//trace reflection rays
		TRACE_REFRACTION = 0x1 << 4,			//trace refraction rays
	};

	inline void SetTraceLevel(int level)		//Set the level of recursion, default is 5
	{
		m_traceLevel = level;
	}

	inline void ResetRenderCount()
	{
		m_renderCount = 0;
	}

	inline Framebuffer *GetFramebuffer() const
	{
		return m_framebuffer;
	}

	TraceFlags m_traceflag;						//current trace flags value default is TRACE_AMBIENT

	Renderer();
	Renderer(int Width, int Hight);
	~Renderer();

	//Trace the scene from a given ray and scene
	//Params:
	//	Scene* pScene		pointer to the scene being traced
	//  Ray& ray			reference to an input ray
	//  Colour incolour		default colour to use when the ray does not intersect with any objects
	//  int tracelevel		the current recursion level of the TraceScene call
	//  bool shadowray		true if the input ray is a shadow ray, could be useful when handling shadows
	virtual Colour TraceScene(Scene* pScene, Ray& ray, Colour incolour, int tracelevel, bool shadowray) = 0;

	//Trace a given scene
	//Params: Scene* pScene   Pointer to the scene to be ray traced
	virtual void DoTrace(Scene* pScene) = 0;
};
