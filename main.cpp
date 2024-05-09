#include <iostream>
#include <vector>
#include <iterator>
#include <memory>
#include <regex>
#include <cstring>
#include <assert.h>
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

void help(const char * program_name) {
	std::cerr << "Como usar:" << std::endl;
	std::cerr << "    " << program_name << " <command> <class_file_path>" << std::endl;
	std::cerr << "Comandos:" << std::endl;
	std::cerr << "    leitor_exibidor" << std::endl;
	std::cerr << "    jvm" << std::endl;
	std::cerr << "Exemplos:" << std::endl;
	std::cerr << "    " << program_name << " leitor_exibidor ./testes/Main.class" << std::endl;
	std::cerr << "    " << program_name << " jvm ./testes/Main.class" << std::endl;
}

const char* shift_args(int & argc, char **argv[]) {
	assert(argc > 0);
	argc--;
	return *(*argv)++;
}

int main(int argc, char* argv[]) {
	const auto program_name = shift_args(argc, &argv);
	if (!argc) {
		std::cerr <<"Error: É necessário um comando"<< std::endl;
		help(program_name);
		return 1;
	}
	const auto command = shift_args(argc, &argv);

	if (strcmp(command, "leitor_exibidor") == 0) {
		if (!argc) {
			std::cerr <<"Error: É necessário um arquivo .class"<< std::endl;
			help(program_name);
			return 1;
		}
		const auto class_file_path = shift_args(argc, &argv);
		auto printer = ClassPrinter(class_file_path);
		printer.Run();
	}
	else {
		std::cerr <<"Error: comando invalido: " << command << std::endl;
		help(program_name);
		return 1;
	}
	return 0;
}
