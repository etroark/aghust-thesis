#include "../include/app.h"

GLuint App::_polygonMode = GL_LINE;

//private functions
void App::initGLFW()
{
	//init OpenGL framework and error check
	if (glfwInit() == GLFW_FALSE)	
	{
		std::cout << "ERROR::GLFW_INIT_FAILED" << std::endl;
		glfwTerminate();
	}
}

void App::initWindow(const char* title, bool resizable)
{
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//setting OpenGL profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _GLverMajor);	//setting major OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _GLverMinor);	//setting minor OpenGL version
	glfwWindowHint(GLFW_RESIZABLE, resizable);	//setting resizable window

	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //for macOS

	_window = glfwCreateWindow(_windowWidth, _windowHeight, title, NULL, NULL);	//creating window

	//error check
	if (_window == nullptr)
	{
		std::cout << "error: GLFW window init failed" << std::endl;
		glfwTerminate();
	}

	glfwGetFramebufferSize(_window, &_framebufferWidth, &_framebufferHeight);	//setting framebuffer size
	glfwSetFramebufferSizeCallback(_window, App::framebufferResizeCallback);

	glfwMakeContextCurrent(_window);	//making the context current
	//glfwSwapInterval(0);	//disable v-sync
}

void App::initGLEW()
{
	//necessary for initializing GLEW
	glewExperimental = GL_TRUE;

	//init GLEW and error check
	if (glewInit() != GLEW_OK)
	{
		std::cout << "error: glewInit" << std::endl;
		glfwTerminate();
	}
}

void App::initOpenGLOptions()
{
	glEnable(GL_DEPTH_TEST);	//enabling storing z-values of the fragments in the buffer

	glDisable(GL_CULL_FACE); //disable cull face to draw from both sides

	//glEnable(GL_CULL_FACE);	//enabling culling for greater performance
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);	//setting winding order to CCW - counter-clockwise (3 -> 1 -> 2)

	glEnable(GL_BLEND);	//enabling blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//for blending colors (src: apha, dst: 1 - alpha, it works smh)

	glEnable(GL_LINE_SMOOTH);	//enable line antialiasing
	glLineWidth(2.f);	//thicker the lines

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//filling polygons (other options: points or lines)
}

void App::initMatrices()
{
	_ViewMatrix = glm::lookAt(_camPosition, glm::vec3(.0f), glm::vec3(.0f, 1.f, .0f));	//init view matrix

	_ProjectionMatrix = glm::perspective
	(
		glm::radians(_fov),
		static_cast<float>(_framebufferWidth) / _framebufferHeight,
		_nearPlane,
		_farPlane
	);	//init projection matrix
}

void App::initShader()
{
	_shader = new Shader
	(
		_GLverMajor,
		_GLverMinor,
		"resource/vertexCore.glsl",	//glsl - OpenGL shading language
		"resource/fragmentCore.glsl"
	);
}

/*
void App::initTextures()
{
	//_textures.push_back(new Texture("images/texture.png", GL_TEXTURE_2D));	//add 2D texture to vector
	//_textures.push_back(new Texture("images/textureSpecular.png", GL_TEXTURE_2D));
}
*/

void App::initMaterials()
{
	_materials.push_back(new Material(glm::vec3(.5f), glm::vec3(1.f), glm::vec3(2.f)));
}

//void App::initModels(const char* fileName, glm::vec3 position1, glm::vec3 position2, glm::vec3 position3, glm::vec3 position4, glm::vec3 rotation, glm::vec3 scale, glm::vec3 color, GLdouble percentage)
void App::initModels(const char* fileName, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 color, GLdouble percentage)
{
	for (auto*& i : _models)
		delete i;
	_models.clear();

	//original model
	_models.push_back(new Model
	(
		//position1,	//position
		position,	//position
		rotation,	//rotation
		scale,	//scale
		_materials[0],
		fileName,
		color
	));

	//original mesh
	_models.push_back(new Model
	(
		//position2,	//position
		position,	//position
		rotation,	//rotation
		scale,	//scale
		_materials[0],
		fileName,
		color,
		true
	));
	
	//simplified mesh
	_models.push_back(new Model
	(
		//position3,	//position
		position,	//position
		rotation,	//rotation
		scale,	//scale
		_materials[0],
		fileName,
		color,
		true,
		true,
		percentage
	));
	
	//aeap mesh
	_models.push_back(new Model
	(
		_models[2],
		//position4,
		position,	//position
		rotation,
		scale
	));
}

void App::initLights()
{
	_lights.push_back(new glm::vec3(.0f));
}

void App::initUniforms()
{
	_shader->setMat4fv(_ViewMatrix, "ViewMatrix");
	_shader->setMat4fv(_ProjectionMatrix, "ProjectionMatrix");
	_shader->setVec3f(*_lights[0], "lightPos0");
}

void App::updateUniforms()
{
	//update view matrix (camera)
	_ViewMatrix = _camera.getViewMatrix();

	initUniforms();

	//update framebuffer size and projection matrix
	glfwGetFramebufferSize(_window, &_framebufferWidth, &_framebufferHeight);

	_ProjectionMatrix = glm::perspective
	(
		glm::radians(_fov),
		static_cast<float>(_framebufferWidth) / _framebufferHeight,
		_nearPlane,
		_farPlane
	);

	_shader->setMat4fv(_ProjectionMatrix, "ProjectionMatrix");
}

//constructor
App::App
(
	const char* title,
	const int width, const int height,
	const int GLverMajor, const int GLverMinor,
	bool resizable
) : _windowWidth(width), _windowHeight(height), _GLverMajor(GLverMajor), _GLverMinor(GLverMinor), _camera()
{
	//init variables
	_window = nullptr;
	_framebufferWidth = _windowWidth;
	_framebufferHeight = _windowHeight;

	_fov = 90.f;		//fov
	_nearPlane = .1f;	//draw distance close range
	_farPlane = 1000.f;	//draw distance long range

	initGLFW();
	initWindow(title, resizable);
	initGLEW();
	initOpenGLOptions();

	//glfwSetKeyCallback(_window, enterCallback);
	
	initMatrices();
	initShader();
	//initTextures();
	initMaterials();
	initLights();
	initUniforms();

	//initModels(fileName, position1, position2, position3, position4, rotation, scale, color, percentage);
	//glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	//disabling the cursor
}

//destructor
App::~App()
{
	glfwDestroyWindow(_window);
	glfwTerminate();

	delete _shader;
	/*for (size_t i = 0; i < _textures.size(); ++i)
		delete _textures[i];*/
	for (size_t i = 0; i < _materials.size(); ++i)
		delete _materials[i];
	for (auto*& i : _models)
		delete i;
	for (size_t i = 0; i < _lights.size(); ++i)
		delete _lights[i];
}

//accessors
int App::getWindowShouldClose()
{
	return glfwWindowShouldClose(_window);
}

//modifiers
void App::setWindowShouldClose()
{
	glfwSetWindowShouldClose(_window, GLFW_TRUE);
}

//functions
void App::updateDt()
{
	_curTime = static_cast<float>(glfwGetTime());
	_dt = _curTime - _lastTime;
	_lastTime = _curTime;
}

void App::updateMouseInput()
{
	if
	(glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS &&
		(
			!glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS &&
			!glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS &&
			!glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS &&
			!glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS
		)
	)
	{
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwGetCursorPos(_window, &_mouseX, &_mouseY);

		if (_firstMouse)
		{
			_lastMouseX = _mouseX;
			_lastMouseY = _mouseY;
			_firstMouse = false;
		}

		//calculate offset
		_mouseOffsetX = -_mouseX + _lastMouseX;
		_mouseOffsetY = -_lastMouseY + _mouseY;

		//set last X and Y
		_lastMouseX = _mouseX;
		_lastMouseY = _mouseY;
	}
	else if
	(!glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS &&
		(
			glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS ||
			glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS ||
			glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS ||
			glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS
		)
	)
	{
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwGetCursorPos(_window, &_mouseX, &_mouseY);

		if (_firstMouse)
		{
			_lastMouseX = _mouseX;
			_lastMouseY = _mouseY;
			_firstMouse = false;
		}

		//calculate offset
		_mouseOffsetX = _mouseX - _lastMouseX;
		_mouseOffsetY = _lastMouseY - _mouseY;

		//set last X and Y
		_lastMouseX = _mouseX;
		_lastMouseY = _mouseY;
	}
	else
	{
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		_firstMouse = true;
		_mouseOffsetX = 0.0;
		_mouseOffsetY = 0.0;
	}
}

void App::updateKeyboardInput()
{
	//program
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		setWindowShouldClose();

	//camera
	if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
		_camera.move(_dt, FORWARD);
	if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
		_camera.move(_dt, BACKWARD);
	if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
		_camera.move(_dt, LEFT);
	if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
		_camera.move(_dt, RIGHT);
}

void App::updateInput()
{
	updateMouseInput();
	updateKeyboardInput();
	_camera.rotate(_dt, _mouseOffsetX, _mouseOffsetY);

	//move light with camera
	*_lights[0] = _camera.getPosition();
}

void App::update()
{
	updateDt();
	updateInput();
}

void App::clear()
{
	glClearColor(.25f, .25f, .25f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void App::render(int meshMode)
{
	//clear
	//clear();

	//update the uniforms
	updateUniforms();

	//render models
	//for (auto& i : _models)
	//	i->render(_shader, App::_polygonMode);

	//render selected model
	if(_models.size() == 4)
		_models[meshMode]->render(_shader, App::_polygonMode);

	//end draw
	//glfwSwapBuffers(_window);
	//glFlush();

	//free
	glBindVertexArray(0);
	glUseProgram(0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//static functions
void App::framebufferResizeCallback(GLFWwindow* window, int fbW, int fbH)
{
	glViewport(0, 0, fbW, fbH);
}

/*
void App::enterCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		{
			switch (App::_polygonMode)
			{
			case GL_LINE:
				App::_polygonMode = GL_FILL;
				break;
			default:
				App::_polygonMode = GL_LINE;
				break;
			}
		}
	}
}
*/