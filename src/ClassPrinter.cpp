//
// Created by matheus on 4/9/24.
//

#include "../include/ClassPrinter.h"

std::string ClassPrinter::ClassName(const cp_info *Entry) {

    if(Entry->tag != ConstantPoolTag::CONSTANT_Class) // Classe == Object
		return {""};

	return (*ClassFile->constant_pool)[Entry->name_index]->AsString();

}

void ClassPrinter::Run() {
    ClassFile = Loader->GetClass(main_file);
    PrintClassFile();
}

void ClassPrinter::PrintClassFile(){

	const auto constant_pool = *ClassFile->constant_pool;
	const auto this_class_name = ClassName(constant_pool[ClassFile->this_class]);
	const auto super_class_name = ClassName(constant_pool[ClassFile->super_class]);

    std::cout << std::endl
			  << "Leitor Exibidor: " << this_class_name
			  << std::endl << std::endl
			  << "this_class:   #" << ClassFile->this_class
			  << "       // " << this_class_name << std::endl
			  << "super_class:  #" << ClassFile->super_class
			  << "       // " << super_class_name << std::endl;

	// TODO(ruan): Adicionar uma função para criar uma string dizendo
	// quais flags sao
	std::cout << "Flags: 0x"<< std::setw(5)
			  << std::right << std::setfill('0')
			  << std::setbase(16) << ClassFile->access_flags << std::endl
			  << std::setbase(10) << std::setfill(' ');


    PrintConstantPoolTable();
    PrintInterfaces();
    PrintFields();
    PrintMethods();
    PrintAttributes();

    //ClassFile = Loader->GetClassFromName(super_class_name);
    /*
	if (ClassFile) {
		PrintClassFile();
	}
     */
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
    // outputBuffer.append("Attributes: \n");
    // for(auto Attribute: *Attributes){
    //     PrintAttributeEntry(Attribute);
    //     outputBuffer.append("\n");
    // }
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

	// TODO(ruan): Adicionar uma função para criar uma string dizendo
	// quais flags sao
	std::cout << std::setw(2) <<  ""
			  << "Attributes:" << std::endl;

    for(auto Attribute: *Method->attributes){
        PrintAttributeEntry(Attribute);
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
