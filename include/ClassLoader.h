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
#include "ClassLoaderErrors.h"

// ainda n sei como que tem q retornar essas info, ent por enquanto so vai dar load
class ClassLoader {
public:
     ClassLoader() = default;
    ~ClassLoader() = default;

    void LoadMain(char* nomeArquivo);

    void PrintClass();

private:
    //carrega arquivo em buffer
    void LoadFile(const char* nomeArquivo);


    class_file* LoadClass(const char* nomeArquivo);

    // funcoes que leem o iterador do buffer  retoram a proxima entrada
    // convertida pra little endian e incrementam iterador
    u1 read_u1();
    u2 read_u2();
    u4 read_u4();

    template<typename T>
    std::vector<T>* read_vec(int length);


    // cria as estruturas a partir do buffer_iterator
    void BuildConstantPoolInfo();
    attribute_info* BuildAttributeInfo();
    field_info* BuildFieldInfo();
    method_info* BuildMethodInfo();


    void CheckMagic();
    void CheckVersion();

    // chama builder de structs de acordo com counter e adiciona ao vector
    void BuildConstantPoolTable();
    void BuildInterfaces();
    void BuildFields();
    void BuildMethods();
    void BuildAttributes();
    void BuildAttributes(int attribute_count, std::vector<attribute_info*> &attributes); // pro attributes dentro do field e method info

    void FormatCheck();

    void PrintConstantPoolTable(class_file* ClassFile);

    std::vector<uint8_t>*       file_buffer; // pra poder liberar o arquivo dps
    buffer_iterator             iter; // ler bytes sem ter q recalcular o offset
    class_file*                 current_file;
    std::map<char*,class_file*> class_files;

};


#endif //JVM_CLASSLOADER_H
