// Definição das estruturas auxiliares necessárias
#ifndef JVM_ATTRIBUTESTRUCTS_H
#define JVM_ATTRIBUTESTRUCTS_H
#include "typedefs.h"
#include "ClassFileStructs.h"

struct Exception_tableEntry {
    u2 start_pc;
    u2 end_pc;
    u2 handler_pc;
    u2 catch_type;
};

struct InnerClasses {
    u2 inner_class_info_index;
    u2 outer_class_info_index;
    u2 inner_name_index;
    u2 inner_class_access_flags;
};

struct line_number_table {
    u2 start_pc;
    u2 line_number;
};

struct verification_type_info {
    u1 tag;
    union {
        u2 cpool_index;   // Used by ITEM_Object and ITEM_Uninitialized
        u2 offset;        // Used by ITEM_Uninitialized
    };
};

struct stack_map_frame {
    u1 frame_type; // Pode variar entre 0-63, 64-127, 247, 248-250, 251, 252-254, 255
    union {
        struct { // same_frame
            // Nenhum campo adicional
        } same_frame;

        struct { // same_locals_1_stack_item_frame e same_locals_1_stack_item_frame_extended
            verification_type_info stack;
        } same_locals_1_stack_item_frame;

        struct { // same_frame_extended
            u2 offset_delta;
        } same_frame_extended;

        struct { // chop_frame, same_frame_extended, append_frame
            u2 offset_delta;
        } chop_frame;

        struct { // full_frame
            u2 offset_delta;
            u2 number_of_locals;
            std::vector<verification_type_info>* locals;
            u2 number_of_stack_items;
            std::vector<verification_type_info>* stack;
        } full_frame;
    };
};

struct stack_map_table_attribute {
    u2 number_of_entries;
    std::vector<stack_map_frame>* entries;
};

struct source_file_attribute {
    u2 sourcefile_index;
};

struct signature_attribute {
    u2 signature_index;
};

struct constant_value_attribute {
    u2 constantvalue_index;
};

struct exception_attribute {
    u2 number_of_exceptions;
    std::vector<u2>* exception_index_table;
};

struct code_attribute {
    u2 max_stack;
    u2 max_locals;
    u4 code_length;
    std::vector<u1>* code;
    u2 exception_table_length;
    std::vector<Exception_tableEntry*>* exception_table;
    u2 attributes_count;
    std::vector<attribute_info*>* attributes;
};

struct inner_classes_attribute {
    u2 number_of_classes;
    std::vector<InnerClasses*>* classes;
};

#endif JVM_ATTRIBUTESTRUCTS_H