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

float ticks_to_sec(clock_t ticks) {
	return (float)ticks / CLOCKS_PER_SEC;
}

float now() {
	return ticks_to_sec(clock());
}

std::string to_display = "";

class ToInspect {
public:
	int x;
	int y;
	char* name;

	void Update() {}
};

/* vtable aside
struct GameObject_Vtable {
	void (*Update)(GameObject_Struct* ths, float dt);
	void (*Render)(GameObject_Struct* ths);
};

GameObject_Vtable go_vtable = {
	go_update,
	go_render
};

GameObject_Vtable player_vtable = {
	player_update,
	player_render
};

struct GameObject_Struct {
	int x, y, z;
	int xVel, yVel, zVel;
	void* sprite;
	GameObject_Vtable* vtable;
};
*/

// GameObject* go = new GameObject();
// go->Update(dt);

// Same as:
// GameObject_Struct* go = create_go_struct();
//    go->vtable = go_vtable;
// go->vtable->Update(&go, dt);


class GameObject {
public:
	int x, y, z;
	int xVel, yVel, zVel;
	void* sprite;

	virtual void Update(float dt) {
		x += xVel * dt;
		y += yVel * dt;
		z += zVel * dt;
	}

	void Render() {
		// Renders the sprite at position (x, y, z)
	}
};

size_t SerializeGameObjectAsString(const GameObject* go, char* buffer, size_t buffer_len)
{
	// One option: Write the game object as text
	std::stringstream go_strstream;
	go_strstream << go->x << " "
		<< go->y << " "
		<< go->z << " "
		<< go->xVel << " "
		<< go->yVel << " "
		<< go->zVel;
	std::string go_str = go_strstream.str();
	size_t characters_written = 0;
	for (int i = 0; i < go_str.size() && i < buffer_len; i++)
	{
		characters_written++;
		buffer[i] = go_str[i];
	}
	// Done! Game object now lives in buffer as a string.

	return characters_written;
}

template<typename T>
size_t copy_to_buffer(char* buffer, T* value, size_t buffer_len) {
	if (sizeof(T) > buffer_len) return 0;
	char* as_bytes = (char*)value;
	for (int i = 0; i < sizeof(T); i++) {
		buffer[i] = as_bytes[i];
	}
	return sizeof(T);
}

template<typename T>
size_t read_from_buffer(char* buffer, T* out_value) {
	memcpy(out_value, buffer, sizeof(T));
	return sizeof(T);
}

size_t SerializeGameObjectAsBytes(const GameObject* go, char* buffer, size_t buffer_len)
{
	// Another option: Write as binary
	size_t write_head = 0;
	write_head += copy_to_buffer(buffer, &go->x, buffer_len);
	write_head += copy_to_buffer(&buffer[write_head], &go->y, buffer_len - write_head);
	write_head += copy_to_buffer(&buffer[write_head], &go->z, buffer_len - write_head);
	write_head += copy_to_buffer(&buffer[write_head], &go->xVel, buffer_len - write_head);
	write_head += copy_to_buffer(&buffer[write_head], &go->yVel, buffer_len - write_head);
	write_head += copy_to_buffer(&buffer[write_head], &go->zVel, buffer_len - write_head);
	return write_head;
}

size_t DeserializeGameObjectFromBytes(GameObject* go, char* buffer, size_t buffer_len)
{
	size_t read_head = 0;
	read_head += read_from_buffer(buffer, &go->x);
	read_head += read_from_buffer(&buffer[read_head], &go->y);
	read_head += read_from_buffer(&buffer[read_head], &go->z);
	read_head += read_from_buffer(&buffer[read_head], &go->xVel);
	read_head += read_from_buffer(&buffer[read_head], &go->yVel);
	read_head += read_from_buffer(&buffer[read_head], &go->zVel);
	return read_head;
}

class Player : public GameObject {
	void Update(float dt) override {
		// If a button is down, move
	}
};

std::vector<GameObject*> objects;

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

		for (const GameObject* go : objects) {
			sock.Send((char*)go, sizeof(GameObject));
		}

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

int run_server() {
	// Simple demo to demonstrate serialization
	// over TCP
	// Create a socket, wait for folks to connect
	Socket listen_sock(Socket::Family::INET, Socket::Type::STREAM);
	listen_sock.Bind(Address("0.0.0.0", 36925));
	listen_sock.Listen();

	while (true) {
		Socket conn_sock = listen_sock.Accept();
		bool connection_alive = true;
		std::vector<GameObject*> game_objects;
		while (connection_alive) {
			char buffer[4096];

			int nbytes_recvd = conn_sock.Recv(buffer, sizeof(buffer));
			if (nbytes_recvd == -1) {
				if (conn_sock.GetLastError() == Socket::SOCKLIB_ETIMEDOUT) {
					// No data was available to receive.
				}
				else {
					perror("recv()");
					exit(1);
				}
			}
			// Expect data in a specific format --
			//     First, the number of game objects
			int num_gameobjects = 0;
			read_from_buffer(buffer, &num_gameobjects);
			std::cout << "Reading " << num_gameobjects << " objects.\n";
			game_objects.clear();
			game_objects.reserve(num_gameobjects);
			size_t buffer_offset = 0;

			for (int i = 0; i < num_gameobjects; i++) {
				GameObject* go = new GameObject;
				buffer_offset += DeserializeGameObjectFromBytes(go,
					&buffer[buffer_offset], sizeof(buffer) - buffer_offset);
				game_objects.push_back(go);
			}
		}
	}

	// Keep a list of game objects


	// When we receive a list of game objects, replicate it
	//    on our end
}

int main(int argc, char *argv[]) {
	SockLibInit();
	atexit(SockLibShutdown);

	if (argc > 1) {
		return run_server();
	}

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

	GameObject go;
	go.x = 100001;
	go.y = 100002;
	go.z = 100003;
	go.xVel = 100010;
	go.yVel = 100011;
	go.zVel = 100012;

	go.sprite = "Hello, there!";
	std::cout << "===== Game Object as Bytes =====\n";
	print_as_bytes((char*)&go, sizeof(go));

	{
		char buffer[4096];
		size_t amt_written = SerializeGameObjectAsString(&go, buffer, sizeof(buffer));
		std::string go_str(buffer, amt_written);
		std::cout << "===== Game Object as String =====\n";
		std::cout << "===== Size: " << amt_written << "   ====\n";
		std::cout << "\"" << go_str << "\"" << std::endl;
	}

	{
		char buffer[4096] = { 0 };
		size_t amt_written = SerializeGameObjectAsBytes(&go, buffer, sizeof(buffer));
		std::cout << "==== Serialized game object as Bytes =====\n";
		std::cout << "===== Size: " << amt_written << "   ====\n";
		print_as_bytes(buffer, amt_written);
	}

	return 0;

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
