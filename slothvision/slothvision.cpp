#include <Windows.h>
#include "render/render.h"

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{

	Render render;
	


	//last thing to be called
	render.start(hinst);

	return(0);
}

//char udp_buffer[512] = { 0 };
//
//int nogavica = connect(25564);
//uint32_t add = 0;
//uint8_t *tmp = (uint8_t*)&add;
//tmp[0] = 127;
//tmp[1] = 0;
//tmp[2] = 0;
//tmp[3] = 1;
//sendData(add, nogavica, "neki", 4, 25565);
//
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