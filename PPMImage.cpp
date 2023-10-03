#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class RGB
{
public:
    char red;
    char green;
    char blue;
    RGB(char r, char g, char b)
    {
        red = r;
        green = g;
        blue = b;
    }
};

class PPMImage
{
public:
    int width, height;
    string max_color;
    vector<vector<RGB>> pixels;

    PPMImage()
    {
        height = 0;
        width = 0;
        max_color = 255;
    }

    bool loadImage(string path)
    {
        ifstream file(path, ios::binary);
        if (!file.is_open())
        {
            cerr << "unable to open file" << endl;
            return false;
        }

        string magic;
        file >> magic;
        if (magic != "P6")
        {
            cerr << "wrong format" << endl;
            return false;
        }

        file >> width;
        file >> height;
        file >> max_color;
        // remove newline
        file.get();

        pixels.resize(height, vector<RGB>(width, RGB(0, 0, 0)));

        for (int row = 0; row < height; row++)
        {
            for (int column = 0; column < width; column++)
            {
                unsigned char r, g, b;
                file.read(reinterpret_cast<char *>(&r), 1);
                file.read(reinterpret_cast<char *>(&g), 1);
                file.read(reinterpret_cast<char *>(&b), 1);
                pixels[row][column] = RGB(r, g, b);
            }
        }

        return true;
    }

    bool writeImage(string path)
    {
        ofstream file(path, ios::binary);
        if (!file.is_open())
        {
            cerr << "unable to open file" << endl;
            return false;
        }

        file << "P6" << endl;
        file << width << " " << height << endl;
        file << max_color << endl;

        for (int row = 0; row < height; row++)
        {
            for (int column = 0; column < width; column++)
            {
                file.write(reinterpret_cast<char *>(&pixels[row][column].red), 1);
                file.write(reinterpret_cast<char *>(&pixels[row][column].green), 1);
                file.write(reinterpret_cast<char *>(&pixels[row][column].blue), 1);
            }
        }
        return true;
    }
};

// testing purposes
// int main(int argc, char **argv)
// {
//     PPMImage image;
//     image.loadImage("stop_1.ppm");
//     image.writeImage("test.ppm");

//     return 0;
// }