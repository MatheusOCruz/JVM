//
// Created by matheus on 4/9/24.
//

#ifndef JVM_CLASSPRINTER_H
#define JVM_CLASSPRINTER_H
#include <iostream>
#include <algorithm>

#include "ClassFileStructs.h"
#include "ClassLoader.h"

//TODO: depois tenho q jogar as funcao de print pra ca pra n virar bagunca o loader
class ClassPrinter {
public:
    explicit ClassPrinter(char* _main_file) : main_file(_main_file) {}
    void Run();

private:
    char* main_file;
};


#endif //JVM_CLASSPRINTER_H
