#include "../include/gui.h"

Gui::Gui
(
	const char* title,
	const int width, const int height,
	const int GLverMajor, const int GLverMinor,
	bool resizable
)
{
	_app = new App(title, width, height, GLverMajor, GLverMinor, resizable);
}

void Gui::newFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Gui::clear()
{
	glClearColor(.25f, .25f, .25f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Gui::render()
{
	ImGui::Begin("front panel", 0, 1 | 2 | 4);
	ImGui::SetWindowSize({ 350, 700 });
	ImGui::SetWindowPos({ 10, 10 });
	ImGui::Text("Use WSAD to move the camera.\nDrag the mouse cursor to rotate the camera.");
	if (ImGui::Button("reset the camera"))
	{
		_app->_camera.reset();
	}

	ImGui::Text("camera speed");
	ImGui::InputFloat("##camSpeed", &_app->_camera._movementSpeed);
	ImGui::SameLine();
	if (ImGui::Button("reset"))
	{
		_app->_camera._movementSpeed = 10.f;
	}

	ImGui::Text("\nfile path");
	ImGui::InputText("##filePath", _filePath, 100);
	ImGui::Text("simplify modifier (%%)");
	ImGui::SliderFloat("##percentage", &_percentage, 0.f, 100.f);
	if (ImGui::Button("load & calculate"))
	{
		_log = ">loading";
		_toInit = true;
	}
	ImGui::SameLine();
	ImGui::Text((_log).c_str());

	ImGui::Text("\n");
	ImGui::Checkbox("filled polygons", &_filledPolygons);

	ImGui::Text("\n");
	if (ImGui::RadioButton("original model", &_meshMode, 0)) { _filledPolygons = true; }
	if (ImGui::RadioButton("original mesh", &_meshMode, 1)) { _filledPolygons = false; }
	if (ImGui::RadioButton("simplified mesh", &_meshMode, 2)) { _filledPolygons = false; }
	if (ImGui::RadioButton("quasi-regular mesh", &_meshMode, 3)) { _filledPolygons = false; }

	if (_app->_models.size() == 4 && _app->_models[0]->_meshes[0]->_vertices.size() > 0)
	{
		ImGui::Text("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		ImGui::Text(static_cast<std::string>("original mesh vertices count: " + std::to_string(_app->_models[1]->_meshes[0]->_simpleVertices.size())).c_str());
		ImGui::Text(static_cast<std::string>("\nsimplified mesh vertices count: " + std::to_string(_app->_models[2]->_meshes[0]->_simpleVertices.size())).c_str());
		ImGui::Text(static_cast<std::string>("processing time: " + std::to_string(_app->_models[2]->_meshes[0]->_simplifyTime) + " s").c_str());
		ImGui::Text(static_cast<std::string>("\nquasi-regular mesh vertices count: " + std::to_string(_app->_models[3]->_meshes[0]->_simpleVertices.size())).c_str());
		ImGui::Text(static_cast<std::string>("processing time: " + std::to_string(_app->_models[3]->_meshes[0]->_aeapTime) + " s").c_str());
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Gui::loop()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(_app->_window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	ImGui::StyleColorsLight();

	while (!_app->getWindowShouldClose())
	{
		newFrame();

		_app->update();
		_app->render(_meshMode);

		if (_toInit)
		{
			_app->initModels
			(
				_filePath,	//obj file name
				glm::vec3(-6.f, 1.f, -4.f),	//position
				glm::vec3(90.0f, 180.0f, 0.0f),	//rotation
				glm::vec3(1.f),	//scale
				glm::vec3(1.f, .5f, .0f),	//vertex color
				_percentage / 100.f //percentage of the number of vertices (simplified : original)
			);

			_log = ObjLoader::_log;
			_toInit = false;
		}

		render();

		switch (_filledPolygons)
		{
		case true:
			App::_polygonMode = GL_FILL;
			break;
		default:
			App::_polygonMode = GL_LINE;
			break;
		}

		glfwPollEvents();
		glfwSwapBuffers(_app->_window);
		glFlush();

		clear();
	}

	shutdown();
}