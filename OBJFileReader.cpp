#include <stdio.h>
#include <string.h>
#include "OBJFileReader.h"

static int firstPassOBJRead(const char* filename, int* vertex_count, int* vert_normal_count, int* vert_texcoord_count, int* triangle_count)
{
	FILE* pfile;

	*vertex_count = 0;
	*vert_normal_count = 0;
	*vert_texcoord_count = 0;
	*triangle_count = 0;

#if defined(WINDOWS) || defined(WIN32)
	fopen_s(&pfile, filename, "r");
#else
	pfile = fopen(filename, "r");
#endif

	if (!pfile)
	{
		//something has gone wrong when opening the file.
		return 1;
	}

	char tempbuffer[1024];
	char* strread = fgets(tempbuffer, 1024, pfile);

	do
	{
		char* v = &(tempbuffer[0]);
		if (*v == 'v')
		{
			if (*(v + 1) == 'n')		*vert_normal_count += 1;
			else if (*(v + 1) == 't')	*vert_texcoord_count += 1;
			else					*vertex_count += 1;
		}
		else if (*v == 'f')
		{
			int numIndices = 0;
			char* vs = strstr(v, " ");

			//vs = strstr(vs+1, " ");
			while (vs)
			{
				//numIndices += 1;
				vs = strstr(vs + 1, "/");
				if (vs)
					numIndices += 1;
			}

			int numTriangles = 1;//numIndices / 2 - 2;
			*triangle_count += numTriangles;
		}

		strread = fgets(tempbuffer, 1024, pfile);

	} while (strread != NULL);

	fclose(pfile);

	return 0;
}

static int secondPassOBJRead(const char* filename, int nVerts, int nNormals, int nTexcoords, Triangle* mesh)
{
	FILE* pfile;

#if defined(WINDOWS) || defined(WIN32)
	fopen_s(&pfile, filename, "r");
#else
	pfile = fopen(filename, "r");
#endif
	if (!pfile)
	{
		//something has gone wrong when opening the file.
		return 1;
	}

	char tempbuffer[1024];
	char* strread = fgets(tempbuffer, 1024, pfile);
	int vertex_read = 0;
	int normal_read = 0;
	int texcoord_read = 0;
	int triangle_read = 0;

	Vector3 *normal_buffer = new Vector3[nNormals];
	Vector3 *vertex_buffer = new Vector3[nVerts];
	Vector3 *texcoord_buffer = new Vector3[nTexcoords];

	do
	{
		char* v = &(tempbuffer[0]);
		if (*v == 'v')
		{
			if (*(v + 1) == 'n')
			{
#if defined(WINDOWS) || defined(WIN32)
				sscanf_s(v, "%*s %f %f %f", &(normal_buffer[normal_read][0]),
					&(normal_buffer[normal_read][1]),
					&(normal_buffer[normal_read][2]));
#else
				sscanf(v, "%*s %f %f %f", &(normal_buffer[normal_read][0]), 
					&(normal_buffer[normal_read][1]),
					&(normal_buffer[normal_read][2]));
#endif
				normal_read += 1;
			}
			else if (*(v + 1) == 't')
			{
#if defined(WINDOWS) || defined(WIN32)
				sscanf_s(v, "%*s %f %f", &(texcoord_buffer[texcoord_read][0]),
					&(texcoord_buffer[texcoord_read][1]));
#else
				sscanf(v, "%*s %f %f", &(texcoord_buffer[texcoord_read][0]),
					&(texcoord_buffer[texcoord_read][1]));
#endif
				texcoord_read += 1;
			}
			else
			{
#if defined(WINDOWS) || defined(WIN32)
				sscanf_s(v + 1, "%f %f %f", &(vertex_buffer[vertex_read][0]), 
					&(vertex_buffer[vertex_read][1]),
					&(vertex_buffer[vertex_read][2]));
#else
				sscanf_s(v + 1, "%f %f %f", &(vertex_buffer[vertex_read][0]),
					&(vertex_buffer[vertex_read][1]),
					&(vertex_buffer[vertex_read][2]));
#endif
				vertex_read += 1;
			}
		}
		else if (*v == 'f')
		{
			int dummy;
			int index1;
			int index2;
			int index3;
			int index4;

			int nidx1, nidx2, nidx3, nidx4;

			int tidx1, tidx2, tidx3, tidx4;
#if defined(WINDOWS) || defined(WIN32)
			if (sscanf_s(v, "%*s %d/%d/%d %d/%d/%d %d/%d/%d", &index1, &tidx1, &nidx1,
				&index2, &tidx2, &nidx2,
				&index3, &tidx3, &nidx3) == 9)
#else
			if (sscanf(v, "%*s %d/%d/%d %d/%d/%d %d/%d/%d", &index1, &tidx1, &nidx1,
				&index2, &tidx2, &nidx2,
				&index3, &tidx3, &nidx3) == 9)
#endif
			{
				mesh[triangle_read].SetVertices(vertex_buffer[index1-1], vertex_buffer[index2-1], vertex_buffer[index3-1]);
				mesh[triangle_read].SetNormals(normal_buffer[nidx1-1], normal_buffer[nidx2-1], normal_buffer[nidx3-1]);
				mesh[triangle_read].SetTexCoords(texcoord_buffer[tidx1-1], texcoord_buffer[tidx2-1], texcoord_buffer[tidx3-1]);
			}
			else
			{
				//something is seriously fucked. abort
				//I don't want to handle them
				continue;
			}
			triangle_read += 1;
		}

		strread = fgets(tempbuffer, 1024, pfile);

	} while (strread != NULL);

	fclose(pfile);

	delete [] vertex_buffer;
	delete [] normal_buffer;
	delete [] texcoord_buffer;

	return 0;
}

int importOBJMesh(const char* filename, Triangle** triangles)
{
	int num_triangles = 0;
	int num_vertices = 0;
	int num_normals = 0;
	int num_texcoords = 0;

	firstPassOBJRead(filename, &num_vertices, &num_normals, &num_texcoords, &num_triangles);

	*triangles = new Triangle[num_triangles];

	secondPassOBJRead(filename, num_vertices, num_normals, num_texcoords, *triangles);

	return num_triangles;
}
