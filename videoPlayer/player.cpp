#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include <iostream>
 
using namespace std;
using namespace cv;

class player{
private:
  VideoCapture* cap;
  string waterMark;
  string name;
public:
  player(string);
  ~player();
  void display();
  bool isOpen();
  Mat  frameRgb2Yuv(Mat frame);
  Mat  frameYuv2Rgb(Mat frame);
  void displayImage();
  bool isOpen();
  void getColorHistograms();
  void printHistogram(vector<int> hist, char color);
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

void player::getColorHistograms(){
  Mat frame;
  this->cap->read(frame);
  vector<int> histogramR(256,0);
  vector<int> histogramG(256,0);
  vector<int> histogramB(256,0);
  int r, g, b;

  for(int i = 0; i < frame.rows; i++){
      for(int j = 0; j < frame.cols; j++){
          b = frame.at<Vec3b>(i,j)[0]; //blue
          g = frame.at<Vec3b>(i,j)[1]; //green
          r = frame.at<Vec3b>(i,j)[2]; //red
          histogramB.insert(histogramB.begin() + b, ++histogramB[b]);
          histogramG.insert(histogramG.begin() + g, ++histogramG[g]);
          histogramG.insert(histogramG.begin() + r, ++histogramR[r]);
      }
  }

  printHistogram(histogramR, 'r');
  printHistogram(histogramG, 'g');
  printHistogram(histogramB, 'b');
}

void player::printHistogram(vector<int> hist, char color){
  Mat img = imread("background.jpg", 1);
  Point p1, p2;
  Scalar colorScalar;

  switch (color){
    case 'r':
      colorScalar = Scalar(0, 0, 255);
      break;
    case 'g':
      colorScalar = Scalar(0, 255, 0);
      break;
    case 'b':
      colorScalar = Scalar(255, 0, 0);
      break;
    default:
      colorScalar = Scalar(0, 255, 0);
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

static Mat addWatermark(Mat frame, Mat watermark, int x, int y, float alpha)
{
  alpha = alpha / 100;
  if (watermark.empty())
  {
    cout << "no watermark" << endl;
  }
  int i = 0;
  int j = 0;
  for (int row = 0; row < watermark.rows; row++)
  {
    for (int col = 0; col < watermark.cols; col++)
    {
      if (row + y < frame.rows && col + x < frame.cols)
      {
        frame.at<Vec3b>(row + y, col + x) = frame.at<Vec3b>(row + y, col + x) * (1 - alpha) + watermark.at<Vec3b>(row, col) * alpha;
      }
    }
  }
  return frame;
}

static Mat toGrayscale(Mat frame)
{
  for (int row = 0; row < frame.rows; row++)
  {
    for (int col = 0; col < frame.cols; col++)
    {
      Vec3b pixel = frame.at<Vec3b>(row, col);
      uchar result = pixel[0] * 0.11 + pixel[1] * 0.59 + pixel[2] * 0.30;
      frame.at<Vec3b>(row, col) = Vec3b(result, result, result);
    }
  }
  return frame;
}

int main(){
  player p1("deti.jpg");

  if(!p1.isOpen()){
    cout << "error open video";
    return -1;
  }

  p1.getColorHistograms();
   
  return 0;
}