// Quick refresher on reading from files,
// using streams, and using strings.

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

int main()
{
	std::cout << "Hello, world!\n";

	std::fstream my_file("test.txt");
	// for (int i = 0; i < n; i++)
	std::string line1;
	std::string line2;
	std::string line_with_nums;
	std::getline(my_file, line1);
	std::getline(my_file, line2);
	std::getline(my_file, line_with_nums);
	std::stringstream ss(line_with_nums);
	// Prefer using strtol()
	// Can also try using sscanf()
	int x = 0;
	int y = 0;
	float z = 0;
	ss >> x >> y >> z;
	// Want to test if this worked!
	float sum = x + y + z;
	std::cout << "Sum is: " << sum << std::endl;
	std::cout << line_with_nums << std::endl;
}