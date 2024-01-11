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