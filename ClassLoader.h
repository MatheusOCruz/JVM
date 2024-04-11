//
// Created by matheus on 4/2/24.
//

#ifndef JVM_CLASSLOADER_H
#define JVM_CLASSLOADER_H
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <map>
#include <iterator>
#include <memory>

#include "ClassFileEnums.h"
#include "ClassFileStructs.h"
#include "typedefs.h"

// ainda n sei como que tem q retornar essas info, ent por enquanto so vai dar load
class ClassLoader {
public:
     ClassLoader() = default;
    ~ClassLoader() = default;

    void LoadMain(char* nomeArquivo);
    //le um arquivo individual
    void LoadClass(const char* nomeArquivo);
    void PrintClass();

private:
    //carrega arquivo em buffer
    void LoadFile(const char* nomeArquivo);

    // funcoes que leem o iterador do buffer  retoram a proxima entrada
    // convertida pra little endian e incrementam iterador
    u1 read_u1();
    u2 read_u2();
    u4 read_u4();

    template<typename T>
    std::vector<T>* read_vec(int length);


    // cria as estruturas a partir do buffer_iterator
    cp_info* BuildConstantPoolInfo();
    attribute_info* BuildAttributeInfo();
    field_info* BuildFieldInfo();
    method_info* BuildMethodInfo();

    // chama builder de structs de acordo com counter e adiciona ao vector
    void BuildConstantPoolTable(class_file* Entry);
    void BuildInterfaces(class_file* Entry);
    void BuildFields(class_file* Entry);
    void BuildMethods(class_file* Entry);
    void BuildAttributes(class_file* Entry);
    void BuildAttributes(int attribute_count, std::vector<attribute_info*> &attributes); // pro attributes dentro do field e method info

    void PrintConstantPoolTable(std::_Rb_tree_iterator<std::pair<char *const, class_file *>> ClassFile);

    std::vector<uint8_t>* file_buffer; // pra poder liberar o arquivo dps
    buffer_iterator       iter; // ler bytes sem ter q recalcular o offset
    u4 magic;
    std::map<char*,class_file*>* class_files;

};


#endif //JVM_CLASSLOADER_H
