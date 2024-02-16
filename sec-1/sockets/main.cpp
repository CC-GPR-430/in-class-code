#include <iostream>
#include <sstream>
#include <string>
#include <time.h>

#include "socklib.h"
#include "defer.h"

void do_client() {

	// Using UDP

	// Step 1: Make a socket. Use DGRAM instead of STREAM to
	// make a UDP socket instead of a TCP socket.
	Socket udp_sock(Socket::Family::INET, Socket::Type::DGRAM);

	// That's it actually
	// Now we can send and recv :)

	std::string msg_to_send("Hello, server! How are you?");
	// Send to this computer, which "127.0.0.1" always refers to.
	Address server_addr("127.0.0.1", 10245);

	// Want to wait some amount of time for a response, and if
	// we don't get it, we assume it's not coming. So, we want
	// to resend our message and wait again.

	float wait_time = 0.1f;
	while (true)
	{
		udp_sock.SetTimeout(wait_time);

		char buffer[4096];
		size_t nbytes_sent = udp_sock.SendTo(
			msg_to_send.data(), msg_to_send.size(),
			server_addr);
		Address from_addr;
		std::cout << "Waiting on recv()...\n";
		int nbytes_recvd = udp_sock.RecvFrom(
			buffer, sizeof(buffer),
			from_addr);
		
		// With the timeout, RecvFrom() might return either because:
		// - It received data successfully, or
		// - A timeout occurred.
		// We can tell which situation we're in by looking at
		// the value returned.
		// nbytes_recved == -1 => no data was received.
		// nbytes_recved >  0  => nbytes_recved bytes were received.
		if (nbytes_recvd == -1)
		{
			// For some reason, we didn't get data. We need to check
			// what that reason was.
			if (udp_sock.GetLastError() == Socket::Error::SOCKLIB_ETIMEDOUT)
			{
				// This means we timed out
				// We want to try again!
				// Resend / call recvfrom() again
				std::cout << "Timed out!\n";
				wait_time *= 2;
				if (wait_time >= 5.0)
				{
					std::cout << "Exceeded max wait time. Giving up.\n";
					break;
				}
			}
			else
			{
				// Unknown error. Abort.
				std::cout << "Unknown error occurred. Aborting.\n";
			}
		}
		if (nbytes_recvd > 0)
		{
			std::cout << "Got a message! We're done here.\n";
			// BUT WAIT! Who sent this data?! We need to confirm it's who
			// we think it is!
			// Couple options for how to test:
			// 1. Check addresses for equality -- from_addr should be the same
			//		as serv_addr
			//** 2. Include some data with the client request that only the client
			//      knows. So for example, send:
			//    "REQUEST_ID|MESSAGE_BODY"
			// Ex. "82734|Hi there server!"
			//      REQUEST_ID should be *randomly generated* -- otherwise, an
			//		interloper can figure out what the next request ID will be
			//		and use that to masquerade as the server.
			//		Then, when we receive a message, it should include the same request ID.
			//		If not, we ignore it.
			// 3. You can actually use connect() on UDP sockets!
			//		This doesn't work the same as connect() with TCP, in the sense
			//		that it doesn't offer any real guarantees. But, it will essentially
			//		automatically do option (1) for you, so you don't have to do it
			//		explicitly. That is, the socket will check the incoming address
			//		for you, and make sure it matches the address specified in connect().
			//		Also, when using connect(), you can use send() and recv(), not just
			//		sendto() and recvfrom().
			//
			// NOTE that a determined attacker can circumvent ALL OF these techniques.
			// When handling sensitive data, the only option is to use well-tested
			// encryption techniques (which we'll cover later).
			break;
		}
	}
	return;

	// SendTo() takes an extra parameter as compared to Send():
	// the address to which the data should be sent.
	// The same socket can send data to any address, unlike
	// with TCP, where a socket is bound a single connection.
	size_t nbytes_sent = udp_sock.SendTo(msg_to_send.data(),
		msg_to_send.size(),
		server_addr);

	std::cout << "Sent " << nbytes_sent << " bytes to "
		<< server_addr << "\n";

	char buffer[4096];
	Address msg_from;

	// RecvFrom returns the number of bytes received
	// and creates a new address, which it stores in the
	// last parameter. This address is the address from
	// which the data arrived.
	size_t nbytes_recvd = udp_sock.RecvFrom(buffer, sizeof(buffer), msg_from);
	std::string msg_recvd(buffer, nbytes_recvd);

	std::cout << "Received " << nbytes_recvd << " bytes from " << msg_from
		<< ": '" << msg_recvd << "'\n";
}

void do_server()
{
	// As usual, make a socket
	Socket udp_sock(Socket::Family::INET, Socket::Type::DGRAM);

	// Now bind to a port so other devices know where to send their data.
	// Recall that "0.0.0.0" => Receive connections/datagrams from anywhere
	// and "127.0.0.1" => Only accept data from this machine (i.e., local
	// processes).
	Address my_address("0.0.0.0", 10245);
	udp_sock.Bind(my_address);

	// Now we're ready to receive data!
	char buffer[4096];
	Address from_address;
	size_t nbytes_recvd = udp_sock.RecvFrom(buffer, sizeof(buffer), from_address);
	std::string msg_recvd(buffer, nbytes_recvd);
	std::cout << "Received " << nbytes_recvd << " bytes from " << from_address
		<< ": '" << msg_recvd << "'.\n";

	std::string msg_to_send("Hello! I'm so sad :)");

	size_t nbytes_sent = udp_sock.SendTo(msg_to_send.data(), msg_to_send.size(), from_address);
	std::cout << "Sent " << nbytes_sent << " bytes to " << from_address << ".\n";
}

int main(int argc, char *argv[]) {
	SockLibInit();
	defer _([]() {SockLibShutdown();});
	srand(time(NULL));

	// Run client if no args passed on command line;
	// otherwise, run server.
	if (argc == 1) do_client();
	else do_server();

	return 0;
}
