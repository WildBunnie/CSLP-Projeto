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
     * Used for when the media passed as argument is a video.
    */
	VideoCapture* cap;
	/**
     * @brief Media file name.
     * 
     * Used as media title and for reading the file when the media passed as argument is an image.
    */
	string name;
public:
	/**
     * @brief Constructor.
     * 
     * \param media A string representing the path to the media file.
    */
	player(String media);
	/**
     * @brief Destructor.
     * 
    */
	~player();
	/**
     * @brief Displays the processed videos.
     * 
     * \param filter The filter passed as a command-line argument by the user.
	 * \param watermark The watermark passed as argument when the filter in question is "watermark".
    */	
	void display(string filter, string watermark);
	/**
     * @brief Checks if video instance is open or not.
     * 
    */
	bool isOpen();
	/**
     * @brief 
     * 
    */
	Mat frameRgb2Yuv(Mat frame);
	Mat frameYuv2Rgb(Mat frame);
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