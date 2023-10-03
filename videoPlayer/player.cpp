#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
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
  void Display();
  bool isOpen();
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

void player::Display(){
  while(1){
    Mat frame,color_frame;
    this->cap->read(frame);
    for(int i = 0; i < frame.rows; i++){
      for(int j = 0; j < frame.cols; j++)
          cout << frame.row(i).col(j);
    }
    
    
    cvtColor(frame,color_frame,cv::COLOR_BGR2YUV);
    imshow(this->name,color_frame);
    char c=(char)waitKey(25);
      if(c==27)
        break;
  }
  cap->release();
  destroyAllWindows();
}
 


int main(){
  player p1("vd2.mp4");

  if(!p1.isOpen()){
    cout << "error open video";
    return -1;
  }

  p1.Display();
   
  return 0;
}