#include "Sensors.h"
#include <sstream>

using OVR::OvrPlatform::GamepadButtons;

Sensors::Sensors() :
	_theThread(),
	_stop(false),
	_go(false),
	_mode(),
	_orientRoll(0),
	_orientPitch(0),
	_orientYaw(0),
	_orientLastTime()
{
}

Sensors::~Sensors()
{
	_stop = true;
	if (_theThread.joinable()) {
		_theThread.join();
	}
}

void Sensors::start()
{
	_theThread = std::thread(&Sensors::loop, this);
}

void Sensors::setOrientation(float roll, float pitch, float yaw)
{
	auto time = std::chrono::high_resolution_clock::now();
	std::lock_guard<std::mutex > lock(_orientLock);
	_orientRoll = roll;
	_orientPitch = pitch;
	_orientYaw = yaw;
	_orientLastTime = time;
}

void Sensors::getOrientation(float & roll, float & pitch, float & yaw)
{
	std::lock_guard<std::mutex > lock(_orientLock);
	roll = _orientRoll;
	pitch = _orientPitch;
	yaw = _orientYaw;
}

void Sensors::getGamepadState(OVR::OvrPlatform::GamepadState & gamepadState)
{
	std::lock_guard<std::mutex > lock(_padLock);
	gamepadState = _padState;
}

bool Sensors::isGo()
{
	std::lock_guard<std::mutex > lock(_goLock);
	return _go;
}

void Sensors::forceGo()
{
	std::lock_guard<std::mutex > lock(_goLock);
	_go = true;
}

void Sensors::clearGo()
{
	std::lock_guard<std::mutex > lock(_goLock);
	_go = false;
}

void Sensors::getMode(Mode & mode)
{
	std::lock_guard<std::mutex > lock(_modeLock);
	mode = _mode;
}

void Sensors::loop()
{
	std::stringstream ss;
	ss << "Sensors: Gamepad count=" << _padManager.GetGamepadCount();
	WriteLog(ss.str().c_str());

	while (!_stop) {
		if (_padManager.GetGamepadCount()) {
			bool go = isGo();
			OVR::OvrPlatform::GamepadState state;
			_padManager.GetGamepadState(&state);
			if (go) {
				if (state.Buttons & GamepadButtons::Gamepad_B) {
					WriteLog("Sensors: !!! STOP !!!");
					clearGo();
					{
						std::lock_guard<std::mutex > lock(_padLock);
						_padState = state;
						// Set controls to zero
						_padState.LX = 0;
						_padState.LY = 0;
						_padState.LT = 0;
						_padState.RX = 0;
						_padState.RY = 0;
						_padState.RT = 0;
					}
				} else {
					{
						std::lock_guard<std::mutex > lock(_padLock);
						_padState = state; // Make sensor readings public
					}
					// State parsing in the case of GO flag goes here ...
				}
			} else if ((state.Buttons ^ (GamepadButtons::Gamepad_L1 | GamepadButtons::Gamepad_R1 | GamepadButtons::Gamepad_A)) == 0) {
				WriteLog("Sensors: GO!");
				forceGo();
			}
			// State parsing regardless of GO flag goes here ...
			switch (state.Buttons) { // Allow only single button commands
				case GamepadButtons::Gamepad_Up: {
					std::lock_guard<std::mutex > lock(_modeLock);
					if (++_mode.control > Mode::CONTROL_NONE)
						_mode.control = 0;
					_mode.disp();
					break;
				} case GamepadButtons::Gamepad_Left: {
					std::lock_guard<std::mutex > lock(_modeLock);
					if (++_mode.robot > Mode::ROBOT_NONE)
						_mode.robot = 0;
					_mode.disp();
					break;
				} case GamepadButtons::Gamepad_Down: {
					std::lock_guard<std::mutex > lock(_modeLock);
					if (++_mode.panTilt > Mode::PANTILT_NONE)
						_mode.panTilt = 0;
					_mode.disp();
					break;
				} case GamepadButtons::Gamepad_Right: {
					std::lock_guard<std::mutex > lock(_modeLock);
					if (++_mode.marker > Mode::MARKER_NONE)
						_mode.marker = 0;
					_mode.disp();
					break;
				}
			}
			//state.Debug();

			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}
}

Sensors::Mode::Mode() :
	control(CONTROL_NONE),
	robot(ROBOT_NONE),
	panTilt(PANTILT_NONE),
	marker(MARKER_NONE)
{
}

void Sensors::Mode::disp()
{
	std::stringstream ss;
	ss << "Sensors: Mode";
	ss << " control=" << control;
	ss << " robot=" << robot;
	ss << " panTilt=" << panTilt;
	ss << " marker=" << marker;
	WriteLog(ss.str().c_str());
}
