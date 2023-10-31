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
         * \brief Bit buffer used for writing in a file.
         * 
         */
        char bitBuffer = 0;
        /**
         * \brief Bit counter used when writing to a file.
         * 
         */
        unsigned int currentBit = 0;
        /**
         * \brief Bit buffer used for reading from a file.
         * 
         */
    public:
        /**
         * \brief Constructor.
         * 
         * \param name The file path.
         * \param mode The mode in witch the BitStream operates 'r' to read, 'w' to write
         */
        BitStream(string,char);
        /**
         * \brief Destructor.
         * 
         * Calls the function close().
         */
        ~BitStream();
        /**
         * \brief Writes any remaining bits and closes the file stream.
         * 
         */
        void close();
        /**
         * \brief Opens a file as a filestream.
         * 
         * \param name The file path.
         */
        void open(string);
        /**
         * \brief Checks if the file stream has bits left to read.
         * 
         * \return true if there are remaining bits, false if there are not.
         */
        bool hasNext();
        /**
         * \brief Writes a single bit to the file stream.
         * 
         * \param bit the bit to be written.
         */
        void writeBit(int);
        /**
         * \brief Writes an array of bits to the file stream.
         * 
         * \param bit the array of bits to be written.
         * \param size the size of the bit array.
         */
        void writeBits(int,int);
        /**
         * \brief Reads a single bit from the file stream.
         * 
         * \return The bit read.
         */
        int readBit();
        /**
         * \brief Reads an array of bits from the file stream.
         * 
         * \return The bit array read.
         * \param n how many bits to be read.
         */
        int readBits(int);
};