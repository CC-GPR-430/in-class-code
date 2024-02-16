#include <iostream>
#include <sstream>
#include <string>
#include <time.h>

#include "socklib.h"
#include "defer.h"

void do_client() {
	const float max_timeout = 5.0f;
	// Create a UDP client

	// Step 1: Create a socket. Use DGRAM to make
	// a UDP socket, instead of STREAM (which we
	// used to make TCP sockets)
	Socket udp_sock(Socket::Family::INET, Socket::Type::DGRAM);

	// And... That's it! Now we can send and receive!

	std::string msg_to_send("Hi there, server!");

	float wait_time = 1.0f;
	udp_sock.SetTimeout(wait_time);

	// Send data to 127.0.0.1, which always means
	// "this machine."
	Address srv_addr("127.0.0.1", 54345);
	// Use sendto() instead of send() with UDP sockets.
	// Must also provide desination address.

	int nbytes_recvd = -1;
	char buffer[4096];
	Address from_addr;
	while (nbytes_recvd == -1) {
		size_t nbytes_sent = udp_sock.SendTo(
			msg_to_send.data(), msg_to_send.size(),
			srv_addr);
		std::cout << "Sent " << nbytes_sent << " bytes to " << srv_addr
			<< ".\n";

		nbytes_recvd = udp_sock.RecvFrom(buffer,
			sizeof(buffer), from_addr);

		if (nbytes_recvd == -1)
		{
			// RecvFrom failed! Need to check why.
			if (udp_sock.GetLastError() == Socket::Error::SOCKLIB_ETIMEDOUT) {
				std::cout << "Timed out after " << wait_time << " seconds.\n";
				wait_time *= 2;
				if (wait_time > max_timeout)
				{
					std::cout << "Gave up after " << wait_time << " seconds.\n";
					return;
				}
				else
				{
					std::cout << "Retrying with wait_time = " << wait_time << "...\n";
					udp_sock.SetTimeout(wait_time);
				}
			}
			else
			{
				std::cout << "Unrecognized error.\n";
				return;
			}
		}
	}

	// Always have to check RecvFrom's return value.
	// -1 means something went wrong!

	// Try "exponential backoff" -- retry if failed, doubling
	// the wait time. Give up after some predetermined
	// max_timeout threshold is reached.

	std::string str_msg_recvd(buffer, nbytes_recvd);
	std::cout << "Received " << nbytes_recvd << " from " << from_addr
		<< ": '" << str_msg_recvd << "'.\n";
}

void do_server() {
	// Again, make a socket:
	Socket udp_sock(Socket::Family::INET, Socket::Type::DGRAM);

	// For the sever, bind() so that others
	// know what port to send data to.

	// Recall, "0.0.0.0" => Accept connections/datagrams
	// from any address.
	// "127.0.0.1" => Accept connections only from
	// processes on this machine.
	Address my_addr("0.0.0.0", 54345);
	udp_sock.Bind(my_addr);

	// No need to call Listen(), because we're not
	// actually accepting connections -- just
	// datagrams.

	// Server needs to stay alive for client's
	// recvfrom() to not exit with an error.
	while (true) {
		char buffer[4096];
		Address from_addr;
		int nbytes_recvd = udp_sock.RecvFrom(buffer, sizeof(buffer),
			from_addr);
		std::string str_msg_recvd(buffer, nbytes_recvd);
		std::cout << "Received " << nbytes_recvd << " bytes from "
			<< from_addr << ": '" << str_msg_recvd << "'.\n";
	}

	std::string msg_to_client("Hello there, General Kenobi!");
	/*size_t nbytes_sent = udp_sock.SendTo(
		msg_to_client.data(), msg_to_client.size(),
		from_addr);
	std::cout << "Sent " << nbytes_sent << " bytes.\n"; */
}

int main(int argc, char *argv[]) {
	SockLibInit();
	defer _([]() {SockLibShutdown();});

	// If we don't receive any command line
	// arguments, run the client; otherwise,
	// run the server.
	if (argc == 1) do_client();
	else do_server();

	return 0;
}
