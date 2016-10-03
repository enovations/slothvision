#include <Windows.h> 
#include "render/render.h"
#include "network/CameraStream.h"
#include "network/NetworkManager.h"
#include "control/control.h"
#include "sensors/Sensors.h"
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
	raspiros.b4 = 100;

	Sensors sensors;
	sensors.start();

	while (true)
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	network_manager::MSG_requestCameraVideoData(5000, raspicam1);
	network_manager::MSG_requestCameraVideoData(5001, raspicam2);

	Control control(raspiros, &sensors);
	
	Render render;

	CameraStream leftCameraStream(5000);
	leftCameraStream.name = "Left";
	leftCameraStream.start();

	CameraStream rightCameraStream(5001);
	rightCameraStream.name = "Right";
	rightCameraStream.start();

	render.leftCameraStream = &leftCameraStream;
	render.rightCameraStream = &rightCameraStream;
	render.sensors = &sensors;
	//start control data sender

	control.start();

	//last thing to be called
	render.start(hinst);

	network_manager::MSG_requestCameraStop(raspicam1);
	network_manager::MSG_requestCameraStop(raspicam2);
	//std::this_thread::sleep_for(std::chrono::milliseconds(100));

	return(0);
}