//
// Created by matheus on 4/9/24.
//

#ifndef JVM_CLASSPRINTER_H
#define JVM_CLASSPRINTER_H
#include <iostream>
#include <algorithm>

#include "ClassFileStructs.h"
#include "ClassLoader.h"
#include "OpcodePrinter.h"
#include <unordered_map>

//TODO: depois tenho q jogar as funcao de print pra ca pra n virar bagunca o loader
class ClassPrinter {
public:
    ClassPrinter(char* _main_file) : main_file(_main_file), output_file(nullptr) {}
    ClassPrinter(char* _main_file, char* _output_file) : main_file(_main_file), output_file(_output_file) {}
    void Run();

private:
    void SaveInFile();

    void PrintClassFile();
    void PrintMetaData();
    void PrintConstantPoolTable();
    void PrintInterfaces();
    void PrintFields();
    void PrintMethods();
    void PrintAttributes();
    void PrintAttributes(std::vector<attribute_info*>*);
    void PrintConstantPoolEntry(cp_info* Entry);

    void PrintFieldEntry();
    void PrintMethodEntry(method_info* Method);
    void PrintAttributeEntry(attribute_info* Attribute);

    std::string outputBuffer;
    char* main_file;
    char* output_file;
    class_file* ClassFile;
    OpcodePrinter CodePrinter;
};


#endif //JVM_CLASSPRINTER_H
