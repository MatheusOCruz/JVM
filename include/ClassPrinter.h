//
// Created by matheus on 4/9/24.
//

#ifndef JVM_CLASSPRINTER_H
#define JVM_CLASSPRINTER_H
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "ClassFileStructs.h"
#include "ClassLoader.h"
#include "OpcodePrinter.h"
#include <unordered_map>

//TODO: depois tenho q jogar as funcao de print pra ca pra n virar bagunca o loader
class ClassPrinter {
public:
    ClassPrinter(const char* _main_file) : main_file(_main_file), output_file(nullptr) {}
    ClassPrinter(const char* _main_file, const char* _output_file) : main_file(_main_file), output_file(_output_file) {}
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
	void PrintConstantPoolEntry(const cp_info *Entry, size_t idx);

    void PrintFieldEntry();
    void PrintMethodEntry(method_info* Method);
    void PrintAttributeEntry(attribute_info* Attribute);

    std::string outputBuffer;
    const char* main_file;
    const char* output_file;
    class_file* ClassFile;
    OpcodePrinter CodePrinter;
};


#endif //JVM_CLASSPRINTER_H
