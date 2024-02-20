#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <stdlib.h>

#include "socklib.h"
#include "defer.h"

// Returns false when connection is done. Returns a true
// when there is more to handle.
bool srv_handle_connection(Socket& conn_sock)
{
	static char buffer[4096] = { 0 };
	int nbytes_recvd = conn_sock.Recv(buffer, sizeof(buffer));

	// First, ALWAYS check for errors!
	// nbytes_recvd == -1 indicates an error.
	if (nbytes_recvd == -1)
	{
		// Print the system error message.
		perror("recv()");

		// Don't wanna deal with errors -- just crash.
		exit(1);
	}

	// Next, check to see if the connection was closed.
	// nbytes_recvd == 0 indicates other end hung up.
	if (nbytes_recvd == 0)
	{
		// Stop handling this connection and move on
		// to a new connection.
		return false;
	}

	// nbytes_recvd is definitely positive at this point.
	std::string msg_str(buffer, nbytes_recvd);
	std::cout << "Received message '" << msg_str << "'\n";

	std::string response("Not implemented");
	conn_sock.Send(response.data(), response.size());

	return true;
}

void do_server()
{
	// TCP Server: Must pick STREAM for Type
	Socket listen_sock(Socket::Family::INET, Socket::Type::STREAM);

	// Next: Bind to an address (same as UDP server)
	Address srv_addr("0.0.0.0", 20000);
	listen_sock.Bind(srv_addr);

	// Next: Listen() on socket
	// This makes this socket into a "listen socket",
	// which means you're no longer allowed to call
	// send/recv on this socket -- only accept().
	//
	// Listen() takes an optional parameter, which
	// is the number of pending connections you're
	// willing to allow. If this is 16, the 17th
	// pending connection will be immediately
	// rejected.
	// By default, backlog = 16.
	listen_sock.Listen();

	// Accept connections forever -- if someone wants
	// to shut the server down, they can do it over
	// my dead body.
	// By which I mean, they can use the task manager.
	while (true)
	{
		// Next step is to accept() connections.
		// Returns a socket representing the connection,
		// which can have send(), recv() and close()
		// called on it without disrupting the listen
		// socket.
		std::cout << "Waiting for connection...\n";
		Socket conn_sock = listen_sock.Accept();
		std::cout << "Received connection!\n";
		// Now, keep talking to the client until
		// they shut down the connection.
		while (srv_handle_connection(conn_sock))
		{
			// Logic is all handled in srv_handle_connection().
		}
		std::cout << "Connection hung up.\n";
	}
}

int main(int argc, char *argv[]) {
	SockLibInit();
	defer _([]() {SockLibShutdown();});

	do_server();

	return 0;
}
