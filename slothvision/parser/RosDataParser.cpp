#include <iostream>
#include "RosDataParser.h"

using namespace std;

void RosDataParser::parsePosition(string str, double& roll, double& pitch, double& yaw) {
    string s(str);
    istringstream iss(s);

    string prefix;
    iss >> prefix;
    if(prefix == "position:") {
        iss >> roll;
        iss >> pitch;
        iss >> yaw;
    } else {
        cout << "Invalid input!" << endl;
    }
}

void RosDataParser::parseSonar(string str, bool& new_sonar, double& x, double& y) {
    string s(str);
    istringstream iss(s);

    string prefix;
    iss >> prefix;
    if(prefix == "sonar:" || prefix == "sonar") {
        string temp;
        iss >> temp;
        if(temp == "newsonar:"){
            new_sonar = true;
            iss >> x;
            iss >> y;
        }else {
            new_sonar = false;
            x = stof(temp);
            iss >> y;
        }
    } else {
        cout << "Invalid input!" << endl;
    }
}

void RosDataParser::parseBattery(string str, double &batteryLevel) {
    string s(str);
    istringstream iss(s);

    string prefix;
    iss >> prefix;
    if(prefix == "battery:") {
        iss >> batteryLevel;
    } else {
        cout << "Invalid input!" << endl;
    }
}
