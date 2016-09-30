#include "control.h"

Control::Control(uint32_t address) {
    this->address = address;
}

void Control::sendUpdate() {
    std::string robotMessage = "r " + std::to_string(speed) + " " + std::to_string(steer);
    int stumf = network::connect(23569);
    network::sendData(address, stumf, (char *) robotMessage.c_str(), static_cast<int>(robotMessage.size()), 25565);
}
