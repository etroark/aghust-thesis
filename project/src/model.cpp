#include "../include/model.h"

//constructors
Model::Model(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Material* material, const char* objFile, glm::vec3 color, bool simple, bool simplify, GLdouble percentage, GLuint type)
	: _position(position), _rotation(rotation), _scale(scale), _material(material)
{
	//std::vector<Vertex> mesh = loadObj(objFile, color);
	ObjLoader objLoader;
	objLoader.loadObj(objFile, color);

	_meshes.push_back(new Mesh
	(
		objLoader,
		type,
		percentage,
		position,
		glm::vec3(.0f),
		rotation,
		scale,
		simple,
		simplify
	));
}

Model::Model(const Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
	: _position(position), _rotation(rotation), _scale(scale), _material(model->_material)
{
	_meshes.push_back(new Mesh
	(
		model->_meshes[0], position, rotation, scale
	));
}

//public function
void Model::render(Shader* shader, GLuint polygonMode)
{
	_material->sendToShader(*shader);	//send material to shader

	shader->use();	//use the program

	//draw
	for (auto& i : _meshes)
	{
		i->render(shader, polygonMode);
	}
}