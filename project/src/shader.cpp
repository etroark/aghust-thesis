#include "../include/shader.h"

std::string Shader::loadShaderSource(const char* fileName)
{
	//load file and save the source
	std::string temp = "";
	std::string src = "";
	std::ifstream inFile;

	inFile.open(fileName);

	if (inFile.is_open())
	{
		while (std::getline(inFile, temp))
			src += temp + "\n";
	}
	else
	{
		std::cout << "could not open " << fileName << std::endl;
	}

	inFile.close();

	//change the OpenGL version to match the application
	std::string versionNr =
		std::to_string(_versionMajor) +
		std::to_string(_versionMinor) +
		"0";
	src.replace(src.find("#version"), 12, "#version " + versionNr);

	return src;
}

GLuint Shader::loadShader(GLenum type, const char* fileName)
{
	char infoLog[512];
	GLint success;

	GLuint shader = glCreateShader(type);	//create shader of the designated type
	std::string strSrc = loadShaderSource(fileName);
	const GLchar* src = strSrc.c_str();
	glShaderSource(shader, 1, &src, NULL);	//give the shader the glsl source
	glCompileShader(shader);	//compile shader

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);	//get shader vector of ints
	if (!success)	//check if everything went ok
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "could not compile " << fileName << std::endl;
		std::cout << infoLog << std::endl;
	}

	return shader;
}

void Shader::linkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	char infoLog[512];
	GLint success;

	_id = glCreateProgram();	//create the program

	glAttachShader(_id, vertexShader);	//attach vertex shader

	glAttachShader(_id, fragmentShader);	//attach fragment shader

	glLinkProgram(_id);	//link the program

	glGetProgramiv(_id, GL_LINK_STATUS, &success);	//get program vevtor of ints
	if (!success)	//check if everything went ok
	{
		glGetProgramInfoLog(_id, 512, NULL, infoLog);
		std::cout << "could not link the program" << std::endl;
		std::cout << infoLog << std::endl;
	}

	glUseProgram(0);	//use the program
}

Shader::Shader(const int versionMajor, const int versionMinor, const char* vertexFile, const char* fragmentFile) : _versionMajor(versionMajor), _versionMinor(versionMinor)
{
	_vertexShader = loadShader(GL_VERTEX_SHADER, vertexFile);	//load vertex shader

	_fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentFile);	//load fragment shader

	linkProgram(_vertexShader, _fragmentShader);	//link shaders to the program

	//shaders created and attached to the program, no need to keeping them
	glDeleteShader(_vertexShader);
	glDeleteShader(_fragmentShader);
}

