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

const int GOLOMB = 16;
const int PERIODICITY = 6;
const int BLOCKSIZE = 32;
const int SEARCHAREA = 3;
const int QUANTIZATION = -1;
const char MODE = 'e';
const string ENCODING = "hybrid";
const string OUTPUT = "outputfile";
const int T = 0;

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
    cout << "  -t:No default ->test mode 0-lossless 1-lossy" << endl;

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

void Testlossless(string fileToEncode){
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
    for(int i = 1; i < 6; i++){
        int searchArea = i;
        for(int j = 1; j < 7; j++){
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
    cout << "intra" << endl;
    remove(encodeFile.c_str());
    time(&start);
    EncodeHybrid(encodeFile,fileToEncode,0,bestBlockSize,bestSearchArea,QUANTIZATION,QUANTIZATION,QUANTIZATION,bestGolomb);
    time(&end);
    int size = filesize(encodeFile.c_str());
    cout << "Compression Ration: " << filesize(fileToEncode.c_str())/size << " intra encoding time taken: " << end-start << " seconds" << endl;

    cout << "bestGolomb:" << bestGolomb << endl;
    cout << "bestSearchArea:" << bestSearchArea << endl;
    cout << "bestBlockSize:" << bestBlockSize << endl;
}

void testLossy(string fileToEncode){
    time_t start,end;
    string encodeFile = "encodeFile";
    cout << "periodicity and quantization" << endl;
    int bestPeriodicity = 0;
    int bestQuantization = 0;
    int bestFilesize = filesize(fileToEncode.c_str());
    for(int i = 1; i < 5; i++){
        int periodicity = i*2;
        for(int j = 1; j < 8; j++){
            int quantization = pow(2,j);
            remove(encodeFile.c_str());
            time(&start);
            EncodeHybrid(encodeFile,fileToEncode,periodicity,BLOCKSIZE,SEARCHAREA,quantization,quantization,quantization,GOLOMB);
            time(&end);
            int size = filesize(encodeFile.c_str());
            if(bestFilesize > size){
                bestFilesize = size;
                bestPeriodicity = periodicity;
                bestQuantization = quantization;
            }
            cout << "Compression Ration: " << filesize(fileToEncode.c_str())/size << " periodicity: "<< periodicity <<" quantization: " <<  quantization <<  " time taken: " << end-start << " seconds" << endl;
        }
    }
    cout << "best quantization:" << bestQuantization << endl;
    cout << "best periodicity:" << bestPeriodicity << endl;
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
        if(quantization <= 0 || quantization > 255){
            cerr << "0 < quantization < 255" << endl;
            exit(0);
        }
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
        if(encoding == "lossy" && quantization == -1){
            cerr << "no quantization specified" << endl;
            printHelp();
            exit(1);
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
        char t = args.getValue(i).at(0);
        if(t == '0'){
            Testlossless(args.getValueNoOption());
        }else if(t=='1'){
            testLossy(args.getValueNoOption());
        }else{
            cerr << "t not valid" << endl;
            printHelp();
            exit(1);
        }
        exit(0);
    }
    if(mode == 'e'){
        EncodeHybrid(output,args.getValueNoOption(),periodicity,blockSize,searchArea,quantization,quantization,quantization,golomb);
    }else if(mode == 'd'){
        if(output == OUTPUT){
            output = "outputfile.y4m";
        }
        DecodeHybrid(output,args.getValueNoOption(),golomb);
    }else{
        cout << "invalid mode " << endl;
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

