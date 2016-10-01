#pragma once

#include <string>

namespace network_manager {
	struct IPv4{ unsigned int b1, b2, b3, b4; };
	int MSG_requestCameraVideoData(int port, struct network_manager::IPv4 ip_raspberry);
	int MSG_requestCameraStop(network_manager::IPv4 ip_raspberry);
	void sendStringUDP(std::string data, IPv4 ip, int port);
}