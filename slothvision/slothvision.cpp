#include <Windows.h>
#include "render/render.h"

int WINAPI
WinMain(HINSTANCE
hinst, HINSTANCE, LPSTR, int)
{

/*Render render;



//last thing to be called
render.start(hinst);*/

int nogavica = connect(8008);
uint32_t add = 0;
uint8_t *tmp = (uint8_t * ) & add;
tmp[0] = 192;
tmp[1] = 168;
tmp[2] = 0;
tmp[3] = 100;
sendData(add, nogavica,
"neki", 4);
disconnect(nogavica);

TerminateWinsock();

return(0);
}