#include <vector>
#include <algorithm>
#include <iostream>
#include "BitStream.h"
#include "Golomb.h"
#include "intraEncoder.h"

using namespace std;

int jpeg_ls_predictor(int a, int b, int c){
    if (c >= max(a,b)){
        return min(a, b);
    }
    else if(c <= min(a,b)){
        return max(a, b);
    }
    else{
        return a + b - c;
    }
} 

