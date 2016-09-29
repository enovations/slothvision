//
// Created by Vid Drobnič on 9/29/16.
//

#ifndef SLOTHVISION_PID_H
#define SLOTHVISION_PID_H

#include <chrono>

class PID {
protected:
    double p, i, d, previousError, integration, imax;
    int direction = 1;
    long previousTime = -1;
public:
    PID(double, double, double, double, int);
    double calculate(double, double);
    void reset();
};

#endif //SLOTHVISION_PID_H
