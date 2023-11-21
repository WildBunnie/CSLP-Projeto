#include "Block.hpp"

Block::Block(int size = 8, vector<vector<int>> pixels){
    this->blockSize = size;
    this->pixels = pixels;
}

int Block::getBlockSize(){
    return this->blockSize;
}

vector<vector<int>> Block::getPixels(){
    return this->pixels;
}

Block Block::operator-(Block const& block){
    vector<vector<int>> res;
    vector<vector<int>> pixelsBlock2 = block.pixels; 
    for(int i = 0; i < this->blockSize; i++){
		for(int j = 0; j < this->blockSize; j++){
            res[i][j] = pixels[i][j] - pixelsBlock2[i][j];
        }
    }  
    return Block(8, res); 
}