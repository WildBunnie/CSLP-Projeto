#include "ArgParser.h"
#include <string>
#include <vector>
#include <iostream>


ArgParser::ArgParser(std::vector<std::string> args){
    this->args = args;
}


int ArgParser::hasOpetion(std::string str){
    for(int i = 0; i < args.size(); i++ ){
        if(args[i].compare(str) == 0){
            return i;
        }
    }
    return -1;
}

std::string  ArgParser::getValue(int i){
    return args[i+1];
}

std::string ArgParser::getValueNoOption(){
    if (args.size() == 0){
        return "";
    }
    if(args[0].at(0) != '-'){
        return args[0];
    }

    for(int i = 1; i < args.size(); i++ ){
        if(args[i-1].at(0) != '-'){
            return args[i];
        }
    }
    return "";
}