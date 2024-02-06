// Quick refresher on reading from files,
// using streams, and using strings.

#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

void write_to_file()
{
	// Can pass flags as second argument, such as:
	//    std::ios_base::in -> read-only
	//	  std::ios_base::out -> write-only
	//    std::ios_base::trunc -> Open file and destroy existing contents
	//    std::ios_base::app -> Open file and append to existing contents
	//    ....
	std::ofstream my_file("test.txt");
	my_file << "Hello, I'm a file!\n";
	my_file << "Here are some numbers:\n";
	my_file << 12 << " " << 42 << " " << 273.4f << std::endl;
}

void read_words()
{
	std::ifstream my_file("test.txt");
	std::string word1;
	std::string word2;
	// This reads a single "word" from the file, not a line!
	my_file >> word1 >> word2;
	std::cout << word1 << word2 << std::endl;
}

void read_line()
{
	std::ifstream my_file("test.txt");
	std::string line1;
	// Reads one line from my_file into line1
	// Consumes '\n' from "my_file", but does not put it
	//    in line1.
	std::getline(my_file, line1);
	std::cout << line1 << std::endl;
	if (line1.back() == '\n')
	{
		std::cout << "getline() adds \\n to string\n";
	}
	else
	{
		std::cout << "getline() skips the endline character\n";
	}
}

void read_whole_file()
{
	std::fstream my_file("test.txt");
	// for (int i = 0; i < n; i++)
	for (std::string line; std::getline(my_file, line); )
	{
		std::cout << line << std::endl;
	}
	std::cout << "Finished reading file.\n";
}

void conversion_with_sstream(const std::string& str)
{
	std::stringstream ss(str);
	// Prefer using strtol()
	// Can also try using sscanf()

	int x = 0;
	int y = 0;
	float z = 0;
	ss >> x >> y >> z;
	// Want to test if this worked!
	if (ss.fail())
	{
		std::cout << "Failed to convert values!\n";
	}
	else
	{
		float sum = x + y + z;
		std::cout << "Sum is: " << sum << std::endl;
	}
}

void write_an_integer(int* x)
{
	// 73 seems like a good number to write!
	*x = 73;
}

void use_strtol(const std::string& line)
{
	const char* beg = line.c_str();
	char* end;
	int value = strtol(beg, &end, 10);
	if (end == beg)
	{
		std::cout << "Failed to convert!\n";
	}
	std::cout << "read " << value << std::endl;
	value = strtol(end, &end, 10);
	std::cout << "read " << value << std::endl;
}

/*
char all_the_memory[2 ^ 64];
int int_pos; // max value is 2 ^ 32
size_t pos;  // max value is 2 ^ 32 on 32-bit, 2^64 on 64-bit
all_the_memory[pos] // pos is always big enough to reach the last
all_the_memory[int_pos] // not necessarily true for ints
*/

void write_file()
{

	std::fstream my_file("test.txt");
	// for (int i = 0; i < n; i++)
	std::string line1;
	std::string line2;
	std::string line_with_nums;
	std::getline(my_file, line1);
	std::getline(my_file, line2);
	std::getline(my_file, line_with_nums);
	use_strtol(line_with_nums);
	/*
	std::cout << line_with_nums << std::endl;

	int x, y, z;

	int num_converted = sscanf(line_with_nums.c_str(), "j k l %d %d %d", &x, &y, &z);
	if (num_converted != 3)
	{
		std::cout << "Failed to convert " << 3 - num_converted << " items!\n";
	}
	else
	{
		std::cout << x << " " << y << " " << z << std::endl;
	}
	*/
}
