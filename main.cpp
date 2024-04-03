#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <memory>
#include "ClassLoader.h"


int main(int argc, char* argv[]) {
    auto TesteLoader = new ClassLoader();
    TesteLoader->LoadClass("/home/matheus/CLionProjects/JVM/Main.class");
    return 0;
}
