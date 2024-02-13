#include <iostream>
#include <sstream>
#include <string>

#include "socklib.h"

void do_client()
{
	// Files : Disk :: Socket : Network Card

	// Step 1: Create a socket!
	// INET => IPv4 Network Protocol
	// STREAM => TCP Transport Protocol
	Socket socket(Socket::Family::INET, Socket::Type::STREAM);

	// Note that this socket is not connected to any address.
	// This is like making a file stream like so:
	//     std::fstream my_file;

	// Step 2: Specify an Address!
	// std::string str_address("142.250.65.174");
	std::string str_address("127.0.0.1");
	Address address(str_address);

	// Step 3: Connect() the socket to the address
	//			port 80 -> http requests
	socket.Connect(address, 15000);

	std::cout << "Connected to " << str_address << "!\n";

	// Now you can send() and recv() according to your protocol!

	// Send() has same interface as file.write()
	std::string msg_to_send("Hello, google!");
	size_t nbytes_sent = socket.Send(msg_to_send.data(), msg_to_send.size());
	std::cout << "Sent " << nbytes_sent << " bytes.\n";

	// Recv() has same interface as file.read()
	// Note that Recv() will _block_, halting your program
	// until data comes in (similar to std::cin).
	char buffer[4096];
	size_t nbytes_recvd = socket.Recv(buffer, sizeof(buffer));

	// Note that, like with read(), buffer is neither length-prefixed
	// nor null-terminated -- it's NOT a string, it's just a chunk
	// of bytes.

	// We can make a string if we want, though:
	std::string msg_recvd(buffer, nbytes_recvd);

	std::cout << "Received message '" << msg_recvd << "'!\n";
}

void do_server()
{
	// Create a socket!
	Socket listen_sock(Socket::Family::INET, Socket::Type::STREAM);

	// Make an address we want to listen on. Only two options:
	// 0.0.0.0 => Accept connections from anywhere
	// 127.0.0.1 => Accept connections only from this machine
	Address srv_address("0.0.0.0");

	// Bind to a port so that data sent to that port
	// comes to this process
	listen_sock.Bind(srv_address, 15000);

	// Open this port up to incoming connections, so
	// Connect() will work
	listen_sock.Listen();

	std::cout << "Waiting for connections...\n";
	// Accept() will fail if you forget to call Listen()
	// Accept() works like Recv() in that it waits for
	// information to come in from another device.
	// It's also unusual in that it returns a new socket,
	// which represents the new connection.
	Socket conn_sock = listen_sock.Accept();

	std::cout << "Connection received!\n";

	// Now we can Send/Recv freely on conn_sock, and can accept
	// new connections using listen_sock if we want!

	char buffer[4096];
	size_t nbytes_received = conn_sock.Recv(buffer, sizeof(buffer));
	std::string cli_msg(buffer, nbytes_received);
	std::cout << "Received message '" << cli_msg << "' from client!\n";

	std::string srv_msg("Nice message");
	conn_sock.Send(srv_msg.data(), srv_msg.size());
}

int main(int argc, char *argv[])
{
	SockLibInit();
	
	if (argc == 1) do_client();
	else do_server();

	SockLibShutdown();

	return 0;
}