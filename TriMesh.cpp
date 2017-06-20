#include <stdlib.h>
#include "TriMesh.h"
#include "OBJFileReader.h"

TriMesh::TriMesh()
{
	m_triangles = NULL;
	m_primtype = PRIMTYPE::PRIMTYPE_TRIMESH;
}

TriMesh::TriMesh(const char* filename) : TriMesh()
{
	LoadTriMeshFromOBJFile(filename);
}

TriMesh::~TriMesh()
{
	delete [] m_triangles;
}

void TriMesh::LoadTriMeshFromOBJFile(const char* filename)
{
  	m_numtriangles = importOBJMesh(filename, &m_triangles);
}

RayHitResult TriMesh::IntersectByRay(Ray& ray)
{
	RayHitResult result = Ray::s_defaultHitResult;

	for (int i = 0; i < m_numtriangles; i++)
	{
		Vector3 v1 = m_triangles[i].m_vertices[1].m_position - m_triangles[i].m_vertices[0].m_position;
		Vector3 v2 = m_triangles[i].m_vertices[2].m_position - m_triangles[i].m_vertices[0].m_position;
		Vector3 normal = v1.CrossProduct(v2);
		
		if (normal.DotProduct(ray.GetRay()) < 0.0)
		{

			Primitive* prim = static_cast<Primitive*>(&m_triangles[i]);

			RayHitResult tempresult = prim->IntersectByRay(ray);

			if (tempresult.t < result.t)
			{
				tempresult.data = this;
				result = tempresult;
			}
		}
	}

	return result;
}
