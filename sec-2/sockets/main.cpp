#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <stdlib.h>

#include "socklib.h"
#include "defer.h"

// Returns true if loop should continue -- returns false
// if other end hung up.
bool srv_handle_connection(Socket& conn_sock)
{
	char buffer[4096];
	int nbytes_recvd = conn_sock.Recv(buffer, sizeof(buffer));

	// Must ALWAYS check for errors!
	// nbytes_recvd == -1 iff an error occurred
	if (nbytes_recvd == -1)
	{
		// Print an error message about what went wrong
		// This will print a message that looks like this:
		// recv(): Connection was reset by peer.
		perror("recv()");
		// Exit the program -- we can't continue.
		exit(1);
	}

	// Next, check if client hung up connection.
	// recv() returns 0 iff this is the case.
	if (nbytes_recvd == 0)
	{
		// We're done talking to this client -- accept
		// a new connection.
		return false;
	}

	// If we made it here, nbyte_recvd > 0.
	std::string msg_str(buffer, nbytes_recvd);
	std::cout << "Received message '" << msg_str << "'\n";

	std::string response("Not implemented.\n");
	conn_sock.Send(response.data(), response.size());

	// Successful back-and-forth -- ask caller to keep looping.
	return true;
}

void do_server()
{
	// Make TCP Server
	
	// First, make a socket as usual
	// Pick STREAM to make a TCP socket
	Socket listen_sock(Socket::Family::INET, Socket::Type::STREAM);

	// Next, bind to an address
	Address srv_addr("0.0.0.0", 20000);
	listen_sock.Bind(srv_addr);

	// Now, we call Listen().
	// This turns the given socket into a "Listen Socket",
	// which can accept() connections, but CANNOT
	// send() or recv() data.
	// Listen() takes an optional parameter, `backlog`, which
	// specifies how many attempted connections can be
	// queued before new connections are automatically
	// rejected.
	listen_sock.Listen();

	// Servers should loop forever, continuing to accept connections.
	// If anyone wants to close the server, they can do it over
	// my dead body.
	// By which I mean, they can do it over task manager.
	while (true)
	{
		// Now, accept one incoming connection, waiting if necessary.
		// Returned socket represents an ongoing connection with
		// a single client, so can call send(), recv(), and even close()
		// without interfering with the listen socket.
		std::cout << "Waiting for connection...\n";
		Socket conn_sock = listen_sock.Accept();
		std::cout << "Accepted connection.\n";

		// Talk to the client until they hang up.
		while (srv_handle_connection(conn_sock))
		{
			// All logic is handled in srv_handle_connection,
			// so nothing to do here.
		}

		std::cout << "Other end hung up.\n";
	}
}

int main(int argc, char *argv[]) {
	SockLibInit();
	defer _([]() {SockLibShutdown();});

	do_server();

	return 0;
}
