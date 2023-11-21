#include <vector>
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include "Block.hpp"

using namespace std;

class Frame{
    private:
        Mat frame;
        vector<vector<Block>> blocks;
        int block_size;
        int seach_area;
    public:
        Frame(Mat frame, int block_size, int seach_area);
        Block getBlock(int x, int y);
        Block searchBestBlock(Block bref);
};