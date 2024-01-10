#include "Golomb.h"
#include <cmath>
#include <math.h>
#include "BitStream.h"
#include <string>

using namespace std;

Golomb::Golomb(BitStream* bs,int n){
    m = n;
    bitStream = bs;
    bits = floor(log2(m));
}

void Golomb::encodeNumber(int number){
    if(number > 0){
        number = number * 2;
    }
    else if(number < 0){
        number = (number*-2)-1;
    }
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

    while(bitStream->readBit() != 0){
        quotient++;
    }

    int reminder = bitStream->readBits(bits);

    if (reminder >= pow(2,bits+1)-m){
        reminder = reminder<<1;
        reminder |= bitStream->readBit();
        reminder = reminder - pow(2,bits+1)+m;
    }
    
    int number = quotient*m+reminder;

    if(number != 0){
        if(number%2 == 0){
            number /= 2;
        }
        else{
            number = (number+1)/-2;
        }
    }
    return number;
}

void Golomb::encodeString(string str){
    int size = str.length();
    encodeNumber(size);
    for(int i=0; i < size; i++){
        encodeNumber(str[i]);
    }
}

string Golomb::decodeString(){
    int size = decodeNumber();
    string ret = "";
    for(int i=0; i < size; i++){
        int num = decodeNumber();
        ret.append(string(1,char(num)));
    }
    return ret;
}
