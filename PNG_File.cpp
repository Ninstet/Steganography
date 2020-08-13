#include "PNG_File.h"
#include <iostream>
// #include <stdio.h>
// #include <stdlib.h>



// ---------- GLOBAL VARIABLES ----------



#define BYTE_SIZE 8 // Number of bits in a byte
#define PNG_SIG_LEN 8 // Length of a PNG image signature
#define SIZE_WIDTH 32 // The number of bits used for storing the length of a file



// ---------- FUNCTIONS ----------



int ipow(int base, int exp) {
    // Function for returning the power of a number as an integer
    
    int result = 1;
    while (exp) {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}


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



// ---------- MAIN ----------



PNG_File::PNG_File(const char* inputFileName) {
    /*
    This is the constructor for the PNG_File class.
    This takes a file name as input and performs error handling whilst loading 
    the file and using the libPNG library to process the PNG image into memory.
    */

    std::cout << "_____________________| LOAD |_____________________\n";
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
    
    std::cout << "Done! (" << filesize(inputFileName) << " bytes)\n";
    std::cout << "Width: " << width << ", Height: " << height << "\n\n";

}



void PNG_File::encode(const char* fileToEncodeName) {
    /*
    This is the encode method for the PNG_File class.
    This takes a file name as input and performs error handling whilst loading
    and encoding the file into the preloaded PNG file.
    */

    std::cout << "____________________| ENCODE |____________________\n";
    std::cout << "Loading data file " << fileToEncodeName << "... ";

    // Load the file into memory

    FILE* fileToEncode; // Attempt to open the file

    if (fopen_s(&fileToEncode, fileToEncodeName, "rb") != 0) { // Check if file is already open
        std::cout << "ERROR: File is already open!\n";
        exit(1);
    }

    unsigned char buffer = 0;
    unsigned int size = filesize(fileToEncodeName);

    std::cout << "Done! (" << size << " bytes) \n";

    // Encoding the data file into the PNG image

    for (int y = 0; y < height; y++) { // Loop over every row in the PNG image
        int x = 0;

        if (y == 0) { // For the first row of the PNG image
            std::cout << "Writing file size to first line... ";

            for (x; x < SIZE_WIDTH; x++) { // For all the bits needed to store the size of the data
                //std::cout << ".";
                
                if (size & ipow(2, x)) { // Encode the file size
                    *(row_pointers[y] + x) |= 1;
                } else {
                    *(row_pointers[y] + x) &= 0xFE;
                }
            }

            std::cout << "Done!\n";
            std::cout << "Writing file data... ";
        }

        // For y = 0, x = 32 however for y > 0, x = 0
        // Therefore we start writing after the size has been written

        for (x; x < width * 3; x++) { // Loop over every column in the PNG image
            if (x % BYTE_SIZE == 0) { // Check if one byte has been written
                if (!fread(&buffer, 1, 1, fileToEncode)) { // Read next byte and check if we have read the entire file
                    std::cout << "Done!\n\n";
                    goto loop_end;
                }
            }
            
            //std::cout << ".";

            if (buffer & ipow(2, x % BYTE_SIZE)) { // Encode the file data
                *(row_pointers[y] + x) |= 1;
            } else {
                *(row_pointers[y] + x) &= 0xFE;
            }
        }

        if (y >= height) { // Data file is too large to encode into PNG
            exit(1);
        }
    }
    
    loop_end:

    fclose(fileToEncode);
    
}



void PNG_File::decode(const char* outputFileName) {
    /*
    This is the decode method for the PNG_File class.
    This decodes the loaded PNG image and writes the
    data to to the output file given as an argument.
    */

    std::cout << "____________________| DECODE |____________________\n";
    std::cout << "Creating data file " << outputFileName << "... ";

    // Prepare data file for writing

    FILE* outputFile; // File we will output the data to

    if (fopen_s(&outputFile, outputFileName, "wb") != 0) { // Check if file is already open
        std::cout << "ERROR: File is already open!\n";
        exit(1);
    }

    std::cout << "Done!\n";

    unsigned char buffer = 0;
    unsigned int size = 0;

    // Decoding the data file from the PNG image

    for (int y = 0; y < height; y++) { // Loop over every row in the PNG image
        int x = 0;

        if (y == 0) { // For the first row of the PNG image
            std::cout << "Reading file size from first line... ";

            for (x; x < SIZE_WIDTH; x++) { // For all the bits needed to store the size of the data
                //std::cout << ".";

                size |= (*(row_pointers[y] + x) & 1) << x;
            }

            std::cout << "Done! (" << size << " bytes)\n";
            std::cout << "Reading file data... ";
        }

        // For y = 0, x = 32 however for y > 0, x = 0
        // Therefore we start reading after the size has been read

        for (x; x < width * 3; x++) { // Loop over every column in the PNG image
            if ((x > SIZE_WIDTH || y > 0) && x % BYTE_SIZE == 0) { // Check if one byte has been written AND we have read at least one iteration of the PNG image
                fwrite(&buffer, 1, 1, outputFile); // Write that char (1 byte) to the output file
                buffer = 0; // Reset the buffer
            }
            
            //std::cout << ".";

            if (((width * y) * 3 + x) == size * BYTE_SIZE + SIZE_WIDTH) {
                std::cout << "Done!\n\n";
                goto loop_end;
            }

            buffer |= ((*(row_pointers[y] + x) & 1) << x % BYTE_SIZE); // Read the LSB from the current pixel in the PNG image
        }
    }

    loop_end:

    fclose(outputFile);

}



void PNG_File::save(const char* outputFileName) {
    /*
    This is the save method for the PNG_File class.
    This outputs the PNG image object to a PNG file.
    */

    std::cout << "_____________________| SAVE |_____________________\n";
    std::cout << "Saving PNG image as " << outputFileName << "... ";

    FILE* outputFile; // File we will output the data to

    if (fopen_s(&outputFile, outputFileName, "wb") != 0) { // Check if file is already open
        std::cout << "ERROR: File is already open!\n";
        exit(1);
    }

    // Initialize the PNG structure for writing

    write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!write_ptr) {
        std::cout << "ERROR: Write pointer error\n";
        exit(1);
    }

    // Write the PNG rows to the PNG file

    png_init_io(write_ptr, outputFile);
    png_set_rows(write_ptr, info_ptr, row_pointers);
    png_write_png(write_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    fclose(outputFile);

    std::cout << "Done! (" << filesize(outputFileName) << " bytes)\n\n";
    
}