#include <iostream>
#include <sstream>
#include <string>

#include "socklib.h"

void do_client()
{
	// Sockets : Network Card :: Files : Hardrive

	// INET => IPv4 Network Protocol
	// Stream => TCP Transport Protocol
	Socket socket(Socket::Family::INET, Socket::Type::STREAM);

	// Created a socket, but it doesn't talk to anything yet.

	// Created an address (this is google.com's IP address).
	std::string str_address("127.0.0.1"); // <- Use IP address, not MAC address
	Address address(str_address);

	// Connect the socket to the given address:
	// Must also provide a port -- port 80 = web traffic
	socket.Connect(address, 15000);

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
}

void do_server()
{
	std::cout << "Running server.\n";
	// Same as before -- make a socket
	Socket listen_socket(Socket::Family::INET, Socket::Type::STREAM);

	// Only two options:
	// 0.0.0.0 = accept connections from anywhere
	// 127.0.0.1 = accept connections only from processes on this machine
	Address srv_address("0.0.0.0");

	// Could also do this on client, but why bother? We don't care about
	// our port number except for the sake of telling other people how
	// to connect to us.
	listen_socket.Bind(srv_address, 15000);

	// Opens up the port for connections
	listen_socket.Listen();

	// Ready for new connections! Use Accept()
	// Accept() is like Recv() in that it blocks
	// (i.e., waits for a new connection to come in)
	// It's also a little unusual in that it returns
	// a new socket!
	Socket conn_socket = listen_socket.Accept();

	// The rest is up to our Application layer protocol --
	// Send() and Recv() as dictated by the protocol.

	char buffer[4096];
	size_t nbytes_recvd = conn_socket.Recv(buffer, sizeof(buffer));
	std::string cli_msg(buffer, nbytes_recvd);

	std::cout << "Received message " << cli_msg << " from client.\n";

	std::string msg_to_send("Hi yourself, client!");
	size_t nbytes_sent = conn_socket.Send(msg_to_send.data(),
		msg_to_send.size());

	std::cout << "Sent " << nbytes_sent << " bytes to client.\n";
}

int main(int argc, char *argv[])
{
	// Set up socket library
	SockLibInit();

	std::cout << "argc: " << argc;

	if (argc == 1) do_client();
	else do_server();

	SockLibShutdown();

	return 0;
}