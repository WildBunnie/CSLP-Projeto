#include<iostream>
#include<fstream>
#include"BitStream.h"

using namespace std;

int main(int argc, char const *argv[])
{
    // criar class bit Stream e abrir ficheiro teste
    BitStream fileBit("files/teste");

    int arr[] = {0,1,0,0,0,1,0,0};
    // escrever um conjuto de bits
    fileBit.writeBits(&arr[0],8);
    //escrever 1 bit
    fileBit.writeBit(1);
    //fechar ficheiro 
    fileBit.close();
    //abrir ficheino novamente
    fileBit.open("files/teste");
    // ler um array de bits
    int* out = fileBit.readBits(8);

    for (size_t i = 0; i < 8; i++)
    {
        cout << arr[i];
    }
    cout << "\n";

    //ler um bit
    cout << "bit:" << fileBit.readBit()<< "\n";
    //fechar ficheiro
    fileBit.close();
        


    return 0;
    
}
