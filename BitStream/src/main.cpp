#include<iostream>
#include<fstream>
#include"BitStream.h"

using namespace std;

int main(int argc, char const *argv[])
{
    BitStream* fileBit;

    fileBit = new BitStream("teste");

    int arr[] = {0,1,0,0,0,1,0,0};

    fileBit->writeBits(&arr[0],8);
    delete(fileBit);
    
    fileBit = new BitStream("teste");
    
    int* out = fileBit->readBits(8);

    for (size_t i = 0; i < 8; i++)
    {
        cout << arr[i] << "\n";
    }
        


    return 0;
    
}
