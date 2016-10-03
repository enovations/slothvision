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

        std::string robotMessage = "r " + std::to_string(speed) + " " + std::to_string(steer);
        network_manager::sendStringUDP(robotMessage, address, 8008);

        std::string gimbaloMessage = "g " + std::to_string(pan) + " " + std::to_string(tilt);
        network_manager::sendStringUDP(gimbaloMessage, address, 8008);
		
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}
