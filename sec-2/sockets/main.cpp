#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <stdlib.h>

#include "socklib.h"
#include "defer.h"

void do_client() {

	// Using UDP

	// Step 1: Make a socket. Use DGRAM instead of STREAM to
	// make a UDP socket instead of a TCP socket.
	Socket udp_sock(Socket::Family::INET, Socket::Type::DGRAM);
	float wait_time = 2.0f;
	// udp_sock.SetTimeout(wait_time);

	// That's it actually
	// Now we can send and recv :)

	std::string msg_to_send("Hello, server! How are you?");
	// Send to this computer, which "127.0.0.1" always refers to.
	Address server_addr("127.0.0.1", 10245);


	char buffer[4096];
	Address msg_from;

	// RecvFrom returns the number of bytes received
	// and creates a new address, which it stores in the
	// last parameter. This address is the address from
	// which the data arrived.
	const float max_wait_time = 5.0f;
	int nbytes_recvd = -1;
	// while (wait_time < max_wait_time && nbytes_recvd == -1)
	{
		// SendTo() takes an extra parameter as compared to Send():
		// the address to which the data should be sent.
		// The same socket can send data to any address, unlike
		// with TCP, where a socket is bound a single connection.
		size_t nbytes_sent = udp_sock.SendTo(msg_to_send.data(),
			msg_to_send.size(),
			server_addr);

		std::cout << "Sent " << nbytes_sent << " bytes to "
			<< server_addr << "\n";


		// Problem: RecvFrom() receives data from *anyone* -- but
		// we only want data that the server sent back! How can
		// we make sure the data we got back is from the server?
		// Some options:
		//	1.	RecvFrom() tells us the address from which the message
		//		came. Can compare that with the server's address
		//		to see if the data came from the server.
		//	2.	We can actually use connect() on UDP sockets! This doesn't
		//		offer any guarantees like it does with TCP, but essentially
		//		this will automatically do step 1 for us -- packets
		//		received from other sources will be automatically
		//		filtered out.
		//		Also, can use send() and recv() after a connect, not just
		//		sendto() and recvfrom().
		//	3.	Send data along with the body of our message that establishes
		//		the ID of that particular message. Like so:
		//		"REQUEST_ID|MESSAGE_BODY"
		//	Ex.	"82935|Hi there, server!"
		//		Then, the server should send back a response that contains
		//		the same REQUEST_ID prefix. If we get a request id that's
		//		not the one we were expecting, we discard the message.
		//		Note that, unlike the previous two options, this is
		//		a change to the application layer protocol. So,
		//		the server has to be on board with this technique. The first
		//		two techniques we can always use.
		//		The request id must be randomly generated -- if it's
		//		predictable, it's easy to spoof.
		// Note that any of these mitigating techniques can be overcome
		// by a determined assailant. When sending sensitive data, your
		// only safe option is using well-tested and well-understood
		// encyption techniques (which will be discussed later).
		int nbytes_recvd = udp_sock.RecvFrom(buffer, sizeof(buffer), msg_from);

		// nbytes_recvd == -1 => We did not receive data for some reason
		// nbytes_recvd  > 0  => We received nbytes_recvd bytes.
		if (nbytes_recvd == -1)
		{
			// An error occurred in our receive!
			if (udp_sock.GetLastError() == Socket::Error::SOCKLIB_ETIMEDOUT)
			{
				std::cout << "Timed out. Maybe retrying...\n";
				// We timed out! Increase the timeout time and try again.
				wait_time *= 2;
				udp_sock.SetTimeout(wait_time);
			}
		}
	}

	if (wait_time > max_wait_time)
	{
		std::cout << "Never received a message!! Now I really AM sad :(\n";
		return;
	}

	std::cout << "Received " << nbytes_recvd << " bytes from " << msg_from
		<< ": '";
	std::cout.write(buffer, nbytes_recvd);
	std::cout << "'\n";
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
	std::cout << "Received " << nbytes_recvd << " bytes from " << from_address
		<< ": '";
	std::cout.write(buffer, nbytes_recvd);
	std::cout << "'.\n";

	std::string msg_to_send("Hello! I'm so sad :)");
	//size_t nbytes_sent = udp_sock.SendTo(msg_to_send.data(), msg_to_send.size(), from_address);
	//std::cout << "Sent " << nbytes_sent << " bytes to " << from_address << ".\n";

	while (true) {}
}

void generate_random_number() {
	int num = rand();
	std::cout << "Picked " << num << "\n.";
}

int main(int argc, char *argv[]) {
	SockLibInit();
	defer _([]() {SockLibShutdown();});

	// Do this ONCE per program:
	srand(time(NULL));

	// Run client if no args passed on command line;
	// otherwise, run server.
	if (argc == 1) do_client();
	else do_server();

	return 0;
}
