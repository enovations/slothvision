#include "control.h"


Control::Control(network_manager::IPv4 address, Sensors* _sensors) :
	address(address),
	thread(),
	running(false),
	sensors(_sensors)
{
}

Control::~Control() {
    running = false;
    if (thread.joinable()) {
        thread.join();
    }
}

void Control::start() {
    running = true;
    thread = std::thread(&Control::sendUpdate, this);
}

void Control::stop() {
    running = false; //TODO Join should be called also here ...
}

void Control::sendUpdate() {
    while (running) {
		bool go = sensors->isGo();
		if (go) {
			Sensors::Mode mode;
			sensors->getMode(mode);
			OVR::OvrPlatform::GamepadState state;
			sensors->getGamepadState(state);
			
			if (mode.robot == Sensors::Mode::ROBOT_MANUAL) {
				speed = state.RY*0.1;
				steer = state.RX*0.5;
			}
			
			if (mode.panTilt == Sensors::Mode::PANTILT_MANUAL) {
				pan = state.LX*7000;
				tilt = state.LY*2000;
			}
		} else {
			speed = 0;
			steer = 0;
			pan = 0;
			tilt = 0;
		}

        std::string robotMessage = "r " + std::to_string(speed) + " " + std::to_string(steer);
        network_manager::sendStringUDP(robotMessage, address, 8008);

        std::string gimbaloMessage = "g " + std::to_string(pan) + " " + std::to_string(tilt);
        network_manager::sendStringUDP(gimbaloMessage, address, 8008);
		
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}
