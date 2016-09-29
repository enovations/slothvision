#include "UDPSocket.h"

WSADATA wsaData;

int32_t InitWinsock() {
    // Initialize Winsock - version 2.2
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return -1;
    }
    return 0;
}

int32_t TerminateWinsock() {
    WSACleanup();
    return 0;
}


int connect(int local_port) {
	int t = 100; // 100 ms timeout
    if (InitWinsock() != 0) {
        return 0;
    }


	int nogavica = (int)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (nogavica == -1) {
        return 0;
    }

    // Set socket options
    setsockopt(nogavica, SOL_SOCKET, SO_RCVTIMEO, (char *) &t, sizeof(t));
    setsockopt(nogavica, SOL_SOCKET, SO_SNDTIMEO, (char *) &t, sizeof(t));

    // Setup socket to be unblocking
    u_long iMode = TRUE;
    ioctlsocket(nogavica, FIONBIO, &iMode);


	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(local_port);
	addr.sin_addr.s_addr = INADDR_ANY;

	// first bind succeeds
	if (bind(nogavica, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		disconnect(nogavica);
		return 0;
	}



    return nogavica;
}

void disconnect(int nogavica) {
    if (nogavica)
        closesocket(nogavica);
}


int32_t sendData(uint32_t addr, int nogavica, char *data, int dataLen, int port) {
    struct sockaddr_in dest_ip;
    dest_ip.sin_family = AF_INET;
    dest_ip.sin_port = htons(port);
    dest_ip.sin_addr.s_addr = addr;

    if (sendto(nogavica, data, dataLen, 0, (const sockaddr*)&dest_ip, sizeof(struct sockaddr_in)) == -1) {
        return -1;
    }
    return 0;
}

int receive(int nogavica, char *rec_buff, int buff_len) {
    fd_set fds;
    struct timeval stimeout;
    FD_ZERO(&fds);
    FD_SET(nogavica, &fds);

	int result;

    stimeout.tv_sec = 0;
    stimeout.tv_usec = 1000 * 5 * 0;

    result = select(nogavica + 1, &fds, NULL, NULL, &stimeout);

    if (result == 0 || result == -1) {
        return (int)0;
    }

    result = recv(nogavica, rec_buff, buff_len, 0);
    return result;

}

