#pragma once

#include "libs.h"

#include "vertex.h"
#include "face.h"

//forward class declaration
class Gui;

/**
 * object loader class
 */
class ObjLoader
{
	friend class Gui;

	std::vector<Vertex> _vertices;	/**< default vertices*/
	std::vector<SimpleVertex> _simpleVertices;	/**< vertices for remeshing purposes*/

	std::vector<GLuint> _indices;	/**< order of drawing triangles between default vertices*/
	std::vector<GLuint> _simpleIndices;	/**< order of drawing triangles between simple vertices*/

	std::vector<Face> _faces;	/**< triangles*/

	static std::string _log;	/**< status of loading process for GUI*/

public:

	/**
	 * method for object loading
	 * @param fileName OBJ file name
	 * @param color destinated vertex color
	 */
	void loadObj(const char* fileName, glm::vec3 color);

	/**
	 * vertices getter
	 * @return vector of vertices
	 */
	inline std::vector<Vertex> getVertices() { return _vertices; }
	/**
	 * simple vertices getter
	 * @return vector of simple vertices
	 */
	inline std::vector<SimpleVertex> getSimpleVertices() { return _simpleVertices; }
	/**
	 * indices getter
	 * @return vector of indices
	 */
	inline std::vector<GLuint> getIndices() { return _indices; }
	/**
	 * simple indices getter
	 * @return vector of simple indices
	 */
	inline std::vector<GLuint> getSimpleIndices() { return _simpleIndices; }
	/**
	 * faces getter
	 * @return vector of faces
	 */
	inline std::vector<Face> getFaces() { return _faces; }
};