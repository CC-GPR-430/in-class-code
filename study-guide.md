# Midterm Topics

The midterm exam will be held March 8 during class. Students will have
the full class period to complete the exam.

The midterm will be comprehensive, so students may be tested on
anything that we have covered so far in class or on assignments. Below
is a set of topics and questions meant to aid students in guiding
their studies.

Students may bring a sheet of _handwritten_ notes, double sided.

The function prototypes of the socklib library will be provided to
students for reference during the exam.

* OSI Layers
	* What are the 5 OSI layers we're focusing on in this class?
	* What number layer is e.g. Network?
	* What layer is layer number 5?
	* What addresses exist at each layer?
	* What does each layer do? (routing, direct communication,
      resolving which process to send data to, etc.)
* Socket Usage
	* Opening, closing, sending/receiving data, initializing/shutting
      down the library.
	* Setting up a server vs. setting up a client
	* You will be expected to write socket code.
	* How do you use each of the functions in the socklib library?
* UDP vs. TCP
	* How do you open a socket with TCP? With UDP?
	* What function calls are necessary for TCP/UDP that aren't
      necessary for the other protocol?
	* What benefits does TCP have? What costs does it have?
	* What benefits does UDP have? What costs does it have?
* Reading from files
	* How do you read text data from a file?
	* How do you read binary data from a file?
	* How do you read from a file until a certain character is
      reached?
* sizeof() uses
	* What does sizeof() do?
	* What will the results be of calling sizeof() on various types?
	* How does sizeof() interact with pointer types?
	* How does sizeof() interact with buffer types?
* strings, character buffers, and converting to/from numeric types
	* What is the difference between an array of char's and a C
      string?
	* How do you get the number of characters in a `std::string`?
	* How can you safely create a `std::string` from a char array of
      known length?
	* How can you convert a series of char's into ints, longs, floats
      or doubles?
		* Be sure you review functions such as `strtol()`,
          `stringstream::operator<<()`, and
          `stringstream::operator>>()`.
	* When converting a series of char's into any of the numeric types
      listed above, how can you verify that the conversion was
      successful?
	* How can you check that a string is a valid number that can be
      converted to a numeric type? (Hint: Try to convert it first!)
