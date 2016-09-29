//
// Created by Vid Drobnič on 9/29/16.
//

#include "pid.h"

PID::PID(double p, double i, double d, double imax, int direction) {
    this->p = p;
    this->i = i;
    this->d = d;
    this->imax = imax;
    this->direction = direction;
}

double PID::calculate(double input, double setpoint) {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();

    if (previousTime == -1) {
        previousTime = currentTime;
        return 0;
    }

    double dt = (currentTime - previousTime) / 1000.0;
    previousTime = currentTime;

    double error = setpoint - input;
    double diff = (error - previousError) / dt;
    integration += error * dt;

    previousError = error;

    if (integration > imax) {
        integration = imax;
    } else if (integration < -imax) {
        integration = -imax;
    }

    return (error * p + integration * i + diff * d) * direction;
}

void PID::reset() {
    previousError = 0;
    integration = 0;
    previousTime = -1;
}
