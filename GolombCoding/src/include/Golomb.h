#include <iostream>
#include <fstream>
#include <cmath>
#include <math.h>
#include <vector>

using namespace std;

class Golomb{
    private:
        int m;
        int bits;
    public:
    Golomb(int n){
        m = n;
        bits = floor(log2(m));
    }
    vector<int>* encodeNumber(int number){
        vector<int>* returnvect = new vector<int>();
        int quotient = floor(number/m);
        int reminder = number % m;

        for (int i = 0; i < quotient; i++){
            returnvect->push_back(1);
        }
        returnvect->push_back(0);


        if (reminder < pow(2,bits+1)-m){
            for (int i = 0; i < bits; i++){
                returnvect->push_back(reminder >> bits-i-1 & 1);
            }     
        }else{
            int nReminder = reminder + pow(2,bits+1)-m;
            for (int i = 0; i < bits+1; i++){
                returnvect->push_back(nReminder >> bits-i & 1);
            } 
        }

        return returnvect;
    }
    int decodeNumber(int* numbers,int size){
        int quotient = 0;
        do{
            quotient++;
        } while (numbers[quotient] != 0);

        int reminder = 0;
        for(int i = quotient+1;i < size; i++){
            reminder |= (numbers[i] << size-(quotient+1)-(i-quotient));
        }
        if (reminder >= pow(2,bits+1)-m){
            reminder = reminder - pow(2,bits+1)+m;
        }

        return quotient*m+reminder;
        
    }
};