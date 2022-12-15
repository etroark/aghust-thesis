/**
 * The remeshing algorithm for 3D models
 * author: Arkadiusz Trojanowski
 * AGH UST Faculty of Physics and Applied Computer Science
 * 2023
 */

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") //hide the console

#include "../include/gui.h"

int main()
{
	Gui gui
	(
		"the remeshing algorithm for 3D models",	//winow title
		1280, 720,	//resolution
		4, 5,	//OpenGL major/minor version
		false	//resizable window
	);

	//main loop
	gui.loop();

	return 0;
}