#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>

// Source: https://stackoverflow.com/a/46931770
std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

void write_to_file()
{
	// Open an existing file
	// Can optionally add bitwise OR'd together flags
	//    as second parameter when opening a file
	//    to specify opening behavior
	// Example:
	//    std::ios_base::in -> open file to be read
	//    std::ios_base::out -> open file to be written to
	//    std::ios_base::in | std::ios_base::out
	//                       -> open file to be read from or written to (default)
	//    std::ios_base::trunc -> "truncate" -- open file and destroy
	//							  existing contents
	//    std::ios_base::app -> "append" -- open file and begin writing
	//								to end
	//    Documentation here:
	//		https://en.cppreference.com/w/cpp/io/basic_fstream/basic_fstream
	std::fstream my_file("test.txt", std::ios_base::in | std::ios_base::out);
	my_file << "This is a line in a file\n";
	my_file << "Here are some numbers:\n";
	my_file << 36 << " " << 42 << " " << 274.3f << std::endl;
}

void read_word_from_file()
{
	std::fstream my_file("test.txt");
	std::string line1;

	// stream extraction into string reads
	// just the first "word"
	my_file >> line1;
	std::cout << line1 << std::endl;
}

void read_line_from_file()
{

	std::fstream my_file("test.txt");
	std::string line1;
	std::string line2;

	std::getline(my_file, line1);
	std::getline(my_file, line2);
	std::cout << line1 << std::endl;
	std::cout << line2 << std::endl;
	if (line1.back() == '\n')
	{
		std::cout << "line1 ends in a newline\n";
	}
	else
	{
		std::cout << "line1 does not contain a newline\n";
	}
}

void read_whole_file()
{
	std::fstream my_file("test.txt");

	// for (int i = 0; i < n; i++)
	// while (true)
	for (std::string line; std::getline(my_file, line); )
	{
		std::cout << line << std::endl;
	}
}

void read_whole_file_into_words()
{
	std::fstream my_file("test.txt");

	// getline() can split on any delimiter, not just '\n'.
	for (std::string line; std::getline(my_file, line, ' '); )
	{
		std::cout << line << std::endl;
	}
}

void read_whole_file_and_split_each_line()
{
	std::fstream my_file("test.txt");

	for (std::string line; std::getline(my_file, line); )
	{
		std::cout << line << std::endl;
		std::vector<std::string> items = split(line, ' ');
		std::cout << "This line has " << items.size() << " words\n";
	}
}

int main()
{
	std::fstream my_file("test.txt");
	
	std::string line1;
	std::string line2;
	std::string line_with_numbers;
	std::getline(my_file, line1);
	std::getline(my_file, line2);
	std::getline(my_file, line_with_numbers);
	std::cout << line_with_numbers << std::endl;
	std::stringstream ss(line_with_numbers);

	// Alternatively, can use
	// strtol() or sscanf()
	int x = 0;
	int y = 0;
	float z = 0;
	ss >> x >> y >> z;
	if (ss.fail())
	{
		std::cout << "Error parsing file\n";
	}
	std::cout << "Sum: " << x + y + z << std::endl;
}