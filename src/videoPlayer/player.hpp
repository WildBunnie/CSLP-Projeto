#ifndef PLAYER_HPP
#define PLAYER_HPP

/**
 * @brief This class represents a video player/image displayer.
 * 
 * It's methods include the conversion between RGB and YUV and low level image processing operations.
 */
class player{
private:
    /**
     * @brief Video capture instance.
     * 
     * It's methods include the conversion between RGB and YUV and low level image processing operations.
    */
	VideoCapture* cap;
	string name;
public:
	player(String video);
	~player();
	void display(string filter, string watermark);
	bool isOpen();
	Mat  frameRgb2Yuv(Mat frame);
	Mat  frameYuv2Rgb(Mat frame);
	void displayImage();
	static void getHistogram(Mat frame, int color);
	static void getColorHistograms(Mat frame);
	static void printHistogram(vector<int> hist, int color);
	static Mat toGrayscale(Mat frame);
	static Mat addWatermark(Mat frame, Mat watermark, float alpha, int x, int y);
	static void histogramEqualization(Mat frame);
	static Mat gaussianBlur(Mat frame, int ksize, double sigma);
	static Mat boxFilter(Mat frame, Size ksize);
};

#endif