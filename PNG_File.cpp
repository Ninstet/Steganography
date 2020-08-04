#include "PNG_File.h"
#include <iostream>
// #include <stdio.h>
// #include <stdlib.h>



#define BYTE_SIZE 8 // Number of bits in a byte
#define PNG_SIG_LEN 8 // Length of a PNG image signature
#define SIZE_WIDTH 32 // The number of bits used for storing the length of a file



unsigned int filesize(const char* filename) {
    // Function for calculating the size of a file

    FILE* f;
    fopen_s(&f, filename, "rb");

    unsigned int size = 0;
    if (fseek(f, 0, SEEK_END) == 0)
        size = ftell(f);
    fclose(f);
    return size;
}



PNG_File::PNG_File(const char* inputFileName) {
    /*
    This is the constructor for the PNG_File class.
    This takes a file name as input and performs error handling whilst loading 
    the file and using the libPNG library to process the PNG image into memory.
    */
    std::cout << "Loading PNG file " << inputFileName << "... ";

    this->inputFileName = inputFileName; // Save the file name

    // Load the file into memory

    FILE* inputFile; // Attempt to open the file

    if (fopen_s(&inputFile, inputFileName, "rb") != 0) { // Check if file is already open
        std::cout << "ERROR: Could not open file\n";
        exit(1);
    }

    // Check the PNG signature

    unsigned char header[BYTE_SIZE];
    fread(header, 1, PNG_SIG_LEN, inputFile); // Read the PNG signature

    if (png_sig_cmp(header, 0, PNG_SIG_LEN)) { // Check that the file is a valid PNG image
        std::cout << "ERROR: File is not a valid PNG image\n";
        exit(1);
    }

    // Set up libPNG data strctures / error handling

    read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); // TODO: Error functions

    if (!read_ptr) { // Check read pointer was created successfully
        std::cout << "ERROR: Read pointer error\n";
        exit(1);
    }

    info_ptr = png_create_info_struct(read_ptr);

    if (!info_ptr) { // Check info pointer was created successfully
        png_destroy_read_struct(&read_ptr, (png_infopp)NULL, (png_infopp)NULL);
        std::cout << "ERROR: Info pointer error\n";
        exit(1);
    }

    png_infop end_info = png_create_info_struct(read_ptr);

    if (!end_info) { // Check end info pointer exists
        png_destroy_read_struct(&read_ptr, (png_infopp)NULL, (png_infopp)NULL);
        std::cout << "ERROR: End pointer error\n";
        exit(1);
    }

    // Set up libPNG IO for PNG

    png_init_io(read_ptr, inputFile); // Initialise IO for the PNG file we have loaded
    png_set_sig_bytes(read_ptr, PNG_SIG_LEN); // Alert libPNG that we read PNG_SIG_LEN bytes at the begining
    png_read_png(read_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL); // Read the entire PNG image into memory

    // MISC

    width = png_get_image_width(read_ptr, info_ptr);
    height = png_get_image_height(read_ptr, info_ptr);
    bit_depth = png_get_bit_depth(read_ptr, info_ptr);
    
    row_pointers = png_get_rows(read_ptr, info_ptr); // row_pointers is an array of pointers with each pointer pointing to one row of image data

    if (bit_depth != BYTE_SIZE) // Make sure that the bit depth is correct
        exit(1);

    fclose(inputFile); // Close the PNG file now that we have read its contents
    
    std::cout << "Done!\n";
    std::cout << "Width: " << width << ", Height: " << height << "\n";

}



void PNG_File::encode(const char* fileToEncodeName) {
    /*
    This is the encode method for the PNG_File class.
    This takes a file name as input and performs error handling whilst loading
    and encoding the file into the preloaded PNG file.
    */
    std::cout << "Loading data file " << fileToEncodeName << "... ";

    // Load the file into memory

    FILE* dataFile; // Attempt to open the file

    if (fopen_s(&dataFile, fileToEncodeName, "rb") != 0) { // Check if file is already open
        std::cout << "ERROR: File is already open!\n";
        exit(1);
    }

    unsigned char buffer = 0;
    unsigned long size = filesize(fileToEncodeName);

    // Encoding the data file into the PNG image

    for (int y = 0; y < height; y++) { // Loop over every row in the PNG image
        int x = 0;

        if (y == 0) {
            for (x; x < SIZE_WIDTH; x++) { // For all the bits needed to store the size of the data
                if ((size & ipow(2, x))) {

                } else {

                }
            }
        }
    }

}



void PNG_File::decode(const char* outputFileName) {
    // TODO: Decoding text from PNG image
}



void save(const char* outputFileName) {
    // TODO: Saving the encoded image to a file
}