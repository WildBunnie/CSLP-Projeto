#include <string>
#include <vector>

/**
 * \brief Represents a RGB pixel.
 *
 */
class RGB
{
public:
    /**
     * \brief The red channel.
     *
     */
    char red;
    /**
     * \brief The green channel.
     *
     */
    char green;
    /**
     * \brief The blue channel.
     *
     */
    char blue;
    /**
     * \brief Constructor.
     *
     */
    RGB(char r, char g, char b);
};

/**
 * \brief Represents a image in PPM format.
 *
 */
class PPMImage
{
public:
    /**
     * \brief The width of the image.
     *
     */
    int width;
    /**
     * \brief The height of the image.
     *
     */
    int height;
    /**
     * \brief The maximum value possible for a RGB channel to have.
     *
     */
    int max_color;
    /**
     * \brief The pixel matrix.
     *
     */
    std::vector<std::vector<RGB>> pixels;

    /**
     * \brief Constructor.
     *
     */
    PPMImage();

    /**
     * \brief Loads PPM image.
     * 
     * \param path The path for the image file.
     * \return true if the loading was successful, false if it an error occurred.
     */
    bool loadImage(const std::string path);
    /**
     * \brief Writes PPM image.
     * 
     * \param path The path for the image file.
     * \return true if the writing was successful, false if it an error occurred.
     */
    bool writeImage(const std::string path);
};