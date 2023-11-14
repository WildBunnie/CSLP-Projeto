#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include "BitStream.h"
#include "Golomb.h"

using namespace std;
using namespace cv;

int jpeg_ls_predictor(int a, int b, int c){
    if (c >= max(a,b)){
        return min(a, b);
    }
    else if(c <= min(a,b)){
        return max(a, b);
    }
    else{
        return a + b - c;
    }
} 

Mat getResidualsJPEG_LS(Mat image){

	Mat residuals(image.rows, image.cols, CV_8UC1);
	for (int row = 0; row < image.rows; row++){
		for (int col = 0; col < image.cols; col++){
			
			int estimate = 0;

			if(row > 0 && col > 0){
				int a = image.at<uchar>(row,col-1);
				int b = image.at<uchar>(row-1,col);
				int c = image.at<uchar>(row-1,col-1);
				estimate = jpeg_ls_predictor(a,b,c);
			}
			residuals.at<uchar>(row,col) = image.at<uchar>(row,col) - estimate;
		}
	}
	return residuals;
}

Mat getOriginalJPEG_LS(Mat residuals){
	
	Mat original = residuals.clone();
	for (int row = 0; row < original.rows; row++){
		for (int col = 0; col < original.cols; col++){
			
			int estimate = 0;

			if(row > 0 && col > 0){
				int a = original.at<uchar>(row,col-1);
				int b = original.at<uchar>(row-1,col);
				int c = original.at<uchar>(row-1,col-1);
				estimate =  jpeg_ls_predictor(a,b,c);
			}

			original.at<uchar>(row,col) = original.at<uchar>(row,col) + estimate;
		}
	}
	return original;
}

int main(int argc, char *argv[]){
    Mat image = imread(argv[1],IMREAD_GRAYSCALE);
	Mat residuals = getResidualsJPEG_LS(image);
	BitStream bs("residuals",'w');
	Golomb gl(&bs,25);
	gl.encodeMat(residuals);
	bs.close();
	BitStream bsd("residuals",'r');
	Golomb decoder(&bsd,25);
	Mat residuals_r = decoder.decodeMat(image.cols,image.rows);
	bsd.close();
	Mat result = getOriginalJPEG_LS(residuals_r);
	// imshow("image", image);  
	// waitKey(0);
	// imshow("image", residuals);  
	// waitKey(0);
	imshow("image", result);  
	waitKey(0);
    return 0;
}