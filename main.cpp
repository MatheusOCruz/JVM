// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <iostream>
#include <vector>
#include <iterator>
#include <memory>
#include <regex>
#include <cstring>
#include "include/ClassLoader.h"



int main(int argc, char* argv[]) {
    /*
    char* ClassFilePath;
    char* OutputFile = nullptr;

    if(argc < 2){
        std::cerr<<"ta faltando o arquivo .class meu nobre, deve torcer pro ateltico...\n";
        return EXIT_FAILURE;
    }
    ClassFilePath = argv[1];
    std::cout<<ClassFilePath<<"\n";
    std::regex ClassFIleTermination (".*\\.class$");

    if (!std::regex_search(ClassFilePath, ClassFIleTermination)){
        std::cerr<<"arquivo fornecido nao e .class";
        return EXIT_FAILURE;
    }
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i],"-o") == 0 && i+1 <=argc){
            OutputFile = argv[++i];
            std::cout<<OutputFile;
        }
    }

    */
    auto TesteLoader = new ClassLoader();

    TesteLoader->LoadMain("/home/matheus/CLionProjects/JVM/testes/Main.class");
    return EXIT_SUCCESS;
}
