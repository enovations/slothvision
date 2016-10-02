#include <Windows.h> 
#include "render/render.h"
#include "network/CameraStream.h"
#include "network/NetworkManager.h"
#include "control/control.h"
#include <string>
#include <sstream>

/*
// Gamepad example (include)
#include "thirdparty/Win32_Gamepad.h"
//*/

network_manager::IPv4 raspicam1;
network_manager::IPv4 raspicam2;

network_manager::IPv4 raspiros;

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	/*
	// Gamepad example
	using OVR::OvrPlatform::GamepadButtons;

	OVR::OvrPlatform::Win32::GamepadManager gm;
	OVR::OvrPlatform::GamepadState gs;
	std::stringstream ss;
	ss << "Gamepad count = " << gm.GetGamepadCount() << "\n";
	OutputDebugString(ss.str().c_str());
	
	if (gm.GetGamepadCount()) {
		for (;;) {
			gm.GetGamepadState(&gs);
			std::stringstream ss;
			ss << " A=" << ((gs.Buttons & GamepadButtons::Gamepad_A) != 0);
			ss << " B=" << ((gs.Buttons & GamepadButtons::Gamepad_B) != 0);
			ss << " Left X=" << gs.LX << " Y=" << gs.LY << " T=" << gs.LT;
			ss << " Right X=" << gs.RX << " Y=" << gs.RY << " T=" << gs.RT;
			ss << "\n";
			OutputDebugString(ss.str().c_str());
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	//*/

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

	network_manager::MSG_requestCameraVideoData(5000, raspicam1);
	network_manager::MSG_requestCameraVideoData(5001, raspicam2);

	Control control (raspiros);
	
	Render render;

	CameraStream leftCameraStream(5000);
	leftCameraStream.name = "Left";
	leftCameraStream.start();

	CameraStream rightCameraStream(5001);
	rightCameraStream.name = "Right";
	rightCameraStream.start();

	render.setLeftCameraStream(&leftCameraStream);
	render.setRightCameraStream(&rightCameraStream);
	//start control data sender

	control.start();

	//last thing to be called
	render.start(hinst);

	network_manager::MSG_requestCameraStop(raspicam1);
	network_manager::MSG_requestCameraStop(raspicam2);
	//std::this_thread::sleep_for(std::chrono::milliseconds(100));

	return(0);
}