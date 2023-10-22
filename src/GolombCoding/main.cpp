#include <iostream>
#include "Golomb.h"
#include <vector>
using namespace std;

int main(int argc, char const *argv[])
{   
    //create Golomb coder with division number
    Golomb coder(10);
    vector<int>* res;
    // encode number receive vector with bits(ints)
    res = coder.encodeNumber(46);

    for (int i = 0; i < res->size(); i++)
    {
        cout << res->at(i);
    }
    cout << "\n";

    //creating array for decode
    int arr[res->size()];
    copy(res->begin(),res->end(),arr);

    //decode array of bits mandar numero de bits+1
    cout << coder.decodeNumber(arr) << "\n";
    //get se o ultimo bit foi usado ou não
    cout << "size remainder was bits+1: " << coder.getLastReminderSizePlus1() << "\n";

    

    return 0;
}
