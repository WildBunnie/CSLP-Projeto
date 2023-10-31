#include<iostream>
#include<fstream>
#include"BitStream.h"

using namespace std;

int main(int argc, char const *argv[])
{
    // criar class bit Stream e abrir ficheiro teste
    BitStream fileBit("teste",'w');

    // escrever um conjuto de bits
    fileBit.writeBits(97,8);
    //escrever 1 bit
    //fileBit.writeBit(1);
    //fechar ficheiro 
    fileBit.close();
    //abrir ficheino novamente
    BitStream fileBit2("teste",'r');
    // ler um array de bits
    int out = fileBit2.readBits(8);

    cout << out;
    cout << "\n";

    //ler um bit
    //cout << "bit:" << fileBit.readBit()<< "\n";
    //fechar ficheiro
    fileBit.close();
        


    return 0;
    
}
