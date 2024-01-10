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

    vector<vector<char>>& getPlane(int plane) {
        switch (plane) {
            case 0:
                return Y;
            case 1:
                return U;
            case 2:
                return V;
            default:
                // Handle invalid plane index, or you can throw an exception
                throw std::out_of_range("Invalid plane index");
        }
    }
};

struct VideoInfo {
    vector<YUVFrame> frames;
    string frame_rate;
    string interlacing;
    string aspect_ratio;
    string color_space;
    string header;
    int cols;
    int rows;
};

bool parseYUV4MPEG2(string filename, VideoInfo& info) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return false;
    }

    string headerLine;
    getline(file >> ws, info.header);
    file.seekg(0);

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
    int blockSizeX = block.blockSizeX;
    int blockSizeY = block.blockSizeY;
    int minDiff = INT_MAX;
    int bestX = 0, bestY = 0;

    for (int x = -searchArea; x <= searchArea; x++) {
        for (int y = -searchArea; y <= searchArea; y++) {
            int previous_col = block.x + x;
            int previous_row = block.y + y;

            if (previous_col < 0 || previous_col + blockSizeX > previous[0].size() ||
                previous_row < 0 || previous_row + blockSizeY > previous.size())
                continue;

            int diff = 0;

            const char* currPtr = &current[block.y][block.x];
            const char* prevPtr = &previous[previous_row][previous_col];

            for (int row = 0; row < blockSizeY; row++) {
                for (int col = 0; col < blockSizeX; col++) {
                    diff += abs(*currPtr++ - *prevPtr++);
                }
                currPtr += (current[0].size() - blockSizeX);
                prevPtr += (previous[0].size() - blockSizeX);
            }

            if (diff < minDiff) {
                minDiff = diff;
                bestX = previous_col;
                bestY = previous_row;
            }
        }
    }

    return Block(bestX, bestY, blockSizeX, blockSizeY);
}

void EncodeInterFrame(YUVFrame& currentFrame, YUVFrame& previousFrame, int blockSize, int searchArea, Golomb *gl)
{
    for (int plane = 0; plane <= 2; plane++)
    {
        for (int row = 0; row < currentFrame.rows; row += blockSize)
        {
            for (int col = 0; col < currentFrame.cols; col += blockSize)
            {
                int blockSizeX = min(blockSize, currentFrame.cols - col);
                int blockSizeY = min(blockSize, currentFrame.rows - row);

                Block block(col, row, blockSizeX, blockSizeY);
                Block bestBlock = FindBestBlock(block, previousFrame.getPlane(plane), currentFrame.getPlane(plane), searchArea);

                int dx = bestBlock.x - block.x;
                int dy = bestBlock.y - block.y;

                gl->encodeNumber(dx);
                gl->encodeNumber(dy);

                int previousX = bestBlock.x;
                int previousY = bestBlock.y;
                int currentX = block.x;
                int currentY = block.y;

                for (int i = 0; i < blockSizeY; i++) {
                    for (int j = 0; j < blockSizeX; j++) {
                        int previous = previousFrame.getPlane(plane)[previousY + i][previousX + j];
                        int current = currentFrame.getPlane(plane)[currentY + i][currentX + j];
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
    for (int plane = 0; plane <= 2; plane++)
    {
        vector<vector<char>> currentPlane(previousFrame.rows, vector<char>(previousFrame.cols));
        for (int row = 0; row < previousFrame.rows; row += blockSize)
        {
            for (int col = 0; col < previousFrame.cols; col += blockSize)
            {
                int blockSizeX = min(blockSize, previousFrame.cols - col);
                int blockSizeY = min(blockSize, previousFrame.rows - row);

                int dx = gl->decodeNumber();
                int dy = gl->decodeNumber();
                for (int i = 0; i < blockSizeY; i++) {
                    for (int j = 0; j < blockSizeX; j++) {
                        currentPlane[row+i][col+j] = previousFrame.getPlane(plane)[row+i+dy][col+j+dx] + gl->decodeNumber();
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
    for (int plane = 0; plane <= 2; plane++)
    {
        for (int row = 0; row < frame.rows; row++)
        {
            for (int col = 0; col < frame.cols; col++)
            {
                int estimate = 0;

                if(row > 0 && col > 0){
                    int a = frame.getPlane(plane)[row][col-1];
                    int b = frame.getPlane(plane)[row-1][col];
                    int c = frame.getPlane(plane)[row-1][col-1];
                    estimate = jpeg_ls_predictor(a,b,c);
                }
                // residuals = original - estimate
                gl->encodeNumber(frame.getPlane(plane)[row][col] - estimate);
            }
        }
    }
}

YUVFrame DecodeIntraFrame(int rows, int cols, Golomb *gl){
	
	YUVFrame original;
    original.rows = rows;
    original.cols = cols;
    for (int plane = 0; plane <= 2; plane++){
        vector<vector<char>> residualsPlane(rows, vector<char>(cols));
        vector<vector<char>> originalPlane(rows, vector<char>(cols));
        for (int row = 0; row < rows; row++){
            for (int col = 0; col < cols; col++){
                
                int estimate = 0;
                residualsPlane[row][col] = gl->decodeNumber();

                if(row > 0 && col > 0){
                    int a = originalPlane[row][col-1];
                    int b = originalPlane[row-1][col];
                    int c = originalPlane[row-1][col-1];
                    estimate = jpeg_ls_predictor(a,b,c);
                }
                // original = residuals + estimate
                originalPlane[row][col] = residualsPlane[row][col] + estimate;
            }
        }
        switch(plane) {
            case 0:
                original.Y = originalPlane;
                break;
            case 1:
                original.U = originalPlane;
                break;
            case 2:
                original.V = originalPlane;
                break;
        }
    }
	return original;
}

void WriteYUVFrameToFile(string fileName, YUVFrame& frame){
    ofstream outputFile(fileName, ios::binary | ios::app);
    outputFile << "FRAME" << endl;
    for (int plane = 0; plane <= 2; plane++)
    {
        for (const auto& row : frame.getPlane(plane)) {
            outputFile.write(row.data(), row.size());
        }
    }
    return;
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

    gl.encodeString(video_info.header);

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
    
    ofstream fileStream(outputFile);

    if (fileStream.is_open()) {
        fileStream << gl.decodeString() << endl;
        fileStream.close();
    } else {
        cerr << "Unable to open the file." << endl;
        return;
    }

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
        WriteYUVFrameToFile(outputFile, frame);
        previousFrame = frame;
    }
}