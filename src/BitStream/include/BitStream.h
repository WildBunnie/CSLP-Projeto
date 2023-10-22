#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/**
 * \brief A class to read/write bits from/to a file.
 *
 */
class BitStream{
    private:
        /**
         * \brief The file stream.
         *
         */
        fstream file;
        /**
         * \brief 
         *
         */
        unsigned bitBufferW = 0;
        unsigned int currentBitW = 0;
        char bitBufferR = 0;
        unsigned int currentBitR = 0;
    public:
        /**
         * \brief Constructor.
         * 
         * \param name The file path.
         */
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