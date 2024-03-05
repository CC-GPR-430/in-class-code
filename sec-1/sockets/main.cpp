#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <stdlib.h>

#include "socklib.h"
#include "defer.h"


// There is a symbol named "x" that has a place
// in memory. If I try to access "x", the compiler
// can find it.
int x = 5;

// Thre is a symbol named "y", but the compiler doesn't
// pick a place for it until I tell it to. So, if I try
// to access it, the compiler won't complain, but when
// the linker fails to find the actual definition of y,
// it sure will.
extern int y;

void do_client()
{
	const char* srv_host = "68.183.63.185";
	int srv_port = 9000;

	Socket sock(Socket::Family::INET, Socket::Type::STREAM);
	Address srv_addr(srv_host, srv_port);

	// Never do this!!!
	// sock.Bind(srv_addr);
	Address cli_addr("0.0.0.0", 0);

	// Only valid bind options: "0.0.0.0", "127.0.0.1"
	sock.Bind(cli_addr);

	// But -- this is the same as having skipped Bind()
	// and gone right to Connect() (or SendTo() on UDP)

	while (true) {
		// Simulate talking to a server indefinitely
	}
}

void say_sizeof(char buffer[]) {
	std::cout << sizeof(buffer) << std::endl;
}

int main(int argc, char *argv[]) {
	SockLibInit();
	defer _([]() {SockLibShutdown();});

	// do_client();

	char buffer[40];
	size_t bufsize = 20;

	std::string my_str(buffer, bufsize);
	char *buffer_p = buffer;
	//   ^ THIS IS THE IMPORTANT PART
//  ^^^^ NOT THIS!

	std::stringstream ss("250 Hello, world!");
	int first_int;
	ss >> first_int;
	// ^^ stringstream::operator>>()
	if (ss.fail()) {
		std::cout << "Failed to convert string to int.\n";
		ss.clear();
		std::string first_word;
		ss >> first_word;
		std::cout << "First word is " << first_word << "\n";
	}

	const char* str = " 20.5f         abc";
	char* end;
	double val = strtod(str, &end);
	size_t chars_converted = end - str;
	std::cout << "Successfully converted " << chars_converted << " chars.\n";
	if (end == str || isalpha(*end)) {
		std::cout << "Failed to convert.\n";
	}

	return 0;
	ss << "more text";
	// ^^ stringstream::operator<<()

	say_sizeof(buffer);
	std::cout << sizeof(buffer) << std::endl;
	std::cout << sizeof(buffer_p) << std::endl;

	return 0;
}
