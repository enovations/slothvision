#ifndef SLOTHVISION_UFP_H
#define SLOTHVISION_UFP_H

#include <stdint.h>
#include "windows.h"
#include "conio.h"
#include <iphlpapi.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int32_t InitWinsock();

int32_t TerminateWinsock();

int connect(int local_port);

void disconnect(int nogavica);


int32_t sendData(uint32_t addr, int nogavica, char *data, int dataLen, int port);

int receive(int nogavica, char *rec_buff, int buff_len);


#endif //SLOTHVISION_UFP_H
