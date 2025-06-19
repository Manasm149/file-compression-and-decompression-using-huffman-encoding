# Huffman Encoder/Decoder

A simple C++ implementation of Huffman Coding for **file compression and decompression**.

## 📦 Features

- Compress any text file using Huffman Encoding
- Decompress encoded `.huff` files back to the original text
- Efficient encoding using frequency-based binary trees
- Bit-level file operations for accurate storage

## 🧠 How It Works

Huffman Coding is a greedy algorithm used for lossless data compression. It assigns variable-length binary codes to characters based on their frequency of occurrence in the input. More frequent characters get shorter codes.

- A frequency map is built from the input file.
- A priority queue (min-heap) constructs the Huffman tree.
- Binary codes are assigned by traversing the tree.
- The original file is converted to a compressed binary form.
- Decompression reads the binary and reverses the process using the tree.



## 🔧 Build & Run

### Requirements

- C++11 or later
- A C++ compiler like `g++`

### Compile

```bash
g++ encoder.cpp -o encoder
g++ decoder.cpp -o decoder


