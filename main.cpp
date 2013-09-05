#include <iostream>
#include "SDL.h"
#include "SDL_net.h"

int main(int argc, char ** argv) {

	IPaddress ip;
	const char *host;
	Uint8 *ipaddr;

	if (SDL_Init(0) == -1) {
		std::cout << "ERROR WITH SDL INIT" << std::endl;
		return -1;
	}

	if (SDLNet_Init() == -1) {
		std::cout << "ERROR WITH SDL NET INIT" << std::endl;
		return -1;
	}

	return 0;
}