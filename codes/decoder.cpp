// decoder.cpp - Huffman Decompression

#include <iostream>             // for standard I/O
#include <fstream>              // for file input/output
#include <unordered_map>        // to map Huffman codes back to characters
#include <bitset>               // to handle fixed-length bit conversions
#include <cstring>              // for memcpy

using namespace std;

int main(int argc, char* argv[]) {
    // Ensure the program is called with correct arguments
    if (argc != 3) {
        cout << "Usage: decoder <compressed_file> <output_file>\n";
        return 1;
    }

    // Open input and output files in binary mode
    ifstream in(argv[1], ios::binary);
    ofstream out(argv[2], ios::binary);
    if (!in || !out) {
        cerr << "File error.\n";
        return 1;
    }

    // Step 1: Read the codebook (Huffman map)
    unsigned char mapSize;
    in.read((char*)&mapSize, 1);  // read how many characters were encoded

    unordered_map<string, char> revCodes;  // reverse mapping: code -> character

    // Step 2: Read each character's Huffman code from file
    for (int i = 0; i < mapSize; i++) {
        char ch, len;
        in.read(&ch, 1);   // read character
        in.read(&len, 1);  // read length of its Huffman code

        char buf[8] = {0};         // buffer to hold 64-bit encoded bits
        in.read(buf, sizeof(buf)); // read 64 bits from file

        bitset<64> bits;
        memcpy(&bits, buf, sizeof(bits));  // convert raw bytes into bitset

        string code = bits.to_string().substr(64 - len); // extract the valid part
        revCodes[code] = ch;  // store in reverse map
    }

    // Step 3: Read the encoded bitstream
    string bitstream = "";
    char byte;

    // Read all bytes except the last one (it stores padding info)
    in.read(&byte, 1);  // first data byte
    while (in.peek() != EOF) {
        bitset<8> bits(byte);
        bitstream += bits.to_string();  // append its 8-bit binary string
        in.read(&byte, 1);              // read next
    }

    // Step 4: Remove padding bits at the end
    int pad = (int)byte;  // padding info is stored in last byte
    if (pad > 0)
        bitstream = bitstream.substr(0, bitstream.size() - pad);

    // Step 5: Decode the bitstream using Huffman codes
    string curr = "";
    for (char b : bitstream) {
        curr += b;
        if (revCodes.count(curr)) {
            out.put(revCodes[curr]);  // write decoded character
            curr = "";                // reset for next character
        }
    }

    cout << "Decompression completed.\n";
    return 0;
}
