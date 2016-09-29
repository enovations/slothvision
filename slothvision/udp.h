#ifndef SLOTHVISION_UFP_H
#define SLOTHVISION_UFP_H

int32_t InitWinsock();

int32_t TerminateWinsock();

SOCKET connect();

void disconnect(SOCKET nogavica);


int32_t sendData(uint32_t addr, SOCKET nogavica, char *data, int dataLen);

int receive(SOCKET nogavica, char *rec_buff, int buff_len);


#endif //SLOTHVISION_UFP_H
