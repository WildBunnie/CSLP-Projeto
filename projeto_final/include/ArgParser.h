#include <iostream>
#include <vector>
#include <string>


class ArgParser{
    private:
        std::vector<std::string> args;
    public:
        ArgParser(std::vector<std::string>);
        int hasOpetion(std::string);
        std::string getValue(int);
        std::string getValueNoOption();
};