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
			  << "       // " << super_class_name << std::endl
			  << "minor version:    " << ClassFile->minor_version << std::endl
			  << "major version:    " << ClassFile->major_version << std::endl;

	// TODO(ruan): Adicionar uma função para criar uma string dizendo
	// quais flags sao
	std::cout << "Flags: 0x"<< std::setw(5)
			  << std::right << std::setfill('0')
			  << std::setbase(16) << ClassFile->access_flags << std::endl
			  << std::setbase(10) << std::setfill(' ');


	std::cout << "interfaces: " << ClassFile->interfaces_count << ", "
			  << "fields: " << ClassFile->fields_count << ", "
			  << "methods: " << ClassFile->methods_count << ", "
			  << "attributes: " << ClassFile->attributes_count << std::endl;

    PrintConstantPoolTable();
	std::cout << std::endl;
    PrintInterfaces();
	std::cout << std::endl;
    PrintFields();
	std::cout << std::endl;
    PrintMethods();
	std::cout << std::endl;
    PrintAttributes();
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
	std::cout << "Fields {" << std::endl;
	for (const auto f: *ClassFile->fields) {
		PrintFieldEntry(f);
		std::cout << std::endl;
	}
	std::cout << "}" << std::endl;
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
	std::cout << "Attributes:" << std::endl;
	for(const auto a: (*ClassFile->attributes)) {
		PrintAttributeEntry(a, 2);
	}
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
	case ConstantPoolTag::CONSTANT_Integer: {
		const u4 bytes  = Entry->bytes;
		const int value = *((int *)(&bytes));
		std::cout << std::setw(FIRST_SEP) << std::right <<  "#"
				  << std::setw(NUM_SEP) << std::left << idx
				  << std::setw(3) << " = "
				  << std::setw(TYPE_SEP) << std::left << "Integer"
				  << std::setw(INFO_SEP) << std::left << value 
				  << std::endl;

		break;
	}
	case ConstantPoolTag::CONSTANT_Float: {
		const u4 bytes  = Entry->bytes;
		const float value = *((float *)(&bytes));
		std::cout << std::setw(FIRST_SEP) << std::right <<  "#"
				  << std::setw(NUM_SEP) << std::left << idx
				  << std::setw(3) << " = "
				  << std::setw(TYPE_SEP) << std::left << "Float"
				  << std::setw(INFO_SEP) << std::left << value 
				  << std::endl;

		break;
	}
	case ConstantPoolTag::CONSTANT_Long: {
		const u8 bytes     = ((u8) Entry->high_bytes << 32) | Entry->low_bytes;
		const long long value = *((long long * )(&bytes));
		std::cout << std::setw(FIRST_SEP) << std::right <<  "#"
				  << std::setw(NUM_SEP) << std::left << idx
				  << std::setw(3) << " = "
				  << std::setw(TYPE_SEP) << std::left << "Long"
				  << std::setw(INFO_SEP) << std::left << value 
				  << std::endl;

		break;
	}
	case ConstantPoolTag::CONSTANT_Double: {
		const u8 bytes     = ((u8) Entry->high_bytes << 32) | Entry->low_bytes;
		const double value = *((double * )(&bytes));
		std::cout << std::setw(FIRST_SEP) << std::right <<  "#"
				  << std::setw(NUM_SEP) << std::left << idx
				  << std::setw(3) << " = "
				  << std::setw(TYPE_SEP) << std::left << "Double"
				  << std::setw(INFO_SEP) << std::left << value 
				  << std::endl;
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
	case ConstantPoolTag::CONSTANT_Methodref: {
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
		std::string info =
			"#"  + std::to_string(Entry->class_index) +
			".#" + std::to_string(Entry->name_and_type_index);

		std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				  << std::setw(NUM_SEP)  << std::left << idx
				  << std::setw(3)  << " = "
				  << std::setw(TYPE_SEP) << std::left << "InterfaceMethodref"
				  << std::setw(INFO_SEP) << std::left << info
				  << std::endl;
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
	case ConstantPoolTag::CONSTANT_MethodHandle: {
		std::string info =
			"#"  + std::to_string(Entry->reference_kind) +
			":#" + std::to_string(Entry->reference_index);

		std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				  << std::setw(NUM_SEP)  << std::left << idx
				  << std::setw(3)  << " = "
				  << std::setw(TYPE_SEP) << std::left << "MethodHandle"
				  << std::setw(INFO_SEP) << std::left << info
				  << std::endl;
		break;
	}
	case ConstantPoolTag::CONSTANT_MethodType: {
		std::string info =
			"#"  + std::to_string(Entry->descriptor_index);
		std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				  << std::setw(NUM_SEP)  << std::left << idx
				  << std::setw(3)  << " = "
				  << std::setw(TYPE_SEP) << std::left << "MethodType"
				  << std::setw(INFO_SEP) << std::left << info
				  << std::endl;
		break;
	}
	case ConstantPoolTag::CONSTANT_InvokeDynamic: {
		std::string info =
			"#"  + std::to_string(Entry->bootstrap_method_attr_index) +
			":#" + std::to_string(Entry->name_and_type_index);

		std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				  << std::setw(NUM_SEP)  << std::left << idx
				  << std::setw(3)  << " = "
				  << std::setw(TYPE_SEP) << std::left << "InvokeDynamic"
				  << std::setw(INFO_SEP) << std::left << info
				  << std::endl;
		break;
	}
	default:
		break;
    }
}

void ClassPrinter::PrintFieldEntry(const field_info *field) {
    const cp_info* field_name_entry = (*ClassFile->constant_pool)[field->name_index];
    const std::string field_name = field_name_entry->AsString();

    const cp_info* field_descriptor = (*ClassFile->constant_pool)[field->descriptor_index];
    const std::string descriptor_name = field_descriptor->AsString();

	const u2 flags = field->access_flags;

	std::cout << std::setw(2) <<  ""
			  << "Name: " << field_name << std::endl
			  << std::setw(2)<< ""
			  << "Descriptor: " << descriptor_name << std::endl
			  << std::setw(2)<< ""
			  << "Flags: 0x"<< std::setw(5) << std::right << std::setfill('0')
			  << std::setbase(16) << flags << std::endl
			  << std::setbase(10) << std::setfill(' ');

	// TODO(ruan): Adicionar uma função para criar uma string dizendo
	// quais flags sao
	if (field->attributes_count) {
		std::cout << std::setw(2) <<  ""
				<< "Attributes:" << std::endl;
		for(auto Attribute: *field->attributes){
			PrintAttributeEntry(Attribute);
		}
	}
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

void ClassPrinter::PrintAttributeEntry(const attribute_info *attr, int indent_width) {

    const cp_info* attribute_name_entry = (*ClassFile->constant_pool)[attr->attribute_name_index];
    const std::string attr_name = attribute_name_entry->AsString();

	if (attr_name == "Code") {
		std::cout << std::setw(indent_width) <<  ""
				  << "Code: " << std::endl;
		std::cout << CodePrinter.CodeToString(
											  attr->code->data(),
											  attr->code_length)
				  << std::endl;
		if (attr->exception_table_length) {
			std::cout << std::setw(indent_width) <<  ""
					<< "Exception Table:" << std::endl;
			std::cout << std::setw(indent_width + 2) << ""
					  << std::setw(6) << std::left << "from"
					  << std::setw(6) << std::left << "to"
					  << std::setw(8) << std::left << "target"
					  << std::setw(6) << std::left << "type" << std::endl;

			for(const auto e: *attr->exception_table){
				std::cout << std::setw(indent_width + 2) << ""
						  << std::setw(6) << std::left << e->start_pc
						  << std::setw(6) << std::left << e->end_pc
						  << std::setw(8) << std::left << e->handler_pc
						  << std::setw(1) << std::left << "#"
						  << std::setw(5) << std::left << e->catch_type
						  << std::endl;
			}
		}

		if (attr->attributes_count) {
			std::cout << std::endl;
			std::cout << std::setw(indent_width) <<  ""
					<< "Attributes:" << std::endl;
			for(const auto a: *attr->attributes){
				PrintAttributeEntry(a, indent_width + 2);
			}
		}
	}
	else if (attr_name == "ConstantValue") {
		std::cout << std::setw(indent_width) <<  ""
			      << "ConstantValue: #" << attr->constantvalue_index
				  << std::endl;
	}
	else if (attr_name == "Exceptions") {
		std::cout << std::setw(indent_width) <<  ""
			      << "Exception Table:" << std::endl;
		for (int i = 0; i < attr->number_of_exceptions; i++) {
			std::cout << std::setw(indent_width + 2) << ""
					  << std::setw(4) << std::left << i << ":   #"
					  << (*attr->exception_index_table)[i] << std::endl;
		}
	}
	else if (attr_name == "SourceFile") {
		std::cout << std::setw(indent_width) <<  ""
			      << "SourceFile: #" << attr->sourcefile_index
				  << std::endl;
	}
	else if (attr_name == "InnerClasses") {
		std::cout << std::setw(indent_width) <<  ""
				  << "InnerClasses:" << std::endl;

		for(const auto c: *attr->classes){
			std::cout << std::setw(indent_width + 2) << ""
			 		  << std::setw(1) << std::left << "#"
			 		  << std::setw(5) << std::left << c->inner_name_index
			 		  << std::setw(2) << std::left << "= "
			 		  << std::setw(1) << std::left << "#"
			 		  << std::setw(5) << std::left << c->inner_class_info_index
			 		  << std::setw(4) << std::left << " of "
			 		  << std::setw(1) << std::left << "#"
			 		  << std::setw(5) << std::left << c->outer_class_info_index
			 		  << std::endl;
		}
	}
	else if (attr_name == "LineNumberTable") {}
	else if (attr_name == "StackMapTable") {}
	else if (attr_name == "NestMembers") {}
	else {
		std::cerr << std::setw(indent_width) <<  ""
				  << "Error: atribute type " << attr_name << " printing not implemented" << std::endl;
	}
}
