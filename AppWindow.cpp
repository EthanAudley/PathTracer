/*---------------------------------------------------------------------
*
* Copyright © 2015  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include "AppWindow.h"
#include "Resource.h"
#include <gl/GL.h>

bool sTrace = false;

AppWindow::AppWindow()
{

}

AppWindow::~AppWindow()
{
	delete m_pRayTracer;
	delete m_pPathTracer;
	delete m_pRenderer;
	delete m_pScene;
}

AppWindow::AppWindow(HINSTANCE hInstance, int width, int height)
{
	InitWindow(hInstance, width, height);
}

HGLRC AppWindow::CreateOGLContext(HDC hdc)
{
	unsigned int pixelformat;
	HGLRC hglrc;

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Support Window
		PFD_SUPPORT_OPENGL |						// Format Support OpenGL
													//PFD_DOUBLEBUFFER,							// OK let's use single buffering
		PFD_TYPE_RGBA,								// Request RGBA Format
		32,											// Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits mask
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// Z-Buffer depth
		0,											// Stencil Buffer
		0,											// Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(pixelformat = ChoosePixelFormat(hdc, &pfd)))
	{
		return 0;
	}

	if (!SetPixelFormat(hdc, pixelformat, &pfd))
	{
		return 0;
	}

	if (!(hglrc = wglCreateContext(hdc)))
	{
		return 0;
	}

	if (!wglMakeCurrent(hdc, hglrc))
	{
		return 0;
	}

	return hglrc;
}

void AppWindow::DestroyOGLWindow()
{
	DestroyOGLContext();

	DestroyWindow(m_hwnd);

	m_hwnd = NULL;
	m_hdc = NULL;
}

BOOL AppWindow::DestroyOGLContext()
{
	if (m_hglrc)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hglrc);
		m_hglrc = NULL;
	}

	if (!ReleaseDC(m_hwnd, m_hdc))
		return FALSE;

	return TRUE;
}

BOOL AppWindow::InitWindow(HINSTANCE hInstance, int width, int height)
{
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
		L"AppWindow", L"TinyRay", WS_OVERLAPPED | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU,
		0, 0, width, height, NULL, NULL, hInstance, NULL);

	if (!m_hwnd)
		return FALSE;

	RECT clientrect;

	GetClientRect(m_hwnd, &clientrect);

	m_hdc = GetDC(m_hwnd);

	if (!(m_hglrc = CreateOGLContext(m_hdc)))
		return FALSE;

	SetWindowPos(m_hwnd, NULL, 10, 10, (width << 1) - clientrect.right, (height << 1) - clientrect.bottom, SWP_HIDEWINDOW | SWP_NOREDRAW);

	m_width = width;
	m_height = height;

	//allocate the ray tracer and the scene
	m_pRayTracer = new RayTracer(width, height);
	m_pPathTracer = new PathTracer(width, height);
	m_pRenderer = m_pRayTracer;
	m_pScene = new Scene();
	m_pScene->SetSceneWidth((float)width / (float)height);

	return TRUE;
}

void AppWindow::SetVisible(BOOL visible)
{
	ShowWindow(m_hwnd, visible ? SW_SHOW : SW_HIDE);
}

void AppWindow::Render()
{
	Colour *pBuffer = m_pRenderer->GetFramebuffer()->GetBuffer();
	m_pRenderer->DoTrace(m_pScene);

	glDrawPixels(m_width, m_height, GL_RGBA, GL_FLOAT, pBuffer);
	glFlush();

	SwapBuffers(m_hdc);
	return;
}

void AppWindow::Resize(int width, int height)
{
	RECT clientrect;
	GetClientRect(m_hwnd, &clientrect);

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, 0.0, height, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return;
}

void AppWindow::InitOGLState()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

BOOL AppWindow::MouseLBDown(int x, int y)
{

	return TRUE;
}

BOOL AppWindow::MouseLBUp(int x, int y)
{
	return TRUE;
}

BOOL AppWindow::MouseMove(int x, int y)
{
	return TRUE;
}

// Method to switch between RayTracer and PathTracer
void AppWindow::SwitchTrace(bool switchTrace)
{
	if (switchTrace)
	{
		m_pRenderer = m_pPathTracer;
	}
	else if (switchTrace == false)
	{
		m_pRenderer = m_pRayTracer;
	}
}

BOOL AppWindow::KeyUp(WPARAM key)
{
	switch (key)
	{
	case VK_F1:
		sTrace = false;
		SwitchTrace(sTrace);
		m_pRenderer->m_traceflag = Renderer::TRACE_AMBIENT;
		break;
	case VK_F2:
		sTrace = false;
		SwitchTrace(sTrace);
		m_pRenderer->m_traceflag = (Renderer::TraceFlags)(Renderer::TRACE_AMBIENT | Renderer::TRACE_DIFFUSE_AND_SPEC
			| Renderer::TRACE_SHADOW);
		break;
	case VK_F3:
		sTrace = false;
		SwitchTrace(sTrace);
		m_pRenderer->m_traceflag = (Renderer::TraceFlags)(Renderer::TRACE_AMBIENT | Renderer::TRACE_DIFFUSE_AND_SPEC
			| Renderer::TRACE_REFLECTION | Renderer::TRACE_SHADOW);
		break;
	case VK_F4:
		sTrace = false;
		SwitchTrace(sTrace);
		m_pRenderer->m_traceflag = (Renderer::TraceFlags)(Renderer::TRACE_AMBIENT | Renderer::TRACE_DIFFUSE_AND_SPEC
			| RayTracer::TRACE_REFRACTION | Renderer::TRACE_SHADOW);
		break;
	case VK_F5:
		sTrace = true;
		SwitchTrace(sTrace);
		m_pRenderer->m_traceflag = (Renderer::TraceFlags)(Renderer::TRACE_REFLECTION);
		m_pRenderer;
		break;
	case VK_F6:
		sTrace = true;
		SwitchTrace(sTrace);
		m_pRenderer->m_traceflag = (Renderer::TraceFlags)(Renderer::TRACE_REFRACTION);
		break;
	case VK_F7:
		sTrace = true;
		SwitchTrace(sTrace);
		m_pRenderer->m_traceflag = (Renderer::TraceFlags)(Renderer::TRACE_REFRACTION | Renderer::TRACE_REFLECTION);
		break;
	case VK_F8:
		sTrace = true;
		SwitchTrace(sTrace);
		m_pRenderer->m_traceflag = (Renderer::TraceFlags)(Renderer::TRACE_AMBIENT);
		break;
	case VK_F9:
		sTrace = true;
		SwitchTrace(sTrace);
		m_pRenderer->m_traceflag = (Renderer::TraceFlags)(Renderer::TRACE_DIFFUSE_AND_SPEC);
		break;
	case VK_F11:
		sTrace = true;
		SwitchTrace(sTrace);
		m_pRenderer->m_traceflag = (Renderer::TraceFlags)(Renderer::TRACE_SHADOW);
		break;
	}
	m_pRenderer->ResetRenderCount();

	return TRUE;
}
