#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include <iostream>
 
using namespace std;
using namespace cv;

class player{
private:
  VideoCapture* cap;
  String waterMark;
  String name;
public:
  player(String);
  ~player();
  void display();
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

void player::display(){
  while(1){
    Mat frame,color_frame;
    this->cap->read(frame);    
    
    cvtColor(frame,color_frame,cv::COLOR_BGR2YUV);
    imshow(this->name,color_frame);
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


int main(){
  player p1("deti.jpg");

  if(!p1.isOpen()){
    cout << "error open video";
    return -1;
  }

  p1.getColorHistograms();
   
  return 0;
}