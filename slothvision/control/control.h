#ifndef SLOTHVISION_CONTROL_H
#define SLOTHVISION_CONTROL_H


#include <cstdint>
#include <string>
#include "../network/UDPSocket.h"

class Control {
private:
    uint32_t address = 0;
protected:
    double speed, steer, pan, tilt;
public:
    Control(uint32_t address);
    void sendUpdate();
};


#endif //SLOTHVISION_CONTROL_H
