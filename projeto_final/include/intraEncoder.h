#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include <iostream>

using namespace std;
using namespace cv;

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
Mat getResidualsJPEG_LS(Mat image);

/**
 * \brief Get original image from residuals using JPEG-LS non-linear predictor.
 * \param residuals Residuals to get original image from.
 * \return Original image on Mat format.
 *
 */
Mat getOriginalJPEG_LS(Mat residuals);

/**
 * \brief Encode a video using JPEG-LS non-linear predictor.
 * \param outputFile Output file name.
 * \param file Input file name.
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