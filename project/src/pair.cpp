#include "../include/pair.h"

void Pair::set(GLuint v1, GLuint v2)
{
	if (v1 < v2)
	{
		_vertices[0] = v1;
		_vertices[1] = v2;
	}
	else
	{
		_vertices[0] = v2;
		_vertices[1] = v1;
	}
}