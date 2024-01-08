#pragma once
#include "BitStream.h"
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>

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
         * \brief Encode an openCV Mat to a bitStream using Golomb.
         *
         * \param image Image in Mat format.
         */
        void encodeMat(cv::Mat);
        /**
         * \brief Decode a Golomb encoded bitStream to a Mat.
         *
         * \param cols Number of columns of the Mat.
         * \param rows Number of rows of the Mat.
         * \return Decoded Mat.
         */
        cv::Mat decodeMat(int cols,int rows);
        /**
         * \brief Encodes String to file.
         * 
         * \param str string to be written.
         */
        void encodeString(string);
        /**
         * \brief decodes String to file.
         * 
         * \return Decoded String.
         */
        string decodeString();
};