#ifndef SLOTHVISION_CONTROL_H
#define SLOTHVISION_CONTROL_H


#include <cstdint>
#include <string>
#include <cstring>
#include <thread>
#include "../network/UDPSocket.h"
#include "../network/NetworkManager.h"

class Control {
protected:
    network_manager::IPv4 address;
    std::thread updatesThread;
    void sendUpdate();
    double speed, steer, pan, tilt;
    bool running;
public:
    Control(network_manager::IPv4 address);
    void start();
    void stop();
};


#endif //SLOTHVISION_CONTROL_H
