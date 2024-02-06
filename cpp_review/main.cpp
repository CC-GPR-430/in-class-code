#include <iostream>
#include <iomanip>
#include <fstream>

namespace BinFiles
{

	union IntConverter
	{
		int as_int;
		unsigned char as_bytes[sizeof(int)];
	};


	struct Weapon
	{
		int current_ammo;
		int max_ammo;
		unsigned char name[10];
		bool is_melee;
	};

	union WeaponConverter
	{
		Weapon as_weapon;
		char as_bytes[sizeof(Weapon)];
	};

	std::ostream& operator<<(std::ostream& stream, IntConverter item)
	{
		stream << "Dec: " << item.as_int << std::endl;
		stream << "Hex: 0x" << std::setfill('0') << std::setw(8) << std::hex << std::right << item.as_int << std::endl;
		stream << "Bytes: ";

		for (int i = 0; i < sizeof(int); i++)
		{
			stream << "0x" << std::setfill('0') << std::setw(2) << +item.as_bytes[i] << " ";
		}

		return stream;
	}

	std::ostream& operator<<(std::ostream& stream, WeaponConverter item)
	{
		// stream << "Dec: " << item.as_int << std::endl;
		// stream << "Hex: 0x" << std::setfill('0') << std::setw(8) << std::hex << std::right << item.as_int << std::endl;
		stream << "Bytes: ";

		for (int i = 0; i < sizeof(WeaponConverter); i++)
		{
			stream << "0x" << std::setfill('0') << std::setw(2) << +item.as_bytes[i] << " ";
		}

		return stream;
	}


	std::ostream& print_bytes(std::ostream& stream, const char* value, size_t len)
	{
		for (int i = 0; i < len; i++)
		{
			stream << "0x" << std::hex << std::setfill('0') << std::setw(2) << +(unsigned char)value[i] << " ";
		}
		stream << std::dec << std::endl;

		return stream;
	}

	void size_examples()
	{

		int int_array[128];

		std::cout << "sizeof(int): " << sizeof(int) << std::endl;
		std::cout << "sizeof(float): " << sizeof(float) << std::endl;
		std::cout << "sizeof(double): " << sizeof(double) << std::endl;
		std::cout << "sizeof(char): " << sizeof(char) << std::endl;
		std::cout << "sizeof(unsigned int): " << sizeof(unsigned int) << std::endl;
		std::cout << "sizeof(long long unsigned int): " << sizeof(long long unsigned int) << std::endl;
		std::cout << "sizeof(int_array): " << sizeof(int_array) << std::endl;

		std::cout << "sizeof(Weapon): " << sizeof(Weapon) << std::endl;

		// Using unions
		IntConverter converter;
		converter.as_int = 312;

		std::cout << "&converter.as_int:   " << &converter.as_int << std::endl;
		std::cout << "&converter.as_bytes: " << &converter.as_bytes << std::endl;

		std::cout << "sizeof(converter.as_int):   " << sizeof(converter.as_int) << std::endl;
		std::cout << "sizeof(converter.as_bytes): " << sizeof(converter.as_bytes) << std::endl;

		std::cout << converter << std::endl;
		converter.as_int = 44213;
		std::cout << converter << std::endl;

		WeaponConverter weapon;
		weapon.as_weapon = { 20, 50, "Gunnnnn", true };

		std::cout << weapon << std::endl;
	}

	void write_to_file()
	{
		std::ofstream binary_file("file1.bin");

		//  int* p;
		//  ^^^ <- not actually the important part

		//  int* pp;
		//     ^ <- this means "A Pointer"

			// Facts about pointers:
			//    - All pointers are the same size.

		std::cout << "sizeof(int*):  " << sizeof(int*) << std::endl;
		std::cout << "sizeof(char*): " << sizeof(char*) << std::endl;

		// ofstream::write(const char* str, int count)
		// Writes `count` bytes from string `str` to the file.

		int to_write = 102;
		binary_file.write((char*)&to_write, sizeof(to_write));

	}

	void str_and_int_comparison_example()
	{

		const char msg[] = "Hello, world!";
		std::cout << msg << std::endl;
		print_bytes(std::cout, msg, sizeof(msg));

		const char number[] = "1357243";
		int number_as_int = 1357243;
		std::cout << number << std::endl;
		std::cout << "ASCII bytes: ";
		print_bytes(std::cout, number, sizeof(number));
		std::cout << "int bytes:   ";
		print_bytes(std::cout, (char*)&number_as_int, sizeof(number_as_int));
	}

	void read_one_int()
	{
		std::ifstream binary_file("file1.bin");

		// ifstream::read(char* str, int count)
		// Reads `count` bytes from the file and copies them to `str`.
		int int_to_read;
		binary_file.read((char*)&int_to_read, sizeof(int_to_read));
		std::cout << int_to_read << std::endl;
	}

	void string_constructor_examples()
	{
		char msg[] = "Hello, world!";

		// ONLY WORKS IF `msg` IS ALREADY NULL-TERMINATED
		std::string str(msg);

		// Makes a string out of 7 bytes of msg
		std::string str_2(msg, 7);

		// ... Uh oh... This copies more bytes from
		// `msg` than exist in `msg`! It's a buffer
		// overflow!
		std::string str_3(msg, 300);

		std::cout << str << std::endl;
		std::cout << str_2 << std::endl;
		// Uncomment this to see what a buffer overflow
		// looks like (assuming it doesn't crash your
		// system!
		//std::cout << str_3 << std::endl;

		// Advanced C++!
		// Can make a string, resize it, and then
		// access the (resized) buffer, copying bytes
		// into that.
		std::string my_string;
		my_string.resize(sizeof(msg));
		memcpy((char*)my_string.data(), &msg, my_string.size());
		std::cout << my_string << std::endl;

	}

	void array_example()
	{
		char msg[] = "Hello, world!";

		std::cout << "sizeof(msg):  " << sizeof(msg) << std::endl;
		std::cout << "sizeof(*msg): " << sizeof(*msg) << std::endl;
		std::cout << "msg has " << sizeof(msg) / sizeof(*msg) << " elements" << std::endl;

		int nums[] = { 20, -3, 45, -700 };
		std::cout << "sizeof(nums):  " << sizeof(nums) << std::endl;
		std::cout << "sizeof(*nums): " << sizeof(*nums) << std::endl;
		std::cout << "nums has " << sizeof(nums) / sizeof(*nums) << " elements" << std::endl;

		// Printing the bytes of the array...
		print_bytes(std::cout, (char*)&nums, sizeof(nums));
		for (int n : nums)
		{
			// ... gives the same thing as printing the bytes of each element of the array.
			print_bytes(std::cout, (char*)&n, sizeof(n));
		}

		// This is very relevant for implement read_n()!

		print_bytes(std::cout, (char*)&nums, sizeof(*nums) * 2);
	}
};

using namespace BinFiles;

int main()
{
	return 0;
}