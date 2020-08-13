#include <iostream>
#include "PNG_File.h"

int main(int argc, char *argv[]) {
	std::cout << "\n";
	std::cout << " _______  _______  _______  _______  _______  __    _  _______  _______  ______    _______  _______  __   __  __   __ \n";
	std::cout << "|       ||       ||       ||       ||   _   ||  |  | ||       ||       ||    _ |  |   _   ||       ||  | |  ||  | |  |\n";
	std::cout << "|  _____||_     _||    ___||    ___||  |_|  ||   |_| ||   _   ||    ___||   | ||  |  |_|  ||    _  ||  |_|  ||  |_|  |\n";
	std::cout << "| |_____   |   |  |   |___ |   | __ |       ||       ||  | |  ||   | __ |   |_||_ |       ||   |_| ||       ||       |\n";
	std::cout << "|_____  |  |   |  |    ___||   ||  ||       ||  _    ||  |_|  ||   ||  ||    __  ||       ||    ___||       ||_     _|\n";
	std::cout << " _____| |  |   |  |   |___ |   |_| ||   _   || | |   ||       ||   |_| ||   |  | ||   _   ||   |    |   _   |  |   |  \n";
	std::cout << "|_______|  |___|  |_______||_______||__| |__||_|  |__||_______||_______||___|  |_||__| |__||___|    |__| |__|  |___|  \n\n\n";

	if (argc < 2 || argc % 2 == 1) {
		std::cout << "ERROR: Too few arguments\n";
		std::cout << "./Steganography <file> -e <encodefile> -d <decodedfile> -s <savedfile>\n";
		exit(1);
	}

	PNG_File file(argv[1]);

	for (int i = 1; i < argc; i++) {

		if (strcmp(argv[i], "-e") == 0) {
			file.encode(argv[++i]);
		} else if (strcmp(argv[i], "-d") == 0) {
			file.decode(argv[++i]);
		} else if (strcmp(argv[i], "-s") == 0) {
			file.save(argv[++i]);
		}
		
	}

	// PNG_File file("bueller.png");
	// file.encode("otter.png");
	// file.save("encode.png");
	// file.decode("decode.png");
}