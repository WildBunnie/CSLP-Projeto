#include "hibridEncoder.h"

int main(int argc, char *argv[]){
    EncodeHybrid("encodedFile","output.mp4",2,16,2);
    DecodeHybrid("outputDecoded","encodedFile");
    return 0;
}