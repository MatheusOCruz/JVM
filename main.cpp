#include <iostream>
#include <iterator>
#include <cstring>
#include <cassert>

#include "include/Jvm/Jvm.h"
#include "include/ClassPrinter/ClassPrinter.h"


#define JVM_MODE 0
#define LEITOR_EXIBIDOR 1



void help(const char * program_name) {
	std::cerr << "Como usar:" << std::endl;
	std::cerr << "    " << program_name << " <command> <class_file_path>" << std::endl;
	std::cerr << "Comandos:" << std::endl;
	std::cerr << "    leitor_exibidor" << std::endl;
	std::cerr << "    jvm" << std::endl;
	std::cerr << "Exemplos:" << std::endl;
	std::cerr << "    " << program_name << " leitor_exibidor ./testes/Main.class" << std::endl;
    std::cerr << "    " << program_name << " leitor_exibidor ./testes/Main.class > saida.txt" << std::endl;
	std::cerr << "    " << program_name << " jvm ./testes/Main.class" << std::endl;
}

const char* shift_args(int & argc, char **argv[]) {
	assert(argc > 0);
	argc--;
	return *(*argv)++;
}

// ESSA E A MAIN FUNCIONAL

int main(int argc, char* argv[]) {

	const auto program_name = shift_args(argc, &argv);
	if (!argc) {
		std::cerr <<"Error: É necessário um comando"<< std::endl;
		help(program_name);
		return EXIT_FAILURE;
	}
	const auto command = shift_args(argc, &argv);

    if (!argc) {
        std::cerr <<"Error: É necessário um arquivo .class"<< std::endl;
        help(program_name);
        return EXIT_FAILURE;
    }

    const auto class_file_path  = shift_args(argc, &argv);
    
    if (strcmp(command, "leitor_exibidor") == 0) {

        std::string output_file_path = "";

        if (argc) {
            output_file_path = std::string(shift_args(argc, &argv));
            std::ofstream outputFile(output_file_path);
        }

        ClassPrinter(class_file_path).Run();

    }
    else if (strcmp(command, "jvm") == 0)
        Jvm(class_file_path).Run();

    else {
        std::cerr << "Error: comando invalido: " << command << std::endl;
        help(program_name);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}



// MAIN PRA TESTE JVM

int maind(){

    int mode = JVM_MODE;

    std::string file = "UsaDog";
    switch (mode) {
        case JVM_MODE:
            Jvm(file).Run();
            break;
        case LEITOR_EXIBIDOR:
            ClassPrinter(file).Run();
            break;

        default:
            return 1;
    }

    return 0;
}
