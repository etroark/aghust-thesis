#pragma once

#include "libs.h"

#include "vertex.h"
#include "pair.h"
#include "face.h"
#include "shader.h"
#include "objLoader.h"

//forward class declaration
class Gui;

/**
 * mesh class
 */
class Mesh
{
	friend Gui;

	//private variables
	std::vector<Vertex> _vertices;	/**< vector of vertices*/
	std::vector<SimpleVertex> _simpleVertices;	/**< vector of simple vertices*/

	std::vector<GLuint> _indices;	/**< vector of vertices ID's in drawing order*/
	std::vector<GLuint> _simpleIndices;	/**< vector of simple vertices ID's in drawing order*/

	GLdouble _percentage;	/**< destinated quantity in % of output vertices compared to input vertices*/

	double _simplifyTime;	/**< variable for calculating simplifying time*/
	double _aeapTime;	/**< variable for calculating isotropic remeshing time*/

	//for simplification purposes
	std::vector<Face> _faces; 	/**< vector of faces*/
	std::set<Pair> _pairsSet; 	/**< set of edges to preserve duplication*/
	std::vector<Pair> _pairs;	/**< vector of edges*/

	GLuint _VAO;	/**< vertex array object ID*/
	GLuint _VBO;	/**< vertex buffer object ID*/
	GLuint _EBO;	/**< element buffer object ID*/
	
	glm::vec3 _position;	/**< position of mesh*/
	glm::vec3 _origin;	/**< origin point of mesh*/
	glm::vec3 _rotation;	/**< rotation of mesh*/
	glm::vec3 _scale;	/**< scale of mesh*/
	
	glm::mat4 _ModelMatrix;	/**< model matrix*/

	bool _simple;	/**< if mesh is not simple, draw it with simple vertices and simple indices*/
	bool _simplify;	/**< run the algotithms if this is true*/
	GLuint _type;	/**< type of primitives to draw*/
	
	//private functions
	/**
	 * inits VAO, VBO and EBO
	 * @param simple checks if mesh is constructed with simple or regular verices
	 */
	void init(bool simple);

	/**
	 * sends model matrix to uniform in shader
	 * @param shader pointer to shader
	 */
	inline void updateUniforms(Shader* shader)
	{
		shader->setMat4fv(_ModelMatrix, "ModelMatrix");
	}

	/**
	 * updates model matrix for shader
	 */
	void updateModelMatrix();
	
public:

	/**
	 * mesh constructor
	 * @param objLoader loader object to get variables from
	 * @param type type of primitives
	 * @param percentage destinated quantity in % of output vertices compared to input vertices
	 * @param position position of mesh
	 * @param origin origin of mesh
	 * @param rotation rotation of mesh
	 * @param scale scale of mesh
	 * @param simple if mesh is not simple, draw it with simple vertices and simple indices
	 * @param simplify run the remeshing algotithms if this is true
	 */
	Mesh
	(
		ObjLoader objLoader,
		GLuint type,
		GLdouble percentage,
		glm::vec3 position = glm::vec3(.0f),
		glm::vec3 origin = glm::vec3(.0f),
		glm::vec3 rotation = glm::vec3(.0f),
		glm::vec3 scale = glm::vec3(1.f),
		bool simple = false,
		bool simplify = false
	);

	/**
	 * mesh copy-like constructor
	 * @param mesh mesh to copy variables from
	 * @param position position of new mesh
	 * @param origin origin of new mesh
	 * @param scale scale of new mesh
	 */
	Mesh(const Mesh* mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	/**
	 * mesh destructor, delete VAO, VBO and EBO
	 */
	~Mesh();
	
	/**
	 * position modifier
	 * @param position new position
	 */
	inline void setPosition(const glm::vec3 position)
	{
		_position = position;
	}
	/**
	 * origin modifier
	 * @param origin new origin point
	 */
	inline void setOrigin(const glm::vec3 origin)
	{
		_origin = origin;
	}
	/**
	 * rotation modifier
	 * @param rotation new rotation
	 */
	inline void setRotation(const glm::vec3 rotation)
	{
		_rotation = rotation;
	}
	/**
	 * scale modifier
	 * @param scale new scale
	 */
	inline void setScale(const glm::vec3 scale)
	{
		_scale = scale;
	}
	
	/**
	 * move the mesh
	 * @param position position vector to add
	 */
	inline void move(const glm::vec3 position)
	{
		_position += position;
	}
	/**
	 * rotatie the mesh
	 * @param position rotation vector to add
	 */
	inline void rotate(const glm::vec3 rotation)
	{
		_rotation += rotation;
	}
	/**
	 * scale the mesh
	 * @param position scale vector to add
	 */
	inline void scaleUp(const glm::vec3 scale)
	{
		_scale += scale;
	}

	/**
	 * render the mesh
	 * @param shader pointer to shader to use
	 * @param polygonMode basically filled/empty triangles
	 */
	void render(Shader* shader, int polygonMode);

	
	//	simplify mesh

	/**
	 * computes initial quadric for simple vertex
	 * @param v reference to Simple Vertex object
	 */
	void computeInitialQuad(SimpleVertex& v);

	/**
	 * computes initial cost for every pair
	 */
	void computeInitialCost();
	/**
	 * computes cost for every pair that contains vertex
	 * @param vertexId ID of vertex
	 */
	void computeCost(GLuint vertexId);

	/**
	 * returns vertex of given ID
	 * @param id vertex ID
	 * @return reference to SimpleVertex object
	 */
	SimpleVertex& getVertex(const GLuint id);
	/**
	 * returns face of given ID
	 * @param id face ID
	 * @return reference to Face object
	 */
	Face& getFace(const GLuint id);

	/**
	 * returns position of vertex in _simpleVertices
	 * @param id vertex ID
	 * @return index in _simpleVertices
	 */
	GLuint findVertexPosition(const GLuint id);
	/**
	 * returns position of face in _faces
	 * @param id face ID
	 * @return index in _faces
	 */
	GLuint findFacePosition(const GLuint id);
	/**
	 * returns position of pair in pairs
	 * @param id pair ID
	 * @return index in _pairs
	 */
	GLuint findPairPosition(const GLuint id);

	/**
	 * checks if face contains vertex
	 * @param faceId face ID
	 * @param vertexID vertex ID
	 */
	bool contains(GLuint faceId, GLuint vertexId);

	/**
	 * collapses an edge
	 * @param newId ID of vertex to modify
	 * @param oldId ID of vertex to delete
	 */
	void collapse(GLuint newId, GLuint oldId);

	/**
	 * quadric error metric simplify algorithm
	 * @param percentage destinated quantity in % of output vertices compared to input vertices
	 */
	void simplifyMesh(GLdouble percentage);

	//as-equilateral-as-possible remeshing (failed)
	//void aeap();
	
	//quasi-regular mesh
	/**
	 * splits an edge
	 * @param pair reference of pair to be splitted
	 */
	void split(Pair& pair);
	/**
	 * flips an edge
	 * @param pair reference of pair to be flipped
	 */
	void flip(Pair& pair);
	/**
	 * relocates a vertex using average position of neighbors
	 * @param pair reference of pair to be flipped
	 */
	void vertexRelocation(SimpleVertex& vertex);

	/**
	 * isotropic remeshing alorithm
	 */
	void incrementalRemeshing();
};
