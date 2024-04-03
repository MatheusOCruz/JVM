//
// Created by matheus on 4/2/24.
//

#ifndef JVM_CLASSLOADER_H
#define JVM_CLASSLOADER_H
#include <iostream>
#include <fstream>
#include <vector>
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
    void LoadClass(const char* nomeArquivo);

private:
    void LoadFile(const char* nomeArquivo);
    // funcoes que leem o iterador do buffer  retoram a proxima entrada
    // convertida pra little endian e incrementam iterador
    u1 read_u1();
    u2 read_u2();
    u4 read_u4();

    template<typename T>
    std::vector<T>* read_vec(int length);


    // cria as estruturas a partir do buffer_iterator
    cp_info* BuildConstantPoolEntry();
    attribute_info* BuildAttributeInfo();
    field_info* BuildFieldInfo();
    method_info* BuildMethodInfo();

    // chama builder de structs de acordo com counter e adiciona ao vector
    void BuildConstantPoolTable();
    void BuildInterfaces();
    void BuildFields();
    void BuildMethods();
    void BuildAttributes();
    void BuildAttributes(int attribute_count, std::vector<attribute_info*> &attributes); // pro attributes dentro do field e method info



    buffer_iterator iter;

    u4                           magic;
    u2                           minor_version;
    u2                           major_version;
    u2                           constant_pool_count;
    std::vector<cp_info*>        constant_pool;
    u2                           access_flags;
    u2                           this_class;
    u2                           super_class;
    u2                           interfaces_count;
    std::vector<u2>*             interfaces;
    u2                           fields_count;
    std::vector<field_info*>     fields;
    u2                           methods_count;
    std::vector<method_info*>    methods;
    u2                           attributes_count;
    std::vector<attribute_info*> attributes;
};


#endif //JVM_CLASSLOADER_H
