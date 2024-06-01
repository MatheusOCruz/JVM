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
    ClassPrinter(const std::string _main_file) : main_file(_main_file) {Loader = new ClassLoader;}
    void Run();

private:
    void SaveInFile();

	std::string ClassName(const cp_info *Entry);

    void PrintClassFile();
    void PrintConstantPoolTable();
    void PrintInterfaces();
    void PrintFields();
    void PrintMethods();
    void PrintAttributes();
    void PrintAttributes(std::vector<attribute_info*>*);
	void PrintConstantPoolEntry(const cp_info *Entry, size_t idx);

    void PrintFieldEntry(const field_info * field);
    void PrintMethodEntry(method_info* Method);
    void PrintAttributeEntry(const attribute_info* Attribute, int indent_width=4);

    const std::string main_file;
    class_file* ClassFile;
    OpcodePrinter CodePrinter;
	ClassLoader* Loader;
};


#endif //JVM_CLASSPRINTER_H
