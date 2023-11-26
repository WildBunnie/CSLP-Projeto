#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include "Golomb.h"
#include "intraEncoder.h"
#include "hibridEncoder.h"

using namespace std;
using namespace cv;


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

    if (currentFrame.size() != predictedFrame.size() || currentFrame.type() != predictedFrame.type()) {
        cout << "Input frames have different sizes or types." << endl;
        return residualImage;
    }
    subtract(currentFrame, predictedFrame, residualImage);

    return residualImage;
}

Mat DecodeInterFrame(Mat previousFrame,Mat residuals,int countMotionVectors, int blockSize,Golomb* gl) {
    vector<MotionVector> motionVectors;
    for(int i = 0; i<countMotionVectors; i++){
        int dx = gl->decodeNumber();
        int dy = gl->decodeNumber();
        int blockx = gl->decodeNumber();
        int blocky = gl->decodeNumber();
        motionVectors.push_back(MotionVector(dx,dy,blockx,blocky));
    }

    Mat predictedFrame = PerformMotionCompensation(previousFrame, motionVectors, blockSize);
    Mat reconstructedFrame = residuals + predictedFrame;
    return reconstructedFrame;
}

void EncodeInterFrame(Mat currentFrame, Mat previousFrame, int blockSize, int searchArea,Golomb* gl){
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
    
    gl->encodeMat(residuals);
    gl->encodeNumber(motionVectors.size());
    for(MotionVector v:motionVectors){
        gl->encodeNumber(v.dx);
        gl->encodeNumber(v.dy);
        gl->encodeNumber(v.blockX);
        gl->encodeNumber(v.blockY);
    }
    
    // DecodeInterFrame(previousFrame, residuals, motionVectors, blockSize);
}

void EncodeHybrid(string outputfile,string inputFile,int periodicity,int blockSize,int SearchArea){
    Mat frame,residuals,lastFrame;
    int counter = periodicity;
    BitStream bs(outputfile,'w');
    Golomb gl(&bs,50);

    VideoCapture cap(inputFile);
    cap >> frame;
    gl.encodeNumber(cap.get(CAP_PROP_FRAME_COUNT));
    gl.encodeNumber(blockSize);
	gl.encodeNumber(frame.cols);
	gl.encodeNumber(frame.rows);
	gl.encodeNumber(cap.get(CAP_PROP_FPS));

    while(!frame.empty()){
        cvtColor(frame,frame,COLOR_BGR2GRAY);
        if(counter < periodicity){
            EncodeInterFrame(frame,lastFrame,blockSize,SearchArea,&gl);
        }else{
            residuals = getResidualsJPEG_LS(frame);
	        gl.encodeMat(residuals);
            gl.encodeNumber(0);
            counter = 0;
        }
        lastFrame = frame.clone();
        counter++;
        cap >> frame;
    }
    cap.release();
}

void DecodeHybrid(string outputFile,string inputFile){
    Mat frame,lastFrame;
    string format = ".mp4";
    BitStream bs(inputFile,'r');
    Golomb gl(&bs,50);

    int fourcc = VideoWriter::fourcc('m','p','4','v');
	int framesCount = gl.decodeNumber();
    int blockSize = gl.decodeNumber();
	int cols = gl.decodeNumber();
	int row = gl.decodeNumber();
	int fps = gl.decodeNumber();
	Size size(cols,row);
	VideoWriter out(outputFile+format,fourcc,fps,size,false);

    for(int i = 0; i < framesCount; i++){
        frame = gl.decodeMat(cols,row);
        int countMotionVectors = gl.decodeNumber();
        if (countMotionVectors == 0){
		    frame = getOriginalJPEG_LS(frame);
        }else{
            frame = DecodeInterFrame(lastFrame,frame,countMotionVectors,blockSize,&gl);
        }
        lastFrame = frame.clone();
		out << frame;
    }

    out.release();

}