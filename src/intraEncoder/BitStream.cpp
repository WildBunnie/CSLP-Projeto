#include "BitStream.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

BitStream::BitStream(string name,char write){
    if(write == 'w'){
        file.open(name,ios_base::out);
    }else if(write == 'r'){
        file.open(name,ios_base::in);
    }else{
        throw "Error invalid argumanet";
    }
};

void BitStream::close(){
    if(currentBit <= 8 && currentBit != 0)
        file.write(reinterpret_cast<char*>(&bitBuffer),1);
    file.close();
}

BitStream::~BitStream(){
    BitStream::close();  
}

bool BitStream::hasNext(){
    if (currentBit < 8 and bitBuffer != 0){
        return true;
    }
    if (file.peek() != EOF){
        return true;
    }
    return false;
}

void BitStream::writeBit(int bit){
    if (bit)
        bitBuffer |= (1<<7-currentBit);


    currentBit++;
    if (currentBit == 8)
    {  
        file.write(reinterpret_cast<char*>(&bitBuffer),1);
        currentBit = 0;
        bitBuffer = 0;
    }
}

void BitStream::writeBits(int bits,int size){
    for (size_t i = 0; i < size; i++){
        writeBit(bits & 1<<size-1-i);
    }
}

int BitStream::readBit(){
    char returnChar;
    if (currentBit >= 8 || currentBit == 0){
        currentBit = 0; 
        file.read(&bitBuffer,sizeof(char));
    }
    returnChar = bitBuffer>>(7-currentBit) & 1;
    currentBit++;
    return returnChar;
}

int BitStream::readBits(int n){
    int val = 0;
    int bit;
    for (size_t i = 0; i < n; i++)
    {
        bit = readBit();
        val |= bit<<n-1-i;
    }

    return val;
    
}