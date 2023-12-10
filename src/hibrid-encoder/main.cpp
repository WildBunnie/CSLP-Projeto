#include "hibridEncoder.h"

int main(int argc, char *argv[]){
    
    
    EncodeHybrid("encodedFile",argv[1],2,16,5);
    DecodeHybrid("outputDecoded","encodedFile");
    return 0;
}