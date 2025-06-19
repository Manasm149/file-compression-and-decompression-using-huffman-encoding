// encoder.cpp - Huffman Compression
#include <iostream>         // for standard I/O
#include <fstream>          // for file I/O
#include <queue>            // for priority_queue
#include <unordered_map>    // for storing frequency and code maps
#include <vector>           // for using vector in priority queue
#include <string>           // for string operations
#include <bitset>           // for fixed-size bit manipulation

using namespace std;

// Node structure for Huffman tree
struct Node {
    char ch;       // the character
    int freq;      // frequency of the character
    Node *left, *right; // pointers to left and right children

    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// Custom comparator for min-heap based on frequency
struct compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq; // min-heap (lowest frequency on top)
    }
};

// Recursive function to build the Huffman codes for each character
void buildCodes(Node* root, const string &s, unordered_map<char, string> &codes) {
    if (!root) return;

    // If leaf node, store the code
    if (!root->left && !root->right) {
        codes[root->ch] = s;
    }

    // Traverse left and right with '0' and '1' appended to the current code
    buildCodes(root->left, s + "0", codes);
    buildCodes(root->right, s + "1", codes);
}

// Function to write the encoded bitstring to file
void writeBitString(ofstream &out, const string &bits) {
    int count = 0;
    unsigned char buffer = 0;

    // Convert each group of 8 bits into a byte and write
    for (char bit : bits) {
        buffer = buffer << 1 | (bit == '1');  // shift left and add current bit
        count++;
        if (count == 8) {
            out.put(buffer);  // write the full byte
            count = 0;
            buffer = 0;
        }
    }

    // Handle remaining bits (padding if needed)
    if (count > 0) {
        buffer = buffer << (8 - count); // pad with 0s
        out.put(buffer);                // write final padded byte
        out.put((char)(8 - count));     // store number of padding bits
    } else {
        out.put((char)0);               // no padding needed
    }
}

// Entry point
int main(int argc, char* argv[]) {
    // Validate command-line arguments
    if (argc != 3) {
        cout << "Usage: encoder <input_file> <output_file>\n";
        return 1;
    }

    // Open input and output files in binary mode
    ifstream in(argv[1], ios::binary);
    ofstream out(argv[2], ios::binary);
    if (!in || !out) {
        cerr << "File error.\n";
        return 1;
    }

    // Step 1: Read the input file
    unordered_map<char, int> freq;
    string data((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

    // Step 2: Count frequencies of each character
    for (char c : data) freq[c]++;

    // Step 3: Create a priority queue of Nodes
    priority_queue<Node*, vector<Node*>, compare> pq;
    for (auto &p : freq) {
        pq.push(new Node(p.first, p.second));  // each node holds one character and its frequency
    }

    // Step 4: Build the Huffman Tree
    while (pq.size() > 1) {
        Node *l = pq.top(); pq.pop();  // smallest frequency
        Node *r = pq.top(); pq.pop();  // second smallest
        Node *m = new Node('\0', l->freq + r->freq);  // internal node
        m->left = l;
        m->right = r;
        pq.push(m);  // push new combined node
    }

    Node* root = pq.top();  // final root of the Huffman Tree

    // Step 5: Generate Huffman codes
    unordered_map<char, string> codes;
    buildCodes(root, "", codes);

    // Step 6: Write the Huffman codebook to the file
    out.put((char)codes.size());  // store number of entries in codebook
    for (auto &p : codes) {
        out.put(p.first);                    // store the character
        out.put((char)p.second.size());     // store the length of its Huffman code
        bitset<64> bits(p.second);          // convert code string to binary
        out.write((char*)&bits, sizeof(bits)); // write raw 64-bit block
    }

    // Step 7: Encode the input using the Huffman codes
    string encoded = "";
    for (char c : data) encoded += codes[c];

    // Step 8: Write the encoded bitstring to output file
    writeBitString(out, encoded);

    cout << "Compression completed.\n";
    return 0;
}
