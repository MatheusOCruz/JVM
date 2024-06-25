//
// Created by matheus on 6/25/24.
//

#include "../../include/ClassPrinter/ConstantPoolPrinter.h"

std::string  ConstantPoolPrinter::CpEntryAsString(u2 index) {
    auto Entry = ConstantPool[index];
    switch (Entry->tag) {
        case ConstantPoolTag::CONSTANT_Utf8:
            return Entry->AsString();

        case ConstantPoolTag::CONSTANT_Integer:
            break;
        case ConstantPoolTag::CONSTANT_Float:
            break;
        case ConstantPoolTag::CONSTANT_Long:
            break;
        case ConstantPoolTag::CONSTANT_Double:
            break;

        case ConstantPoolTag::CONSTANT_Class:
			return CpEntryAsString(Entry->name_index);

        case ConstantPoolTag::CONSTANT_String: 
            return CpEntryAsString(Entry->string_index);

        case ConstantPoolTag::CONSTANT_Fieldref:
        case ConstantPoolTag::CONSTANT_InterfaceMethodref:
        case ConstantPoolTag::CONSTANT_Methodref:
            return CpEntryAsString(Entry->class_index) + "." + CpEntryAsString(Entry->name_and_type_index);

        case ConstantPoolTag::CONSTANT_NameAndType:
			return CpEntryAsString(Entry->name_index) + ":" + CpEntryAsString(Entry->descriptor_index);

        case ConstantPoolTag::CONSTANT_MethodHandle:
			return CpEntryAsString(Entry->reference_kind) + ":" + CpEntryAsString(Entry->reference_index);

        case ConstantPoolTag::CONSTANT_MethodType:
			return CpEntryAsString(Entry->descriptor_index);

        case ConstantPoolTag::CONSTANT_InvokeDynamic:
			return CpEntryAsString(Entry->bootstrap_method_attr_index) + ":" + CpEntryAsString(Entry->name_and_type_index);

    }
    return std::string();
}
