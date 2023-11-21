#include <vector>
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include "Frame.hpp"
#include "Block.hpp"

using namespace std;

Frame::Frame(Mat frame, int block_size, int seach_area){
    this->frame = frame;
    int block_rows = frame.rows/block_size;
    int block_cols = frame.cols/block_size;
    /* for (int i=0; i<frame.rows; i++){
        for (int j=0; j<frame.cols; j++){
            if()
        }
    } */
    /*
    std::vector<std::vector<int>> submatrix;
    for (int i=0; i < block_size-1; i++) {
        submatrix.push_back(frame[i].begin(), std::next(frame[i].begin(), block_size));
    } */
}

Block Frame::getBlock(int x, int y){
    return this->blocks[x][y];
}