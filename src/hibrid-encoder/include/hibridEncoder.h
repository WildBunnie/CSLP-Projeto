#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include "Golomb.h"
#include "intraEncoder.h"

using namespace std;
using namespace cv;

class Block {
public:
    int x; 
    int y; 
    int blockSize;
    Mat frame;

    Block(int posX, int posY, int size, Mat sourceFrame): 
        x(posX), y(posY), blockSize(size), frame(sourceFrame) {}

};

class MotionVector {
public:
    int dx; 
    int dy; 
    int blockX; 
    int blockY;

    MotionVector(int x, int y, int blockPosX, int blockPosY)
        : dx(x), dy(y), blockX(blockPosX), blockY(blockPosY) {}

    MotionVector() : dx(0), dy(0), blockX(0), blockY(0) {}
};

int calculateDifference(Block block1, Block block2);

vector<Block> DivideFrameIntoBlocks(Mat frame, int blockSize);

float distance(int x1, int y1, int x2, int y2);

vector<Block> BlocksInSearchArea(vector<Block> blocks, int search_x, int search_y, int searchArea);

Mat PerformMotionCompensation(Mat previousFrame, vector<MotionVector> motionVectors, int blockSize);

Mat CalculateResidual(Mat currentFrame, Mat predictedFrame);

Mat DecodeInterFrame(Mat previousFrame,Mat residuals,int countMotionVectors, int blockSize,Golomb* gl);

void EncodeInterFrame(Mat currentFrame, Mat previousFrame, int blockSize, int searchArea,Golomb* gl);

void EncodeHybrid(string outputfile,string inputFile,int periodicity,int blockSize,int SearchArea);

void DecodeHybrid(string outputFile,string inputFile);