#include <Windows.h>
#include "render/render.h"
#include "CameraStream.h"


int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	Render render;

	CameraStream leftCameraStream(5000);
	leftCameraStream.name = "Left";
	leftCameraStream.start();

	CameraStream rightCameraStream(5001);
	rightCameraStream.name = "Right";
	rightCameraStream.start();

	render.setLeftCameraStream(&leftCameraStream); 
	render.setRightCameraStream(&rightCameraStream);
	//last thing to be called
	render.start(hinst);

	return(0);
}