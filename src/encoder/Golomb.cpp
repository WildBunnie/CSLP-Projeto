#include "Golomb.h"
#include <cmath>
#include <math.h>
#include "BitStream.h"
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/plot.hpp>

using namespace std;

Golomb::Golomb(BitStream* bs,int n){
    m = n;
    bitStream = bs;
    bits = floor(log2(m));
}

void Golomb::encodeNumber(int number){
    int quotient = floor(number/m);
    int reminder = number % m;

    for (int i = 0; i < quotient; i++){
        bitStream->writeBit(1);
    }
    bitStream->writeBit(0);


    if (reminder < pow(2,bits+1)-m){
        bitStream->writeBits(reminder,bits);  
    }else{
        reminder = reminder + pow(2,bits+1)-m;
        bitStream->writeBits(reminder,bits+1);
    }
}

int Golomb::decodeNumber(){
    int quotient = 0;
    int n;
    int bit;

    while(bitStream->readBit() != 0){
        quotient++;
    }

    int reminder = bitStream->readBits(bits);

    if (reminder >= pow(2,bits+1)-m){
        reminder = reminder<<1;
        reminder |= bitStream->readBit();
        reminder = reminder - pow(2,bits+1)+m;
    }
    return quotient*m+reminder;
}


void Golomb::encodeMat(cv::Mat image){
    for(int i = 0; i < image.rows; i++){
		for(int j = 0; j < image.cols; j++){
			uint p = image.at<uchar>(i,j);

            encodeNumber(p);
        }
    }
}

cv::Mat Golomb::decodeMat(int cols,int rows){
    cv::Mat image(rows,cols,CV_8UC1);
    for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){

            uint p = decodeNumber();

			image.at<uchar>(i,j) = p;
        }
    }
    return image;
}
