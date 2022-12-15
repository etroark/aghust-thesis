#include "../include/objLoader.h"

std::string ObjLoader::_log = "";

void ObjLoader::loadObj(const char* fileName, glm::vec3 color)
{
	//vertices
	std::vector<glm::fvec3> vertexPositions;
	std::vector<glm::fvec2> vertexTexcoords;
	std::vector<glm::fvec3> vertexNormals;

	//face vectors
	std::vector<GLuint> vertexPositionIndices;
	std::vector<GLuint> vertexTexcoordIndices;
	std::vector<GLuint> vertexNormalIndices;

	std::stringstream ss;
	std::string line = "";
	std::string prefix = "";

	glm::vec3 tempVec3;
	glm::vec2 tempVec2;
	GLint tempGLint = 0;
	int counter;
	int fCounter;
	int nrOfFaces = 0;
	int addFace;

	//file open error check
	std::ifstream inFile(fileName);
	if (inFile.is_open())
	{

		//read one line at a time
		while (std::getline(inFile, line))
		{
			//get the prefix
			ss.clear();
			ss.str(line);
			ss >> prefix;

			if (prefix == "v") //vertex position
			{
				ss >> tempVec3.x >> tempVec3.y >> tempVec3.z;
				vertexPositions.push_back(tempVec3);
			}
			else if (prefix == "vt") //vertex texcoord
			{
				ss >> tempVec2.x >> tempVec2.y;
				vertexTexcoords.push_back(tempVec2);
			}
			else if (prefix == "vn") //vertex normal
			{
				ss >> tempVec3.x >> tempVec3.y >> tempVec3.z;
				vertexNormals.push_back(tempVec3);
			}
			else if (prefix == "f")
			{
				counter = 0;
				fCounter = 0;
				while (ss >> tempGLint)
				{
					//pushing indices into correct arrays
					switch (counter)
					{
					case 0:
						vertexPositionIndices.push_back(tempGLint);
						break;
					case 1:
						vertexTexcoordIndices.push_back(tempGLint);
						break;
					case 2:
						vertexNormalIndices.push_back(tempGLint);
						break;
					default:
						break;
					}

					//handling characters
					if (ss.peek() == '/')
					{
						++counter;
						ss.ignore(1, '/');
						if (ss.peek() == '/')
						{
							++counter;
							ss.ignore(1, '/');
						}
					}
					else if (ss.peek() == ' ')
					{
						++counter;
						++fCounter;
						ss.ignore(1, ' ');
					}

					//reset the counter
					if (vertexNormals.size() == 0 && counter > 1)
						counter = 0;
					else if (counter > 2)
						counter = 0;
				}

				nrOfFaces = std::max(fCounter, nrOfFaces);				
			}
			else
			{

			}
		}
		//build final mesh
		_vertices.resize(vertexPositionIndices.size(), Vertex());

		//	load all indices
		for (size_t i = 0; i < _vertices.size(); ++i)
		{
			_vertices[i]._position = vertexPositions[vertexPositionIndices[i] - 1];

			if (vertexTexcoords.size() > 0)
				_vertices[i]._texcoord = vertexTexcoords[vertexTexcoordIndices[i] - 1];

			if (vertexNormals.size() > 0)
				_vertices[i]._normal = vertexNormals[vertexNormalIndices[i] - 1];

			_vertices[i]._color = color;
		}
		
		//	save the indices
		for (int i = 0; i < _vertices.size(); i += nrOfFaces)
		{
			_indices.push_back(i);
			_indices.push_back(i + 1);
			_indices.push_back(i + 2);

			if (nrOfFaces == 4)
			{
				_indices.push_back(i);
				_indices.push_back(i + 2);
				_indices.push_back(i + 3);
			}
		}

		//build final simple mesh
		_simpleVertices.resize(vertexPositions.size(), SimpleVertex());

		//	load all indices
		for (size_t i = 0; i < _simpleVertices.size(); ++i)
		{
			_simpleVertices[i]._id = i;
			_simpleVertices[i]._position = vertexPositions[i];
		}
		//	save indices and faces
		GLuint faceId = 0;
		for (int i = 0; i < vertexPositionIndices.size() - nrOfFaces + 1; i += nrOfFaces)
		{
			_faces.push_back(Face(faceId, { vertexPositionIndices[i] - 1, vertexPositionIndices[i + 1] - 1, vertexPositionIndices[i + 2] - 1 }));
			_simpleVertices[vertexPositionIndices[i] - 1]._faces.insert(faceId);
			_simpleVertices[vertexPositionIndices[i + 1] - 1]._faces.insert(faceId);
			_simpleVertices[vertexPositionIndices[i + 2] - 1]._faces.insert(faceId);
			++faceId;

			if (nrOfFaces == 4)
			{
				_faces.push_back(Face(faceId, { vertexPositionIndices[i] - 1, vertexPositionIndices[i + 2] - 1, vertexPositionIndices[i + 3] - 1 }));
				_simpleVertices[vertexPositionIndices[i] - 1]._faces.insert(faceId);
				_simpleVertices[vertexPositionIndices[i + 2] - 1]._faces.insert(faceId);
				_simpleVertices[vertexPositionIndices[i + 3] - 1]._faces.insert(faceId);
				++faceId;
			}
		}
		ObjLoader::_log = "";
	}
	else
		ObjLoader::_log = ">wrong file path";
}