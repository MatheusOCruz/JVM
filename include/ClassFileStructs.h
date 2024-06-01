//
// Created by matheus on 4/2/24.
//

#ifndef JVM_CLASSFILESTRUCTS_H
#define JVM_CLASSFILESTRUCTS_H

#include <cstdint>
#include <vector>
#include <string>
#include "ClassFileEnums.h"
#include "typedefs.h"
#include "assert.h"
#include "AttributeStructs.h"


struct cp_info{
    ConstantPoolTag tag;
    union {
        struct { //CONSTANT_Class_info, CONSTANT_NameAndType_info
            uint16_t name_index;
            uint16_t descriptor_index;  // CONSTANT_MethodType_info , CONSTANT_NameAndType_info

        };
        struct { // CONSTANT_Fieldref_info, CONSTANT_Methodref_info, CONSTANT_InterfaceMethodref_info, CONSTANT_InvokeDynamic_info
            union{
                uint16_t class_index; // CONSTANT_Fieldref_info, CONSTANT_Methodref_info, CONSTANT_InterfaceMethodref_info
                uint16_t bootstrap_method_attr_index; // CONSTANT_InvokeDynamic_info
            };
            uint16_t name_and_type_index;

        };
        struct { //CONSTANT_String_info
            uint16_t string_index;
        };
        struct{ //CONSTANT_Integer_info, CONSTANT_Float_info
            uint32_t bytes;
        };
        struct{ //CONSTANT_Long_info, CONSTANT_Double_info
            uint32_t high_bytes;
            uint32_t low_bytes;
        };
        struct{ // CONSTANT_UTF8_info
            uint16_t  length;
            std::vector<uint8_t>* bytes_vec; //o vetor ta dando bo, vamo ter q ir de malloc msm
        };
        struct{ // CONSTANT_MethodHandle_info
            uint8_t reference_kind;
            uint16_t reference_index;
        };
    };

    std::string AsString() const noexcept {
        //if(tag != ConstantPoolTag::CONSTANT_Utf8 ){
            //throw   std::runtime_error("Entrada nao e um UTF8\n");
		//	assert(0);
        //}
        return { reinterpret_cast<char*>(bytes_vec->data()), length };
    };
};

struct attribute_info{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    union {
        struct { // ConstantValue_attribute
            u2 constantvalue_index; // entrada com o valor em si
        };
        struct { // Code_attribute
            u2 max_stack; // profundidade maxima da stack pro metodo
            u2 max_locals; // tamanho maximo da array de variaveis locais
            u4 code_length;
            std::vector<u1>*code;
            u2 exception_table_length;
            std::vector<Exception_tableEntry*>*  exception_table;
            u2 attributes_count;
            std::vector<attribute_info*>* attributes;
        };
        struct {// Exceptions_attribute
            u2 number_of_exceptions;
            std::vector<u2>* exception_index_table;
        };
        struct { // InnerClasses_attribute
            u2 number_of_classes;
            std::vector<InnerClasse*>* classes;
        };
        struct{ // source file
            u2 sourcefile_index;
        };
    };
};

struct field_info{
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    std::vector<attribute_info*>* attributes;
};

struct method_info{
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    std::vector<attribute_info*>* attributes;
};

struct class_file{
    u4                            magic;
    u2                            minor_version;
    u2                            major_version;
    u2                            constant_pool_count;
    std::vector<cp_info*>*        constant_pool;
    u2                            access_flags;
    u2                            this_class;
    u2                            super_class;
    u2                            interfaces_count;
    std::vector<u2>*              interfaces;
    u2                            fields_count;
    std::vector<field_info*>*     fields;
    u2                            methods_count;
    std::vector<method_info*>*    methods;
    u2                            attributes_count;
    std::vector<attribute_info*>* attributes;
};

#endif //JVM_CLASSFILESTRUCTS_H

