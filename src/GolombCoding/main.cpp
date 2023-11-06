#include <iostream>
#include "Golomb.h"
#include "BitStream.h"

using namespace std;

int main(int argc, char const *argv[])
{   
    //create Golomb coder with division number
    BitStream bs("teste",'w');
    Golomb coder(&bs,5);
    coder.encodeNumber(5);
    bs.close();
    // encode number receive vector with bits(ints)

    BitStream bs2("teste",'r');
    Golomb decoder(&bs2,5);
    cout << decoder.decodeNumber();

    

    return 0;
}
