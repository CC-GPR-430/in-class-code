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

void literal_examples() {
	// In C++, you can write literal data whenever you want.
	// So, you can write 20, 54.3, 'f', etc.
	int x = 20;
	// lhs -> int, rhs -> int
	int y = 20U;
	// lhs -> int, rhs -> unsigned int
	int z = 30.4;
	// lhs -> int, rhs -> double
	float f = 20.5f;
	// lhs -> float, rhs -> float
	char c = 'm';
	// lhs -> char, rhs -> char
	// same as char c = 109
	int i = 'b';
	// lhs -> int, rhs -> char
	// same as int i = 98
	// What type does the literal "hello, world" have?
	// std::string ?
	// char[] ?
	// C string???
	// char* ?
	// const char*
	// const char[]
	const char* str = "hello, world";
	// lhs -> const char*, rhs -> const char[]
	std::string str2 = "hello, there!";
	// lhs -> std::string, rhs -> const char*
	// This is the same as invoking the constructor:
	//	    std::string str2("hello, there!");
	// which as the prototype:
	//		std::string(const char*);
	// Ok wait so then what is a c++ std::string?
	//      (see aside below)
}

void cstr_examples() {
	// IMPORTANT: Note that string literals in
	// C(++) ALWAYS have a null terminator appended
	// to them. That is, this string:
	// "Hello!"
	//  123456
	// Is an array of SEVEN bytes:
	// Hello!\0
	// 123456 7
	// The compiler helpfully adds the 0 byte (another name
	// for the null terminator) to the end of the string
	// literal.

	{
		// This is a special case where the compiler is allowed
		// to make str however big it needs to be to hold the
		// entire string literal (which, again, always includes
		// a null terminator).
		const char str[] = "hello there, fine people!!";
		// because of const, this is illegal:
		// str[0] = 'b';
		const char* p = str;
		size_t count = 0;

		while (*p != '\0')
		{
			count++;
		}
		std::cout << "The string " << str << " has length " << count << "\n";
		std::cout << "The buffer " << str << " has size   " << sizeof(str) << "\n";
	}

	{
		const char* str = "hello there, fine people!!";
		// Note that, because of const, doing this:
		// str[0] = 'b';
		// is a compiler error.
		const char* p = str;
		size_t count = 0;
		while (*p != '\0')
		{
			count++;
			p++;
		}
		std::cout << "The string           " << str << " has length " << count << "\n";
		std::cout << "The variable holding " << str << " has size   " << sizeof(str) << "\n";
	}

	{
		// Non-const string buffer
		// Can modify contents... including the null terminator >:)
		char str[] = "hello there, fine people!!";
		str[sizeof(str) - 1] = '|';

		// Now, we try our normal counting function...
		const char* p = str;
		size_t count = 0;
		while (*p != '\0')
		{
			// There's no telling when this loop will exit!
			// This is a BUFFER OVERFLOW!
			// The SINGLE BIGGEST security vulnerability ANYWHERE!
			count++;
			p++;
		}

		std::cout << "string '" << str << "' has length " << count << "\n";
	}

	{
		// This is exactly what the C++ compiler gives you
		// when you write "Hello!"
		char str[] = { 'H', 'e', 'l', 'l', 'o', '!', '\0' };

		const char* p = str;
		size_t count = 0;
		while (*p != '\0')
		{
			count++;
			p++;
		}
		std::cout << "The string           " << str << " has length " << count << "\n";
		std::cout << "The variable holding " << str << " has size   " << sizeof(str) << "\n";
	}

	{
		// This IS a character array -- it is NOT a C string!
		char str[] = { 'H', 'e', 'l', 'l', 'o', '!' };

		const char* p = str;
		size_t count = 0;
		while (*p != '\0')
		{
			count++;
			p++;
		}
		std::cout << "The string           " << str << " has length " << count << "\n";
		std::cout << "The variable holding " << str << " has size   " << sizeof(str) << "\n";
	}

	//	~~~*~*~*~*~*~*~*~~		THE POINT	  ~~~*~*~**~*~*~*~*~*~*~*
	// NOTE: The type system is not sufficient to determine whether your
	// char* is a C string or is just a bunch of bytes.
	// The only way you can know whether you have a C string is by
	// having explicit knowledge about the contents of the buffer.
	// If it has a null terminator at the end of it, it's a C string.
	// If it doesn't, it's just a bunch of bytes.
}

void my_strcpy(char* dst, const char* src)
{
	while (*dst++ = *src++);
}

class my_cpp_str {
	// What's a std::string?
	// Essentially, this is a dynamic array, a la std::vector.
	// It's barely different from std::vector<char>.
	my_cpp_str()
	{
		buf = nullptr;
		cap = 0;
		len = 0;
	}

	my_cpp_str(const char* str)
	{
		// We want to allocate a buffer that's the size of the string.
		// Can I do sizeof(str)?
		// NO! That gives us the size of a pointer on the machine!
		// That gives us no information about the size of the buffer
		// that the pointer points to.
		// 
		// Every function in the C and C++ standard libraries
		// that accept a const char* as a parameter (and DON'T take a
		// second parameter the specifies a size) expect a 
		// NULL-TERMINATED STRING.
		// For this reason, null-terminated strings are also called
		// c strings.
		const char* p = str;
		size_t count = 0;
		while (*p != '\0')
		{
			count++;
			p++;
		}
		// count now contains the length of the string.
		buf = new char[count];
		len = count;
		cap = count;
	}

	void push_back(char c)
	{
		if (cap == len) {
			// reallocate
			if (cap == 0)
			{
				cap = 1;
			}
			else
			{
				cap *= 2;
			}
			char* new_buf = new char[cap];
			memcpy(new_buf, buf, len);
			buf = new_buf;
		}
		buf[len++] = c;
		// also add a null terminator (we'll get to it)
	}

private:
	char*  buf;
	size_t cap;
	size_t len;
};

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

	cstr_examples();

	return 0;
}
