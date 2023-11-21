#include <vector>

using namespace std;

class Block{
    private:
        int blockSize;
        vector<vector<int>> pixels;
    public:
        Block(int size, vector<vector<int>> pixels);
        int getBlockSize();
        vector<vector<int>> getPixels();
        Block operator-(Block const& block);
};