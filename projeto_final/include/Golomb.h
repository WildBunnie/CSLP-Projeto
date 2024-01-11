#include "BitStream.h"

/**
 * \brief A entropy encoder built with Golomb codes.
 *
 */
class Golomb{
    private:
        /**
         * \brief Parameter used to divide the number to be encoded.
         *
         */
        int m;
        /**
         * \brief Number of bits to be used in the formula.
         *
         */
        int bits;
        /**
         * \brief BitStream used to write the encoded bits.
        */
        BitStream* bitStream;
    public:
        /**
         * \brief Constructor.
         *
         * \param bs BitStream used to write the encoded bits.
         * \param n Parameter used to divide the number to be encoded.
         */
        Golomb(BitStream* bs,int n);
        /**
         * \brief Encodes number.
         *
         * \param number Number to be encoded.
         */
        void encodeNumber(int);
        /**
         * \brief Decodes number.
         * \return Decoded number.
         */
        int decodeNumber();
        /**
         * \brief Encodes a string, using the encodeNumber function.
         * \param string String to be encoded.
         */
        void encodeString(string);
        /**
         * \brief Decodes a string, using the decodeNumber function.
         * 
         * \return Decoded String.
         */
        string decodeString();
};