#pragma once

#include <thread>
#include <mutex>
#include <chrono>

#include "../thirdparty/Win32_Gamepad.h"

class Sensors
{
public:
	struct Mode
	{
		static const int CONTROL_MANUAL = 0;
		static const int CONTROL_AUTO = 1;
		static const int CONTROL_NONE = 2;

		static const int ROBOT_MANUAL = 0;
		static const int ROBOT_TRACKING = 1;
		static const int ROBOT_NONE = 2;

		static const int PANTILT_MANUAL = 0;
		static const int PANTILT_TRACKING = 1;
		static const int PANTILT_NONE = 2;

		static const int MARKER_TRACKING = 0;
		static const int MARKER_NONE = 1;

		int control;
		int robot;
		int panTilt;
		int marker;

		Mode();
		void disp();
	};

protected:
	std::thread _theThread;
	bool _stop;

	std::mutex _padLock;
	OVR::OvrPlatform::Win32::GamepadManager _padManager;
	OVR::OvrPlatform::GamepadState _padState;
	
	std::mutex _goLock;
	bool _go;

	std::mutex _modeLock;
	Mode _mode;

	std::mutex _orientLock;
	float _orientRoll;
	float _orientPitch;
	float _orientYaw;
	std::chrono::time_point<std::chrono::high_resolution_clock > _orientLastTime;

public:
	Sensors();
	~Sensors();

	void start();

	void setOrientation(float roll, float pitch, float yaw);
	void getOrientation(float &roll, float &pitch, float &yaw);

	void getGamepadState(OVR::OvrPlatform::GamepadState &gamepadState);

	bool isGo();
	void forceGo();
	void clearGo();

	void getMode(Mode &mode);

protected:
	void loop();
};