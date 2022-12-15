#pragma once

#include "libs.h"

#include "mesh.h"
#include "material.h"

//forward class declaration
class Gui;

/**
 * model class
 */
class Model
{
	friend Gui;

	Material* _material;	/**> material pointer*/
	std::vector<Mesh*> _meshes;	/**> vector of meshes*/
	glm::vec3 _position;	/**> position of the model*/
	glm::vec3 _rotation;	/**> rotation of the model*/
	glm::vec3 _scale;	/**> scale of the model*/

public:

	/**
	 * model constructor
	 * @param position model position
	 * @param rotation model rotation
	 * @param scale model scale
	 * @param material model material
	 * @param objFile OBJ file name to load mesh
	 * @param color vertices color
	 * @param simple if mesh is not simple, draw it with simple vertices and simple indices
	 * @param simplify run the algotithms if this is true
	 * @param percentage destinated quantity in % of output vertices compared to input vertices
	 * @param type type of primitives
	 */
	Model(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Material* material, const char* objFile, glm::vec3 color, bool simple = false, bool simplify = false, GLdouble percentage = 0.0, GLuint type = GL_TRIANGLES);
	/**
	 * model copy-like constructor
	 * @param model pointer to Model object
	 * @param position new model position
	 * @param rotation new model rotation
	 * @param scale new model scale
	 */
	Model(const Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	/**
	 * model destructor
	 */
	inline ~Model()
	{
		for (auto*& i : _meshes)
			delete i;
	}

	/**
	 * rotate all meshes
	 * @param rotation rotation
	 */
	inline void rotate(const glm::vec3 rotation)
	{
		for (auto& i : _meshes)
			i->rotate(rotation);
	}

	/**
	 * render model
	 * @param shader pointer to shader
	 * @param polygonMode filled/empty triangles
	 */
	void render(Shader* shader, GLuint polygonMode);
};