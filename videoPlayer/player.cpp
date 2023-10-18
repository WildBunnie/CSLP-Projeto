#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include <iostream>
#include <numeric>
 

using namespace std;
using namespace cv;

class player{
private:
	VideoCapture* cap;
	string name;
public:
	player(String video);
	~player();
	void display();
	bool isOpen();
	Mat  frameRgb2Yuv(Mat frame);
	Mat  frameYuv2Rgb(Mat frame);
	void displayImage();
	static void getHistogram(Mat frame, int color);
	static vector<int> getGreyHistogram(Mat frame);
	static void getColorHistograms(Mat frame);
	static void printHistogram(vector<int> hist, int color);
	static Mat toGrayscale(Mat frame);
	static Mat addWatermark(Mat frame, Mat watermark, float alpha, int x, int y);
	static void histogramEqualization(Mat frame);
	static Mat gaussianBlur(Mat frame, int ksize, double sigma);
	static Mat boxFilter(Mat frame, Size ksize);
};

player::player(String video){
	this->name = video;
	cap = new VideoCapture(video);
}

player::~player(){
	delete cap;
}

bool player::isOpen(){
	return cap->isOpened();
}

Mat player::frameRgb2Yuv(Mat frame){
	for(int i = 0; i < frame.rows; i++){
		for(int j = 0; j < frame.cols; j++){
			uint r = frame.at<Vec3b>(i,j)[2];
			uint g = frame.at<Vec3b>(i,j)[1];
			uint b = frame.at<Vec3b>(i,j)[0];

			uint y =  0.299*r+0.587*g+0.114*b;
			uint cb = 128+0.564*((int)b-(int)y);
			uint cr = 128+0.713*((int)r-(int)y);

			frame.at<Vec3b>(i,j)[0] = y;
			frame.at<Vec3b>(i,j)[1] = cb;
			frame.at<Vec3b>(i,j)[2] = cr;
		}
		}

	return frame;
}

Mat player::frameYuv2Rgb(Mat frame){
	for(int i = 0; i < frame.rows; i++){
		for(int j = 0; j < frame.cols; j++){
			uint y = frame.at<Vec3b>(i,j)[0];
			uint cb = frame.at<Vec3b>(i,j)[1];
			uint cr = frame.at<Vec3b>(i,j)[2];

			uint r = y+1.13983*((int)cr-128);
			uint g = y-0.39465*((int)cb-128)-0.58060*((int)cr-128);
			uint b = y+2.03211*((int)cb-128);

			frame.at<Vec3b>(i,j)[2] = r;
			frame.at<Vec3b>(i,j)[1] = g;
			frame.at<Vec3b>(i,j)[0] = b;
		}
		}
		return frame;
}

void player::display(){
	while(1){
		Mat frame,color_frame;
		this->cap->read(frame);
		frame = this->frameRgb2Yuv(frame);
		frame = this->frameYuv2Rgb(frame);
		imshow(this->name,frame);
		char c=(char)waitKey(25);
			if(c==27)
				break;
	}
	cap->release();
	destroyAllWindows();
}

void player::displayImage(){
	Mat img = imread(this->name, 1);

	imshow("image", img);  
	waitKey(0);  
	
	destroyAllWindows();
 }

void player::getHistogram(Mat frame, int color){
	vector<int> histogram(256,0);
	int colorRGB;

	for(int i = 0; i < frame.rows; i++){
		for(int j = 0; j < frame.cols; j++){
				colorRGB = frame.at<Vec3b>(i,j)[color]; // b = 0, g = 1, r = 2
				histogram.insert(histogram.begin() + colorRGB, ++histogram[colorRGB]);
		}
	}

	printHistogram(histogram, color);
}

vector<int> player::getGreyHistogram(Mat frame){
	vector<int> histogram(256,0);
	int itensity;

	for(int i = 0; i < frame.rows; i++){
		for(int j = 0; j < frame.cols; j++){
				itensity = frame.at<uchar>(i,j); 
				histogram.insert(histogram.begin() + itensity, ++histogram[itensity]);
		}
	}

	printHistogram(histogram, 4);
	return histogram;
}

void player::getColorHistograms(Mat frame){

	for(int i = 0; i < 3; i++){
		getHistogram(frame, i);
	}
}

void player::printHistogram(vector<int> hist, int color){
	Mat img = imread("background.jpg", 1);
	Point p1, p2;
	Scalar colorScalar;

	switch (color){
		case 2:
			colorScalar = Scalar(0, 0, 255);
			break;
		case 1:
			colorScalar = Scalar(0, 255, 0);
			break;
		case 0:
			colorScalar = Scalar(255, 0, 0);
			break;
		default: // greyscale
			colorScalar = Scalar(255, 255, 255);
			break;
	}

	int up_height = *max_element(hist.begin(), hist.end());

  for(int i=0; i < hist.size(); i++){
    p1 = Point(i*10+10, floor(up_height/100) - floor(hist[i]/100)); // right - top
    p2 = Point(i*10, floor(up_height/100)); // left - bottom
    rectangle(img, p1, p2, colorScalar, 1);
  }

	namedWindow("Histogram", WINDOW_NORMAL);
	imshow("Histogram", img);
	waitKey(0);

	destroyAllWindows();
}

Mat player::addWatermark(Mat frame, Mat watermark, float alpha, int x, int y)
{
	for (int row = 0; row < watermark.rows; row++){
		for (int col = 0; col < watermark.cols; col++){
			if (col + x < frame.rows && row + y < frame.cols)
			frame.at<Vec3b>(col + x, row + y) *= 1 - alpha;
			frame.at<Vec3b>(col + x, row + y) += watermark.at<Vec3b>(col, row) * alpha;
		}
	}
	return frame;
}

Mat player::toGrayscale(Mat frame)
{
	Mat result(frame.rows, frame.cols, CV_8UC1);
	for (int row = 0; row < frame.rows; row++){
		for (int col = 0; col < frame.cols; col++){
			Vec3b pixel = frame.at<Vec3b>(row, col);
			result.at<uchar>(row, col) = pixel[0] * 0.11 + pixel[1] * 0.59 + pixel[2] * 0.30;
		}
	}
	return result;
}

Mat player::gaussianBlur(Mat frame, int ksize, double sigma){
	if (ksize % 2 == 0)
	{
		ksize++;
	}

	int radius = ksize / 2;

	Mat kernel(ksize, ksize, CV_64F);
	double sum = 0.0;

	for (int x = -radius; x <= radius; x++) {
		for (int y = -radius; y <= radius; y++) {
			double value = exp(-(x * x + y * y) / (2 * sigma * sigma));
			kernel.at<double>(x + radius, y + radius) = value;
			sum += value;
		}
	}

	kernel /= sum;

	Mat result;
	filter2D(frame, result, -1, kernel);
	return result;
}

Mat player::boxFilter(Mat frame, Size ksize)
{
	Mat kernel = Mat::ones(ksize.height, ksize.width, CV_64F);
	kernel *= 1.0 / (ksize.width * ksize.height);
	cout << kernel << endl;
	Mat result;
	filter2D(frame, result, -1, kernel);
	return result;
}

int main(){
	player p1("vd.mp4");

	if(!p1.isOpen()){
		cout << "error open video";
		return -1;
	}
	 
	return 0;
}