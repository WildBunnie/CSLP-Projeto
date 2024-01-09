#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include "Golomb.h"
#include "intraEncoder.h"
#include "hibridEncoder.h"

using namespace std;
using namespace cv;

Block FindBestBlock(Block block, Mat previousFrame, int searchArea)
{

    int blockSize = block.blockSize;
    int minDiff = INT_MAX;
    int bestX, bestY;
    for (int x = -searchArea; x <= searchArea; x++)
    {
        for (int y = -searchArea; y <= searchArea; y++)
        {

            int col = block.x + x;
            int row = block.y + y;

            if (col < 0 || col + blockSize > previousFrame.cols || row < 0 || row + blockSize > previousFrame.rows)
                continue;

            Rect r(col, row, blockSize, blockSize);
            Mat testMat = previousFrame(r);

            Mat diffMat;
            absdiff(block.frame, testMat, diffMat);
            int diff = sum(diffMat)[0];

            if (diff < minDiff)
            {
                minDiff = diff;
                bestX = col;
                bestY = row;
            }
        }
    }

    Rect r(bestX, bestY, blockSize, blockSize);
    Block result(bestX, bestY, blockSize, previousFrame(r));
    return result;
}

void EncodeInterFrame(Mat currentFrame, Mat previousFrame, int blockSize, int searchArea, Golomb *gl)
{
    for (int row = 0; row + blockSize <= currentFrame.rows; row += blockSize)
    {
        for (int col = 0; col + blockSize <= currentFrame.cols; col += blockSize)
        {
            Rect r(col, row, blockSize, blockSize);
            Block block(col, row, blockSize, currentFrame(r));
            Block bestBlock = FindBestBlock(block, previousFrame, searchArea);
            Mat residuals;
            subtract(block.frame, bestBlock.frame, residuals, noArray(), CV_32S);
            int dx = bestBlock.x - block.x;
            int dy = bestBlock.y - block.y;
            gl->encodeNumber(dx);
            gl->encodeNumber(dy);    
            for(int i = 0; i < blockSize; i++){
                for(int j = 0; j < blockSize; j++){
                    gl->encodeNumber(residuals.at<int>(i,j));
                }
            }
        }
    }
}

Mat DecodeInterFrame(Mat previousFrame, int blockSize, Golomb *gl)
{
    Mat reconstructedFrame(previousFrame.rows, previousFrame.cols, CV_8UC1, Scalar(0));
    // int index = 0;
    for (int row = 0; row + blockSize <= previousFrame.rows; row += blockSize)
    {
        for (int col = 0; col + blockSize <= previousFrame.cols; col += blockSize)
        {
            int dx = gl->decodeNumber();
            int dy = gl->decodeNumber();
            Mat residuals(blockSize, blockSize, CV_32S, Scalar(0));
            for(int i = 0; i < blockSize; i++){
                for(int j = 0; j < blockSize; j++){
                    residuals.at<int>(i,j) = gl->decodeNumber();
                }
            }
            Rect previousRect(col + dx, row + dy, blockSize, blockSize);
            Rect currentRect(col, row, blockSize, blockSize);
            Mat temp;
            add(previousFrame(previousRect), residuals, reconstructedFrame(currentRect), noArray(), CV_8UC1);
        }
    }
    return reconstructedFrame;
}

void EncodeHybrid(string outputfile, string inputFile, int periodicity, int blockSize, int SearchArea)
{
    Mat frame, previousFrame;
    int counter = 0;

    BitStream bs(outputfile, 'w');
    Golomb gl(&bs, 50);

    VideoCapture cap(inputFile);
    gl.encodeNumber(cap.get(CAP_PROP_FRAME_COUNT));  // number of frames
    gl.encodeNumber(cap.get(CAP_PROP_FRAME_HEIGHT)); // frame rows
    gl.encodeNumber(cap.get(CAP_PROP_FRAME_WIDTH));  // frame columns
    gl.encodeNumber(blockSize);                      // block size
	gl.encodeNumber(cap.get(CAP_PROP_FPS));         // fps

    int i = 1;
    while (true)
    {
        if (!cap.read(frame))
        {
            break;
        }

        cout << "encoding frame " << i++ << "/" << cap.get(CAP_PROP_FRAME_COUNT) << endl;
        cout.flush();

        cvtColor(frame, frame, COLOR_BGR2GRAY);

        if (counter == periodicity || previousFrame.empty())
        {
            bs.writeBit(1);
            gl.encodeMat(getResidualsJPEG_LS(frame));
            counter = 0;
        }
        else
        {
            bs.writeBit(0);
            EncodeInterFrame(frame, previousFrame, blockSize, SearchArea, &gl);
            counter += 1;
        }

        previousFrame = frame.clone();
    }

    cap.release();
}

void DecodeHybrid(string outputFile, string inputFile)
{
    Mat frame, lastFrame;
    string format = ".mp4";

    BitStream bs(inputFile, 'r');
    Golomb gl(&bs, 50);

    int frameCount = gl.decodeNumber();
    int rows = gl.decodeNumber();
    int cols = gl.decodeNumber();
    int blockSize = gl.decodeNumber();
	int fps = gl.decodeNumber();

    Size size(cols,rows);
    VideoWriter out(outputFile+format,VideoWriter::fourcc('m','p','4','v'),fps,size,false);
    
    for (int i = 0; i < frameCount; i++)
    {
        cout << "decoding frame " << i+1 << "/" << frameCount << endl;
        int isIntraEncoded = bs.readBit();
        if (isIntraEncoded == 1)
        {
            frame = gl.decodeMat(cols, rows);
            frame = getOriginalJPEG_LS(frame);
        }
        else
        {
            frame = DecodeInterFrame(lastFrame, blockSize, &gl);
        }
        lastFrame = frame.clone();
        out << frame;
        imshow("idk", frame);
        // waitKey(0);
        char c=(char)waitKey(25);
			if(c==27)
				break;
    }
    out.release();
}