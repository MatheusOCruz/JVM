//
// Created by matheus on 4/9/24.
//

#include "../include/ClassPrinter.h"

void ClassPrinter::Run() {
    auto Loader = new ClassLoader();
    Loader->LoadMain(main_file);
    ClassFile = Loader->GetClass(main_file);
    PrintClassFile();
}

void ClassPrinter::PrintClassFile(){
    PrintMetaData();
    PrintConstantPoolTable();
    PrintInterfaces();
    PrintFields();
    PrintMethods();
    PrintAttributes();

    // caso construtor tenha arquivo de saida
    if(output_file)
        SaveInFile();
    else
        std::cout<<outputBuffer;
}

void ClassPrinter::PrintMetaData() {

}

void ClassPrinter::PrintConstantPoolTable() {
    for(auto ConstantPoolEntry : *ClassFile->constant_pool){
    	PrintConstantPoolEntry(ConstantPoolEntry);
    }
}

void ClassPrinter::PrintInterfaces() {

}

void ClassPrinter::PrintFields() {

}

void ClassPrinter::PrintMethods() {
    outputBuffer.append("\nMethods:\n");
    for (auto Method : *ClassFile->methods) {
        PrintMethodEntry(Method);
        outputBuffer.append("\n");
    }

}

void ClassPrinter::PrintAttributes() {

}
void ClassPrinter::PrintAttributes(std::vector<attribute_info*>* Attributes) {
    outputBuffer.append("Attributes: \n");
    for(auto Attribute: *Attributes){
        PrintAttributeEntry(Attribute);
        outputBuffer.append("\n");
    }
}


//TODO: passar todo cout pra um append
void ClassPrinter::PrintConstantPoolEntry(cp_info *Entry) {

    switch (Entry->tag) {
        case ConstantPoolTag::CONSTANT_Utf8: {

            outputBuffer.append("CONSTANT_Utf8\n");
            std::cout<<"  lenght: "<<Entry->length<<"\n  ";
            // futuramente precisamos de uma funcao pra lidar com utf8 e os char de 16 bits do java
            std::cout.write(reinterpret_cast<char*>(Entry->bytes_vec->data()), Entry->length);
            std::cout<<"\n\n";
            break;
        }

        case ConstantPoolTag::CONSTANT_Integer:{

            std::cout<<"CONSTANT_Integer\n";
            std::cout<<"  Bytes: 0x"<< std::hex << Entry->bytes<<"\n\n"<< std::dec;

            break;
        }
        case ConstantPoolTag::CONSTANT_Float: {

            std::cout<<"CONSTANT_Float\n";
            std::cout<<"  Bytes: 0x"<< std::hex << Entry->bytes<<"\n\n"<< std::dec;

            break;
        }

        case ConstantPoolTag::CONSTANT_Long:{

            std::cout<<"CONSTANT_Long\n";
            std::cout<<"  bytes: 0x" << std::hex << Entry->high_bytes << Entry->low_bytes <<"\n\n"<< std::dec;

            break;
        }
        case ConstantPoolTag::CONSTANT_Double: {

            std::cout<<"CONSTANT_Double\n";
            std::cout<<"  bytes: 0x" << std::hex << Entry->high_bytes << Entry->low_bytes <<"\n\n"<< std::dec;

            break;
        }
        case ConstantPoolTag::CONSTANT_Class: {

            std::cout<<"CONSTANT_Class\n";
            std::cout<<"  Class name: "<< Entry->name_index <<"\n\n";

            break;
        }
        case ConstantPoolTag::CONSTANT_String: {

            std::cout<<"CONSTANT_String\n";
            std::cout<<"  string index: "<< Entry->string_index <<"\n\n";

            break;
        }
        case ConstantPoolTag::CONSTANT_Fieldref:{
            std::cout<<"CONSTANT_Fieldref\n";

            std::cout<<"  class index:         "<<Entry->class_index<<"\n";
            std::cout<<"  name and type index: "<<Entry->name_and_type_index<<"\n\n";

            break;
        }
        case ConstantPoolTag::CONSTANT_Methodref:{
            std::cout<<"CONSTANT_Methodref\n";

            std::cout<<"  class index:         "<<Entry->class_index<<"\n";
            std::cout<<"  name and type index: "<<Entry->name_and_type_index<<"\n\n";

            break;
        }
        case ConstantPoolTag::CONSTANT_InterfaceMethodref: {

            std::cout<<"CONSTANT_InterfaceMethodref\n";

            std::cout<<"  class index:         "<<Entry->class_index<<"\n";
            std::cout<<"  name and type index: "<<Entry->name_and_type_index<<"\n\n";

            break;
        }

        case ConstantPoolTag::CONSTANT_NameAndType: {

            std::cout<<"CONSTANT_NameAndType\n";

            std::cout<<"  name index:       "<<Entry->name_index<<"\n";
            std::cout<<"  descriptor index: "<<Entry->descriptor_index<<"\n\n";


            break;
        }

        case ConstantPoolTag::CONSTANT_MethodHandle:
            std::cout<<"CONSTANT_MethodHandle\n";

            std::cout<<"  reference kind:  "<<Entry->reference_kind<<"\n";
            std::cout<<"  reference index: "<<Entry->reference_index<<"\n\n";

            break;
        case ConstantPoolTag::CONSTANT_MethodType:
            std::cout<<"CONSTANT_MethodType\n";
            std::cout<<"  descriptor index: "<<Entry->descriptor_index<<"\n\n";

            break;
        case ConstantPoolTag::CONSTANT_InvokeDynamic:
            std::cout<<"CONSTANT_InvokeDynamic\n";

            std::cout<<"  bootstrap method attr index:  "<<Entry->bootstrap_method_attr_index<<"\n";
            std::cout<<"  name and type index:          "<<Entry->name_and_type_index<<"\n\n";

            break;

        default:
            break;
    }
}




void ClassPrinter::PrintFieldEntry() {

}

void ClassPrinter::PrintMethodEntry(method_info* Method){
    //TODO: append Flags[...] dps do nome e descriptor

    cp_info* method_name_entry = (*ClassFile->constant_pool)[Method->name_index];
    std::string method_name(reinterpret_cast<char*>(method_name_entry->bytes_vec->data()), method_name_entry->length);
    outputBuffer.append("  Name: ").append(method_name).append("\n");

    cp_info* method_descriptor = (*ClassFile->constant_pool)[Method->descriptor_index];
    std::string descriptor_name(reinterpret_cast<char*>(method_descriptor->bytes_vec->data()), method_descriptor->length);
    outputBuffer.append("  Descriptor: ").append(descriptor_name).append("\n");
    outputBuffer.append("  Attribute count: ").append(std::to_string(Method->attributes_count)).append("\n");
    // tem q fazer uma versao separada pro attribute da classe e dos methods/fields
    PrintAttributes(Method->attributes);

}



void ClassPrinter::SaveInFile() {}

void ClassPrinter::PrintAttributeEntry(attribute_info *Attribute) {

    cp_info* attribute_name_entry = (*ClassFile->constant_pool)[Attribute->attribute_name_index];
    std::string attribute_name(reinterpret_cast<char*>(attribute_name_entry->bytes_vec->data()), attribute_name_entry->length);

    static std::unordered_map<std::string, int> cases = {
            {"ConstantValue", 0},
            {"Code", 1},
            //
            {"Exceptions", 3},
            {"InnerClasses", 4},
            //
            {"SourceFile", 8}
    };
    AttributeType AttributeTypeName;
    if(cases.find(attribute_name) != cases.end())
        AttributeTypeName = static_cast<AttributeType>(cases[attribute_name]);
    else
        AttributeTypeName = AttributeType::NotImplemented;

    switch (AttributeTypeName) {
        case AttributeType::Code: {
            outputBuffer.append("    Code:\n");
            outputBuffer.append(CodePrinter.CodeToString(Attribute->code->data(), Attribute->code_length));
            outputBuffer.append("\n");
        }
        defaut: {
            //por enquanto testar so o do code
        };
    }

}
