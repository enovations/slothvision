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
    std::thread thread;
    void sendUpdate();
    double speed, steer, pan, tilt;
    bool running;
public:
    Control(network_manager::IPv4 address);
    ~Control();
    void start();
    void stop();
};


#endif //SLOTHVISION_CONTROL_H
