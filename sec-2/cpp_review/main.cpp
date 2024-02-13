#include <iostream>
#include <iomanip>
#include <fstream>


////// String parsing takes a string like this:
////// "1234"
////// And turns it into the "number" or int:
////// 1234

// Binary representation of 1234:
// 010011010010
// String representation of "1234":
// [ '1'  ][ '2' ][ '3' ][ '4' ]0
//                              ^ Actual number 0 -- end of string.

namespace binary_files
{
    union IntConverter
    {
        int as_int;
        unsigned char as_bytes[sizeof(int)];
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

        stream << std::dec;

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

    void memory_example()
    {

        const char message[] = "Hello world!";
        std::cout << message << std::endl;

        print_bytes(std::cout, message, sizeof(message));

        std::cout << "sizeof(int): " << sizeof(int) << std::endl;
        std::cout << "sizeof(float): " << sizeof(float) << std::endl;
        std::cout << "sizeof(double): " << sizeof(double) << std::endl;
        std::cout << "sizeof(char): " << sizeof(char) << std::endl;
        std::cout << "sizeof(bool): " << sizeof(bool) << std::endl;
        std::cout << "sizeof(unsigned int): " << sizeof(unsigned int) << std::endl;
        std::cout << "sizeof(short): " << sizeof(short) << std::endl;
        std::cout << "sizeof(int*): " << sizeof(int*) << std::endl;
        std::cout << "sizeof(double*): " << sizeof(double*) << std::endl;
        std::cout << "sizeof(char*): " << sizeof(char*) << std::endl;

        int* p;
    //  ^^^  <- NOT THE IMPORTANT PART

        int* pp;
    //     ^ <- This is a POINTER.... (to an int... (probably))

        void* vp;
    //      ^ Still a pointer! This is fine!

            // Memory is basically this: char all_mem[2^32]
            // The least data we need to be able to index
            //    the last char in this array is 32 bits -- 4 bytes.
            // So, we could use an unsigned int and it could access every byte.
            // On a 64-bit system, memory is this: char all_mem[2^64]
            // We need a 64-bit number to access each address in this array.
            // So, we need an 8 byte integer.

            // So, on 32-bit systems, pointers are 4 bytes, and on 64-bit
            // systems, pointers are 8 bytes.

        IntConverter converter;
        converter.as_int = 31273;
        std::cout << converter << std::endl;

        const char number[] = "31273";
        print_bytes(std::cout, number, sizeof(number));
    }

    void write_to_file()
    {
        std::ofstream file("file1.bin");

        // ofstream::write(const char* str, int count)
        // Writes `count` bytes from `str` to the file.

        int num_to_write = 31273;
        file.write((char*)&num_to_write, sizeof(num_to_write));
    }

    void read_from_file()
    {
        std::ifstream file("file1.bin");

        // ifstream::read(char* str, int count)
        // Reads `count` bytes from the file and copies them to `str`.
        int num_to_read = 0;
        file.read((char*)&num_to_read, sizeof(num_to_read));
        std::cout << "Read " << num_to_read << std::endl;
    }

    void something()
    {



        char msg[] = "Hello, world!";
        print_bytes(std::cout, msg, sizeof(msg));

        std::cout << "sizeof(msg):  " << sizeof(msg) << std::endl;
        std::cout << "sizeof(*msg): " << sizeof(*msg) << std::endl;
        std::cout << "msg has " << sizeof(msg) / sizeof(*msg) << " elements\n";

        int int_array[] = { 45, -73, 200, 59 };
        std::cout << "sizeof(int_array):  " << sizeof(int_array) << std::endl;
        std::cout << "sizeof(*int_array): " << sizeof(*int_array) << std::endl;
        std::cout << "int_array has " << sizeof(int_array) / sizeof(*int_array) << " elements\n";

        // print the whole array
        print_bytes(std::cout, (char*)int_array, sizeof(int_array));

        // print each element of the array
        for (int i : int_array)
        {
            print_bytes(std::cout, (char*)&i, sizeof(i));
        }

        // print the first two elements of the array
        print_bytes(std::cout, (char*)int_array, sizeof(*int_array) * 2);
    }
};

using namespace binary_files;

int main()
{
    // string constructors
    // Have a null-terminated string
    char msg[] = "Hello, world!";

    // Want to make a string out of that:
    std::string str1(msg);
    std::cout << str1.size() << std::endl;
    std::cout << str1 << std::endl;

    // Can also make a string out of X bytes
    // of the given array.
    // This is useful when you have an array
    // of bytes that is NOT NULL TERMINATED.
    std::string str2(msg, 7);

    std::cout << str2.size() << std::endl;
    std::cout << str2 << std::endl;

    // Watch out for buffer overflows!
    // std::string str3(msg, 200);

    // std::cout << str3.size() << std::endl;
    // std::cout << str3 << std::endl;

    // Advanced method -- don't use a constructor
    std::string str4;
    str4.resize(sizeof(msg));
    // str4.data() is the pointer to the internal buffer
    // of the string.
    memcpy((void*)str4.data(), msg, str4.size());
    std::cout << str4 << std::endl;
}