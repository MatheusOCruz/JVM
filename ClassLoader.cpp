//
// Created by matheus on 4/2/24.
//

#include "ClassLoader.h"

void ClassLoader::LoadClass(const char *nomeArquivo) {
    LoadFile(nomeArquivo);
    magic               = read_u4();
    minor_version       = read_u2();
    major_version       = read_u2();
    constant_pool_count = read_u2();
    BuildConstantPoolTable();
    access_flags        = read_u2();
    this_class          = read_u2();
    super_class         = read_u2();
    interfaces_count    = read_u2();
    BuildInterfaces();
    interfaces_count    = read_u2();
    BuildInterfaces();
    methods_count       = read_u2();
    BuildMethods();
    attributes_count    = read_u2();
    BuildAttributes();

}
// TODO: tem q pegar esse ponteiro inicial pra poder limpar da memoria dps
void ClassLoader::LoadFile(const char *nomeArquivo) {
    std::ifstream arquivo(nomeArquivo, std::ios::binary);

    if (!arquivo) {
        std::cerr << "Não foi possível abrir o arquivo: " << nomeArquivo << std::endl;
        return;
    }

    // Obter o tamanho do arquivo
    arquivo.seekg(0, std::ios::end);
    std::streampos tamanhoArquivo = arquivo.tellg();
    arquivo.seekg(0, std::ios::beg);

    // Ler o arquivo byte a byte e armazená-lo no vetor buffer
    auto buffer = new std::vector<uint8_t>(tamanhoArquivo);
    arquivo.read(reinterpret_cast<char*>(buffer->data()), tamanhoArquivo);

    iter = buffer->begin();
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

template<typename T>
std::vector<T> *ClassLoader::read_vec(int length) {
    auto temp = new std::vector<T>();
    temp->reserve(length);

    std::copy(iter, iter + (length* sizeof(T)), std::back_inserter(*temp));
    iter += (length* sizeof(T));

    return temp;
}

cp_info *ClassLoader::BuildConstantPoolEntry() {
    auto Entry = new cp_info{};

    switch (Entry->tag = static_cast<ConstantPoolTag>(read_u1())) {
        case ConstantPoolTag::CONSTANT_Utf8: {

            Entry->length    = read_u2();
            Entry->bytes_vec = read_vec<u1>(Entry->length);

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
            throw std::runtime_error("cp_info nao existe amigao");
    }
    return Entry;
}

attribute_info* ClassLoader::BuildAttributeInfo() {
    auto Entry = new attribute_info{};

    Entry->attribute_name_index = read_u2();
    Entry->attribute_length     = read_u4();
    Entry->info                 = read_vec<u1>(Entry->attribute_length);

    return Entry;
}

field_info* ClassLoader::BuildFieldInfo() {
    auto Entry = new field_info{};

    Entry->access_flags     = read_u2();
    Entry->name_index       = read_u2();
    Entry->attributes_count = read_u2();
    Entry->attributes       = new std::vector<attribute_info*>;

    BuildAttributes(Entry->attributes_count, *Entry->attributes);

    return Entry;
}

method_info* ClassLoader::BuildMethodInfo() {
    auto Entry = new method_info{};

    Entry->access_flags     = read_u2();
    Entry->name_index       = read_u2();
    Entry->descriptor_index = read_u2();
    Entry->attributes_count = read_u2();
    Entry->attributes       = new std::vector<attribute_info*>;
    BuildAttributes(Entry->attributes_count, *Entry->attributes);

    return Entry;
}

void ClassLoader::BuildConstantPoolTable() {
    constant_pool.reserve(constant_pool_count);
    constant_pool.push_back(new cp_info{}); // id 0 n conta
    for(int i = 0; i<constant_pool_count-1; i++){
        constant_pool.push_back(BuildConstantPoolEntry());
    }
}
void ClassLoader::BuildInterfaces(){
    interfaces = read_vec<u2>(interfaces_count);
}

void ClassLoader::BuildFields() {
    fields.reserve(fields_count);
    for (int i = 0; i < fields_count; ++i) {
        fields.push_back(BuildFieldInfo());
    }
}

void ClassLoader::BuildMethods() {
    methods.reserve(methods_count);
    for (int i = 0; i < methods_count; ++i) {
        methods.push_back(BuildMethodInfo());
    }
}

void ClassLoader::BuildAttributes() {
    attributes.reserve(attributes_count);
    for (int i = 0; i < attributes_count; ++i) {
        attributes.push_back(BuildAttributeInfo());
    }
}

void ClassLoader::BuildAttributes(int _attributes_count, std::vector<attribute_info *> &_attributes) {
    _attributes.reserve(_attributes_count);
    for (int i = 0; i < _attributes_count; ++i) {
        _attributes.push_back(BuildAttributeInfo());
    }
}






