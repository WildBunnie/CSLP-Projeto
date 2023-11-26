#include <algorithm>

// a == left
// b == up
// c == diagonal
int non_linear_predictor(int a, int b, int c){
    if (c >= std::max(a,b)){
        return std::min(a, b);
    }
    else if(c <= std::min(a,b)){
        return std::max(a, b);
    }
    else{
        return a + b - c;
    }
}