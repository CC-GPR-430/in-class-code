#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <stdlib.h>

#include "socklib.h"
#include "defer.h"

void do_client() {
	const char* srv_host = "65.183.63.165";
	int srv_port = 20000;

	Socket sock(Socket::Family::INET, Socket::Type::STREAM);
	Address srv_addr(srv_host, srv_port);

	// When you bind, you have two IP address options:
	// "127.0.0.1" -> Local connections only (this machine)
	// "0.0.0.0" -> Any network connection

	Address cli_addr("0.0.0.0", 0);

	// Never do this!
	sock.Bind(cli_addr);

	// So, if we call Bind on a client, we have to use the
	// address "0.0.0.0:0".
	// So, all function calls that initiate a connection
	// just do this for you if you haven't done it already.
	// Connect(), SendTo(). These call Bind() for you, if
	// you haven't.

	while (true) {
		// Simulate talking to the server indefinitely
	}
}

struct MyStruct {
	int x;
	int y;
};

int main(int argc, char *argv[]) {

	/*
	* 5. Application
	* 4. Transport
	* 3. Network
	* 2. Data Link
	* 1. Physical
	*/


	/*
	*		 [ R1 ] -> [ R2 ]     [ R5 ]
	*		/     \               /
	* [ C ]        [ R3 ] -> [ R4 ] -> [ S ]
	*  To: 1.2.3.4						1.2.3.4
	* 
	*/
	SockLibInit();
	defer _([]() {SockLibShutdown();});

	char buffer[64];
	char* p_buffer = buffer;

	sizeof(buffer);
	sizeof(p_buffer);

	size_t bufsize = 20;
	for (int i = 0; i < bufsize; i++) {
		buffer[i] = 'a' + i;
	}

	std::string bufstring(buffer, bufsize);
	std::cout << bufstring << "\n";

	std::string my_string("123\000456", 6);
	const char* str = my_string.c_str();
	std::cout << strlen(str) << "\n";
	std::cout << (int)my_string[3] << "\n";
	std::cout << my_string.size() << "\n";
	std::cout << my_string << "\n";

	const char* num_str = "    abc   234  abc";

	std::stringstream ss(num_str);
	int ss_val;
	ss >> ss_val;
	if (ss.fail())
	{
		std::cout << "Failed to convert.\n";
	}
	else
	{
		std::cout << "ss_val is " << ss_val << "\n";
	}


	char* end;
	int val = strtol(num_str, &end, 10);
	size_t nchars_converted = end - num_str;
	std::cout << "Converted " << nchars_converted << " chars.\n";
	if (end == num_str)
	{
		std::cout << "Failed to convert.\n";
	}
	else
	{
		std::cout << "Val is " << val << "\n";
	}

	// do_client();

	return 0;
}
