#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int jpeg_ls_predictor(int a, int b, int c);

Mat getResidualsJPEG_LS(Mat image);

Mat getOriginalJPEG_LS(Mat residuals);

void encodeVideo(string outputFile,string file);

void decodeVideo(string outputFile,string file);