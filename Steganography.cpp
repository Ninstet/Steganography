#include <iostream>
#include "PNG_File.h"

int main() {
	std::cout << "Hello\n\n";

	PNG_File file("bueller.png");
	file.encode("failure.txt");
}
