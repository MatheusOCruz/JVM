//
// Created by matheus on 4/2/24.
//

#include "../../include/ClassLoader/ClassLoader.h"

#include "../../include/utils.h"

class_file* ClassLoader::GetClass( std::string ClassName) {
    if (!((*class_files).count(ClassName))) {
        LoadClass(ClassName);
    }
    return (*class_files)[ClassName];
}

void ClassLoader::LoadClass(const std::string ClassName) {
    if ((*class_files).count(ClassName)) return;

    current_file = new class_file;

    LoadFile(ClassName);

    CheckMagic();
    CheckVersion();
    BuildConstantPoolTable();
    current_file->access_flags = read_u2();
    current_file->this_class   = read_u2();
    current_file->super_class  = read_u2();
    BuildInterfaces();
    BuildFields();
    BuildMethods();
    BuildAttributes();

    FormatCheck();

    (*class_files)[ClassName] = current_file;

    delete file_buffer;



    // Caso Nao seja a classe Object, carrega superclasse de forma recursiva
    if (current_file->super_class == 0)
        return;

    const auto SuperEntry = (*current_file->constant_pool)[current_file->super_class];
    const auto SuperName  = (*current_file->constant_pool)[SuperEntry->name_index]->AsString();
    // LoadClass(SuperName);
}

// #ifdef _WIN32
// 	const auto SEP = '\\';
// #else
// 	const auto SEP = '/';
// #endif
// 
// std::string ClassLoader::FindClass(const std::string ClassName, const std::string Directory) {
// 	// Looking for class in this directory
// 	if (auto dir = opendir(Directory.c_str())) {
//         while (auto f = readdir(dir)) {
//             if (!f->d_name || f->d_name[0] == '.') continue;
//             if (f->d_type == DT_REG) {
// 				// Checking if suffix of file matches ClassName
// 				std::string target = SEP + ClassName + ".class";
// 				std::string file_path = Directory + SEP + f->d_name;
// 				if (target.size() > file_path.size()) continue;
// 
// 				std::string suffix = file_path.substr((int)file_path.size() - (int)target.size(), target.size());
// 				if (suffix == target) return file_path;
// 			}
//         }
//         closedir(dir);
//     }
// 	// Recursive looking in sub directories
// 	if (auto dir = opendir(Directory.c_str())) {
//         while (auto f = readdir(dir)) {
//             if (!f->d_name || f->d_name[0] == '.') continue;
//             if (f->d_type == DT_DIR) {
//                 const auto class_path = FindClass(ClassName, Directory + SEP + f->d_name);
// 				if (class_path.size() > 0) return class_path;
// 			}
//         }
//         closedir(dir);
//     }
// 	return "";
// }

void ClassLoader::LoadFile(std::string ClassName) {
	std::string classPath;
    auto cwd = utils::GetCWD();

    std::regex ObjectClass (".*\\java/lang/Object$");
    if(std::regex_search(ClassName, ObjectClass)){
        classPath = "Object.class";
        classPath = cwd+"/" + classPath;
    }
    else{
        classPath = cwd + "/" + ClassName + ".class";
    }
    #ifdef _WIN32
        std::replace(classPath.begin(),classPath.end(),'/','\\');
    #endif

    std::ifstream arquivo(classPath, std::ios::binary);

    if (!arquivo) {
        std::cerr << "Não foi possível abrir o arquivo: " << classPath << std::endl;
        exit(1);
    }

    // Obter o tamanho do arquivo
    arquivo.seekg(0, std::ios::end);
    std::streampos tamanhoArquivo = arquivo.tellg();
    arquivo.seekg(0, std::ios::beg);

    // Ler o arquivo byte a byte e armazená-lo no vetor buffer
    file_buffer = new std::vector<uint8_t>(tamanhoArquivo);
    arquivo.read(reinterpret_cast<char*>(file_buffer->data()), tamanhoArquivo);

    iter = file_buffer->begin();
    arquivo.close();
}

u1 ClassLoader::read_u1() {
    u1 temp;
    std::copy(iter, iter + sizeof(temp), reinterpret_cast<uint8_t*>(&temp));
    iter+= sizeof(temp);
    return temp;
}

u2 ClassLoader::read_u2() {
    u2 temp;
    std::copy(iter, iter + sizeof(temp), reinterpret_cast<uint8_t*>(&temp));
    iter+= sizeof(temp);
    temp = __builtin_bswap16(temp);
    return temp;
}

u4 ClassLoader::read_u4() {
    u4 temp;
    std::copy(iter, iter + sizeof(temp), reinterpret_cast<uint8_t*>(&temp));
    iter+= sizeof(temp);
    temp = __builtin_bswap32(temp);
    return temp;
}

std::vector<u1> *ClassLoader::read_vec_u1(int length) {
    auto temp = new std::vector<u1>();
    temp->reserve(length);

	for (int i = 0; i < length; i++) temp->push_back(read_u1());
    return temp;
}

std::vector<u2> *ClassLoader::read_vec_u2(int length) {
    auto temp = new std::vector<u2>();
    temp->reserve(length);

	for (int i = 0; i < length; i++) temp->push_back(read_u2());
    return temp;
}


int ClassLoader::BuildConstantPoolInfo() {
    auto Entry = new cp_info{};

    switch (Entry->tag = static_cast<ConstantPoolTag>(read_u1())) {
        case ConstantPoolTag::CONSTANT_Utf8: {

            Entry->length    = read_u2();
            Entry->bytes_vec = read_vec_u1(Entry->length);
            break;
        }
        case ConstantPoolTag::CONSTANT_Integer:
        case ConstantPoolTag::CONSTANT_Float: {

            Entry->bytes = read_u4();

            break;
        }
        case ConstantPoolTag::CONSTANT_Long:
        case ConstantPoolTag::CONSTANT_Double: {

            Entry->high_bytes = read_u4();
            Entry->low_bytes  = read_u4();

            break;
        }
        case ConstantPoolTag::CONSTANT_Class: {

            Entry->name_index = read_u2();

            break;
        }
        case ConstantPoolTag::CONSTANT_String: {

            Entry->string_index = read_u2();

            break;
        }
        case ConstantPoolTag::CONSTANT_Fieldref:
        case ConstantPoolTag::CONSTANT_Methodref:
        case ConstantPoolTag::CONSTANT_InterfaceMethodref: {

            Entry->class_index         = read_u2();
            Entry->name_and_type_index = read_u2();

            break;
        }
        case ConstantPoolTag::CONSTANT_NameAndType: {

            Entry->name_index       = read_u2();
            Entry->descriptor_index = read_u2();

            break;
        }
        case ConstantPoolTag::CONSTANT_MethodHandle:

            Entry->reference_kind  = read_u1();
            Entry->reference_index = read_u2();

            break;
        case ConstantPoolTag::CONSTANT_MethodType:

            Entry->descriptor_index = read_u2();

            break;
        case ConstantPoolTag::CONSTANT_InvokeDynamic:

            Entry->bootstrap_method_attr_index = read_u2();
            Entry->name_and_type_index = read_u2();

            break;
        default:
            throw std::runtime_error("entrada da constant_pool nao existe amigao\n");
    }
    current_file->constant_pool->push_back(Entry);

    // entrada seguinte de Long ou Double nao e um indice valido
    if(Entry->tag == ConstantPoolTag::CONSTANT_Long || Entry->tag == ConstantPoolTag::CONSTANT_Double) {
        current_file->constant_pool->push_back(new cp_info{});
        return 1;
    }
    return 0;
}

attribute_info* ClassLoader::BuildAttributeInfo() {
    auto Entry = new attribute_info{};

    Entry->attribute_name_index = read_u2();
    Entry->attribute_length     = read_u4();

    // pega o nome da constant pool
    cp_info* AttributeNameEntry = (*current_file->constant_pool)[Entry->attribute_name_index];
    std::string AttributeName = AttributeNameEntry->AsString();
    //(reinterpret_cast<char*>(AttributeNameEntry->bytes_vec->data()), AttributeNameEntry->length)
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
    if(cases.find(AttributeName) != cases.end())
        AttributeTypeName = static_cast<AttributeType>(cases[AttributeName]);
    else
        AttributeTypeName = AttributeType::NotImplemented;


    switch (AttributeTypeName) {
        case AttributeType::ConstantValue: {
            Entry->constantvalue_index = read_u2();
            break;
        }
        case AttributeType::Code: {
            Entry->max_stack   = read_u2();
            Entry->max_locals  = read_u2();
            Entry->code_length = read_u4();
            Entry->code = read_vec_u1(Entry->code_length);

            Entry->exception_table_length = read_u2();

            Entry->exception_table = new std::vector<Exception_tableEntry*>;
            Entry->exception_table->reserve(Entry->exception_table_length);
            //TODO: provavelmente tirar esse for daqui
            for (int i = 0; i < Entry->exception_table_length; ++i) {
                auto TableEntry = new Exception_tableEntry{};
                TableEntry->start_pc   = read_u2();
                TableEntry->end_pc     = read_u2();
                TableEntry->handler_pc = read_u2();
                TableEntry->catch_type = read_u2();
                Entry->exception_table->push_back(TableEntry);
            }
            Entry->attributes_count = read_u2();
            Entry->attributes = new std::vector<attribute_info*>;
            BuildAttributes(Entry->attributes_count, *Entry->attributes);
            break;
        }
        //TODO: esse read_vec e so pra pular os bytes, tem q implementar
        case AttributeType::Exceptions: {
            // Lógica para lidar com Exceptions
            Entry->number_of_exceptions = read_u2();
            Entry->exception_index_table = read_vec_u2(Entry->number_of_exceptions);
            break;
        }
        case AttributeType::InnerClasses: {
            Entry->number_of_classes = read_u2();
            Entry->classes = new std::vector<InnerClasse*>;
            for (int i = 0; i < Entry->number_of_classes; ++i) {
                auto ClassEntry = new InnerClasse{};
                ClassEntry->inner_class_info_index   = read_u2();
                ClassEntry->outer_class_info_index   = read_u2();
                ClassEntry->inner_name_index         = read_u2();
                ClassEntry->inner_class_access_flags = read_u2();
                Entry->classes->push_back(ClassEntry);
            }
            break;
        }
        case AttributeType::Synthetic: {
            assert(Entry->attribute_length == 0);
            break;
        }
        case AttributeType::SourceFile: {
            // Lógica para lidar com SourceFile
            Entry->sourcefile_index = read_u2();
            break;
        }
        case AttributeType::NotImplemented: {

            read_vec_u1(Entry->attribute_length);
            break;
        }
    }



    return Entry;
}

void ClassLoader::BuildFieldInfo(){
    auto Entry = new field_info{};

    Entry->access_flags     = read_u2();
    Entry->name_index       = read_u2();
    Entry->descriptor_index = read_u2();
    Entry->attributes_count = read_u2();
    Entry->attributes       = new std::vector<attribute_info*>;

    BuildAttributes(Entry->attributes_count, *Entry->attributes);
    current_file->fields->push_back(Entry);
}

void ClassLoader::BuildMethodInfo() {
    auto Entry = new method_info{};

    Entry->access_flags     = read_u2();
    Entry->name_index       = read_u2();
    Entry->descriptor_index = read_u2();
    Entry->attributes_count = read_u2();
    Entry->attributes       = new std::vector<attribute_info*>;
    BuildAttributes(Entry->attributes_count, *Entry->attributes);

    current_file->methods->push_back(Entry);

}

void ClassLoader::BuildConstantPoolTable() {
    current_file->constant_pool_count = read_u2();
    current_file->constant_pool = new std::vector<cp_info*>;
    current_file->constant_pool->reserve(current_file->constant_pool_count);
    current_file->constant_pool->push_back(new cp_info{}); // id 0 n conta

    for(int i = 0; i< current_file->constant_pool_count-1; i++){
        if(BuildConstantPoolInfo())
            i++;
    }

}
void ClassLoader::BuildInterfaces(){
    current_file->interfaces_count = read_u2();
    current_file->interfaces = read_vec_u2(current_file->interfaces_count);
}

void ClassLoader::BuildFields() {
    current_file->fields_count = read_u2();
    current_file->fields = new std::vector<field_info*>;
    current_file->fields->reserve(current_file->fields_count);
    for (int i = 0; i < current_file->fields_count; ++i) {
        BuildFieldInfo();
    }
}

void ClassLoader::BuildMethods() {
    current_file->methods_count = read_u2();
    current_file->methods = new std::vector<method_info*>;
    current_file->methods->reserve(current_file->methods_count);
    for (int i = 0; i < current_file->methods_count; ++i) {
        BuildMethodInfo();
    }
}

void ClassLoader::BuildAttributes() {
    current_file->attributes_count = read_u2();
    current_file->attributes = new std::vector<attribute_info*>;
    current_file->attributes->reserve(current_file->attributes_count);
    for (int i = 0; i < current_file->attributes_count; ++i) {
        current_file->attributes->push_back(BuildAttributeInfo());
    }
}

void ClassLoader::BuildAttributes(int _attributes_count, std::vector<attribute_info *> &_attributes) {
    _attributes.reserve(_attributes_count);
    for (int i = 0; i < _attributes_count; ++i) {
        _attributes.push_back(BuildAttributeInfo());
    }
}
void ClassLoader::CheckMagic() {
    current_file->magic = read_u4();

    if (current_file->magic != 0xCAFEBABE) {
        throw ClassFormatError("first four bytes must contain the right magic number");
	}
}


void ClassLoader::CheckVersion() {
    current_file->minor_version = read_u2();
    current_file->major_version = read_u2();

    if(current_file->major_version > 52){
        // throw UnsupportedClassVersionError(current_file->major_version);
    }
}

void ClassLoader::FormatCheck() {
    // magic e checado no comeco

    if(!(iter == file_buffer->end())) {
        throw ClassFormatError("The class file must not be truncated or have extra bytes at the end.");
	}

    /*
    TODO: the constant pool must satisfy the constraints documented throughout §4.4.
         For example, each CONSTANT_Class_info structure in the constant pool must contain
         in its name_index item a valid constant pool index for a CONSTANT_Utf8_info
         structure.
         • All field references and method references in the constant pool must have valid
         names, valid classes, and valid descriptors (§4.3).
     */
}



