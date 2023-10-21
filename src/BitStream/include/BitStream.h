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
        void close();
        void open(string);
        bool hasNext();
        void writeBit(int);
        void writeBits(int*,int);
        int readBit();
        int* readBits(int);
};