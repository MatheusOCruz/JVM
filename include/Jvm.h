//
// Created by matheus on 4/11/24.
//

#ifndef JVM_JVM_H
#define JVM_JVM_H

#include <stack>

#include "ClassLoader.h"
#include "JvmStructs.h"

class Jvm {
public:
     explicit Jvm(char* _main_file) : main_file(_main_file) {}
     void Init();
     void Run();


private:

    char* main_file;
    uint32_t pc = 0;

    std::stack<frame*> FrameStack;
};


#endif //JVM_JVM_H
