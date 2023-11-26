#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include "BitStream.h"
#include "Golomb.h"
#include "intraEncoder.h"

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

void encodeVideo(string outputFile,string file){
	VideoCapture cap(file);
	BitStream ouFile(outputFile,'w');
	Mat frame;
	cap.read(frame);
	Golomb encoder(&ouFile,50);
	encoder.encodeNumber(cap.get(CAP_PROP_FRAME_COUNT));
	encoder.encodeNumber(frame.cols);
	encoder.encodeNumber(frame.rows);
	encoder.encodeNumber(cap.get(CAP_PROP_FPS));

	while(1){
		if (frame.empty())
			break;
		cvtColor(frame,frame,COLOR_BGR2GRAY);
		frame = getResidualsJPEG_LS(frame);
		encoder.encodeMat(frame);
		cap >> frame;
	}
	cap.release();
}

void decodeVideo(string outputFile,string file){
	BitStream bs(file,'r');
	string format = ".mp4";
	Mat frame;
	Golomb gl(&bs,50);
	int fourcc = VideoWriter::fourcc('m','p','4','v');
	int framesCount = gl.decodeNumber();
	int cols = gl.decodeNumber();
	int row = gl.decodeNumber();
	int fps = gl.decodeNumber();
	Size size(cols,row);
	VideoWriter out(outputFile+format,fourcc,fps,size,false);
	for(int i = 0; i<framesCount; i++){
		frame = gl.decodeMat(cols,row);
		frame = getOriginalJPEG_LS(frame);
		out << frame;
	}
	out.release();
}

