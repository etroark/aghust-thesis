#include "libs.h"

#include "app.h"

/**
 * GUI class
 */
class Gui
{
	App* _app;	/**> App object pointer*/

	bool _filledPolygons = false;	/**> filled polygons check*/
	char _filePath[100] = "obj files/capybara.obj";	/**> file path char array*/
	GLfloat _percentage = 20.f;	/**> percentage destinated quantity in % of output vertices compared to input vertices*/

	bool _toInit = false;	/**> check if models are ready to initialize*/
	int _meshMode = 1;	/**> mesh mode for radio buttons*/
	std::string _log;	/**> mesh loading log*/

public:

	/**
	 * contructor
	 * @param title GLFW window title
	 * @param width window width
	 * @param height window height
	 * @param GLverMajor OpenGL major version
	 * @param GLverMinor OpenGL minor version
	 * @param resizable resizable window boolean
	 */
	Gui
	(
		const char* title,
		const int width, const int height,
		const int GLverMajor, const int GLverMinor,
		bool resizable
	);

	/**
	 * destructor
	 */
	~Gui()
	{
		delete _app;
	}

	/**
	 * App getter
	 * @return App object pointer
	 */
	App* getApp()
	{
		return _app;
	}

	/**
	 * new ImGui frame
	 */
	void newFrame();
	/**
	 * clear GUI
	 */
	void clear();
	/**
	 * render GUI
	 */
	void render();
	/**
	 * shutdown GUI
	 */
	void shutdown();

	/**
	 * render loop
	 */
	void loop();
};