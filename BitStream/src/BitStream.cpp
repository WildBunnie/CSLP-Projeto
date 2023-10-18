#include "BitStream.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
BitStream::BitStream(string name){
    std::ofstream output(name);
    output.close();
    file.open(name,ios_base::out|ios_base::in);
};
void BitStream::close(){
    bool need_write = 0;
    if(currentBitW <= 8 && currentBitW != 0)
        file.write(reinterpret_cast<char*>(&bitBufferW),1);
    file.close();
}
void BitStream::open(string name){
    file.open(name);
}
BitStream::~BitStream(){
    BitStream::close();
    
}
bool BitStream::hasNext(){
    if (currentBitR < 8)
        return true;
    if (file.peek() != EOF)
        return true;
    return false;
}
void BitStream::writeBit(int bit){
    if (bit)
        bitBufferW |= (1<<7-currentBitW);


    currentBitW++;
    if (currentBitW == 8)
    {  
        file.write(reinterpret_cast<char*>(&bitBufferW),1);
        currentBitW = 0;
        bitBufferW = 0;
    }
}

void BitStream::writeBits(int* bits,int size){
    for (size_t i = 0; i < size; i++){
        writeBit(bits[i]);
    }
    
}

int BitStream::readBit(){
    char returnChar;
    if (currentBitR >= 8 || currentBitR == 0){
        currentBitR = 0; 
        file.read(&bitBufferR,sizeof(char));
    }
    returnChar = bitBufferR>>(7-currentBitR) & 1;
    currentBitR++;
    return returnChar;
}

int* BitStream::readBits(int n){
    int* arr = (int*)malloc(sizeof(int)*n);
    for (size_t i = 0; i < n; i++)
    {
        arr[i] = readBit();
    }

    return arr;
    
}