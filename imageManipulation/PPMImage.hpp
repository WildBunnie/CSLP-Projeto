#include <string>
#include <vector>

class RGB
{
public:
    char red;
    char green;
    char blue;
    RGB(char r, char g, char b);
};

class PPMImage
{
public:
    int width, height;
    std::string max_color;
    std::vector<std::vector<RGB>> pixels;

    PPMImage();

    bool loadImage(const std::string path);
    bool writeImage(const std::string path);
};