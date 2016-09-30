#ifndef TESTEK_ROSDATAPARSER_H
#define TESTEK_ROSDATAPARSER_H

#include <string>
#include <sstream>

using namespace std;

class RosDataParser {
public:
    void parsePosition(string str, double& roll, double& pitch, double& yaw);
    void parseSonar(string str, bool& new_sonar, double& x, double& y);
    void parseBattery(string str, double& batteryLevel);
};


#endif //TESTEK_ROSDATAPARSER_H
