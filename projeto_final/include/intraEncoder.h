#include <iostream>

using namespace std;

/**
 * \brief JPEG-LS non-linear predictor.
 * \param a Left pixel.
 * \param b Upper pixel.
 * \param c Upper left pixel.
 * \return Pixel prediction.
 *
 */
int jpeg_ls_predictor(int a, int b, int c);

/**
 * \brief Get residuals from an image using JPEG-LS non-linear predictor.
 * \param image Image to get residuals from.
 * \return Residuals on Mat format.
 *
 */
void encodeVideo(string outputFile,string file);

/**
 * \brief Decode a video using JPEG-LS non-linear predictor.
 * \param outputFile Output file name.
 * \param file Input file name.
 *
 */
void decodeVideo(string outputFile,string file);