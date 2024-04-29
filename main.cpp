#include <iostream>
#include <vector>
#include <iterator>
#include <memory>
#include <regex>
#include <cstring>
#include "include/Jvm.h"


#include "include/ClassPrinter.h"
#include "include/Jvm.h"

#include "include/OpcodePrinter.h"

#define JVM_MODE 0
#define LEITOR_EXIBIDOR 1
#define HELP 2

void semnome(int argc, char* argv[], char* ClassFilePath,char* OutputFile){
    if(argc < 2)
        throw std::runtime_error("ta faltando o arquivo .class meu nobre, deve torcer pro ateltico...\n");

    ClassFilePath = argv[1];
    std::cout<<ClassFilePath<<"\n";
    std::regex ClassFIleTermination (".*\\.class$");

    if (!std::regex_search(ClassFilePath, ClassFIleTermination))
        throw std::runtime_error("Arquivo fornecido não é .class");

    //futuramente pro printer salvar o output em um arquivo
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 <= argc) {
            OutputFile = argv[++i];
            std::cout << OutputFile;
        }
    }
}

void help(){

}

int main(int argc, char* argv[]) {
    char* ClassFilePath;
    char* OutputFile = nullptr;

    // #define JVM_MODE 0
    //#define LEITOR_EXIBIDOR 1

    int mode = JVM_MODE;
    /*
    try {
        semnome();
     }
     catch(std::runtime_error& e){
        std::cerr << "Erro: " << e.what() << std::endl;
        return EXIT_FAILURE;
     }
*/

    switch (mode) {
        case JVM_MODE: {
            auto JvmInstance = Jvm("C:/Users/Raquel S/OneDrive/Documentos/Raquel/UNB/8semestre/SB/JVM/testes/Main.class");
            JvmInstance.Run();
            break;

        }
        case LEITOR_EXIBIDOR: {
            auto Printer = ClassPrinter("C:/Users/Raquel S/OneDrive/Documentos/Raquel/UNB/8semestre/SB/JVM/testes/Main.class");
            Printer.Run();
            break;
        }
        case HELP: {
            help();
            break;
        }
        default:{
            std::cerr <<"Modo de operacao invalido\n -m 0(default) para JVM ou -m 1 para leitor-exibidor\n ou -h para lista de opcoes"<< std::endl;
            return EXIT_FAILURE;
        }

    }

    // so pra testar cada bytecode
    /*
    auto teste = OpcodePrinter();
    u4 count = 6;
    u1 bytec [] = {0,0, 0x10, 23, 0x10, 43};
    std::cout<< teste.CodeToString(bytec, count);
    */
    return EXIT_SUCCESS;
}
