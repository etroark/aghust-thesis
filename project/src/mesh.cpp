#include "../include/mesh.h"

//private functions
void Mesh::updateModelMatrix()
{
	_ModelMatrix = glm::mat4(1.f);
	_ModelMatrix = glm::translate(_ModelMatrix, _origin);
	_ModelMatrix = glm::rotate(_ModelMatrix, glm::radians(_rotation.x), glm::vec3(1.f, .0f, .0f));
	_ModelMatrix = glm::rotate(_ModelMatrix, glm::radians(_rotation.y), glm::vec3(.0f, 1.f, .0f));
	_ModelMatrix = glm::rotate(_ModelMatrix, glm::radians(_rotation.z), glm::vec3(.0f, .0f, 1.f));
	_ModelMatrix = glm::translate(_ModelMatrix, _position - _origin);
	_ModelMatrix = glm::scale(_ModelMatrix, _scale);
}

void Mesh::init(bool simple)
{
	//generate VAO and bind it
	glCreateVertexArrays(1, &_VAO);
	glBindVertexArray(_VAO);

	//generate VBO and bind it and send data
	glGenBuffers(1, &_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);

	if (_simplify)
		glBufferData(GL_ARRAY_BUFFER, _simpleVertices.size() * sizeof(SimpleVertex), _simpleVertices.data(), GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);
	
	//generate EBO and bind it and send data
	glGenBuffers(1, &_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

	if (_simplify)
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _simpleIndices.size() * sizeof(GLuint), _simpleIndices.data(), GL_STATIC_DRAW);
	else
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);
		
	//set vertex attribute pointers and enable them (input assembly)
	//	position
	if (_simplify)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (GLvoid*)offsetof(SimpleVertex, _position));
	else
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, _position));
	glEnableVertexAttribArray(0);
	if (!simple)
	{
		//	color
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, _color));
		glEnableVertexAttribArray(1);
		//	texcoord
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, _texcoord));
		glEnableVertexAttribArray(2);
		//	normal
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, _normal));
		glEnableVertexAttribArray(3);
	}

	//free
	glBindVertexArray(0);
}

//constructors
Mesh::Mesh
(
	ObjLoader objLoader,
	GLuint type,
	GLdouble percentage,
	glm::vec3 position,
	glm::vec3 origin,
	glm::vec3 rotation,
	glm::vec3 scale,
	bool simple,
	bool simplify
) : _type(type), _position(position), _origin(origin), _rotation(rotation), _scale(scale), _simple(simple), _simplify(simplify)
{
	_vertices = objLoader.getVertices();
	_simpleVertices = objLoader.getSimpleVertices();

	if (simplify)
	{
		_faces = objLoader.getFaces();

		auto startTime = std::chrono::high_resolution_clock::now();

		simplifyMesh(percentage);

		//size_t i = 0;
		for (auto f : _faces)
		{
			for (auto v : f._vertices)
			{
				_simpleIndices.push_back(findVertexPosition(v));
			}
			//++i;
			//if (!(i % 1000))
			//{
				//std::cout << ">building simple mesh:	" << 100.f * i / _faces.size() << "%" << std::endl;
			//}
		}
		//std::cout << std::endl;
		_simplifyTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
	}
	else
		_indices = objLoader.getIndices();

	init(_simple);
	updateModelMatrix();
}

Mesh::Mesh(const Mesh* mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
	: _position(position), _rotation(rotation), _scale(scale), _origin(glm::vec3(.0f)),
	_type(mesh->_type),
	_simpleVertices(mesh->_simpleVertices), _simpleIndices(mesh->_simpleIndices),
	_faces(mesh->_faces), _pairs(mesh->_pairs)
{

	_simplify = true;
	_simple = true;

	auto startTime = std::chrono::high_resolution_clock::now();

	incrementalRemeshing();

	_simpleIndices.clear();
	for (auto f : _faces)
		for (auto v : f._vertices)
			_simpleIndices.push_back(findVertexPosition(v));

	_aeapTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();

	init(_simple);
	updateModelMatrix();
}

//destructor
Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(1, &_VBO);

	glDeleteBuffers(1, &_EBO);
}

//public functions
void Mesh::render(Shader* shader, int polygonMode)
{
	//update uniforms
	updateModelMatrix();
	updateUniforms(shader);

	shader->use();

	//bind VAO
	glBindVertexArray(_VAO);

	//_simple ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	_simplify ? glDrawElements(_type, _simpleIndices.size(), GL_UNSIGNED_INT, 0) : glDrawElements(_type, _indices.size(), GL_UNSIGNED_INT, 0);

	//cleanup
	glBindVertexArray(0);
	glUseProgram(0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::computeInitialQuad(SimpleVertex& v)
{
	std::vector<Face> faces;
	for (auto f : _faces)
	{
		if (f._vertices[0] == v._id || f._vertices[1] == v._id || f._vertices[2] == v._id)
			faces.push_back(f);
	}

	glm::vec3 e1;
	glm::vec3 e2;
	glm::vec3 normal;
	glm::vec4 plane;
	glm::mat4 quad(.0f);
	for (auto f : faces)
	{
		//e1 = _simpleVertices[findVertexPosition(f._vertices[1])]._position - _simpleVertices[findVertexPosition(f._vertices[0])]._position;
		//e2 = _simpleVertices[findVertexPosition(f._vertices[2])]._position - _simpleVertices[findVertexPosition(f._vertices[0])]._position;
		e1 = getVertex(f._vertices[1])._position - getVertex(f._vertices[0])._position;
		e2 = getVertex(f._vertices[2])._position - getVertex(f._vertices[0])._position;
		normal = glm::cross(e1, e2);
		plane = { normal, -glm::dot(_simpleVertices[findVertexPosition(v._id)]._position, normal) };
		quad += glm::outerProduct(plane, plane);
	}

	v._quad = quad;

	//if (!(v._id % 1000)
	//{
	//	std::cout << ">computing quadrics:	" << 100.f * v._id / _simpleVertices.size() << "%" << std::endl;
	//}
}

void Mesh::computeInitialCost()
{
	for (auto& p : _pairs)
	{
		glm::vec3 pTemp = glm::vec3(.0f);
		glm::mat4 qTemp = glm::mat4(.0f);

		pTemp = getVertex(p._vertices[0])._position + getVertex(p._vertices[1])._position;
		qTemp = getVertex(p._vertices[0])._quad + getVertex(p._vertices[1])._quad;

		pTemp /= 2.f;
		glm::vec4 cost = glm::vec4(pTemp.x, pTemp.y, pTemp.z, 1.f);
		cost = cost * qTemp;

		p._cost = glm::dot(cost, cost);
	}
}

void Mesh::computeCost(GLuint vertexId)
{
	for (auto& p : _pairs)
		if (p._vertices[0] == vertexId || p._vertices[1] == vertexId)
		{
			glm::vec3 pTemp = glm::vec3(.0f);
			glm::mat4 qTemp = glm::mat4(.0f);

			pTemp = getVertex(p._vertices[0])._position + getVertex(p._vertices[1])._position;
			qTemp = getVertex(p._vertices[0])._quad + getVertex(p._vertices[1])._quad;

			pTemp /= 2.f;
			glm::vec4 cost = glm::vec4(pTemp.x, pTemp.y, pTemp.z, 1.f);
			cost = cost * qTemp;

			p._cost = glm::dot(cost, cost);
		}
}

SimpleVertex& Mesh::getVertex(const GLuint id)
{
	for (auto& v : _simpleVertices)
		if (v._id == id)
			return v;
}

Face& Mesh::getFace(const GLuint id)
{
	for (auto& f : _faces)
		if (f._id == id)
			return f;
}

GLuint Mesh::findVertexPosition(const GLuint id)
{
	for (size_t i = 0; i < _simpleVertices.size(); ++i)
		if (_simpleVertices[i]._id == id) return i;
}
GLuint Mesh::findFacePosition(const GLuint id)
{
	for (size_t i = 0; i < _faces.size(); ++i)
		if (_faces[i]._id == id) return i;
}
GLuint Mesh::findPairPosition(const GLuint id)
{
	for (size_t i = 0; i < _pairs.size(); ++i)
		if (_pairs[i]._id == id) return i;
}

bool Mesh::contains(GLuint faceId, GLuint vertexId)
{
	if
	(
		getFace(faceId)._vertices[0] == vertexId ||
		getFace(faceId)._vertices[1] == vertexId ||
		getFace(faceId)._vertices[2] == vertexId
	) return true;
	return false;
}

void Mesh::collapse(GLuint newId, GLuint oldId)
{
	glm::vec3 newPosition = (getVertex(newId)._position + getVertex(oldId)._position) / 2.f;
	glm::mat4 newQuad = getVertex(newId)._quad + getVertex(oldId)._quad;

	std::set<GLuint> newNeighbors;
	for (auto& n : getVertex(newId)._neighbors)
		newNeighbors.insert(n);
	for (auto& n : getVertex(oldId)._neighbors)
		newNeighbors.insert(n);

	newNeighbors.erase(oldId);
	newNeighbors.erase(newId);

	getVertex(newId)._position = newPosition;
	getVertex(newId)._neighbors = newNeighbors;
	getVertex(newId)._quad = newQuad;

	//	update neighbors
	for (auto& n : getVertex(newId)._neighbors)
	{
		getVertex(n)._neighbors.erase(oldId);
		getVertex(n)._neighbors.insert(newId);
	}

	//update faces
	//	select faces to be removed
	std::vector<GLuint> faces;
	for (auto& f : getVertex(newId)._faces)
		if (contains(f, oldId))
			faces.push_back(f);

	//	insert v2's faces ids to v1
	for (auto& f : getVertex(oldId)._faces)
		getVertex(newId)._faces.insert(f);

	//remove ereased faces from new vertex
	for (auto f : faces)
		getVertex(newId)._faces.erase(f);

	//	remove oldId from all faces
	for (auto& f : getVertex(newId)._faces)
		for (size_t i = 0; i < 3; ++i)
			if (getFace(f)._vertices[i] == oldId)
				getFace(f)._vertices[i] = newId;

	//remove deleted faces ids from all neighbors
	for (auto& n : getVertex(newId)._neighbors)
		for (auto f : faces)
			getVertex(n)._faces.erase(f);

	//update pairs
	//	delete pairs
	for (size_t i = 0; i < _pairs.size(); ++i)
		if (_pairs[i]._vertices[0] == newId || _pairs[i]._vertices[1] == newId || _pairs[i]._vertices[0] == oldId || _pairs[i]._vertices[1] == oldId)
		{
			_pairs.erase(_pairs.begin() + i);
			--i;
		}
	//	add new pairs
	for (auto n : getVertex(newId)._neighbors)
		_pairs.push_back(Pair(newId, n));

	for (auto f : faces)
		_faces.erase(_faces.begin() + findFacePosition(f));

	//	delete vertex
	_simpleVertices.erase(_simpleVertices.begin() + findVertexPosition(oldId));
}


void Mesh::simplifyMesh(GLdouble percentage)
{
	//compute number of iterations
	size_t maxIt;
	if (percentage == 0.0)
		maxIt = _simpleVertices.size() - 1;
	else
		maxIt = static_cast<size_t>((1.0 - percentage) * _simpleVertices.size());

	//compute the Q matrices for all vertices
	for (auto& v : _simpleVertices)
		computeInitialQuad(v);

	//select all valid pairs
	//	set neighbors of all vertices
	for (auto f : _faces)
	{
		getVertex(f._vertices[0])._neighbors.insert(f._vertices[1]);
		getVertex(f._vertices[0])._neighbors.insert(f._vertices[2]);

		getVertex(f._vertices[1])._neighbors.insert(f._vertices[0]);
		getVertex(f._vertices[1])._neighbors.insert(f._vertices[2]);

		getVertex(f._vertices[2])._neighbors.insert(f._vertices[0]);
		getVertex(f._vertices[2])._neighbors.insert(f._vertices[1]);
	}

	//	update pairs
	for (size_t i = 0; i < _simpleVertices.size(); ++i)
	{
		for (auto& n : _simpleVertices[findVertexPosition(i)]._neighbors)
			_pairsSet.insert(Pair(_simpleVertices[findVertexPosition(i)]._id, n));
	}
	for (auto& p : _pairsSet)
	{
		_pairs.push_back(p);
	}
	_pairsSet.clear();

	for (int i = 0; i < _pairs.size(); ++i)
		_pairs[i]._id = i;

	//compute the optimal contraction target for each valid pair; the error of this target vertex becomes the cost of contracting that pair
	computeInitialCost();

	for (size_t i = 0; i < maxIt; ++i)
	{
		//std::cout << ">iteration: " << i + 1 << "/" << maxIt << std::endl;

		//find the index of the pair with the lowest cost
		GLfloat lowestCost = _pairs[0]._cost;
		GLuint lowestCostIdx;
		for (size_t i = 0; i < _pairs.size(); ++i)
		{
			if (_pairs[i]._cost <= lowestCost)
			{
				lowestCostIdx = i;
				lowestCost = _pairs[i]._cost;
			}
		}

		//iteratively remove the pair (v1, v2) of lest cost, contract this pair, and update the costs of all valid pairs involving v
		//	create a new simple vertex
		GLuint newId = _pairs[lowestCostIdx]._vertices[0];
		GLuint oldId = _pairs[lowestCostIdx]._vertices[1];

		collapse(newId, oldId);

		//compute the optimal contraction target for each valid pair; the error of this target vertex becomes the cost of contracting that pair
		computeCost(newId);
	}
}
/*
std::array<std::array<GLdouble, 3>, 3> transpose(std::array<std::array<GLdouble, 3>, 3> matrix)
{
	return
	{{
		{matrix[0][0], matrix[1][0], matrix[2][0]},
		{matrix[0][1], matrix[1][1], matrix[2][1]},
		{matrix[0][2], matrix[1][2], matrix[2][2]}
	}};
}

GLdouble det(GLdouble a, GLdouble b, GLdouble c, GLdouble d)
{
	return a * d - b * c;
}

std::array<std::array<GLdouble, 3>, 3> inverse(std::array<std::array<GLdouble, 3>, 3> matrix)
{
	GLdouble det0 = matrix[0][0] * det(matrix[1][1], matrix[1][2], matrix[2][1], matrix[2][2]) - matrix[0][1] * det(matrix[1][0], matrix[1][2], matrix[2][0], matrix[2][2]) + matrix[0][2] * det(matrix[1][0], matrix[1][1], matrix[2][0], matrix[2][1]);
	matrix = transpose(matrix);

	return
	{{
		{
			det(matrix[1][1], matrix[1][2], matrix[2][1], matrix[2][2]) / det0,
			-det(matrix[1][0], matrix[1][2], matrix[2][0], matrix[2][2]) / det0,
			det(matrix[1][0], matrix[1][1], matrix[2][0], matrix[2][1]) / det0
		},
		{
			-det(matrix[0][1], matrix[0][2], matrix[2][1], matrix[2][2]) / det0,
			det(matrix[0][0], matrix[0][2], matrix[2][0], matrix[2][2]) / det0,
			-det(matrix[0][0], matrix[0][1], matrix[2][0], matrix[2][1]) / det0
		},
		{
			det(matrix[0][1], matrix[0][2], matrix[1][1], matrix[1][2]) / det0,
			-det(matrix[0][0], matrix[0][2], matrix[1][0], matrix[1][2]) / det0,
			det(matrix[0][0], matrix[0][1], matrix[1][0], matrix[1][1]) / det0
		}
	}};
}

std::array<std::array<GLdouble, 3>, 3> sum(std::array<std::array<GLdouble, 3>, 3> matrix1, std::array<std::array<GLdouble, 3>, 3> matrix2)
{
	std::array<std::array<GLdouble, 3>, 3> result;

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			result[i][j] = matrix1[i][j] + matrix2[i][j];

	return result;
}

std::array<std::array<GLdouble, 3>, 3> multiply(std::array<std::array<GLdouble, 3>, 3> matrix1, std::array<std::array<GLdouble, 3>, 3> matrix2)
{
	std::array<std::array<GLdouble, 3>, 3> result;

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
		{
			result[i][j] = 0;
			for (int k = 0; k < 3; ++k)
				result[i][j] += matrix1[i][k] * matrix2[k][j];
		}

	return result;
}

std::array<std::array<GLdouble, 3>, 3> divide(std::array<std::array<GLdouble, 3>, 3> matrix, GLdouble scalar)
{
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			matrix[i][j] /= scalar;
	return matrix;
}

std::array<std::array<GLdouble, 3>, 3> pseudoInverse(std::array<std::array<GLdouble, 3>, 3> matrix)
{
	return multiply(inverse(multiply(transpose(matrix), matrix)), transpose(matrix));
}

std::array<std::array<GLdouble, 3>, 3> findTransformationMatrix(std::array<std::array<GLdouble, 3>, 3> triangle, std::array<std::array<GLdouble, 3>, 3> invUnit)
{
	return multiply(triangle, invUnit);
}

std::array<std::array<GLdouble, 3>, 3> getRotationMatrix(std::array<std::array<GLdouble, 3>, 3> matrix)
{
	Eigen::Matrix<double, 3, 3> T
	{
		{matrix[0][0], matrix[0][1], matrix[0][2]},
		{matrix[1][0], matrix[1][1], matrix[1][2]},
		{matrix[2][0], matrix[2][1], matrix[2][2]}
	};

	Eigen::BDCSVD<Eigen::Matrix<double, 3, 3>> svd(T, Eigen::ComputeFullU | Eigen::ComputeFullV);

	auto U = svd.matrixU();
	auto V = svd.matrixV();

	auto R = U * (V.transpose());

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			matrix[i][j] = R(i, j);

	return matrix;
}

GLdouble area(std::array<std::array<GLdouble, 3>, 3> triangle)
{
	GLdouble a, b, c;
	a = sqrt(pow(triangle[0][1] - triangle[0][0], 2.0) + pow(triangle[1][1] - triangle[1][0], 2.0) + pow(triangle[2][1] - triangle[2][0], 2.0));
	b = sqrt(pow(triangle[0][2] - triangle[0][0], 2.0) + pow(triangle[1][2] - triangle[1][0], 2.0) + pow(triangle[2][2] - triangle[2][0], 2.0));
	c = sqrt(pow(triangle[0][2] - triangle[0][1], 2.0) + pow(triangle[1][2] - triangle[1][1], 2.0) + pow(triangle[2][2] - triangle[2][1], 2.0));

	GLdouble s = (a + b + c) / 2.0;
	return sqrt(s * (s - a) * (s - b) * (s - c));
}

GLdouble calculateScale(std::array<std::array<GLdouble, 3>, 3> triangle)
{
	GLdouble unitArea = sqrt(3.0) / 4.0;

	return sqrt(area(triangle) / unitArea);
}

void Mesh::aeap()
{
	std::array<std::array<GLdouble, 3>, 3> unit = { {{-0.5 + 1.0, 0.0 + 1.0, 0.0 + 1.0}, {0.5 + 1.0, 0.0 + 1.0, 0.0 + 1.0}, {0.0 + 1.0, 0.0 + 1.0, sqrt(3.0) / 2.0 + 1.0}} };
	std::array<std::array<GLdouble, 3>, 3> invUnit = pseudoInverse(unit);

	std::array<std::array<GLdouble, 3>, 3> equilateral;
	std::vector<std::array<std::array<GLdouble, 3>, 3>> newTriangles;

	std::array<std::array<GLdouble, 3>, 3> triangle;
	std::array<std::array<GLdouble, 3>, 3> R;
	std::vector<std::array<std::array<GLdouble, 3>, 3>> L;

	GLdouble s;

	//local step
	for (auto& f : _faces)
	{
		triangle = 
		{ {
		{getVertex(f._vertices[0])._position.x, getVertex(f._vertices[1])._position.x, getVertex(f._vertices[2])._position.x},
		{getVertex(f._vertices[0])._position.y, getVertex(f._vertices[1])._position.y, getVertex(f._vertices[2])._position.y},
		{getVertex(f._vertices[0])._position.z, getVertex(f._vertices[1])._position.z, getVertex(f._vertices[2])._position.z}
		} };

		R = getRotationMatrix(findTransformationMatrix(triangle, invUnit));
		s = calculateScale(triangle);
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				R[i][j] *= s;
		equilateral = multiply(R, unit);
		L.push_back(multiply(equilateral, pseudoInverse(triangle)));
	}

	//global step
	int i = 0;
	for (auto& f : _faces)
	{
		triangle =
		{ {
		{getVertex(f._vertices[0])._position.x, getVertex(f._vertices[1])._position.x, getVertex(f._vertices[2])._position.x},
		{getVertex(f._vertices[0])._position.y, getVertex(f._vertices[1])._position.y, getVertex(f._vertices[2])._position.y},
		{getVertex(f._vertices[0])._position.z, getVertex(f._vertices[1])._position.z, getVertex(f._vertices[2])._position.z}
		} };

		newTriangles.push_back(multiply((divide(sum(L[i], transpose(L[i])), area(triangle) / 2.0)), pseudoInverse(multiply(transpose(triangle), transpose(pseudoInverse(triangle))))));
		++i;
	}


	i = 0;
	for (auto& f : _faces)
	{
		getVertex(f._vertices[0])._position.x = newTriangles[i][0][0];
		getVertex(f._vertices[0])._position.y = newTriangles[i][0][1];
		getVertex(f._vertices[0])._position.z = newTriangles[i][0][2];

		getVertex(f._vertices[1])._position.x = newTriangles[i][1][0];
		getVertex(f._vertices[1])._position.y = newTriangles[i][1][1];
		getVertex(f._vertices[1])._position.z = newTriangles[i][1][2];

		getVertex(f._vertices[2])._position.x = newTriangles[i][2][0];
		getVertex(f._vertices[2])._position.y = newTriangles[i][2][1];
		getVertex(f._vertices[2])._position.z = newTriangles[i][2][2];
		++i;
	}

}
*/

GLdouble calculateLength(GLdouble x, GLdouble y, GLdouble z)
{
	return sqrt(pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0));
}

void Mesh::split(Pair& pair)
{
	SimpleVertex e1 = getVertex(pair._vertices[0]);
	SimpleVertex e2 = getVertex(pair._vertices[1]);

	//get faces
	std::vector<GLuint> faces;
	for (auto& f : e1._faces)
	{
		if (contains(f, e2._id))
			faces.push_back(f);
	}
	//std::cout << faces.size() << "\t";

	//other vertices
	std::vector<GLuint> v;
	for (int i = 0; i < faces.size(); ++i)
		for (int j = 0; j < 3; ++j)
			if (getFace(faces[i])._vertices[j] != e1._id && getFace(faces[i])._vertices[j] != e2._id)
				v.push_back(getFace(faces[i])._vertices[j]);

	//std::cout << v.size() << "\t";

	if (v.size() == faces.size() && v.size() == 2)
	{
		//add new vertex
		glm::vec3 newPosition = (e1._position + e2._position) / 2.f;
		GLuint newId = _simpleVertices[_simpleVertices.size() - 1]._id + 1;

		_simpleVertices.push_back(SimpleVertex(newId, newPosition));

		//remove old faces from e2
		for (auto f : faces)
			e2._faces.erase(f);

		//change starting faces' vertrices
		for (int i = 0; i < faces.size(); ++i)
		{
			getFace(faces[i])._vertices[0] = e1._id;
			getFace(faces[i])._vertices[1] = v[i];
			getFace(faces[i])._vertices[2] = newId;
			getFace(faces[i]).sortVertices();
		}

		//add neighbors to new Vertex
		getVertex(newId)._neighbors.insert(e1._id);
		getVertex(newId)._neighbors.insert(e2._id);
		for (auto v_ : v)
			getVertex(newId)._neighbors.insert(v_);

		//remove neighbors from pair
		e1._neighbors.erase(e2._id);
		e2._neighbors.erase(e1._id);

		//add new vertex to other vertices' neighbors
		e1._neighbors.insert(newId);
		e2._neighbors.insert(newId);
		for (auto v_ : v)
			getVertex(v_)._neighbors.insert(newId);

		GLuint newFaceId;
		//add faces
		for (auto v_ : v)
		{
			newFaceId = _faces[_faces.size() - 1]._id + 1;
			_faces.push_back(Face(newFaceId, { {e2._id, v_, newId} }));
		}

		//add new faces to vertices
		for (int i = 0; i < v.size(); ++i)
		{
			e2._faces.insert(_faces[_faces.size() - 1]._id - i);
			getVertex(v[i])._faces.insert(_faces[_faces.size() - 1]._id - i);
			getVertex(newId)._faces.insert(_faces[_faces.size() - 1]._id - i);
		}
		for (auto f : faces)
			getVertex(newId)._faces.insert(f);

		//update pairs
		//	add new pairs
		for (auto v_ : v)
		{
			_pairs.push_back(Pair(newId, v_));
			_pairs[_pairs.size() - 1]._id = _pairs[_pairs.size() - 2]._id + 1;
		}
		_pairs.push_back(Pair(newId, e2._id));
		_pairs[_pairs.size() - 1]._id = _pairs[_pairs.size() - 2]._id + 1;
		//	change the starging pair
		pair.set(e1._id, newId);
	}
	else
		_pairs.erase(_pairs.begin() + findPairPosition(pair._id));
}

void Mesh::flip(Pair& pair)
{
	SimpleVertex e1 = getVertex(pair._vertices[0]);
	SimpleVertex e2 = getVertex(pair._vertices[1]);

	int deviation1, deviation2;
	GLuint newFaceId;

	//get faces
	std::vector<GLuint> faces;
	for (auto& f : e1._faces)
	{
		if (contains(f, e2._id))
			faces.push_back(f);
	}
	//std::cout << faces.size() << "\t";

	//other vertices
	std::vector<GLuint> v;
	for (int i = 0; i < faces.size(); ++i)
		for (int j = 0; j < 3; ++j)
			if (getFace(faces[i])._vertices[j] != e1._id && getFace(faces[i])._vertices[j] != e2._id)
				v.push_back(getFace(faces[i])._vertices[j]);

	if (v.size() == 2 && v.size() == faces.size())
	{
		deviation1 =
			abs(static_cast<int>(e1._neighbors.size() - 6)) +
			abs(static_cast<int>(e2._neighbors.size() - 6)) +
			abs(static_cast<int>(getVertex(v[0])._neighbors.size() - 4)) +
			abs(static_cast<int>(getVertex(v[1])._neighbors.size() - 4));

		deviation2 =
			abs(static_cast<int>(getVertex(v[0])._neighbors.size() - 6)) +
			abs(static_cast<int>(getVertex(v[1])._neighbors.size() - 6)) +
			abs(static_cast<int>(e2._neighbors.size() - 4)) +
			abs(static_cast<int>(e1._neighbors.size() - 4));

		if (deviation2 - deviation1 < 0)
		{
			//remove faces from vertices
			e1._faces.erase(faces[1]);
			e2._faces.erase(faces[0]);

			//update neighbors
			getVertex(v[0])._neighbors.insert(v[1]);
			getVertex(v[1])._neighbors.insert(v[0]);
			getVertex(e1._id)._neighbors.erase(e2._id);
			getVertex(e2._id)._neighbors.erase(e1._id);

			//change pair
			pair.set(v[0], v[1]);

			//update faces and add them to vertices
			getFace(faces[0])._vertices[0] = v[0];
			getFace(faces[0])._vertices[1] = v[1];
			getFace(faces[0])._vertices[2] = e1._id;
			getVertex(v[1])._faces.insert(faces[0]);
			getFace(faces[0]).sortVertices();

			getFace(faces[1])._vertices[0] = v[0];
			getFace(faces[1])._vertices[1] = v[1];
			getFace(faces[1])._vertices[2] = e2._id;
			getVertex(v[0])._faces.insert(faces[1]);
			getFace(faces[1]).sortVertices();
		}
	}

}

void Mesh::vertexRelocation(SimpleVertex& vertex)
{
	glm::vec3 avg(0.f);

	for (auto n : vertex._neighbors)
	{
		avg += getVertex(n)._position;
	}

	avg /= vertex._neighbors.size();
	vertex._position = avg;
}

void Mesh::incrementalRemeshing()
{
	//get target edge length (in this case average length of all pairs)
	GLdouble L = 0.0;
	for (auto& p : _pairs)
		L += calculateLength
		(
			getVertex(p._vertices[0])._position[0] -
			getVertex(p._vertices[1])._position[0],
			getVertex(p._vertices[0])._position[1] -
			getVertex(p._vertices[1])._position[1],
			getVertex(p._vertices[0])._position[2] -
			getVertex(p._vertices[1])._position[2]
		);
	L /= _pairs.size();

	GLdouble L_max = 4.0 * L / 3.0;
	GLdouble L_min = 4.0 * L / 5.0;

	//collapse edges shorter than L_min
	for (int i = 0; i < _pairs.size(); ++i)
	{
		if
		(
			calculateLength
			(
				getVertex(_pairs[i]._vertices[0])._position[0] -
				getVertex(_pairs[i]._vertices[1])._position[0],
				getVertex(_pairs[i]._vertices[0])._position[1] -
				getVertex(_pairs[i]._vertices[1])._position[1],
				getVertex(_pairs[i]._vertices[0])._position[2] -
				getVertex(_pairs[i]._vertices[1])._position[2]
			) < L_min
		)
		collapse(_pairs[i]._vertices[0], _pairs[i]._vertices[1]);
	}

	//split edges longer than L_max
	for (int i = _pairs.size() - 1; i >= 0; --i)
	{
		if
		(
			calculateLength
			(
				getVertex(_pairs[i]._vertices[0])._position[0] -
				getVertex(_pairs[i]._vertices[1])._position[0],
				getVertex(_pairs[i]._vertices[0])._position[1] -
				getVertex(_pairs[i]._vertices[1])._position[1],
				getVertex(_pairs[i]._vertices[0])._position[2] -
				getVertex(_pairs[i]._vertices[1])._position[2]
			) > L_max
		)
		split(_pairs[i]);
	}

	//flip edges (failed)
	//for (int i = _pairs.size() - 1; i >= 0; --i)
	//	flip(_pairs[i]);

	//vertex relocation
	for (auto& v : _simpleVertices)
		vertexRelocation(v);
}