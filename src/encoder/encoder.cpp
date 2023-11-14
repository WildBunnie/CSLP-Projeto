#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

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

Mat getEstimatesJPEG_LS(Mat image){

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
	
	for (int row = 0; row < residuals.rows; row++){
		for (int col = 0; col < residuals.cols; col++){
			
			int estimate = 0;

			if(row > 0 && col > 0){
				int a = residuals.at<uchar>(row,col-1);
				int b = residuals.at<uchar>(row-1,col);
				int c = residuals.at<uchar>(row-1,col-1);
				estimate =  jpeg_ls_predictor(a,b,c);
			}

			residuals.at<uchar>(row,col) = residuals.at<uchar>(row,col) + estimate;
		}
	}
	return residuals;
}

int main(int argc, char *argv[]){
    Mat image = imread(argv[1],IMREAD_GRAYSCALE);
	// imshow("image", image);  
	// waitKey(0);  
	Mat residuals = getEstimatesJPEG_LS(image);
	Mat result = getOriginalJPEG_LS(residuals);
	imshow("image", residuals);  
	waitKey(0);
    return 0;
}