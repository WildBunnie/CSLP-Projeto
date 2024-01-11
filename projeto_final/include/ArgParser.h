#include <iostream>
#include <vector>
#include <string>

/**
 * \brief A class to parse command line arguments.
 *
 */
class ArgParser{
    /**
     * \brief The vector of arguments.
     *
     */
    private:
        std::vector<std::string> args;
    public:
        /**
         * \brief Constructor.
         * 
         * \param args The vector of arguments.
         */
        ArgParser(std::vector<std::string>);
        /**
         * \brief Checks if the vector of arguments has a given option.
         * 
         * \param str The option to be checked.
         * \return The index of the option if it exists, -1 if it does not.
         */
        int hasOpetion(std::string);
        /**
         * \brief Gets the value of an option.
         * 
         * \param i The index of the option.
         * \return The value of the option.
         */
        std::string getValue(int);
        /**
         * \brief Gets the first value that is not an option.
         * 
         * \return The value.
         */
        std::string getValueNoOption();
};