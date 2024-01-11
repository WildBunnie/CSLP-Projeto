#include "Golomb.h"
#include "intraEncoder.h"
#include <vector>


using namespace std;


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

    /**
     * \brief Overload of the operator << to print a block.
     * 
     * \param os The ostream.
     * \param block The block to be printed. 
     * \return The ostream with the block information.
     */
    friend ostream& operator<<(std::ostream& os, const Block& block) {
        os << "Block: (x=" << block.x << ", y=" << block.y << ")";
        return os;
    }
};

/**
 * \brief A structuer to represent a frame.
 *
 */
struct YUVFrame {
    /**
     * \brief The Y plane.
     *
     */
    vector<vector<char>> Y;
    /**
     * \brief The U plane.
     *
     */
    vector<vector<char>> U;
    /**
     * \brief The V plane.
     *
     */
    vector<vector<char>> V;
    /**
     * \brief The number of rows of the frame.
     *
     */
    int rows;
    /**
     * \brief The number of columns of the frame.
     *
     */
    int cols;

    /**
     * \brief function to get a plane of the frame.
     * 
     * \param plane The index of the plane to be returned.
     * \return The plane.
     */
    vector<vector<char>>& getPlane(int plane) {
        switch (plane) {
            case 0:
                return Y;
            case 1:
                return U;
            case 2:
                return V;
            default:
                throw std::out_of_range("Invalid plane index");
        }
    }
};

/**
 * \brief A structure to represent the video information.
 *
 */
struct VideoInfo {
    /**
     * \brief The frames of the video.
     *
     */
    vector<YUVFrame> frames;
    /**
     * \brief The frame rate of the video.
     *
     */
    string frame_rate;
    /**
     * \brief The interlacing of the video.
     *
     */
    string interlacing;
    /**
     * \brief The aspect ratio of the video.
     *
     */
    string aspect_ratio;
    /**
     * \brief The color space of the video.
     *
     */
    string color_space;
    /**
     * \brief The header of the video.
     *
     */
    string header;
    /**
     * \brief The number of columns of the video.
     *
     */
    int cols;
    /**
     * \brief The number of rows of the video.
     *
     */
    int rows;
};

/**
 * \brief A parser for videos in YUV4MPEG2 format.
 * 
 * \param filename The name of the file to be parsed.
 * \param info The object where the video information will be stored.
 * \return true if the file was parsed successfully, false otherwise.
 *
 */
bool parseYUV4MPEG2(string filename, VideoInfo& info);

/**
 * \brief Search algoritthm to find best block within the defined search area.
 * 
 * \param block The block to be compared to.
 * \param previous The previous frame.
 * \param current The current frame.
 * \param searchArea The size of the search area.
 */ 
Block FindBestBlock(Block block, const vector<vector<char>>& previous, const vector<vector<char>>& current, int searchArea);

/**
 * \brief Inter frame decoder.
 *
 * \param currentFrame The current frame.
 * \param previousFrame The previous frame.
 * \param blockSize The size of each block.
 * \param searchArea The size of the search area.
 * \param gl The Golomb object used to decode the motion vectors.
 * \param quantizationY The quantization value for the Y plane.
 * \param quantizationU The quantization value for the U plane.
 * \param quantizationV The quantization value for the V plane.
 */
void EncodeInterFrame(YUVFrame& currentFrame, YUVFrame& previousFrame, int blockSize, int searchArea, Golomb *gl, int quantizationY, int quantizationU, int quantizationV);

/**
 * \brief Inter frame encoder.
 *
 * \param previousFrame The previous frame.
 * \param residuals The calculated residuals.
 * \param countMotionVectors The value of the actual count of the motion vectors.
 * \param blockSize The size of each block.
 * \param gl The Golomb object used to encode the motion vectors.
 */
YUVFrame DecodeInterFrame(YUVFrame& previousFrame, int blockSize, Golomb *gl);

/**
 * \brief Intra frame encoder.
 *
 * \param frame The frame to be encoded.
 * \param blockSize The size of each block.
 * \param gl The Golomb object used to encode the residuals.
 */
void EncodeIntraFrame(YUVFrame frame, int blockSize, Golomb *gl);

/**
 * \brief Intra frame decoder.
 *
 * \param rows The number of rows of the frame.
 * \param cols The number of columns of the frame.
 * \param gl The Golomb object used to decode the residuals.
 * \return The decoded frame.
 */
YUVFrame DecodeIntraFrame(int rows, int cols, Golomb *gl);

/**
 * \brief Writes a YUV frame to a file.
 * 
 * \param fileName The name of the file.
 * \param frame The frame to be written.
 */
void WriteYUVFrameToFile(string fileName, YUVFrame& frame);

/**
 * \brief Hybrid (inter + intra) encoder.
 *
 * \param outputfile The name of the output file.
 * \param inputFile The name of the input file.
 * \param periodicity The periodicity of the key frames.
 * \param blockSize The size of each block.
 * \param searchArea The size of the search area.
 * \param quantizationY The quantization value for the Y plane.
 * \param quantizationU The quantization value for the U plane.
 * \param quantizationV The quantization value for the V plane.
 * \param golomb The golomb value.
 */
void EncodeHybrid(string outputfile, string inputFile, int periodicity, int blockSize, int SearchArea, int quantizationY, int quantizationU, int quantizationV,int golomb);

/**
 * \brief Hybrid (inter + intra) frame decoder.
 *
 * \param outputFile The name of the output file.
 * \param inputFile The name of the input file.
 * \param golomb The golomb value.
 */
void DecodeHybrid(string outputFile,string inputFile, int golomb);