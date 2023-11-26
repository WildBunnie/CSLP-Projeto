#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>

using namespace std;
using namespace cv;

class Block {
public:
    int x; 
    int y; 
    int blockSize;
    Mat frame;

    Block(int posX, int posY, int size, Mat sourceFrame): 
        x(posX), y(posY), blockSize(size), frame(sourceFrame.clone()) {}

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

int calculateDifference(Block block1, Block block2) {
    
    Mat Mat1 = block1.frame;
    Mat Mat2 = block2.frame;

    double diff = 0.0;
    for (int x = 0; x < block1.blockSize ; x++) {
        for (int y = 0; y < block1.blockSize; y++) {
            if (x >= 0 && x < Mat1.cols && y >= 0 && y < Mat1.rows){
                diff += abs(Mat1.at<uchar>(block1.y + y, block1.x + x) - Mat2.at<uchar>(block2.y + y, block2.x + x));
            }
        }
    }

    return diff;
}

vector<Block> DivideFrameIntoBlocks(Mat frame, int blockSize){
    vector<Block> blocks;

    if (frame.empty()) {
        cout << "Empty frame." << endl;
        return blocks;
    }

    for (int y = 0; y < frame.rows; y += blockSize) {
        for (int x = 0; x < frame.cols; x += blockSize) {
            Block block(x, y, blockSize, frame);
            blocks.push_back(block);
        }
    }

    return blocks;
}

float distance(int x1, int y1, int x2, int y2) 
{ 
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0); 
} 

vector<Block> BlocksInSearchArea(vector<Block> blocks, int search_x, int search_y, int searchArea){
    vector<Block> result_blocks;

    for(Block block : blocks){
        float dist = distance(search_x, search_y, block.x, block.y);
        if(dist / block.blockSize <= searchArea){
            result_blocks.push_back(block);
        }
    }

    return result_blocks;
}

Mat PerformMotionCompensation(Mat previousFrame, vector<MotionVector> motionVectors, int blockSize) {
    Mat predictedFrame = previousFrame.clone(); // Create a copy of the previous frame

    for (const MotionVector& mv : motionVectors) {
        int blockX = mv.blockX;
        int blockY = mv.blockY;
        int dx = mv.dx;
        int dy = mv.dy;

        // Update the predicted frame using motion vectors
        for (int i = 0; i < blockSize; ++i) {
            for (int j = 0; j < blockSize; ++j) {
                int currentX = blockX + i + dx;
                int currentY = blockY + j + dy;
                int prevX = blockX + i;
                int prevY = blockY + j;

                if (prevX >= 0 && prevX < previousFrame.cols && prevY >= 0 && prevY < previousFrame.rows
                && currentX >= 0 && currentX < predictedFrame.cols && currentY >= 0 && currentY < predictedFrame.rows) {
                    predictedFrame.at<uchar>(currentY, currentX) = previousFrame.at<uchar>(prevY, prevX);
                }
            }
        }
    }

    return predictedFrame;
}


Mat CalculateResidual(Mat currentFrame, Mat predictedFrame) {
    Mat residualImage;

    // Check if the input frames have the same size and type
    if (currentFrame.size() != predictedFrame.size() || currentFrame.type() != predictedFrame.type()) {
        cout << "Input frames have different sizes or types." << endl;
        return residualImage;
    }

    // Calculate the residual image by subtracting the predicted frame from the current frame
    subtract(currentFrame, predictedFrame, residualImage);

    return residualImage;
}

void DecodeInterFrame(Mat previousFrame, Mat residuals, vector<MotionVector> motionVectors, int blockSize) {
    Mat predictedFrame = PerformMotionCompensation(previousFrame, motionVectors, blockSize);
    Mat reconstructedFrame = residuals + predictedFrame;
    imshow("idk", reconstructedFrame);
    waitKey(0);
}

void EncodeInterFrame(Mat currentFrame, Mat previousFrame, int blockSize, int searchArea) {
    vector<Block> blocks = DivideFrameIntoBlocks(currentFrame, blockSize);
    vector<Block> previousBlocks = DivideFrameIntoBlocks(previousFrame, blockSize);
    vector<MotionVector> motionVectors;

    for(Block block : blocks){

        int minDiff = INT_MAX;
        MotionVector MV;
        vector<Block> searchAreaBlocks = BlocksInSearchArea(previousBlocks, block.x, block.y, searchArea);
        
        for(Block previousBlock : searchAreaBlocks){
            int diff = calculateDifference(block, previousBlock);
            if(diff < minDiff){
                minDiff = diff;
                MV.blockX = previousBlock.x;
                MV.blockY = previousBlock.y;
                MV.dx = block.x - previousBlock.x;
                MV.dy = block.y - previousBlock.y;
            }
        }
        motionVectors.push_back(MV);
    }

    Mat predictedFrame = PerformMotionCompensation(previousFrame, motionVectors, blockSize);
    Mat residuals = CalculateResidual(currentFrame, predictedFrame);
    DecodeInterFrame(previousFrame, residuals, motionVectors, blockSize);
}


int main(int argc, char *argv[]){
	Mat frame,lastFrame;
    VideoCapture cap("/home/bunnie/.desktop/vd2.mp4");
    cap.read(lastFrame);
    cvtColor(lastFrame, lastFrame, COLOR_BGR2GRAY);
    while(1){
        cap.read(frame);
        cvtColor(frame, frame, COLOR_BGR2GRAY);
        EncodeInterFrame(frame.clone(), lastFrame.clone(), 32, 2);
        lastFrame = frame.clone();
    }
    return 0;
}