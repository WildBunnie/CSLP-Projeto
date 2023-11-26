#include "intraEncoder.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]){
	cout << "encoding" << endl;
	encodeVideo("booga","vid.y4m");
	cout << "decoding" << endl;
	decodeVideo("woof","booga");
    return 0;
}