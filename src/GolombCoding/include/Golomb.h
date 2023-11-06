#pragma once
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
         * \brief Used to write bits encoded
        */
        BitStream* bitStream;
    public:
        /**
         * \brief Constructor.
         *
         * \param n Parameter used to divide the number to be encoded.
         */
        Golomb(BitStream* bs,int n);
        /**
         * \brief Encodes number.
         *
         * \param number Number to be encoded.
         * \return Number encoding.
         */
        void encodeNumber(int);
        /**
         * \brief Decodes number.
         *
         * \param numbers array of bits (ints) of number encoded.
         */
        int decodeNumber();
};