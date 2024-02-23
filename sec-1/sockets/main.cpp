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

	// Some examples of literal values:
	int x = 50;
	// lhs -> int, rhs -> int
	float f = 30;
	// lhs -> float, rhs -> int
	int y = 200U;
	// lhs -> int, rhs -> unsigned int
	float f2 = 20.5;
	// lhs -> float, rhs -> double
	float f3 = 20.5f;
	// lhs -> float, rhs -> float
	int z = 50L;
	// lhs -> int, rhs -> long
	// "Not implemented" is a literal.
	// What type is it?
	// char[] ?
	// char* ?
	// const char* !
	// ^^^ const means you can't change the values pointed to
	//     by this pointer. That is, this:
	// const char* str = "hello, world";
	// str[0] = 'f';
	//     is illegal! because str is a const ptr.

	std::string cpp_str = "Not implemented";
	// lhs -> std::string, rhs -> const char*

	const char* str = "hello, world";
	// str[0] = 'f';
	// ^^ compiler error :(
	buffer[0] = 'f';
	// ^^ allowed :)

	// string is NOT a const char* -- it has one, but it isn't one.
	// How do we get a const char* out of a std::string?
	// std::string::c_str()!! :D
	std::string response("Not implemented");
	conn_sock.Send(response.data(), response.size());
	// response.c_str()[0] = 'f';
	// ^^ compiler error (lhs is const char*)
	// response.data()[0] = 'f';
	// ^^ a-ok; rhs is char*

	return true;
}

void str_literal_examples() {

	{
		const char* str1 = "Hello!";
		// lhs -> const char*, rhs -> const char*
		// Let's count our string:
		size_t count = 0;
		const char* p = str1;
		while (*p != '\0') {
			count++;
			p++;
		}

		std::cout << "The string " << str1 << " has length " << count << std::endl;
	}

	{
		const char str2[] = "Hello!";
		const char* p = str2;
		size_t count = 0;
		while (*p != '\0') {
			count++;
			p++;
		}
		std::cout << "The string " << str2 << " has length " << count << std::endl;
		std::cout << "The char buffer " << str2 << " has size " << sizeof(str2)
				<< " (because it contains a null terminator not counted in the "
				<< "length of the string)" << std::endl;
	}

	{
		// Non-const buffer -- let's remove the null terminator >:)
		char str3[] = "Hello!";
		str3[sizeof(str3) - 1] = 'b';
		// NOW what happens when we calculate the length?

		const char* p = str3;
		size_t count = 0;
		while (*p != '\0') {
			// There's no telling when this loop will exit!!
			count++;
			p++;
		}

		std::cout << "string " << str3 << " has length " << count << std::endl;
	}

	{
		// NOT a c string -- just an array of characters (bytes)
		// BUT we added the null-terminator by hand.
		char str4[] = { 'H', 'e', 'l', 'l', 'o', '!', '\0' };
		const char* p = str4;
		size_t count = 0;
		while (*p != '\0') {
			count++;
			p++;
		}

		std::cout << "string " << str4 << " has length " << count << std::endl;
		std::cout << "The char buffer " << str4 << " has size " << sizeof(str4) << std::endl;
	}

	{
		// NOT a c string -- just an array of characters (bytes)
		char str5[] = { 'H', 'e', 'l', 'l', 'o', '!' };
		const char* p = str5;
		size_t count = 0;
		while (*p != '\0') {
			count++;
			p++;
		}

		std::cout << "string " << str5 << " has length " << count << std::endl;
		std::cout << "The char buffer " << str5 << " has size " << sizeof(str5) << std::endl;

	}

}

class my_cpp_str {
	// Wait scott what on earth is a c++ string then?

	// Basically just a vector/dynamic array.


public:
	my_cpp_str() {
		buf = nullptr;
		len = 0;
		cap = 0;
	}

	my_cpp_str(const char* str) {
		// First, we want to allocate our buffer.
		// So, we need to know how long str is.
		// But, our caller didn't pass in a variable
		// that tells us the length of str.
		// So, how can we figure it out?
		// WELL!!
		// There's a trick out C strings, including string literals.
		// They all end with '\0'.
		// That is, this literal:
		// "Hello!"
		//  123456789
		// Is actually stored by the C++ compiler with
		// an extra '\0' byte at the end:
		// char my_str[] = "Hello!" (makes an array whose length is
		//							 equal to the number of bytes
		//							 in the literal "Hello!")
		// sizeof(my_str) = not 6, but 7!!!
		// The above line is equivalent to this:
		// char my_str[] = {'H', 'e', 'l', 'l', 'o', '!', '\0'};
		// So, how do we count the number of characters in a string?
		// We start at the beginning and proceed until we hit '\0'!

		const char* str2 = str;
		size_t count = 0;
		while (*str2 != '\0') {
			count++;
			str2++;
		}

		buf = new char[count];
		len = count;
		cap = count;
	}

private:
	char*  buf;
	size_t len;
	size_t cap;
};

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

	str_literal_examples();

	return 0;
}
