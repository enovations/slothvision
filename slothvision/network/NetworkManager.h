#pragma once

#include <string>

namespace network_manager {
	struct IPv4{};
	int MSG_requestCameraVideoData(int port);
	void sendStringUDP(std::string data, IPv4 ip, int port);
}