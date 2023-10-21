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
	static void printHistogram(vector<int> hist, int color);
	static void getHistogram(Mat frame, int color);
	static Mat histogramEqualization(Mat frame);
public:
	player(String video);
	~player();
	void display(string filter, string watermark);
	void displayImage(string option, string watermark);
	bool isOpen();
	static Mat  frameRgb2Yuv(Mat frame);
	static Mat  frameYuv2Rgb(Mat frame);
	static void getColorHistograms(Mat frame);
	static Mat toGrayscale(Mat frame);
	static Mat addWatermark(Mat frame, Mat watermark, float alpha, int x, int y);
	static Mat grayHistEqualization(Mat frame);
	static Mat colorHistEqualization(Mat frame);
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

void player::display(string filter, string watermark = ""){
	while(1){
		Mat frame,newframe,color_frame;
		this->cap->read(frame);

		if (filter == "grayscale"){
			newframe = toGrayscale(frame);
		}
		else if (filter == "gaussian"){
			newframe = gaussianBlur(frame, 5, 2);
		}
		else if (filter == "blur"){
			newframe = boxFilter(frame, Size(5,5));
		}
		else if (filter == "watermark"){
			Mat w = imread(watermark);
			newframe = addWatermark(frame, w, 1, 0, 0);
		}
		else if (filter == "rgb2yuv"){
			newframe = frameRgb2Yuv(frame);
		}
		else if (filter == "yuv2rgb"){
			newframe = frameYuv2Rgb(frame);
		}
		else if (filter == "colorhisteq"){
			newframe = colorHistEqualization(frame);
		}
		else if (filter == "grayhisteq"){
			newframe = grayHistEqualization(frame);
		}
		else{
			newframe = frame;
		}
		imshow(this->name,newframe);
		char c=(char)waitKey(25);
			if(c==27)
				break;
	}
	cap->release();
	destroyAllWindows();
}

void player::displayImage(string option, string watermark = ""){
    Mat img = imread(this->name, 1);
	Mat newImg;

	if(option == "histogram"){
		getColorHistograms(img);
	}
	else if(option == "grayhisteq"){
		newImg = grayHistEqualization(img);
	}
	else if(option == "colorhisteq"){
		newImg = colorHistEqualization(img);
	}
	else if (option == "grayscale"){
		newImg = toGrayscale(img);
	}
	else if (option == "gaussian"){
		newImg = gaussianBlur(img, 5, 2);
	}
	else if (option == "blur"){
		newImg = boxFilter(img, Size(5,5));
	}
	else if (option == "watermark"){
		Mat w = imread(watermark);
		newImg = addWatermark(img, w, 1, 0, 0);
	}
	else if (option == "rgb2yuv"){
		newImg = frameRgb2Yuv(img);
	}
	else if (option == "yuv2rgb"){
		newImg = frameYuv2Rgb(img);
	}

	if(newImg.rows > 0 && newImg.cols > 0){
		imshow("image", newImg);  
		waitKey(0);  

		destroyAllWindows();
	}
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

void player::getColorHistograms(Mat frame){

	for(int i = 0; i < 3; i++){
		getHistogram(frame, i);
	}
}

void player::printHistogram(vector<int> hist, int color){
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
		default:
			colorScalar = Scalar(255, 255, 255);
			break;
	}

	int histSize = 256;
	int hist_w = 512, hist_h = 400;
	int bin_w = cvRound( (double) hist_w/histSize );
	Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
	normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

	for(int i=1; i < histSize; i++){
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist[i-1]) ),
		Point( bin_w*(i), hist_h - cvRound(hist[i]) ),
		colorScalar, 2, 8, 0 );
	}

	imshow("Histogram", histImage);
	waitKey(0);

	destroyAllWindows();
}

Mat player::histogramEqualization(Mat frame)
{
	Mat histogram;
    float Range[] = {0, 256}; 
    const float* HistRange = {Range};
	int npixels = 256; 
	calcHist(&frame, 1, 0, Mat(), histogram, 1, &npixels, &HistRange, true, false);

	Mat cumsum = histogram.clone();
    for (int i = 1; i < 256; ++i) {
        cumsum.at<float>(i) += cumsum.at<float>(i - 1);
    }
    cumsum /= cumsum.at<float>(npixels - 1);
    cumsum *= 255;

	Mat equalizedImage(frame.size(), frame.type());
    for (int i = 0; i < frame.rows; ++i) {
        for (int j = 0; j < frame.cols; ++j) {
            equalizedImage.at<uchar>(i, j) = saturate_cast<uchar>(cumsum.at<float>(frame.at<uchar>(i, j)));
        }
	}

	return equalizedImage;
}

Mat player::grayHistEqualization(Mat frame)
{
	Mat grayscaleImage(frame.rows, frame.cols, CV_8UC1);
	grayscaleImage = toGrayscale(frame);
	Mat equalizedImage = histogramEqualization(grayscaleImage);
	return equalizedImage;
}

Mat player::colorHistEqualization(Mat frame)
{
	Mat ycrcb_img;
	cvtColor(frame,ycrcb_img, COLOR_BGR2YCrCb);

    vector<Mat> channels;
    split(ycrcb_img, channels);
    channels[0] = histogramEqualization(channels[0]);
    merge(channels, ycrcb_img);
    cvtColor(ycrcb_img, ycrcb_img, COLOR_YCrCb2BGR);

	return ycrcb_img;
}

Mat player::addWatermark(Mat frame, Mat watermark, float alpha, int x, int y)
{
	for (int row = 0; row < watermark.rows; row++){
		for (int col = 0; col < watermark.cols; col++){
			if (col + x < frame.rows && row + y < frame.cols){
				frame.at<Vec3b>(row + x, col + y) *= 1 - alpha;
				frame.at<Vec3b>(row + x, col + y) += watermark.at<Vec3b>(row, col) * alpha;
			}
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
	Mat result;
	filter2D(frame, result, -1, kernel);
	return result;
}

int main(int argc, char *argv[]){

	// Check if at least one argument is provided
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <video/image> [option]" << std::endl;
        return 1;
    }

	string media = argv[1];
	player p1(media);

	string delimiter = ".";
	string ext = media.substr(media.find(delimiter), media.size()-1);
	bool isImage = false;
	if (ext == ".jpg" || ext == ".png" || ext == ".jpeg")
		isImage = true;

	if(!p1.isOpen()){
		cout << "Could not open video.\n";
		return -1;
	}

	string option = "";
	if (argc >= 3) {
        option = argv[2];
    }

	if(option=="watermark"){
		if(argc >= 4){
			if(isImage)
				p1.displayImage(option, argv[3]);
			else
				p1.display(option, argv[3]);
		}
		else{
			std::cerr << "Usage for watermark: " << argv[0] << " <video> watermark <watermark>" << std::endl;
		}
	}
	else{
		if(isImage)
			p1.displayImage(option);
		else
			p1.display(option);
	}
	 
	return 0;
}