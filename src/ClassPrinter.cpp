//
// Created by matheus on 4/9/24.
//

#include "../include/ClassPrinter.h"


void ClassPrinter::Run() {
    auto Loader = new ClassLoader();
    Loader->LoadClass(main_file);
    ClassFile = Loader->GetClass(main_file);
    PrintClassFile();
}

void ClassPrinter::PrintClassFile(){
    std::cout << std::endl
			  << "Leitor Exibidor: "
			  << std::endl << std::endl;
    PrintGeneralInfo();
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

void ClassPrinter::PrintGeneralInfo() {
    std::cout << "General Information:" << std::endl;
    std::cout << "Minor version: " << ClassFile->minor_version << std::endl;
    std::cout << "Major version: " << ClassFile->major_version << std::endl;
    std::cout << "Constant pool count: " << ClassFile->constant_pool->size() << std::endl;
    std::cout << "Access flags: 0x" << std::hex << ClassFile->access_flags << std::dec << std::endl;
    std::cout << "This class: " << ClassFile->this_class << std::endl;
    std::cout << "Super class: " << ClassFile->super_class << std::endl;
    std::cout << "Interfaces count: " << ClassFile->interfaces_count << std::endl;
    std::cout << "Fields count: " << ClassFile->fields_count << std::endl;
    std::cout << "Methods count: " << ClassFile->methods_count << std::endl;
    std::cout << "Attributes count: " << ClassFile->attributes_count << std::endl;
    std::cout << std::endl;
}

void ClassPrinter::PrintMetaData() {

}

void ClassPrinter::PrintConstantPoolTable() {
	std::cout << "Constant pool:" << std::endl;
	const auto & constant_pool = *ClassFile->constant_pool;
    for(size_t i = 1; i < constant_pool.size(); i++) {
    	PrintConstantPoolEntry(constant_pool[i], i);
    }
	std::cout << std::endl;

}

void ClassPrinter::PrintInterfaces() {

}

void ClassPrinter::PrintFields() {

}

void ClassPrinter::PrintMethods() {
	std::cout << "Methods:" << std::endl;
    for (auto Method : *ClassFile->methods) {
		std::cout << "{" << std::endl;
        PrintMethodEntry(Method);
		std::cout << "}" << std::endl;
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

#define FIRST_SEP 3
#define NUM_SEP 3
#define TYPE_SEP 20
#define INFO_SEP 20

void ClassPrinter::PrintConstantPoolEntry(const cp_info *Entry, size_t idx) {

    switch (Entry->tag) {
        case ConstantPoolTag::CONSTANT_Utf8: {
			const std::string name = Entry->AsString();
			std::cout << std::setw(FIRST_SEP) << std::right <<  "#"
				      << std::setw(NUM_SEP) << std::left << idx
				      << std::setw(3) << " = "
				      << std::setw(TYPE_SEP) << std::left << "Utf8"
				      << std::setw(INFO_SEP) << std::left << name
				      << std::endl;
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
			std::string info =
				"#"  + std::to_string(Entry->name_index);

			std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				      << std::setw(NUM_SEP)  << std::left << idx
				      << std::setw(3)  << " = "
				      << std::setw(TYPE_SEP) << std::left << "Class"
				      << std::setw(INFO_SEP) << std::left << info
				      << std::endl;

            break;
        }
        case ConstantPoolTag::CONSTANT_String: {
			std::string info =
				"#"  + std::to_string(Entry->string_index);
			std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				      << std::setw(NUM_SEP)  << std::left << idx
				      << std::setw(3)  << " = "
				      << std::setw(TYPE_SEP) << std::left << "String"
				      << std::setw(INFO_SEP) << std::left << info
				      << std::endl;
            break;
        }
        case ConstantPoolTag::CONSTANT_Fieldref: {
			std::string info =
				"#"  + std::to_string(Entry->class_index) +
				".#" + std::to_string(Entry->name_and_type_index);

			std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				      << std::setw(NUM_SEP)  << std::left << idx
				      << std::setw(3)  << " = "
				      << std::setw(TYPE_SEP) << std::left << "Fieldref"
				      << std::setw(INFO_SEP) << std::left << info
				      << std::endl;
            break;
        }
        case ConstantPoolTag::CONSTANT_Methodref:{
			std::string info =
				"#"  + std::to_string(Entry->class_index) +
				".#" + std::to_string(Entry->name_and_type_index);

			std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				      << std::setw(NUM_SEP)  << std::left << idx
				      << std::setw(3)  << " = "
				      << std::setw(TYPE_SEP) << std::left << "Methodref"
				      << std::setw(INFO_SEP) << std::left << info
				      << std::endl;
            break;
        }
        case ConstantPoolTag::CONSTANT_InterfaceMethodref: {

            std::cout<<"CONSTANT_InterfaceMethodref\n";

            std::cout<<"  class index:         "<<Entry->class_index<<"\n";
            std::cout<<"  name and type index: "<<Entry->name_and_type_index<<"\n\n";

            break;
        }

        case ConstantPoolTag::CONSTANT_NameAndType: {
			std::string info =
				"#"  + std::to_string(Entry->name_index) +
				":#" + std::to_string(Entry->descriptor_index);

			std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				      << std::setw(NUM_SEP)  << std::left << idx
				      << std::setw(3)  << " = "
				      << std::setw(TYPE_SEP) << std::left << "NameAndType"
				      << std::setw(INFO_SEP) << std::left << info
				      << std::endl;
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

    const cp_info* method_name_entry = (*ClassFile->constant_pool)[Method->name_index];
    const std::string method_name = method_name_entry->AsString();

    const cp_info* method_descriptor = (*ClassFile->constant_pool)[Method->descriptor_index];
    const std::string descriptor_name = method_descriptor->AsString();
    // tem q fazer uma versao separada pro attribute da classe e dos methods/fields

	const u2 flags = Method->access_flags;

	std::cout << std::setw(2) <<  ""
			  << "Name: " << method_name << std::endl
			  << std::setw(2)<< ""
			  << "Descriptor: " << descriptor_name << std::endl
			  << std::setw(2)<< ""
			  << "Flags: 0x"<< std::setw(5) << std::right << std::setfill('0')
			  << std::setbase(16) << flags << std::endl
			  << std::setbase(10) << std::setfill(' ');

	std::cout << std::setw(2) <<  ""
			  << "Attributes:" << std::endl;

    for(auto Attribute: *Method->attributes){
        PrintAttributeEntry(Attribute);
        outputBuffer.append("\n");
    }

}



void ClassPrinter::SaveInFile() {}

void ClassPrinter::PrintAttributeEntry(attribute_info *Attribute) {

    cp_info* attribute_name_entry = (*ClassFile->constant_pool)[Attribute->attribute_name_index];
    std::string attribute_name = attribute_name_entry->AsString();
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
		std::cout << std::setw(4) <<  ""
				  << "Code: " << std::endl;
		std::cout << CodePrinter.CodeToString(Attribute->code->data(), Attribute->code_length)
				  << std::endl;
        }
        defaut: {
            //por enquanto testar so o do code
        };
    }

}
