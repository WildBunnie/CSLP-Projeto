#include "hibridEncoder.h"

int main(int argc, char *argv[]){
    EncodeHybrid("encodedFile",argv[1],5,16,2,-1,-1,-1);
    DecodeHybrid("outputDecoded.y4m","encodedFile");
    return 0;
}