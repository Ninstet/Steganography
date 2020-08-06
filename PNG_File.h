#pragma once
#include <png.h>

class PNG_File {

public:
    
    const char* inputFileName; // Name of the PNG file we wish to encode / decode

    PNG_File(const char* inputFileName); // Constructor for the PNG_File class

    void encode(const char* fileToEncodeName); // Method for encoding data into the PNG from a file

    void decode(const char* outputFileName); // Method for decoding the PNG file then outputting the data to a file

    void save(const char* outputFileName); // Method for saving the newly created PNG to a file

private:

    png_structp read_ptr;
    png_infop info_ptr;
    png_bytep* row_pointers;
    png_structp write_ptr;

    unsigned int width;
    unsigned int height;
    unsigned int bit_depth;

};