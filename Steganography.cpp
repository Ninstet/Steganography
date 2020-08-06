#include <iostream>
#include "PNG_File.h"

int main() {
	std::cout << "Hello\n\n";

	PNG_File file("bueller.png");
	file.encode("otter.png");
	file.save("encode.png");
	file.decode("decode.png");
}
