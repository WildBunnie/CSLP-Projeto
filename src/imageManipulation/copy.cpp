#include "PPMImage.hpp"
#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    if(argc < 3){
        cout << "Invalid number of arguments. (./copy [image path] [output path])" << endl;
        return -1;
    }

    PPMImage image;
    image.loadImage(argv[1]);
    image.writeImage(argv[2]);

    return 0;
}