#include <stdint.h>
#include "windows.h"
#include "conio.h"
#include <iphlpapi.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


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


SOCKET connect() {
    int result;
    int t = 100; // 100 ms timeout
    if (InitWinsock() != 0) {
        return 0;
    }


    SOCKET nogavica = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (nogavica == -1) {
        return 0;
    }

    // Set socket options
    setsockopt(nogavica, SOL_SOCKET, SO_RCVTIMEO, (char *) &t, sizeof(t));
    setsockopt(nogavica, SOL_SOCKET, SO_SNDTIMEO, (char *) &t, sizeof(t));

    // Setup socket to be unblocking
    u_long iMode = TRUE;
    ioctlsocket(nogavica, FIONBIO, &iMode);
    return nogavica;
}

void disconnect(SOCKET nogavica) {
    if (nogavica)
        closesocket(nogavica);
}


int32_t sendData(uint32_t addr, SOCKET nogavica, char *data, int dataLen) {
    struct sockaddr_in dest_ip;
    dest_ip.sin_family = AF_INET;
    dest_ip.sin_family.sin_port = htons(20055);
    dest_ip.sin_family.sin_addr.s_addr = addr;

    uint32_t retries1 = 0;
    uint32_t retries2 = 0;
    int result;

    if (sendto(nogavica, data, dataLen, 0, &dest_ip, sizeof(struct sockaddr_in)) == -1) {
        return -1;
    }
    return 0;
}

int receive(SOCKET nogavica, char *rec_buff, int buff_len) {
    fd_set fds;
    struct timeval stimeout;
    FD_ZERO(&fds);
    FD_SET(nogavica, &fds);

    stimeout.tv_sec = 0;
    stimeout.tv_usec = 1000 * 5;

    result = select(nogavica + 1, &fds, NULL, NULL, &stimeout);

    if (result == 0 || result == -1) {
        return 0;
    }

    result = recv(nogavica, rec_buff, buff_len, 0);
    return result;

}

