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
            break;
        case ConstantPoolTag::CONSTANT_String: {
            auto str = ConstantPool[Entry->string_index]->AsString();
            str = " <"+str+">";
            return str;
        }
        case ConstantPoolTag::CONSTANT_Fieldref:
        case ConstantPoolTag::CONSTANT_Methodref:{
            auto Class = ConstantPool[Entry->class_index];
            auto NameAndType = ConstantPool[Entry->name_and_type_index];
            auto ClassName = ConstantPool[Class->name_index]->AsString();
            auto Name = ConstantPool[NameAndType->name_index]->AsString();
            auto Descriptor = ConstantPool[NameAndType->descriptor_index]->AsString();
            auto str = " <"+ClassName+"."+Name+" : "+Descriptor+">";
            return str;
        }

        case ConstantPoolTag::CONSTANT_InterfaceMethodref:
            break;
        case ConstantPoolTag::CONSTANT_NameAndType:
            break;
        case ConstantPoolTag::CONSTANT_MethodHandle:
            break;
        case ConstantPoolTag::CONSTANT_MethodType:
            break;
        case ConstantPoolTag::CONSTANT_InvokeDynamic:
            break;
    }
    return std::string();
}
