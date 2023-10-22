#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include <iostream>
#include <numeric>

using namespace std;
using namespace cv;

/**
 * @brief This class represents a video player/image displayer.
 * 
 * It's methods include the conversion between RGB and YUV and low level image processing operations.
 * The decision to make all the processing methods static was made to allow one object to manipulate more
 * than one media. Since the videos and images are never saved, it can be done and avoids the creation of
 * a large number of objects.
 */
class Player{
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
    /**
     * @brief Displays a histogram. Only used internally by other functions.
     * 
     * Drawing implementation based on example code found on OpenCV documentation, at: https://docs.opencv.org/3.4/d8/dbc/tutorial_histogram_calculation.html
     * 
     * \param hist The histogram to be displayed.
     * \param color The color (R,G or B) of the drawing of the histogram.
    */
    static void printHistogram(vector<int> hist, int color);
    /**
     * @brief Calculates a histogram. Only used internally by other functions.
     *      
     * \param frame The image to calculate from.
     * \param color The channel to be considered and the color (R,G or B) of the drawing of the histogram.
    */
	static void getHistogram(Mat frame, int color);
    /**
     * @brief Applies histogram equalization. Only used internally by other functions.
     * 
     * \param frame The frame to be converted.
     * \return The converted frame.
    */
	static Mat histogramEqualization(Mat frame);
public:
	/**
     * @brief Constructor.
     * 
     * \param media A string representing the path to the media file.
    */
	Player(String media);
	/**
     * @brief Destructor.
     * 
    */
	~Player();
	/**
     * @brief Displays the processed videos.
     * 
     * \param function A static function from the class player that corresponds to the filter
     * choosen by the user. The function is called with the Mat created from the media passed in 
     * the constructor as its argument.
    */	
	void display(function<Mat(Mat)>);
	/**
     * @brief Checks if video instance is open or not.
     * 
     * \return true if it's open, false if it's not.
    */
	bool isOpen();
	/**
     * @brief Converts a frame/image from the RGB scale to the YUV scale.
     * 
     * \param frame The frame to be converted.
     * \return The converted frame.
    */
	static Mat frameRgb2Yuv(Mat frame);
	/**
     * @brief Converts a frame/image from the YUV scale to the RGB scale.
     * 
     * \param frame The frame to be converted.
     * \return The converted frame.
    */
	static Mat frameYuv2Rgb(Mat frame);
	/**
     * @brief Displays the three histograms (for Red, Green and Blue) from an image.
     * 
     * Uses private auxiliar functionns to calculate and display the histograms.
     * 
     * \param frame The frame to be converted.
     * \return The converted frame.
    */
	static void getColorHistograms(Mat frame);
	/**
     * @brief Converts a frame/image to grayscale.
     * 
     * \param frame The frame to be converted.
     * \return The converted frame.
    */
	static Mat toGrayscale(Mat frame);
	/**
     * @brief Converts a frame/image to a black and white image based on a specified threshold.
     * 
     * \param frame The frame to be converted.
     * \param threshold The value of the threshold.
     * \return The converted frame.
    */
	static Mat threshold(Mat frame, double threshold);
    /**
     * @brief Adds a watermark to a frame/image.
     * 
     * \param frame The frame to which the watermark will be added.
     * \param watermark The image used as the watermark.
     * \param alpha The transparency of the watermark (value ranges from 0 to 1).
     * \param x X-axis value of the watermark position.
     * \param y Y-axis value of the watermark position.
     * \return The converted frame.
    */
	static Mat addWatermark(Mat frame, Mat watermark, float alpha, int x, int y);
    /**
     * @brief Applies histogram equalization to a gray frame/image.
     * 
     * Uses a auxiliar private function.
     * 
     * \param frame The frame to be converted.
     * \return The converted frame.
    */
	static Mat grayHistEqualization(Mat frame);
    /**
     * @brief Applies histogram equalization to a colored frame/image.
     * 
     * Uses a auxiliar private function.
     * 
     * \param frame The frame to be converted.
     * \return The converted frame.
    */
	static Mat colorHistEqualization(Mat frame);
    /**
     * @brief Applies gaussian blur to a gray frame/image.
     * 
     * \param frame The frame to be converted.
     * \return The converted frame.
    */
	static Mat gaussianBlur(Mat frame, int ksize, double sigma);
    /**
     * @brief Applies blur to a gray frame/image using a box filter.
     * 
     * \param frame The frame to be converted.
     * \return The converted frame.
    */
	static Mat boxFilter(Mat frame, Size ksize);
};

#endif