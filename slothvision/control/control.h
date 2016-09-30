#ifndef SLOTHVISION_CONTROL_H
#define SLOTHVISION_CONTROL_H


#include <cstdint>
#include <string>
#include <cstring>
#include "../network/UDPSocket.h"
#include "../network/NetworkManager.h"

class Control {
private:
    network_manager::IPv4 address;
protected:
    double speed, steer, pan, tilt;
public:
    Control(network_manager::IPv4 address);
    void sendUpdate();
};


#endif //SLOTHVISION_CONTROL_H
