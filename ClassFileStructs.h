//
// Created by matheus on 4/2/24.
//

#ifndef JVM_CLASSFILESTRUCTS_H
#define JVM_CLASSFILESTRUCTS_H

#include <cstdint>
#include <vector>
#include "ClassFileEnums.h"

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
            std::vector<uint8_t> *bytes_vec; //o vetor ta dando bo, vamo ter q ir de malloc msm
        };
        struct{ // CONSTANT_MethodHandle_info
            uint8_t reference_kind;
            uint16_t reference_index;
        };
    };
};

struct attribute_info{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    std::vector<uint8_t>* info; // em tese nesse caso aqui e byte, num sei
};

struct field_info{
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    std::vector<attribute_info*>* attributes;
};
struct method_info{
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    std::vector<attribute_info*>* attributes;
};


#endif //JVM_CLASSFILESTRUCTS_H
