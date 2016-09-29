#include <Windows.h>
#include "render/render.h"

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{

	Render render;
	


	//last thing to be called
	render.start(hinst);

	return(0);
}