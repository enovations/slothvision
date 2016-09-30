#include "NetworkManager.h"
#include <Windows.h>
#include "UDPSocket.h"

void network_manager::sendStringUDP(std::string data, network_manager::IPv4 ip, int port) {
	char udp_buffer[512] = { 0 };
	int nogavica = network::connect(port);
	uint32_t add = 0;
	uint8_t *tmp = (uint8_t*)&add;
	tmp[0] = ip.b1;
	tmp[1] = ip.b2;
	tmp[2] = ip.b3;
	tmp[3] = ip.b4;
	network::sendData(add, nogavica, (char*)data.c_str(), data.size(), port);
	network::disconnect(nogavica);
}

bool getIPAddress(network_manager::IPv4 & myIP)
{
	char szBuffer[1024];

#ifdef WIN32
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 0);
	if (::WSAStartup(wVersionRequested, &wsaData) != 0)
		return false;
#endif


	if (gethostname(szBuffer, sizeof(szBuffer)) == SOCKET_ERROR)
	{
#ifdef WIN32
		WSACleanup();
#endif
		return false;
	}

	struct hostent *host = gethostbyname(szBuffer);
	if (host == NULL)
	{
#ifdef WIN32
		WSACleanup();
#endif
		return false;
	}

	//Obtain the computer's IP
	myIP.b1 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b1;
	myIP.b2 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b2;
	myIP.b3 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b3;
	myIP.b4 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b4;

#ifdef WIN32
	WSACleanup();
#endif
	return true;
}

int network_manager::MSG_requestCameraVideoData(int port, struct network_manager::IPv4 ip_raspberry) {

	//my address to which to send
	network_manager::IPv4 address;

	if (getIPAddress(address)) {

		//request ip
		std::string toSend = "ip: " + std::to_string(address.b1) + "." + std::to_string(address.b2) + "." + std::to_string(address.b3) + "." + std::to_string(address.b4);
		network_manager::sendStringUDP(toSend, ip_raspberry, 8008);

		//request port
		toSend = "port: " + port;
		network_manager::sendStringUDP(toSend, ip_raspberry, 8008);

		return 0;
	}
	else {
		return -1;
	}

}

//while (1)
//{
//	int newData = receive(nogavica, udp_buffer, 512);
//
//	if (newData)
//	{
//		CString tmp(udp_buffer);
//
//		if (tmp == "Exit")
//		{
//			exit(0);
//		}
//		/*
//		// Data received here...
//		if (strcmp(udp_buffer, "Exit") == 0)
//		{
//		exit(0);
//		}
//		*/
//		// Clear the buffer
//		memset(udp_buffer, 0, sizeof(udp_buffer));
//	}
//
//
//
//
//
//
//}
//disconnect(nogavica);
//TerminateWinsock();