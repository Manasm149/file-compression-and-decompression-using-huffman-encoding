// decoder.cpp - Huffman Decompression
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <bitset>
#include <cstring>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: decoder <compressed_file> <output_file>\n";
        return 1;
    }

    ifstream in(argv[1], ios::binary);
    ofstream out(argv[2], ios::binary);
    if (!in || !out) {
        cerr << "File error.\n";
        return 1;
    }

    unsigned char mapSize;
    in.read((char*)&mapSize, 1);
    unordered_map<string, char> revCodes;

    for (int i = 0; i < mapSize; i++) {
        char ch, len;
        in.read(&ch, 1);
        in.read(&len, 1);
        char buf[8] = {0};
        in.read(buf, sizeof(buf));
        bitset<64> bits;
        memcpy(&bits, buf, sizeof(bits));
        string code = bits.to_string().substr(64 - len);
        revCodes[code] = ch;
    }

    string bitstream = "";
    char byte;
    in.read(&byte, 1);
    while (in.peek() != EOF) {
        bitset<8> bits(byte);
        bitstream += bits.to_string();
        in.read(&byte, 1);
    }

    int pad = (int)byte;
    if (pad > 0)
        bitstream = bitstream.substr(0, bitstream.size() - pad);

    string curr = "";
    for (char b : bitstream) {
        curr += b;
        if (revCodes.count(curr)) {
            out.put(revCodes[curr]);
            curr = "";
        }
    }

    cout << "Decompression completed.\n";
    return 0;
}