#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <stdlib.h>

#include "socklib.h"
#include "defer.h"

float ticks_to_sec(clock_t ticks) {
	return (float)ticks / CLOCKS_PER_SEC;
}

float now() {
	return ticks_to_sec(clock());
}

std::string to_display = "";

class NetworkModule {
public:
	NetworkModule():
		sock(),
		message_buffer{ 0 }
	{
		SockLibInit();
	}

	~NetworkModule() {
		SockLibShutdown();
	}

	void Init() {
		SockLibInit();
		sock.Create(Socket::Family::INET, Socket::Type::STREAM);
		sock.Connect(Address("68.183.63.165", 7778));
		sock.SetNonBlockingMode(true);
	}

	void Update(float dt, int frame_num) {
		// Don't need to update this module _every_ frame...
		// Every other frame is plenty.
		if (frame_num % 2 == 0) return;

		// Do we have to send out? Then send it.
		if (rand() % 4 != 0) {
			std::stringstream ss;
			ss << "LIST";
			for (int i = 0; i < 5; i++) {
				ss << " " << rand() % 500;
			}

			std::string to_send = ss.str();
			sock.Send(to_send.c_str(), to_send.size());
		}
		
		// Did anyone send anything back to us?
		int nbytes_recvd = sock.Recv(message_buffer, sizeof(message_buffer));
		if (nbytes_recvd == -1) {
			if (sock.GetLastError() == Socket::SOCKLIB_EWOULDBLOCK) {
				to_display = "No message this frame.\n";
			}
			else {
				std::cerr << "Unexpected error!\n";
				abort();
			}
		}
		else if (nbytes_recvd == 0) {
			std::cerr << "Connection unexpectedly closed!\n";
			abort();
		}
		else {
			to_display = std::string(message_buffer, nbytes_recvd);
		}
	}

private:
	Socket sock;
	char message_buffer[4096];
};

int main(int argc, char *argv[]) {
	// Initialize the system
	float last_frame = now();
	bool quit = false;
	int frame_num = 0;
	// One second per frame -- wow, that's slow!
	const float targetDt = 1;

	// Initialize the player "object"
	float character_pos = 0;
	float xVelocity = 1;

	// Initialize the socket system
	NetworkModule netsys;
	netsys.Init();

	while (!quit) {
		float time = now();
		float dt = time - last_frame;
		if (dt < targetDt)
			continue;
		frame_num++;

		// Process input -- is the keyboard pressed,
		// is the mouse down, etc.?
		netsys.Update(dt, frame_num);
		
		// Update all the game objects
		character_pos += xVelocity * dt;

		// Render
		// Clear the back buffer
		system("cls");

		for (int i = 0; i < character_pos; i++) {
			std::cout << " ";
		}
		std::cout << "@";
		std::cout << std::endl << std::endl;
		std::cout << to_display;

		to_display = "";
		last_frame = time;
	}


	return 0;
}
