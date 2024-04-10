//
// Created by matheus on 4/9/24.
//

#ifndef JVM_ATTRIBUTES_H
#define JVM_ATTRIBUTES_H
#include "typedefs.h"
#include "ClassFileStructs.h"

//TODO: falta um bocado

struct ConstantValue_attribute {
    u2 attribute_name_index; // utf-8 com valor "ConstantValue"
    u4 attribute_length;    // tem q ser 2
    u2 constantvalue_index; // entrada com o valor em si
};

struct Code_attribute {
    u2 attribute_name_index; // utf-8 com "Code"
    u4 attribute_length;
    u2 max_stack; // profundidade maxima da stack pro metodo
    u2 max_locals; // tamanho maxiom da array de variaveis locais
    u4 code_length;
    std::vector<u1>*code;
    u2 exception_table_length;
    struct {
        u2 start_pc;
        u2 end_pc;
        u2 handler_pc;
        u2 catch_type;
        }  exception_table; // em tese isso tem o tamanho[exception_table_length], mas deixando assim fica melhor pra acessar, usa o lenght pra controle de leitura
    u2 attributes_count;
    std::vector<attribute_info*>* attributes;
};

//TODO: esse aqui parece q e do cao
struct StackMapTable_attribute {
    u2 attribute_name_index;  // utf-8 com "StackMapTable"
    u4 attribute_length;
    u2 number_of_entries;
    //stack_map_frame entries[number_of_entries]; aindda n sei cole desse stack_map_frame
};


struct Exceptions_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 number_of_exceptions;
    std::vector<u2>* exception_index_table;
};


struct InnerClasses_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 number_of_classes;
    struct {
        u2 inner_class_info_index;
        u2 outer_class_info_index;
        u2 inner_name_index;
        u2 inner_class_access_flags;
    } classes; // mesma coisa do code, mais facil acessar assim
};

struct EnclosingMethod_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 class_index;
    u2 method_index;
};

struct Synthetic_attribute {
u2 attribute_name_index;
u4 attribute_length;
};

struct Signature_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 signature_index;
};

struct SourceFile_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 sourcefile_index;
};

struct SourceDebugExtension_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    std::vector<u1>* debug_extension;
};

struct LineNumberTable_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 line_number_table_length;
    struct { u2 start_pc;
        u2 line_number;
    } line_number_table; // tendeu ja ne
};

struct LocalVariableTable_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 local_variable_table_length;
    struct {
        u2 start_pc;
        u2 length;
        u2 name_index;
        u2 descriptor_index;
        u2 index;
    } local_variable_table; // duvido adivinhar
};

#endif //JVM_ATTRIBUTES_H
