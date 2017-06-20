#include <stdlib.h>
#include <glut.h>

#include "Scene.h"
#include "RayTracer.h"

static RayTracer* g_raytracer = NULL;
static Scene* g_scene = NULL;
static bool start_tracing = false;
static int screenwidth = 1280;
static int screenheight = 720;

void reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, 0.0, height, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void display()
{
	reshape( screenwidth, screenheight);
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);

	Colour *pBuffer = g_raytracer->GetFramebuffer()->GetBuffer();

	if ( start_tracing && g_raytracer && g_scene )
		g_raytracer->DoRayTrace(g_scene);
	
	glDrawPixels(screenwidth, screenheight, GL_RGB, GL_FLOAT, pBuffer);

	glFinish();
}

void key( unsigned char c, int x, int y)
{
	switch ( c )
	{
		case '1': //ambient only
			g_raytracer->m_traceflag = (RayTracer::TraceFlag)RayTracer::TRACE_AMBIENT;
			break;
		case '2': //full lighting
			g_raytracer->m_traceflag = (RayTracer::TraceFlag)(RayTracer::TRACE_AMBIENT|RayTracer::TRACE_DIFFUSE_AND_SPEC);
			break;
		case '3': //shadow
			g_raytracer->m_traceflag = (RayTracer::TraceFlag)(RayTracer::TRACE_AMBIENT|RayTracer::TRACE_DIFFUSE_AND_SPEC|RayTracer::TRACE_SHADOW);
			break;
		case '4': //reflect
			g_raytracer->m_traceflag = (RayTracer::TraceFlag)(RayTracer::TRACE_AMBIENT|RayTracer::TRACE_DIFFUSE_AND_SPEC|RayTracer::TRACE_REFLECTION);
			break;
		case '5': //refract
			g_raytracer->m_traceflag = (RayTracer::TraceFlag)(RayTracer::TRACE_AMBIENT|RayTracer::TRACE_DIFFUSE_AND_SPEC|RayTracer::TRACE_SHADOW|RayTracer::TRACE_REFRACTION);
			break;
		case '6': //awesome
			g_raytracer->m_traceflag = (RayTracer::TraceFlag)(RayTracer::TRACE_AMBIENT|RayTracer::TRACE_DIFFUSE_AND_SPEC|RayTracer::TRACE_SHADOW|RayTracer::TRACE_REFLECTION|RayTracer::TRACE_REFRACTION);
			break;
		case 's':
			start_tracing = true;
			break;
	}
	g_raytracer->ResetRenderCount();
	glutPostRedisplay();
}

void cleanup()
{
	delete g_scene;
	delete g_raytracer;
}


int main ( int argc, char** argv )
{
	g_raytracer = new RayTracer(screenwidth, screenheight);
	
	g_scene = new Scene();
	g_scene->SetSceneWidth( (float)screenwidth/(float)screenheight );

	atexit(cleanup);

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(screenwidth, screenheight);

	glutCreateWindow("TinyRay macOS");

	glutDisplayFunc(display);
	glutKeyboardFunc( key );

	glutMainLoop();
	
	return 0;
}
