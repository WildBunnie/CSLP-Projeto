#include "ArgParser.h"
#include <vector>
#include <string>
#include "hibridEncoder.h"
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <cmath>

using namespace std;

const int GOLOMB = 50;
const int PERIODICITY = 2;
const int BLOCKSIZE = 32;
const int SEARCHAREA = 5;
const int QUANTIZATION = -1;
const char MODE = 'e';
const string ENCODING = "hybrid";
const string OUTPUT = "outputfile";

void printHelp(){
    cout << "How to use INPUTFILE [OPTIONS]" << endl;
    cout << "  -option:defaultValue -> description" << endl;
    cout << "  -h    ->prints this message" <<endl;
    cout << "  -g:"<< GOLOMB << " ->golomb value" <<endl;
    cout << "  -b:"<< BLOCKSIZE << " ->block size" <<endl;
    cout << "  -s:"<< SEARCHAREA << "  ->search area" <<endl;
    cout << "  -p:"<< PERIODICITY << "  ->periodicity" <<endl;
    cout << "  -q:"<< QUANTIZATION << "  -> quantization" << endl;
    cout << "  -m:"<< MODE << "  ->mode encode(e), decode(d)" <<endl;
    cout << "  -e:"<< ENCODING <<  " ->encoding type [hybrid,intra,lossy]" <<endl;
    cout << "  -o:"<< OUTPUT <<  " ->output file" <<endl;
    cout << "  -t    ->test mode" << endl;

}

double filesize(const char* filename)
{
    ifstream file(filename, std::ios::binary);

    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        streampos fileSize = file.tellg();
        double fileSizeMB = static_cast<double>(fileSize);// / 1048576.0;
        file.close();
        return fileSizeMB;
    } else {
        cerr << "Error opening file." << std::endl;
    }
    return -1;
}

void TestGolomb(string fileToEncode){
    time_t start,end;
    string encodeFile = "encodeFile";
    int periodicity = PERIODICITY;
    int blockSize = BLOCKSIZE;
    int searchArea = SEARCHAREA;
    int bestGolomb = 0;
    int bestFilesize = filesize(fileToEncode.c_str());
    cout << "changing Golomb" << endl;
    for(int i = 3; i < 8; i++){
        int golomb = pow(2,i);
        remove(encodeFile.c_str());
        time(&start);
        EncodeHybrid(encodeFile,fileToEncode,periodicity,blockSize,searchArea,QUANTIZATION,QUANTIZATION,QUANTIZATION,golomb);
        time(&end);
        int size = filesize(encodeFile.c_str());
        if(bestFilesize > size){
            bestFilesize = size;
            bestGolomb = golomb;
        }
        cout << "Compression Ration: " << filesize(fileToEncode.c_str())/size << " golombValue: "<< golomb << " time taken: " << end-start << " seconds" << endl;
    }
    cout << "search Area" << endl;
    int bestSearchArea = 0;
    int bestBlockSize = 0;
    bestFilesize = filesize(fileToEncode.c_str());
    for(int i = 1; i < 5; i++){
        int searchArea = 5*i;
        for(int i = 1; i < 7; i++){
            int blockSize = pow(2,i);
            remove(encodeFile.c_str());
            time(&start);
            EncodeHybrid(encodeFile,fileToEncode,periodicity,blockSize,searchArea,QUANTIZATION,QUANTIZATION,QUANTIZATION,bestGolomb);
            time(&end);
            int size = filesize(encodeFile.c_str());
            if(bestFilesize > size){
                bestFilesize = size;
                bestSearchArea = searchArea;
                bestBlockSize = blockSize;
            }
            cout << "Compression Ration: " << filesize(fileToEncode.c_str())/size << "  searchArea: "<< searchArea  << "MB blockSize: "<< blockSize << " time taken: " << end-start << " seconds" << endl;
        }
    }
    cout << "periodicity" << endl;
    int bestPeriodicity = 0;
    bestFilesize = filesize(fileToEncode.c_str());
    for(int i = 1; i < 6; i++){
        int periodicity = i*2;
        remove(encodeFile.c_str());
        time(&start);
        EncodeHybrid(encodeFile,fileToEncode,periodicity,bestBlockSize,bestSearchArea,QUANTIZATION,QUANTIZATION,QUANTIZATION,bestGolomb);
        time(&end);
        int size = filesize(encodeFile.c_str());
        if(bestFilesize > size){
            bestFilesize = size;
            bestPeriodicity = periodicity;
        }
        cout << "Compression Ration: " << filesize(fileToEncode.c_str())/size << " periodicity: "<< periodicity << " time taken: " << end-start << " seconds" << endl;
    }
    cout << "quantization" << endl;
    int bestQuantization = 0;
    bestFilesize = filesize(fileToEncode.c_str());
    for(int i = 1; i < 8; i++){
        int quantization = pow(2,i);
        remove(encodeFile.c_str());
        time(&start);
        EncodeHybrid(encodeFile,fileToEncode,bestPeriodicity,bestBlockSize,bestSearchArea,quantization,quantization,quantization,bestGolomb);
        time(&end);
        int size = filesize(encodeFile.c_str());
        if(bestFilesize > size){
            bestFilesize = size;
            bestQuantization = i;
        }
        cout << "Compression Ration: " << filesize(fileToEncode.c_str())/size << " quantization: "<< i << " time taken: " << end-start << " seconds" << endl;
    }
    cout << "intra" << endl;
    remove(encodeFile.c_str());
    time(&start);
    EncodeHybrid(encodeFile,fileToEncode,0,bestBlockSize,bestSearchArea,bestQuantization,bestQuantization,bestQuantization,bestGolomb);
    time(&end);
    int size = filesize(encodeFile.c_str());
    cout << "Compression Ration: " << filesize(fileToEncode.c_str())/size << " intra encoding time taken: " << end-start << " seconds" << endl;

    cout << "bestGolomb:" << bestGolomb << endl;
    cout << "bestSearchArea:" << bestSearchArea << endl;
    cout << "bestPeriodicity:" << bestPeriodicity << endl;
    cout << "bestBlockSize:" << bestBlockSize << endl;
    cout << "bestQuantization:" << bestQuantization << endl;
}


int main(int argc, char *argv[]){
    if(argc < 2){
        cerr << "Missing arguments" << endl;
        printHelp();
        exit(1);
    }
    std::vector<std::string> woof(argv+1 ,argc + argv);
    ArgParser args(woof);
    int golomb = GOLOMB;
    int periodicity = PERIODICITY;
    int blockSize = BLOCKSIZE;
    int searchArea = SEARCHAREA;
    int quantization = QUANTIZATION;
    char mode = MODE;
    string encoding = ENCODING;
    string output = OUTPUT;

    int i = args.hasOpetion("-h");
    if (i != -1){
        printHelp();
        exit(0);
    }

    i = args.hasOpetion("-g");
    if (i > -1){
        golomb = stoi(args.getValue(i));
    }
    i = args.hasOpetion("-b");
    if (i > -1){
        blockSize = stoi(args.getValue(i));
    }
    i = args.hasOpetion("-s");
    if (i > -1){
        searchArea = stoi(args.getValue(i));
    }
    i = args.hasOpetion("-p");
    if (i > -1){
        periodicity = stoi(args.getValue(i));
    }
    i = args.hasOpetion("-q");
    if (i > -1){
        quantization = stoi(args.getValue(i));
    }
    i = args.hasOpetion("-m");
    if (i > -1){
        mode = args.getValue(i).at(0);
        if (mode != 'e' && mode != 'd'){
            cerr << "mode not acceptable" << endl;
            printHelp();
            exit(1);
        }
    }
    i = args.hasOpetion("-e");
    if (i > -1){
        encoding = args.getValue(i);
        vector<string> option = {"hybrid","intra","lossy"};
        if (!(find(option.begin(),option.end(),encoding) != option.end())){
            cerr << "encoding not acceptable" << endl;
            printHelp();
            exit(1);
        }
        if(encoding != "lossy"){
            quantization = -1;
        }
        if(encoding == "intra"){
            periodicity = 0;
        }
    }
    i = args.hasOpetion("-o");
    if (i > -1){
        output = args.getValue(i);
    }

    if(args.getValueNoOption() == ""){
        cerr << "no input file" << endl;
        printHelp();
        exit(1);
    }

    i = args.hasOpetion("-t");
    if (i > -1){
        TestGolomb(args.getValueNoOption());
        exit(0);
    }

    if(mode == 'e'){
        EncodeHybrid(output,args.getValueNoOption(),periodicity,blockSize,searchArea,quantization,quantization,quantization,golomb);
    }else{
        if(output == OUTPUT){
            output = "outputfile.y4m";
        }
        DecodeHybrid(output,args.getValueNoOption(),golomb);
    }

    std::cout << args.getValueNoOption() << std::endl;

    //print de opcoes
    cout << "golomb: " << golomb << endl;
    cout << "block Size: " << blockSize << endl;
    cout << "search Area: " << searchArea << endl;
    cout << "periodicity: " << periodicity << endl;
    cout << "quantization: " << quantization << endl;
    cout << "mode: " <<mode<< endl;
    cout << "encoding: " << encoding << endl;

    
    return 0;
}

