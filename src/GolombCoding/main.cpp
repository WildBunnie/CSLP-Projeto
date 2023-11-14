#include <iostream>
#include "Golomb.h"
#include "BitStream.h"
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>

using namespace std;

int main(int argc, char const *argv[])
{   
    //create Golomb coder with division number
    BitStream bs("teste",'w');
    Golomb coder(&bs,5);
    coder.encodeNumber(5);
    bs.close();
    // encode number receive vector with bits(ints)

    BitStream bs2("teste",'r');
    Golomb decoder(&bs2,5);
    cout << decoder.decodeNumber();

    cv::Mat img1 = cv::imread("teste.jpg", cv::IMREAD_GRAYSCALE);

    BitStream imagebs("imageGol",'w');
    Golomb imageCoder(&imagebs,10);
    imageCoder.encodeMat(img1);
    int cols = img1.cols;
    int rows = img1.rows;

    imagebs.close();
    BitStream imagebsR("imageGol",'r');
    Golomb imageDecoder(&imagebsR,10);

    cv::Mat img2 = imageDecoder.decodeMat(cols,rows);

    cv::imshow("woof",img2);

    cv::waitKey(0);



    

    return 0;
}
