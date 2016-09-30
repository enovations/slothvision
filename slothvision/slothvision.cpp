#include <Windows.h>
#include "render/render.h"
#include "network/CameraStream.h"
#include "network/NetworkManager.h"
#include "control/control.h"
#include <string>
#include <sstream>

network_manager::IPv4 raspicam1;
network_manager::IPv4 raspicam2;

network_manager::IPv4 raspiros;

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{

	raspicam1.b1 = 192;
	raspicam1.b2 = 168;
	raspicam1.b3 = 0;
	raspicam1.b4 = 50;

	raspicam2.b1 = 192;
	raspicam2.b2 = 168;
	raspicam2.b3 = 0;
	raspicam2.b4 = 51;

	raspiros.b1 = 192;
	raspiros.b2 = 168;
	raspiros.b3 = 0;
	raspiros.b4 = 105;

	network_manager::MSG_requestCameraVideoData(5000, raspicam1);
	network_manager::MSG_requestCameraVideoData(5001, raspicam2);

	Control control (raspiros);

	Render render;
	
	CameraStream leftCameraStream(5000);
	leftCameraStream.start();

	CameraStream rightCameraStream(5001);
	rightCameraStream.start();

	render.setLeftCameraStream(&leftCameraStream); 
	render.setRightCameraStream(&rightCameraStream);

	//start control data sender
	control.start();

	//last thing to be called
	render.start(hinst);

	return(0);
}