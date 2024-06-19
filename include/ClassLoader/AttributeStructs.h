//
// Created by matheus on 4/9/24.
//

#ifndef JVM_ATTRIBUTESTRUCTS_H
#define JVM_ATTRIBUTESTRUCTS_H
#include "../typedefs.h"
#include "ClassFileStructs.h"

//TODO: falta um bocado

struct Exception_tableEntry{
    u2 start_pc;
    u2 end_pc;
    u2 handler_pc;
    u2 catch_type;
};


struct InnerClasse {
    u2 inner_class_info_index;
    u2 outer_class_info_index;
    u2 inner_name_index;
    u2 inner_class_access_flags;
};

//TODO: esse aqui parece q e do cao
struct StackMapTable_attribute {
    u2 attribute_name_index;  // utf-8 com "StackMapTable"
    u4 attribute_length;
    u2 number_of_entries;
    //stack_map_frame entries[number_of_entries]; aindda n sei cole desse stack_map_frame
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

struct a{
    u2 start_pc;
    u2 length;
    u2 name_index;
    u2 descriptor_index;
    u2 index;
};
struct LocalVariableTable_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 local_variable_table_length;
    std::vector<a>* local_variable_table; // duvido adivinhar
};

#endif //JVM_ATTRIBUTESTRUCTS_H
