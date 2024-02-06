#include <iostream>
#include <sstream>
#include <string>

#include "socklib.h"

int main(int argc, char *argv[])
{
	// Set up socket library
	SockLibInit();

	// Sockets : Network Card :: Files : Hardrive

	Socket socket(Socket::Family::INET, Socket::Type::STREAM);

	// Created a socket, but it doesn't talk to anything yet.

	// Created an address (this is google.com's IP address).
	std::string str_address("68.183.63.165"); // <- Use IP address, not MAC address
	Address address(str_address);

	// Connect the socket to the given address:
	// Must also provide a port -- port 80 = web traffic
	socket.Connect(address, 7778);

	std::cout << "Connected to " << str_address << "!\n";

	// Send data (this is the same interface as file.write())
	std::string msg_to_send("Hi there, google!");
	size_t nbytes_sent = socket.Send(msg_to_send.data(),
									 msg_to_send.size());

	std::cout << "Sent " << nbytes_sent << " bytes to " << str_address << "\n";

	// Let's see if google has anything to say back to us...
	char buffer[4096];

	// Recv() will _block_, meaning it waits until data comes
	// in (just like std::cin does).
	size_t nbytes_recved = socket.Recv(buffer, sizeof(buffer));

	// Now, buffer has nbytes_recved written into it.
	std::string msg_recved(buffer, nbytes_recved);

	std::cout << str_address << " says '" << msg_recved << "'.\n";

	SockLibShutdown();

	return 0;
}