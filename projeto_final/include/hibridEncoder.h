#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include "Golomb.h"
#include "intraEncoder.h"

using namespace std;
using namespace cv;

/**
 * \brief A class to represent a fixed-size block of pixels from a frame.
 *
 */
class Block {
public:
    /**
     * \brief The x coordinate of the block.
     *
     */
    int x;
    /**
     * \brief The y coordinate of the block.
     *
     */ 
    int y; 
    /**
     * \brief The size of the block in the x axis.
     *
     */
    int blockSizeX;
        /**
     * \brief The size of the block in the y axis.
     *
     */
    int blockSizeY;

    /**
     * \brief Constructor.
     *
     * \param posX The x coordinate of the block.
     * \param posY The y coordinate of the block.
     * \param sizeX The size of the block in the x axis.
     * \param sizY The size of the block in the y axis.
     * \param sourceFrame The frame from which the block was extracted.
     */
    Block(int posX, int posY, int sizeX, int sizeY): 
        x(posX), y(posY), blockSizeX(sizeX), blockSizeY(sizeY) {}

    friend ostream& operator<<(std::ostream& os, const Block& block) {
        os << "Block: (x=" << block.x << ", y=" << block.y << ")";
        return os;
    }
};

/**
 * \brief A class to represent a motion vector.
 *
 */
class MotionVector {
public:
    /**
     * \brief Horizontal component of the motion vector.
     *
     */
    int dx; 
    /**
     * \brief Vertical component of the motion vector.
     *
     */
    int dy;
    /**
     * \brief The x coordinate of the block.
     *
     */ 
    int blockX; 
    /**
     * \brief The y coordinate of the block.
     *
     */
    int blockY;

    /**
     * \brief Constructor.
     *
     * \param x Horizontal component of the motion vector.
     * \param y Vertical component of the motion vector.
     * \param blockPosX The x coordinate of the block.
     * \param blockPosY The y coordinate of the block.
     */
    MotionVector(int x, int y, int blockPosX, int blockPosY)
        : dx(x), dy(y), blockX(blockPosX), blockY(blockPosY) {}

    /**
     * \brief Default constructor.
     * 
     */
    MotionVector() : dx(0), dy(0), blockX(0), blockY(0) {}
};

/**
 * \brief Calculates the difference between two blocks.
 *
 * \param block1 The first block.
 * \param block2 The second block.
 * \return The difference between the two blocks.
 */
int calculateDifference(Block block1, Block block2);

/**
 * \brief Divides a frame into blocks.
 *
 * \param frame The frame to be divided.
 * \param blockSize The size of the blocks.
 * \return A vector containing the blocks.
 */
vector<Block> DivideFrameIntoBlocks(Mat frame, int blockSize);

/**
 * \brief Calculates the distance between two points.
 *
 * \param x1 The x coordinate of the first point.
 * \param y1 The y coordinate of the first point.
 * \param x2 The x coordinate of the second point.
 * \param y2 The y coordinate of the second point.
 * \return The distance between the two points.
 */
float distance(int x1, int y1, int x2, int y2);

/**
 * \brief Returns the blocks that are in the search area.
 *
 * \param blocks The blocks to be searched.
 * \param search_x The x coordinate of the search area.
 * \param search_y The y coordinate of the search area.
 * \param searchArea The size of the search area.
 * \return A vector containing the blocks that are in the search area.
 */
vector<Block> BlocksInSearchArea(vector<Block> blocks, int search_x, int search_y, int searchArea);

/**
 * \brief Performs motion compensation.
 *
 * \param previousFrame The previous frame.
 * \param motionVectors The motion vectors.
 * \param blockSize The size of each block.
 * \return The predicted frame.
 */
Mat PerformMotionCompensation(Mat previousFrame, vector<MotionVector> motionVectors, int blockSize);

/**
 * \brief Calculates the residual between two frames.
 *
 * \param currentFrame The current frame.
 * \param predictedFrame The predicted frame.
 * \return The residual.
 */
Mat CalculateResidual(Mat currentFrame, Mat predictedFrame);

/**
 * \brief Inter frame encoder.
 *
 * \param previousFrame The previous frame.
 * \param residuals The calculated residuals.
 * \param countMotionVectors The value of the actual count of the motion vectors.
 * \param blockSize The size of each block.
 * \param gl The Golomb object used to encode the motion vectors.
 */
Mat DecodeInterFrame(Mat previousFrame, int blockSize, Golomb* gl);
//Mat DecodeInterFrame(Mat previousFrame, int blockSize, Golomb* gl, vector<int> dxs, vector<int> dys, vector<Mat> allResiduals);

/**
 * \brief Inter frame decoder.
 *
 * \param currentFrame The current frame.
 * \param previousFrame The previous frame.
 * \param blockSize The size of each block.
 * \param searchArea The size of the search area.
 * \param gl The Golomb object used to decode the motion vectors.
 */
void EncodeInterFrame(Mat currentFrame, Mat previousFrame, int blockSize, int searchArea,Golomb* gl);

/**
 * \brief Hybrid (inter + intra) encoder.
 *
 * \param outputfile The name of the output file.
 * \param inputFile The name of the input file.
 * \param periodicity The periodicity of the key frames.
 * \param blockSize The size of each block.
 * \param searchArea The size of the search area.
 */
void EncodeHybrid(string outputfile,string inputFile,int periodicity,int blockSize,int SearchArea);

/**
 * \brief Hybrid (inter + intra) frame decoder.
 *
 * \param outputFile The name of the output file.
 * \param inputFile The name of the input file.
 */
void DecodeHybrid(string outputFile,string inputFile);