//
// Created by matheus on 4/9/24.
//

#include "../../include/ClassPrinter/ClassPrinter.h"

/**
 * @brief retorna o nome de uma classe a partir de sua entrada na constant pool
 * @param Entry  ponteiro para a entrada que representa uma classe
 * @return String contendo o nome da classe na entrada
 */
std::string ClassPrinter::ClassName(const cp_info *Entry) {

    if(Entry->tag != ConstantPoolTag::CONSTANT_Class) // Classe == Object
		return {""};

	return (*ClassFile->constant_pool)[Entry->name_index]->AsString();

}
/**
 *  Função principal, chamada na main, que inicializa classes internas e
 *  invoca o ClassLoader para obter a representação
 *  interna do .class a ser exibido, então invoca o método interno PrintClassFile
 */
void ClassPrinter::Run() {
    ClassFile = Loader->GetClass(main_file);
    PoolPrinter = new ConstantPoolPrinter(ClassFile->constant_pool);
    CodePrinter = new OpcodePrinter(PoolPrinter);
    PrintClassFile();
}

/**
 * Função responsável por invocar os métodos internos na ordem adequada
 * para a apresentação em ordem e formatar a saida do arquivo .class
 */
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
			  << "major version:    " << ClassFile->major_version << std::endl
			  << "java version:     " << GetJavaVersion(ClassFile->major_version, ClassFile->minor_version) << std::endl;

	std::cout << "Flags: (0x"<< std::setw(5) << std::right << std::setfill('0')
			  << std::setbase(16) << ClassFile->access_flags
			  << std::setbase(10) << std::setfill(' ')
			  << ") " << ClassAccessFlagToString(ClassFile->access_flags) << std::endl;


	std::cout << "interfaces: " << ClassFile->interfaces_count << ", "
			  << "fields: " << ClassFile->fields_count << ", "
			  << "methods: " << ClassFile->methods_count << ", "
			  << "attributes: " << ClassFile->attributes_count << std::endl;

    PrintConstantPoolTable();
	std::cout << std::endl;
    PrintFields();
	std::cout << std::endl;
    PrintMethods();
	std::cout << std::endl;
    PrintAttributes();
	std::cout << std::endl;
    PrintInterfaces();
}

/**
 * retorna a versao Java corresponsente a versao do arquivo .class
 * @param major_version
 * @param minor_version
 * @return versao java
 */
std::string ClassPrinter::GetJavaVersion(u2 major_version, u2 minor_version) {
	if (major_version < 47)
		return "JDK 1.2 (Java 2)";
	switch(major_version) {
	case 47: return "JDK 1.3 (Java 3)";
	case 48: return "JDK 1.4 (Java 4)" ;
	case 49: return "Java SE 5";
	case 50: return "Java SE 6";
	case 51: return "Java SE 7";
	case 52: return "Java SE 8";
	case 53: return "Java SE 9";
	case 54: return "Java SE 10";
	case 55: return "Java SE 11";
	case 56: return "Java SE 12";
	case 57: return "Java SE 13";
	case 58: return "Java SE 14";
	case 59: return "Java SE 15";
	case 60: return "Java SE 16";
	case 61: return "Java SE 17";
	case 62: return "Java SE 18";
	case 63: return "Java SE 19";
	case 64: return "Java SE 20";
	case 65: return "Java SE 21";
	case 66: return "Java SE 22";
	default: return "Unknown Java Version";
	}
}

/**
 *  Itera por cada entrada da constant Pool e
 *  chama a funcao responsavel por seu print
 */
void ClassPrinter::PrintConstantPoolTable() {
	std::cout << "Constant pool:" << std::endl;
	const auto & constant_pool = *ClassFile->constant_pool;
    for(size_t i = 1; i < constant_pool.size(); i++) {
    	PrintConstantPoolEntry(constant_pool[i], i);
    }
	std::cout << std::endl;

}
/**
 *  Itera por cada Interaface e
 *  chama a funcao responsavel por seu print
 */
void ClassPrinter::PrintInterfaces() {
	if (ClassFile->interfaces_count > 0) {
		std::cout << "Interfaces:" << std::endl;
		for (const auto idx: *ClassFile->interfaces) {
			std::cout << std::setw(2) << ""
					  << "#" << idx << std::endl;
		}
	}
}
/**
 *  Itera por cada Field e
 *  chama a funcao responsavel por seu print
 */
void ClassPrinter::PrintFields() {
	if (ClassFile->fields_count > 0) {
		std::cout << "Fields {" << std::endl;
		for (const auto f: *ClassFile->fields) {
			PrintFieldEntry(f);
			std::cout << std::endl;
		}
		std::cout << "}" << std::endl;
	}
}
/**
 *  Itera por cada método e
 *  chama a funcao responsavel por seu print
 */
void ClassPrinter::PrintMethods() {
	std::cout << "Methods:" << std::endl;
    for (auto Method : *ClassFile->methods) {
		std::cout << "{" << std::endl;
        PrintMethodEntry(Method);
		std::cout << "}" << std::endl;
    }
}
/**
 *  Itera por cada Atributo e
 *  chama a funcao responsavel por seu print
 */
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
/**
 * @brief Print formatado de uma entrada da constant pool
 * @param Entry ponteiro para struct que representa a entrada
 * @param idx indice da entrada na constant pool
 */
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

		std::string info_string = PoolPrinter->CpEntryAsString(Entry->name_index);

		std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				  << std::setw(NUM_SEP)  << std::left << idx
				  << std::setw(3)  << " = "
				  << std::setw(TYPE_SEP) << std::left << "Class"
				  << std::setw(INFO_SEP) << std::left << info
				  << std::left << "// " << info_string
				  << std::endl;

		break;
	}
	case ConstantPoolTag::CONSTANT_String: {
		std::string info =
			"#"  + std::to_string(Entry->string_index);
		std::string info_string = PoolPrinter->CpEntryAsString(Entry->string_index);

		std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				  << std::setw(NUM_SEP)  << std::left << idx
				  << std::setw(3)  << " = "
				  << std::setw(TYPE_SEP) << std::left << "String"
				  << std::setw(INFO_SEP) << std::left << info
				  << std::left << "// " << info_string
				  << std::endl;
		break;
	}
	case ConstantPoolTag::CONSTANT_Fieldref: {
		std::string info =
			"#"  + std::to_string(Entry->class_index) +
			".#" + std::to_string(Entry->name_and_type_index);

		std::string info_string =
			PoolPrinter->CpEntryAsString(Entry->class_index) +
			"." + PoolPrinter->CpEntryAsString(Entry->name_and_type_index);

		std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				  << std::setw(NUM_SEP)  << std::left << idx
				  << std::setw(3)  << " = "
				  << std::setw(TYPE_SEP) << std::left << "Fieldref"
				  << std::setw(INFO_SEP) << std::left << info
				  << std::left << "// " << info_string
				  << std::endl;
		break;
	}
	case ConstantPoolTag::CONSTANT_Methodref: {
		std::string info =
			"#"  + std::to_string(Entry->class_index) +
			".#" + std::to_string(Entry->name_and_type_index);

		std::string info_string =
			PoolPrinter->CpEntryAsString(Entry->class_index) +
			"." + PoolPrinter->CpEntryAsString(Entry->name_and_type_index);

		std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				  << std::setw(NUM_SEP)  << std::left << idx
				  << std::setw(3)  << " = "
				  << std::setw(TYPE_SEP) << std::left << "Methodref"
				  << std::setw(INFO_SEP) << std::left << info
				  << std::left << "// " << info_string
				  << std::endl;
		break;
	}
	case ConstantPoolTag::CONSTANT_InterfaceMethodref: {
		std::string info =
			"#"  + std::to_string(Entry->class_index) +
			".#" + std::to_string(Entry->name_and_type_index);

		std::string info_string =
			PoolPrinter->CpEntryAsString(Entry->class_index) +
			"." + PoolPrinter->CpEntryAsString(Entry->name_and_type_index);

		std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				  << std::setw(NUM_SEP)  << std::left << idx
				  << std::setw(3)  << " = "
				  << std::setw(TYPE_SEP) << std::left << "InterfaceMethodref"
				  << std::setw(INFO_SEP) << std::left << info
				  << std::left << "// " << info_string
				  << std::endl;
		break;
	}
	case ConstantPoolTag::CONSTANT_NameAndType: {
		std::string info =
			"#"  + std::to_string(Entry->name_index) +
			":#" + std::to_string(Entry->descriptor_index);

		std::string info_string =
			PoolPrinter->CpEntryAsString(Entry->name_index) +
			":" + PoolPrinter->CpEntryAsString(Entry->descriptor_index);

		std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				  << std::setw(NUM_SEP)  << std::left << idx
				  << std::setw(3)  << " = "
				  << std::setw(TYPE_SEP) << std::left << "NameAndType"
				  << std::setw(INFO_SEP) << std::left << info
				  << std::left << "// " << info_string
				  << std::endl;
		break;
	}
	case ConstantPoolTag::CONSTANT_MethodHandle: {
		std::string info =
			"#"  + std::to_string(Entry->reference_kind) +
			":#" + std::to_string(Entry->reference_index);

		std::string info_string =
			PoolPrinter->CpEntryAsString(Entry->reference_kind) +
			":" + PoolPrinter->CpEntryAsString(Entry->reference_index);

		std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				  << std::setw(NUM_SEP)  << std::left << idx
				  << std::setw(3)  << " = "
				  << std::setw(TYPE_SEP) << std::left << "MethodHandle"
				  << std::setw(INFO_SEP) << std::left << info
				  << std::left << "// " << info_string
				  << std::endl;
		break;
	}
	case ConstantPoolTag::CONSTANT_MethodType: {
		std::string info =
			"#"  + std::to_string(Entry->descriptor_index);

		std::string info_string = PoolPrinter->CpEntryAsString(Entry->descriptor_index);
		std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				  << std::setw(NUM_SEP)  << std::left << idx
				  << std::setw(3)  << " = "
				  << std::setw(TYPE_SEP) << std::left << "MethodType"
				  << std::setw(INFO_SEP) << std::left << info
				  << std::left << "// " << info_string
				  << std::endl;
		break;
	}
	case ConstantPoolTag::CONSTANT_InvokeDynamic: {
		std::string info =
			"#"  + std::to_string(Entry->bootstrap_method_attr_index) +
			":#" + std::to_string(Entry->name_and_type_index);

		std::string info_string =
			PoolPrinter->CpEntryAsString(Entry->bootstrap_method_attr_index) +
			":" + PoolPrinter->CpEntryAsString(Entry->name_and_type_index);

		std::cout << std::setw(FIRST_SEP)  << std::right <<  "#"
				  << std::setw(NUM_SEP)  << std::left << idx
				  << std::setw(3)  << " = "
				  << std::setw(TYPE_SEP) << std::left << "InvokeDynamic"
				  << std::setw(INFO_SEP) << std::left << info
				  << std::left << "// " << info_string
				  << std::endl;
		break;
	}
	default:
		break;
    }
}
/**
 * @brief Print formatado de um field
 * @param field Ponteiro para uma struct responsavel por representar um field
 */
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
			  << "Flags: (0x"<< std::setw(5) << std::right << std::setfill('0')
			  << std::setbase(16) << flags
			  << std::setbase(10) << std::setfill(' ')
			  << ") " << FieldAccessFlagToString(flags) << std::endl;

	if (field->attributes_count) {
		std::cout << std::setw(2) <<  ""
				<< "Attributes:" << std::endl;
		for(auto Attribute: *field->attributes){
			PrintAttributeEntry(Attribute);
		}
	}
}

/**
 *  @brief Print formatado de um método
 *
 * @param Method Ponteiro para uma struct responsavel por representar um método
 */
void ClassPrinter::PrintMethodEntry(method_info* Method){
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
			  << "Flags: (0x"<< std::setw(5) << std::right << std::setfill('0')
			  << std::setbase(16) << flags
			  << std::setbase(10) << std::setfill(' ')
			  << ") " << MethodAccessFlagToString(flags) << std::endl;

	std::cout << std::setw(2) <<  ""
			  << "Attributes:" << std::endl;

    for(auto Attribute: *Method->attributes){
        PrintAttributeEntry(Attribute);
    }
}


/**
 *
 * @param attr Ponteiro para uma struct responsavel por representar um atributo
 * @param indent_width indicador da identação necessária para formatação correta
 */
void ClassPrinter::PrintAttributeEntry(const attribute_info *attr, int indent_width) {

    const cp_info* attribute_name_entry = (*ClassFile->constant_pool)[attr->attribute_name_index];
    const std::string attr_name = attribute_name_entry->AsString();

	if (attr_name == "Code") {
		std::cout << std::setw(indent_width) <<  ""
				  << "Code: " << std::endl;
		std::cout << CodePrinter->CodeToString(
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
						  << "//  " << PoolPrinter->CpEntryAsString(e->catch_type)
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
				  << "//  " << PoolPrinter->CpEntryAsString(attr->constantvalue_index)
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
				  << "//  " << PoolPrinter->CpEntryAsString(attr->sourcefile_index)
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
					  << " //  " << PoolPrinter->CpEntryAsString(c->inner_name_index)
				      << " = "   << PoolPrinter->CpEntryAsString(c->inner_class_info_index)
				      << " of "   << PoolPrinter->CpEntryAsString(c->outer_class_info_index)
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

/**
 * @brief transforma os bytes que representam as flags  de um field em uma string com seus nomes
 * @param flag u2 que representa as flags de acesso do field
 * @return string contendo o nome das flags ativas
 */
std::string ClassPrinter::FieldAccessFlagToString(u2 flag) {
	std::vector<std::string> flag_vec;
	for (unsigned int bit = 0; bit < 32; bit++) {
		if (flag & (1 << bit)) {
			switch((FieldAccessFlag) (1 << bit)) {
			case FieldAccessFlag::ACC_PUBLIC: {
				flag_vec.push_back("ACC_PUBLIC");
				break;
			}
			case FieldAccessFlag::ACC_PRIVATE: {
				flag_vec.push_back("ACC_PRIVATE");
				break;
			}
			case FieldAccessFlag::ACC_PROTECTED: {
				flag_vec.push_back("ACC_PROTECTED");
				break;
			}
			case FieldAccessFlag::ACC_STATIC: {
				flag_vec.push_back("ACC_STATIC");
				break;
			}
			case FieldAccessFlag::ACC_FINAL: {
				flag_vec.push_back("ACC_FINAL");
				break;
			}
			case FieldAccessFlag::ACC_SYNCHRONIZED: {
				flag_vec.push_back("ACC_SYNCHRONIZED");
				break;
			}
			case FieldAccessFlag::ACC_VOLATILE: {
				flag_vec.push_back("ACC_VOLATILE");
				break;
			}
			case FieldAccessFlag::ACC_TRANSIENT: {
				flag_vec.push_back("ACC_TRANSIENT");
				break;
			}
			case FieldAccessFlag::ACC_NATIVE: {
				flag_vec.push_back("ACC_NATIVE");
				break;
			}
			case FieldAccessFlag::ACC_INTERFACE: {
				flag_vec.push_back("ACC_INTERFACE");
				break;
			}
			case FieldAccessFlag::ACC_ABSTRACT: {
				flag_vec.push_back("ACC_ABSTRACT");
				break;
			}
			case FieldAccessFlag::ACC_STRICT: {
				flag_vec.push_back("ACC_STRICT");
				break;
			}
			case FieldAccessFlag::ACC_SYNTHETIC: {
				flag_vec.push_back("ACC_SYNTHETIC");
				break;
			}
			case FieldAccessFlag::ACC_ANNOTATION: {
				flag_vec.push_back("ACC_ANNOTATION");
				break;
			}
			case FieldAccessFlag::ACC_ENUM: {
				flag_vec.push_back("ACC_ENUM");
				break;
			}
			}
		}
	}
	std::string flag_string = "";
	for (int i = 0; i < (int)flag_vec.size(); i++) {
		if (i > 0) flag_string += ", ";
		flag_string += flag_vec[i];
	}
	return flag_string;
}
/**
 * @brief transforma os bytes que representam as flags de uma classe em uma string com seus nomes
 * @param flag u2 que representa as flags de acesso da classe
 * @return string contendo o nome das flags ativas
 */
std::string ClassPrinter::ClassAccessFlagToString(u2 flag) {
	std::vector<std::string> flag_vec;
	for (unsigned int bit = 0; bit < 32; bit++) {
		if (flag & (1 << bit)) {
			switch((ClassAccessFlag) (1 << bit)) {
			case ClassAccessFlag::ACC_PUBLIC: {
				flag_vec.push_back("ACC_PUBLIC");
				break;
			}	 
			case ClassAccessFlag::ACC_FINAL: {
				flag_vec.push_back("ACC_FINAL");
				break;
			}	 
			case ClassAccessFlag::ACC_SUPER: {
				flag_vec.push_back("ACC_SUPER");
				break;
			}	 
			case ClassAccessFlag::ACC_INTERFACE: {
				flag_vec.push_back("ACC_INTERFACE");
				break;
			}	 
			case ClassAccessFlag::ACC_ABSTRACT: {
				flag_vec.push_back("ACC_ABSTRACT");
				break;
			}	 
			case ClassAccessFlag::ACC_SYNTHETIC: {
				flag_vec.push_back("ACC_SYNTHETIC");
				break;
			}	 
			case ClassAccessFlag::ACC_ANNOTATION: {
				flag_vec.push_back("ACC_ANNOTATION");
				break;
			}	 
			case ClassAccessFlag::ACC_ENUM: {
				flag_vec.push_back("ACC_ENUM");
				break;
			}
			}
		}
	}
	std::string flag_string = "";
	for (int i = 0; i < (int)flag_vec.size(); i++) {
		if (i > 0) flag_string += ", ";
		flag_string += flag_vec[i];
	}
	return flag_string;
}
/**
 * @brief transforma os bytes que representam as flags de um método em uma string com seus nomes
 * @param flag u2 que representa as flags de acesso do método
 * @return string contendo o nome das flags ativas
 */
std::string ClassPrinter::MethodAccessFlagToString(u2 flag) {
	std::vector<std::string> flag_vec;
	for (unsigned int bit = 0; bit < 32; bit++) {
		if (flag & (1 << bit)) {
			switch((MethodAccessFlag) (1 << bit)) {
			case MethodAccessFlag::ACC_PUBLIC: {
				flag_vec.push_back("ACC_PUBLIC");
				break;
			}
			case MethodAccessFlag::ACC_PRIVATE: {
				flag_vec.push_back("ACC_PRIVATE");
				break;
			}
			case MethodAccessFlag::ACC_PROTECTED: {
				flag_vec.push_back("ACC_PROTECTED");
				break;
			}
			case MethodAccessFlag::ACC_STATIC: {
				flag_vec.push_back("ACC_STATIC");
				break;
			}
			case MethodAccessFlag::ACC_FINAL: {
				flag_vec.push_back("ACC_FINAL");
				break;
			}
			case MethodAccessFlag::ACC_SYNCHRONIZED: {
				flag_vec.push_back("ACC_SYNCHRONIZED");
				break;
			}
			case MethodAccessFlag::ACC_BRIDGE: {
				flag_vec.push_back("ACC_BRIDGE");
				break;
			}
			case MethodAccessFlag::ACC_VARARGS: {
				flag_vec.push_back("ACC_VARARGS");
				break;
			}
			case MethodAccessFlag::ACC_NATIVE: {
				flag_vec.push_back("ACC_NATIVE");
				break;
			}
			case MethodAccessFlag::ACC_ABSTRACT: {
				flag_vec.push_back("ACC_ABSTRACT");
				break;
			}
			case MethodAccessFlag::ACC_STRICT: {
				flag_vec.push_back("ACC_STRICT");
				break;
			}
			case MethodAccessFlag::ACC_SYNTHETIC: {
				flag_vec.push_back("ACC_SYNTHETIC");
				break;
			}
			}
		}
	}
	std::string flag_string = "";
	for (int i = 0; i < (int)flag_vec.size(); i++) {
		if (i > 0) flag_string += ", ";
		flag_string += flag_vec[i];
	}
	return flag_string;
}


