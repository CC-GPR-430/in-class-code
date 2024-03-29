#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <stdlib.h>

#include "socklib.h"
#include "defer.h"

void print_as_bytes(char* object, size_t bytes) {
	for (int i = 0; i < bytes; i++) {
		if (i % 16 == 0) printf("\n");
		printf(" 0x%x", (unsigned char)object[i]);
	}
	printf("\n");
}

class GameObject {
public:
	// Note that it's not easy to determine
	// how a specific memory layout within
	// a class will map to binary.

	// Importantly, especially due to alignment,
	// the same class could look different in
	// binary on two different systems.
	int x;
	int y;
	int z;
	int xVel;
	int yVel;
	int zVel;

	// Virtual functions are (basically) function pointers,
	// so they affect the size of an object and make
	// doing a simple memcpy() impossible.
	virtual void Update(float dt) {
		x++;
		y++;
	}

	int GetX() {
		return x;
	}

	void* sprite;
};

// Member functions are essentially just this:
int GameObject_GetX(GameObject* ths);

struct GameObject_Struct {
	int x;
	int y;
	void* sprite;
	void (*Update)(GameObject_Struct*, float);
};

// One option -- serialize to a string.
size_t SerializeGameObjectAsString(const GameObject* go, char* buffer, size_t buffer_size)
{
	std::stringstream go_ss;

	// Need spaces (or other delimiters) between each value
	// Because otherwise "100" could mean either 100 or 1, 0, 0.
	// So spaces tell us when one number has ended.
	go_ss << go->x << " "
		<< go->y << " "
		<< go->z << " "
		<< go->xVel << " "
		<< go->yVel << " "
		<< go->zVel;
	std::string go_str = go_ss.str();

	size_t bytes_copied = 0;
	// Could replace the for loop below with strncpy(buffer, go_str.c_str(), buffer_size);
	for (int i = 0; i < go_str.size() && i < buffer_size; i++) {
		buffer[i] = go_str[i];
		bytes_copied++;
	}

	return bytes_copied;
}

template <typename T>
size_t copy_to_buffer(char* buffer, T* object, size_t buffer_size)
{
	if (sizeof(T) > buffer_size) return 0;
	char* object_as_bytes = (char*)object;
	for (int i = 0; i < sizeof(T); i++) {
		buffer[i] = object_as_bytes[i];
	}
	return sizeof(T);
}

// Second option: Serialize as binary
size_t SerializeGameObjectAsBytes(const GameObject* go, char* buffer, size_t buffer_size)
{
	size_t bytes_written = 0;
	bytes_written += copy_to_buffer(&buffer[bytes_written], &go->x, buffer_size - bytes_written);
	bytes_written += copy_to_buffer(&buffer[bytes_written], &go->y, buffer_size - bytes_written);
	bytes_written += copy_to_buffer(&buffer[bytes_written], &go->z, buffer_size - bytes_written);
	bytes_written += copy_to_buffer(&buffer[bytes_written], &go->xVel, buffer_size - bytes_written);
	bytes_written += copy_to_buffer(&buffer[bytes_written], &go->yVel, buffer_size - bytes_written);
	bytes_written += copy_to_buffer(&buffer[bytes_written], &go->zVel, buffer_size - bytes_written);
	return bytes_written;
}

void byte_demo()
{
	GameObject go;
	go.x = 100005;
	go.y = 100006;
	go.z = 100007;
	go.xVel = 100100;
	go.yVel = 100101;
	go.zVel = 100102;
	go.sprite = "Hello, world!";
	std::cout << "==== Game Object As Bytes ====\n";
	print_as_bytes((char*)&go, sizeof(go));

	{
		char buffer[4096];
		size_t bytes_written = SerializeGameObjectAsString(&go, buffer, sizeof(buffer));
		// Could then do sock.Send(str_buffer, bytes_written);
		std::string go_str(buffer, bytes_written);
		std::cout << "\n==== Game Object Serialized as String ====\n";
		std::cout << "==== Size: " << bytes_written << " bytes   ====\n";
		std::cout << "\"" << go_str << "\"" << std::endl;
	}

	{
		char buffer[4096];
		size_t bytes_written = SerializeGameObjectAsBytes(&go, buffer, sizeof(buffer));
		// Could then do sock.Send(str_buffer, bytes_written);
		std::cout << "\n==== Game Object Serialized as Bytes ====\n";
		std::cout << "==== Size: " << bytes_written << " bytes   ====\n";
		print_as_bytes(buffer, bytes_written);

	}
}

float clocks_to_secs(clock_t clocks) {
	return (float)clocks / CLOCKS_PER_SEC;
}

float time_now() {
	return clocks_to_secs(clock());
}

int main(int argc, char *argv[]) {
	byte_demo();
	return 0;

	// Game loop structure
	// ===================
	// 
	//	While the game is running:
	//		If enough time has elapsed:
	//			Poll inputs
	//			Update all game objects
	//			Render game to the screen

	// Initialize system variables
	bool quit = false;
	float last_frame_time = time_now();
	float targetDt = 1;
	std::string msg_to_display;

	// Initialize starting game objects
	float character_pos = 0;
	float xVelocity = 1;

	// Initialize networking module
	SockLibInit();
	defer _([]() {SockLibShutdown();});
	Socket sock(Socket::Family::INET, Socket::Type::STREAM);
	sock.Connect(Address("68.183.63.165", 7778));
	sock.SetNonBlockingMode(true);
	char message_buffer[4096];

	std::vector<GameObject*> objects;

	// Run game loop
	while (!quit) {
		float now = time_now();
		float dt = now - last_frame_time;
		if (dt < targetDt)
			continue;

		// Poll inputs
		if (rand() % 2 == 0) {
			char buffer[64];
			size_t strsize = 0;
			strsize += snprintf(buffer, sizeof(buffer), "LIST");
			for (int i = 0; i < 5; i++) {
				strsize += snprintf(buffer + strsize,
					sizeof(buffer) - strsize,
					" %i", rand() % 500);
			}

			/*
			std::stringstream ss;
			ss << "LIST";
			for (int i = 0; i < 5; i++) {
				ss << " " << rand() % 500;
			}
			*/
			sock.Send(buffer, strsize);
		}

		for (const GameObject* go : objects) {
			// Send the state of our game objects
			// Would like to do something simple like this:
			//sock.Send((char*)go, sizeof(GameObject));
			// But too many things can go wrong!
			// - Might have a different memory layout on receiving end
			// - If our class has a pointer, this will break
			// - If our class has a virtual function, ditto
		}

		int nbytes_recvd = sock.Recv(message_buffer, sizeof(message_buffer));
		if (nbytes_recvd == -1) {
			if (sock.GetLastError() == Socket::SOCKLIB_EWOULDBLOCK) {
				msg_to_display = "No message this frame.\n";
			}
			else {
				perror("recv()");
				abort();
			}
		}
		else if (nbytes_recvd == 0) {
			std::cerr << "Connection hung up unexpectedly!\n";
			abort();
		}
		else {
			// The server sent us a list of objects -- copy them on our end
			for (GameObject* go : objects) {
				// This won't work:
				//memcpy(go, message_buffer, sizeof(GameObject));
				// But too many things can go wrong!
				// - Might have a different memory layout on receiving end
				// - If our class has a pointer, this will break
				// - If our class has a virtual function, ditto
			}
			msg_to_display = std::string(message_buffer, nbytes_recvd);
		}

		// Update all game objects
		character_pos += xVelocity * dt;

		// Render game to the screen
		// Clear back buffer
		system("cls");
		// Draw to back buffer
		for (int i = 0; i < character_pos; i++) {
			std::cout << " ";
		}
		std::cout << "@";
		std::cout << "\n\n" << msg_to_display;

		last_frame_time = now;
	}

	return 0;
}
