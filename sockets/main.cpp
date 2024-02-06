#include <iostream>
#include <sstream>
#include <string>

#include "socklib.h"

int main(int argc, char *argv[])
{
	SockLibInit();
	// Files : Disk :: Socket : Network Card

	// Step 1: Create a socket!
	Socket socket(Socket::Family::INET, Socket::Type::STREAM);

	// Note that this socket is not connected to any address.
	// This is like making a file stream like so:
	//     std::fstream my_file;

	// Step 2: Specify an Address!
	// std::string str_address("142.250.65.174");
	std::string str_address("68.183.63.165");
	Address address(str_address);

	// Step 3: Connect() the socket to the address
	//			port 80 -> http requests
	socket.Connect(address, 7778);

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

	SockLibShutdown();

	return 0;
}