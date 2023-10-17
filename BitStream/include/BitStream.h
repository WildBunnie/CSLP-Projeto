#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class BitStream{
    private:
        fstream file;
        unsigned bitBufferW = 0;
        unsigned int currentBitW = 0;
        char bitBufferR = 0;
        unsigned int currentBitR = 0;
    public:
        BitStream(string);
        ~BitStream();
        bool hasNext();
        void writeBit(int);
        void writeBits(int* bits,int size);
        int readBit();
        int* readBits(int n);
};