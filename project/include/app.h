#pragma once

#include "libs.h"

#include "camera.h"
#include "shader.h"
#include "material.h"
#include "model.h"

//forward class declaration
class Gui;

/**
 * app class
 */
class App
{
	friend class Gui;

	//	window
	GLFWwindow* _window;	/**> OpenGL Framework window*/
	const int _windowWidth;	/**> window width*/
	const int _windowHeight;	/**> window height*/
	int _framebufferWidth;	/**> famebuffer width*/
	int _framebufferHeight;	/**> framebuffer height*/

	//	OpenGL context
	const int _GLverMajor;	/**> OpenGL major version*/
	const int _GLverMinor;	/**> OpenGL minor version*/

	//	delta time
	float _dt = .0f;	/**> delta time*/
	float _curTime = .0f;	/**> current time*/
	float _lastTime = .0f;	/**> current time - delta time*/

	//	mouse input
	double _lastMouseX = .0f;	/**> mouse X position in last time*/
	double _lastMouseY = .0f;	/**> mouse Y position in last time*/
	double _mouseX = .0f;	/**> current mouse X position*/
	double _mouseY = .0f;	/**> current mouse Y position*/
	double _mouseOffsetX = .0f;	/**> _mouseX - _lastMouseX*/
	double _mouseOffsetY = .0f;	/**> _mouseY - _lastMouseY*/
	bool _firstMouse = true;	/**> special check for calculation offset*/

	//	camera
	Camera _camera;	/**> Camera object*/

	//	matrices
	glm::mat4 _ViewMatrix;		/**> matrix that can transform everything around the camera*/
	glm::mat4 _ProjectionMatrix;	/**> projection matrix*/

	//	vectors
	glm::vec3 _camPosition = glm::vec3(.0f);	/**> camera position*/

	//	render variables
	float _fov;	/**> field of view*/
	float _nearPlane;	/**> projection matrix near plane*/
	float _farPlane;	/**> projection matrix far plane*/

	//	shader
	Shader* _shader;	/**> Shader object pointer*/

	//	materials
	std::vector<Material*> _materials;	/**> vector of materials*/

	//	models
	std::vector<Model*> _models;	/**> vector of models*/

	//	lights
	std::vector<glm::vec3*> _lights;	/**> vector of simple lights (position only)*/

	//static variable
	static GLuint _polygonMode;	/**> filled/empty triangles*/

	/**
	 * initialize OpenGL Framework
	 */
	void initGLFW();
	/**
	 * initialize GLFW window
	 * @param title title of the window
	 * @param resizable if one wants to be able to resize the window. I don't
	 */
	void initWindow(const char* title, bool resizable);
	/**
	 * initialize OpenGL Extension Wrangler
	 */
	void initGLEW();
	/**
	 * initialize some OpenGL options
	 */
	void initOpenGLOptions();
	/**
	 * initialize view and projection matrix
	 */
	void initMatrices();
	/**
	 * initialize shader
	 */
	void initShader();
	/**
	 * initialize materials
	 */
	void initMaterials();
	/**
	 * initialize models
	 * @param fileName OBJ file name
	 * @param position model position
	 * @param rotation model rotation
	 * @param scale model scale
	 * @param color vertices color
	 * @param percentage destinated quantity in % of output vertices compared to input vertices
	 */
	void initModels(const char* fileName, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 color, GLdouble percentage);
	/**
	 * initialize lights position
	 */
	void initLights();
	/**
	 * initialize uniforms (view matrix, projection matrix, light position)
	 */
	void initUniforms();

	/**
	 * update view and projection matrices uniforms
	 */
	void updateUniforms();

public:

	/**
	 * App constructor
	 * @param title GLFW window title
	 * @param width window width
	 * @param height window height
	 * @param GLverMajor OpenGL major version
	 * @param GLverMinor OpenGL minor version
	 * @param resizable resizable window boolean
	 */
	App
	(
		const char* title,
		const int width, const int height,
		const int GLverMajor, const int GLverMinor,
		bool resizable
	);

	/**
	 * App destructor
	 */
	virtual ~App();

	/**
	 * get "window should close" parameter
	 */
	int getWindowShouldClose();

	/**
	 * set the window to be closed
	 */
	void setWindowShouldClose();

	/**
	 * update delta time
	 */
	void updateDt();
	/**
	 * update mouse input
	 */
	void updateMouseInput();
	/**
	 * update keyboard input
	 */
	void updateKeyboardInput();
	/**
	 * uses updateMouseInput and updateKeyboardInput
	 */
	void updateInput();
	/**
	 * uses updateDt and updateInput
	 */
	void update();
	/**
	 * clear color buffer, clear depth buffer
	 */
	void clear();
	/**
	 * update uniforms and render desired model
	 */
	void render(int meshMode);

	/**
	 * framebuffer resize callback
	 * @param window GLFW window
	 * @param fbW framebuffer width
	 * @param fbH framebuffer height
	 */
	static void framebufferResizeCallback(GLFWwindow* window, int fbW, int fbH);
};