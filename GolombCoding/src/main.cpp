#include <iostream>
#include "include/Golomb.h"
#include <vector>
using namespace std;

int main(int argc, char const *argv[])
{
    Golomb coder(10);
    vector<int>* res;
    res = coder.encodeNumber(42);

    for (int i = 0; i < res->size(); i++)
    {
        cout << res->at(i);
    }
    cout << "\n";

    int arr[res->size()];
    copy(res->begin(),res->end(),arr);
    cout << coder.decodeNumber(arr,res->size()) << "\n";
    

    return 0;
}
