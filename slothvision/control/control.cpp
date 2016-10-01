#include "control.h"


Control::Control(network_manager::IPv4 address): address(address), thread(), running(false) {}

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
    running = false;
}

void Control::sendUpdate() {
    while (running) {

        std::string robotMessage = "r " + std::to_string(speed) + " " + std::to_string(steer);
        network_manager::sendStringUDP(robotMessage, address, 8008);

        std::string gimbaloMessage = "g " + std::to_string(pan) + " " + std::to_string(tilt);
        network_manager::sendStringUDP(gimbaloMessage, address, 8008);
		//TODO Sleep ...
    }
}
