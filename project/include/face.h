#pragma once

#include "libs.h"

/**
 * mesh face class
 */
struct Face
{
	GLuint _id;
	std::array<GLuint, 3> _vertices = { {0, 0, 0} };

	/**
	 * face constructor
	 * @param id new face ID
	 * @param vertices array of vertices for the new face
	 */
	inline Face(GLuint id, std::array<GLuint, 3> vertices) : _id(id), _vertices(vertices)
	{
		std::sort(_vertices.begin(), _vertices.end());
	}

	/**
	 * vertex array sort method
	 */
	inline void sortVertices()
	{
		std::sort(_vertices.begin(), _vertices.end());
	}
};