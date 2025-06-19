// encoder.cpp - Huffman Compression
#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <bitset>

using namespace std;

struct Node {
    char ch;
    int freq;
    Node *left, *right;

    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

void buildCodes(Node* root, const string &s, unordered_map<char, string> &codes) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->ch] = s;
    }
    buildCodes(root->left, s + "0", codes);
    buildCodes(root->right, s + "1", codes);
}

void writeBitString(ofstream &out, const string &bits) {
    int count = 0;
    unsigned char buffer = 0;

    for (char bit : bits) {
        buffer = buffer << 1 | (bit == '1');
        count++;
        if (count == 8) {
            out.put(buffer);
            count = 0;
            buffer = 0;
        }
    }

    if (count > 0) {
        buffer = buffer << (8 - count);
        out.put(buffer);
        out.put((char)(8 - count));  // padding info
    } else {
        out.put((char)0);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: encoder <input_file> <output_file>\n";
        return 1;
    }

    ifstream in(argv[1], ios::binary);
    ofstream out(argv[2], ios::binary);
    if (!in || !out) {
        cerr << "File error.\n";
        return 1;
    }

    unordered_map<char, int> freq;
    string data((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

    for (char c : data) freq[c]++;
    priority_queue<Node*, vector<Node*>, compare> pq;

    for (auto &p : freq) pq.push(new Node(p.first, p.second));
    while (pq.size() > 1) {
        Node *l = pq.top(); pq.pop();
        Node *r = pq.top(); pq.pop();
        Node *m = new Node('\0', l->freq + r->freq);
        m->left = l; m->right = r;
        pq.push(m);
    }

    Node* root = pq.top();
    unordered_map<char, string> codes;
    buildCodes(root, "", codes);

    out.put((char)codes.size());
    for (auto &p : codes) {
        out.put(p.first);
        out.put((char)p.second.size());
        bitset<64> bits(p.second);
        out.write((char*)&bits, sizeof(bits));
    }

    string encoded = "";
    for (char c : data) encoded += codes[c];
    writeBitString(out, encoded);

    cout << "Compression completed.\n";
    return 0;
}

