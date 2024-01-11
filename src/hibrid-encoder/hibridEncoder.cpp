#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>
#include "Golomb.h"
#include "intraEncoder.h"
#include "hibridEncoder.h"

using namespace std;
using namespace cv;

struct YUVFrame {
    vector<vector<char>> Y;
    vector<vector<char>> U;
    vector<vector<char>> V;
    int rows;
    int cols;

    YUVFrame clone() const {
        YUVFrame cloned;
        cloned.rows = rows;
        cloned.cols = cols;

        // Deep copy for each vector
        cloned.Y = Y;
        cloned.U = U;
        cloned.V = V;

        return cloned;
    }
};

struct VideoInfo {
    vector<YUVFrame> frames;
    string frame_rate;
    string interlacing;
    string aspect_ratio;
    string color_space;
    int cols;
    int rows;
};

bool parseYUV4MPEG2(string filename, VideoInfo& info) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return false;
    }

    string format;
    file >> format;
    if(format != "YUV4MPEG2"){
        cerr << "Invalid video format";
        return false;
    }

    string current = "";
    while(true){
        file >> current;
        if(current == "FRAME"){
            break;
        }

        char first = current[0];
        switch(first) {
            case 'W':
                current.erase(0,1);
                info.cols = stoi(current);
                break;
            case 'H':
                current.erase(0,1);
                info.rows = stoi(current);
                break;
            case 'F':
                info.frame_rate = current;
                break;
            case 'I':
                info.interlacing = current;
                break;
            case 'A':
                info.aspect_ratio = current;
                break;
            case 'C':
                info.color_space = current;
                break;
        }
    }

    if(info.color_space.find("444") == string::npos){
        cerr << "This program currently only supports videos with 4:4:4 color space";
        return false;
    }
    
    vector<YUVFrame> frames;
    while(true){
        file.get(); // remove newline

        vector<vector<char>> y_plane(info.rows, vector<char>(info.cols));
        vector<vector<char>> u_plane(info.rows, vector<char>(info.cols));
        vector<vector<char>> v_plane(info.rows, vector<char>(info.cols));

        for (int i = 0; i < info.rows; ++i) {
            file.read(y_plane[i].data(), info.cols);
        }

        for (int i = 0; i < info.rows; ++i) {
            file.read(u_plane[i].data(), info.cols);
        }

        for (int i = 0; i < info.rows; ++i) {
            file.read(v_plane[i].data(), info.cols);
        }

        YUVFrame frame;
        frame.Y = y_plane;
        frame.U = u_plane;
        frame.V = v_plane;
        frame.rows = info.rows;
        frame.cols = info.cols;
        frames.push_back(frame);
        
        // get rid of "FRAME"
        string text;
        file >> text;
        
        if(file.eof()){
            break;
        }
    }
    file.close();
    info.frames = frames;
    return true;
}

Block FindBestBlock(Block block, const vector<vector<char>>& previous, const vector<vector<char>>& current, int searchArea) {
    int blockSize = block.blockSize;
    int minDiff = INT_MAX;
    int bestX = 0, bestY = 0;

    for (int x = -searchArea; x <= searchArea; x++) {
        for (int y = -searchArea; y <= searchArea; y++) {
            int previous_col = block.x + x;
            int previous_row = block.y + y;

            if (previous_col < 0 || previous_col + blockSize > previous[0].size() ||
                previous_row < 0 || previous_row + blockSize > previous.size())
                continue;

            int diff = 0;

            // Use pointers to access elements for efficiency
            const char* currPtr = &current[block.y][block.x];
            const char* prevPtr = &previous[previous_row][previous_col];

            for (int row = 0; row < blockSize; row++) {
                for (int col = 0; col < blockSize; col++) {
                    diff += abs(*currPtr++ - *prevPtr++);
                }
                currPtr += (current[0].size() - blockSize);
                prevPtr += (previous[0].size() - blockSize);
            }

            if (diff < minDiff) {
                minDiff = diff;
                bestX = previous_col;
                bestY = previous_row;
            }
        }
    }

    return Block(bestX, bestY, blockSize);
}

void EncodeInterFrame(YUVFrame& currentFrame, YUVFrame& previousFrame, int blockSize, int searchArea, Golomb *gl)
{
    vector<vector<char>> currentPlanes[3] = {currentFrame.Y, currentFrame.U, currentFrame.V};
    vector<vector<char>> previousPlanes[3] = {previousFrame.Y, previousFrame.U, previousFrame.V};
    for (int plane = 0; plane <= 2; plane++)
    {
        for (int row = 0; row + blockSize <= currentFrame.rows; row += blockSize)
        {
            for (int col = 0; col + blockSize <= currentFrame.cols; col += blockSize)
            {
                Block block(col, row, blockSize);
                Block bestBlock = FindBestBlock(block, previousPlanes[plane], currentPlanes[plane], searchArea);
                int dx = bestBlock.x - block.x;
                int dy = bestBlock.y - block.y;

                gl->encodeNumber(dx);
                gl->encodeNumber(dy);

                int previousX = bestBlock.x;
                int previousY = bestBlock.y;
                int currentX = block.x;
                int currentY = block.y;

                for (int i = 0; i < blockSize; i++) {
                    for (int j = 0; j < blockSize; j++) {
                        int previous = previousPlanes[plane][previousY + i][previousX + j];
                        int current = currentPlanes[plane][currentY + i][currentX + j];
                        gl->encodeNumber(current - previous);
                    }
                }
            }
        }
    }
}

YUVFrame DecodeInterFrame(YUVFrame& previousFrame, int blockSize, Golomb *gl)
{
    YUVFrame reconstructedFrame;
    reconstructedFrame.rows = previousFrame.rows;
    reconstructedFrame.cols = previousFrame.cols;
    vector<vector<char>> previousPlanes[3] = {previousFrame.Y, previousFrame.U, previousFrame.V};
    for (int plane = 0; plane <= 2; plane++)
    {
        vector<vector<char>> currentPlane(previousFrame.rows, vector<char>(previousFrame.cols));
        for (int row = 0; row + blockSize <= previousFrame.rows; row += blockSize)
        {
            for (int col = 0; col + blockSize <= previousFrame.cols; col += blockSize)
            {
                int dx = gl->decodeNumber();
                int dy = gl->decodeNumber();
                for (int i = 0; i < blockSize; i++) {
                    for (int j = 0; j < blockSize; j++) {
                        // cout << endl << row << " " << col << endl;
                        // cout << dx << " " << dy << endl;
                        // cout << previousPlanes[plane].size() << " " << previousPlanes[plane][0].size() << endl;
                        // cout << i+dy << " " << j+dx << endl;
                        currentPlane[row+i][col+j] = previousPlanes[plane][row+i+dy][col+j+dx] + gl->decodeNumber();
                    }
                }
            }
        }
        switch(plane) {
            case 0:
                reconstructedFrame.Y = currentPlane;
                break;
            case 1:
                reconstructedFrame.U = currentPlane;
                break;
            case 2:
                reconstructedFrame.V = currentPlane;
                break;
        }
    }
    return reconstructedFrame;
}

void EncodeIntraFrame(YUVFrame frame, int blockSize, Golomb *gl){
    vector<vector<char>> planes[3] = {frame.Y, frame.U, frame.V};
    for (int plane = 0; plane <= 2; plane++)
    {
        for (int row = 0; row < frame.rows; row++)
        {
            for (int col = 0; col < frame.cols; col++)
            {
                int estimate = 0;

                if(row > 0 && col > 0){
                    int a = planes[plane][row][col-1];
                    int b = planes[plane][row-1][col];
                    int c = planes[plane][row-1][col-1];
                    estimate = jpeg_ls_predictor(a,b,c);
                }
                gl->encodeNumber(planes[plane][row][col] - estimate);
            }
        }
    }
}

YUVFrame DecodeIntraFrame(int rows, int cols, Golomb *gl){
	
	YUVFrame original;
    original.rows = rows;
    original.cols = cols;
    for (int plane = 0; plane <= 2; plane++){
        vector<vector<char>> originalPlane(rows, vector<char>(cols));
        vector<vector<char>> currentPlane(rows, vector<char>(cols));
        for (int row = 0; row < rows; row++){
            for (int col = 0; col < cols; col++){
                
                int estimate = 0;
                originalPlane[row][col] = gl->decodeNumber();

                if(row > 0 && col > 0){
                    int a = originalPlane[row][col-1];
                    int b = originalPlane[row-1][col];
                    int c = originalPlane[row-1][col-1];
                    estimate = jpeg_ls_predictor(a,b,c);
                }

                currentPlane[row][col] = originalPlane[row][col] + estimate;
            }
        }
        switch(plane) {
            case 0:
                original.Y = currentPlane;
                break;
            case 1:
                original.U = currentPlane;
                break;
            case 2:
                original.V = currentPlane;
                break;
        }
    }
	return original;
}

void EncodeHybrid(string outputfile, string inputFile, int periodicity, int blockSize, int SearchArea)
{   
    VideoInfo video_info;
    parseYUV4MPEG2(inputFile, video_info);
    
    BitStream bs(outputfile, 'w');
    Golomb gl(&bs, 50);

    gl.encodeNumber(video_info.frames.size()); // number of frames
    gl.encodeNumber(video_info.rows);          // frame rows
    gl.encodeNumber(video_info.cols);          // frame columns
    gl.encodeNumber(blockSize);                // block size

    int counter = 0;
    YUVFrame previousFrame, frame;
    for(int i = 0; i < video_info.frames.size(); i++){
        cout << "encoding frame " << i+1 << "/" << video_info.frames.size() << endl;
        frame = video_info.frames[i];

        if (counter == periodicity || i == 0){
            bs.writeBit(1);
            EncodeIntraFrame(frame, blockSize, &gl);
            counter = 0;
        }
        else {
            bs.writeBit(0);
            EncodeInterFrame(frame, previousFrame, blockSize, SearchArea, &gl);
            counter += 1;
        }
        previousFrame = frame;
    }
}

void DecodeHybrid(string outputFile, string inputFile)
{

    BitStream bs(inputFile, 'r');
    Golomb gl(&bs, 50);

    int frameCount = gl.decodeNumber();
    int rows = gl.decodeNumber();
    int cols = gl.decodeNumber();
    int blockSize = gl.decodeNumber();
    
    YUVFrame previousFrame, frame;
    for (int i = 0; i < frameCount; i++)
    {
        cout << "decoding frame " << i+1 << "/" << frameCount << endl;
        int isIntraEncoded = bs.readBit();
        if (isIntraEncoded == 1)
        {
            frame = DecodeIntraFrame(rows, cols, &gl);
        }
        else
        {
            frame = DecodeInterFrame(previousFrame, blockSize, &gl);
        }
        previousFrame = frame.clone();
    }
    // out.release();
}