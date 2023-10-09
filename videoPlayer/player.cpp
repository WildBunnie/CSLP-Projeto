#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
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
 


int main(){
  player p1("vd.mp4");

  if(!p1.isOpen()){
    cout << "error open video";
    return -1;
  }

  p1.display();
   
  return 0;
}