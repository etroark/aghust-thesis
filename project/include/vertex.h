#pragma once

#include "libs.h"

/**
 * vertex class for original model
 */
struct Vertex
{
	glm::vec3 _position;
	glm::vec3 _color;
	glm::vec2 _texcoord;
	glm::vec3 _normal;
};

/**
 * simple vertex for remeshing, containing only position information from the original file
 */
struct SimpleVertex
{
	GLuint _id;
	glm::vec3 _position;

	glm::mat4 _quad = glm::mat4(.0f); /**< quadric matrix used for QEM remeshing*/

	std::set<GLuint> _neighbors;
	std::set<GLuint> _faces;

	/**
	* default constructor
	*/
	inline SimpleVertex() {}
	/**
	 * constructor
	 * @param id new vertex ID
	 * @param position new vertex position
	 */
	inline SimpleVertex(GLuint id, glm::vec3 position) : _id(id), _position(position) {}
};