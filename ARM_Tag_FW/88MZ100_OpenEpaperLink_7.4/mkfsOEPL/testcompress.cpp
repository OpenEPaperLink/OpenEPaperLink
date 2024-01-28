#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

int main() {
    // Input string to compress
    const char* inputString = "This is a sample string to compress and add a gzip header.werugweroiguie;wroigj;eworijg;oewirjg;oeiwrjg;oiewrj;goiewjr;ogiejw;rojig;ewoirjg;oewijrg;oewirj;goeiwj;roiwheg;orihg";

    // Length of the input string
    size_t inputLength = strlen(inputString);

    // Allocate memory for compressed data
    size_t compressedBufferSize = compressBound(inputLength) + 18;  // Additional space for gzip header
    char* compressedBuffer = (char*)malloc(compressedBufferSize);

    // Compression parameters with reduced window size (8 bits)
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = (uInt)inputLength;
    stream.next_in = (Bytef*)inputString;
    stream.avail_out = (uInt)compressedBufferSize;
    stream.next_out = (Bytef*)compressedBuffer;

    // Initialize compression with gzip header and reduced window size (8 bits)
    if (deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 8, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        fprintf(stderr, "Error initializing zlib for compression\n");
        free(compressedBuffer);
        return 1;
    }

    // Perform compression
    if (deflate(&stream, Z_FINISH) != Z_STREAM_END) {
        fprintf(stderr, "Error compressing data\n");
        deflateEnd(&stream);
        free(compressedBuffer);
        return 1;
    }

    // Finalize compression
    if (deflateEnd(&stream) != Z_OK) {
        fprintf(stderr, "Error finalizing compression\n");
        free(compressedBuffer);
        return 1;
    }

    // Display results
    printf("Original string length: %zu bytes\n", inputLength);
    printf("Compressed data length: %lu bytes\n", (unsigned long)stream.total_out);

    // Clean up
    free(compressedBuffer);

    return 0;
}
